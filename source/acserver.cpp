#include <iostream>

#include "acserver.h"

using std::cout;
using std::endl;

ENetPeer* ac::acServer::ClientInfo::GetPeerRef() const
{
    return peer;
}

double ac::acServer::ClientInfo::GetTimeOutTimer() const
{
    return timeOutTimer;
}

void ac::acServer::ClientInfo::SetName( const string &newName )
{
    name = newName;
}

void ac::acServer::ClientInfo::ResetTimeOutTimer()
{
    timeOutTimer = 0.0;
}

void ac::acServer::ClientInfo::UpdateTimeOutTimer( const double elapsedTime )
{
    timeOutTimer += elapsedTime;
}

ac::acServer::ClientInfo::ClientInfo( ENetPeer* const peer )
    : peer( peer ),
      timeOutTimer( 0.0 )
{
}

ac::acServer::ClientInfo::~ClientInfo()
{
}

int ac::acServer::GetClientIndexFromID( const unsigned int id ) const
{
    for ( int clientIndex = 0; clientIndex < clients.size(); clientIndex++ )
    {
        if ( clients[(unsigned int) clientIndex].GetPeerRef()->connectID == id )
        {
            return clientIndex;
        }
    }
    return -1;
}

void ac::acServer::Broadcast( const string &message ) const
{
    ENetPacket *packet = enet_packet_create( message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE );
    enet_host_broadcast( server, 0, packet );
}

unsigned int ac::acServer::Ping( const unsigned int client ) const
{
    int clientIndex = GetClientIndexFromID( client );
    if ( clientIndex != -1 )
    {
        return clients[clientIndex].GetPeerRef()->roundTripTime;
    }
    else
    {
        cout << "ac::Server::Ping() - Could not find ID among connected clients." << endl;
        return 0;
    }
}

void ac::acServer::Send( const unsigned int client, const string &message ) const
{
    ENetPacket *packet = enet_packet_create( message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE);
    int clientIndex = GetClientIndexFromID( client );

    if ( clientIndex != -1 )
    {
        enet_peer_send( clients[(unsigned int) clientIndex].GetPeerRef(), 0, packet );
    }
    else
    {
        cout << "acServer::Send() - Could not send packet because the target ID is not among connected clients." << endl;
    }
}

bool ac::acServer::Start( const int port )
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    server = enet_host_create( &address, 32, 2, 0, 0 );

    if ( server == NULL )
    {
        cout << "acServer::Start() - An error occured while trying to create an ENet server host." << endl;
        return false;
    }
    else
    {
        return true;
    }
}

void ac::acServer::Stop()
{
    enet_host_destroy( server );
}

vector< pair<unsigned int, string> > ac::acServer::Update( const double elapsedTime )
{
    for ( vector<ClientInfo>::iterator it = clients.begin(); it != clients.end(); ++it )
    {
        it->UpdateTimeOutTimer( elapsedTime );
    }

    vector< pair<unsigned int, string> > received;

    //check for received packets
    ENetEvent event;

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
                    cout << "acServer::Update() - Could not find ID among connected clients." << endl;
                }
                event.peer->data = NULL; //reset client's information
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

ac::acServer::acServer( const double timeOutLimit )
    : server( NULL ),
      timeOutLimit( timeOutLimit )
{
}

ac::acServer::~acServer()
{
}
