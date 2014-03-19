#include <iostream>

#include "frmr/fnet/server.h"

using std::cout;
using std::endl;

ENetPeer* frmr::fnet::frmrServer::ClientInfo::GetPeerRef() const
{
    return peer;
}

double frmr::fnet::frmrServer::ClientInfo::GetTimeOutTimer() const
{
    return timeOutTimer;
}

void frmr::fnet::frmrServer::ClientInfo::ResetTimeOutTimer()
{
    timeOutTimer = 0.0;
}

void frmr::fnet::frmrServer::ClientInfo::UpdateTimeOutTimer( const double elapsedTime )
{
    timeOutTimer += elapsedTime;
}

frmr::fnet::frmrServer::ClientInfo::ClientInfo( ENetPeer* const peer )
    : peer( peer ),
      timeOutTimer( 0.0 )
{
}

frmr::fnet::frmrServer::ClientInfo::~ClientInfo()
{
}

int frmr::fnet::frmrServer::GetClientIndexFromID( const unsigned int id ) const
{
    for ( unsigned int clientIndex = 0; clientIndex < clients.size(); clientIndex++ )
    {
        if ( clients[clientIndex].GetPeerRef()->connectID == id )
        {
            return clientIndex;
        }
    }
    return -1;
}

void frmr::fnet::frmrServer::Broadcast( const string &message, const bool reliable ) const
{
    ENetPacket *packet;

    if ( reliable )
    {
        packet = enet_packet_create( message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE );
    }
    else
    {
        packet = enet_packet_create( message.c_str(), message.size(), 0 );
    }
    enet_host_broadcast( server, 0, packet );
}

unsigned int frmr::fnet::frmrServer::Ping( const unsigned int client ) const
{
    int clientIndex = GetClientIndexFromID( client );
    if ( clientIndex != -1 )
    {
        return clients[clientIndex].GetPeerRef()->roundTripTime;
    }
    else
    {
        cout << "frmr::Server::Ping() - Could not find ID among connected clients." << endl;
        return 0;
    }
}

void frmr::fnet::frmrServer::Send( const unsigned int client, const string &message, const bool reliable ) const
{
    ENetPacket *packet;

    if ( reliable )
    {
        packet = enet_packet_create( message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE );
    }
    else
    {
        packet = enet_packet_create( message.c_str(), message.size(), 0 );
    }

    int clientIndex = GetClientIndexFromID( client );

    if ( clientIndex != -1 )
    {
        enet_peer_send( clients[(unsigned int) clientIndex].GetPeerRef(), 0, packet );
    }
    else
    {
        cout << "frmrServer::Send() - Could not send packet because the target ID is not among connected clients." << endl;
    }
}

void frmr::fnet::frmrServer::SetName( const string &newName )
{
    name = newName;
}

bool frmr::fnet::frmrServer::Start( const int port )
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    server = enet_host_create( &address, 32, 2, 0, 0 );

    if ( server == NULL )
    {
        cout << "frmrServer::Start() - An error occured while trying to create an ENet server host." << endl;
        return false;
    }
    else
    {
        return true;
    }
}

void frmr::fnet::frmrServer::Stop()
{
    enet_host_destroy( server );
}

vector< pair<unsigned int, string> > frmr::fnet::frmrServer::Update( const double elapsedTime )
{
	//update time out timer for each client
    for ( vector<ClientInfo>::iterator it = clients.begin(); it != clients.end(); ++it )
    {
        it->UpdateTimeOutTimer( elapsedTime );
    }

    vector< pair<unsigned int, string> > received;

    //check for received packets
    ENetEvent event;

	//iterate over each received packet
    while ( enet_host_service( server, &event, 0 ) )
    {
        //determine which client sent it
        int clientIndex = GetClientIndexFromID( event.peer->connectID );
        if ( clientIndex != -1 )
        {
            clients[(unsigned int) clientIndex].ResetTimeOutTimer();
        }

        switch( event.type )
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                cout << "New client connected." << endl;
                clients.push_back( ClientInfo( event.peer ) );
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                string message ( (char*) event.packet->data, event.packet->dataLength );
                received.push_back( make_pair( event.peer->connectID, message ) );
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                cout << "Client disconnected." << endl;
                int clientIndex = GetClientIndexFromID( event.peer->connectID );
                if ( clientIndex != -1 )
                {
                    clients.erase( clients.begin() + clientIndex );
                }
                else
                {
                    cout << "frmrServer::Update() - Could not find ID among connected clients." << endl;
                }
                event.peer->data = NULL; //reset client's information
                break;
            }

            case ENET_EVENT_TYPE_NONE:
			{
				break;
			}
        }
    }

    //check if any clients have timed out
    for ( vector<ClientInfo>::iterator it = clients.begin(); it != clients.end(); ++it )
    {
        if ( it->GetTimeOutTimer() >= timeOutLimit )
        {
            enet_peer_disconnect( it->GetPeerRef(), 0 );
            clients.erase( it );
        }
    }

    return received;
}

frmr::fnet::frmrServer::frmrServer( const double timeOutLimit )
    : server( NULL ),
      timeOutLimit( timeOutLimit )
{
    enet_initialize();
}

frmr::fnet::frmrServer::~frmrServer()
{
    enet_deinitialize();
}
