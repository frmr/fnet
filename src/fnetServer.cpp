#include <iostream>

#include "fnetServer.h"

using std::cerr;
using std::endl;

ENetPeer* fnet::Server::ClientInfo::GetPeerRef() const
{
    return peer;
}

double fnet::Server::ClientInfo::GetTimeOutTimer() const
{
    return timeOutTimer;
}

void fnet::Server::ClientInfo::ResetTimeOutTimer()
{
    timeOutTimer = 0.0;
}

void fnet::Server::ClientInfo::UpdateTimeOutTimer( const double elapsedTime )
{
    timeOutTimer += elapsedTime;
}

fnet::Server::ClientInfo::ClientInfo( ENetPeer* const peer )
    :   peer( peer ),
        timeOutTimer( 0.0 )
{
}

fnet::Server::ClientInfo::~ClientInfo()
{
    delete peer;
}

int fnet::Server::GetClientIndexFromID( const unsigned int id ) const
{
    unsigned int clientIndex =  0;
    for ( auto client : clients )
    {
        if ( client.GetPeerRef()->connectID == id )
        {
            return clientIndex;
        }
        clientIndex++;
    }
    return -1;
}

void fnet::Server::Broadcast( const string& message, const bool reliable ) const
{
    ENetPacket* packet;

    if ( reliable )
    {
        packet = enet_packet_create( message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE );
    }
    else
    {
        packet = enet_packet_create( message.c_str(), message.size(), 0 );
    }
    enet_host_broadcast( server, 0, packet );
    delete packet;
}

unsigned int fnet::Server::Ping( const unsigned int client ) const
{
    int clientIndex = GetClientIndexFromID( client );
    if ( clientIndex != -1 )
    {
        return clients[clientIndex].GetPeerRef()->roundTripTime;
    }
    else
    {
        cerr << "frmr::Server::Ping() - Could not find ID among connected clients." << endl;
        return 0;
    }
}

void fnet::Server::Send( const unsigned int client, const string &message, const bool reliable ) const
{
    ENetPacket* packet;

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
        cerr << "Server::Send() - Could not send packet because the target ID is not among connected clients." << endl;
    }
    delete packet;
}

void fnet::Server::SetName( const string& newName )
{
    name = newName;
}

bool fnet::Server::Start( const int port )
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    server = enet_host_create( &address, 32, 2, 0, 0 );

    if ( server == NULL )
    {
        cerr << "Server::Start() - An error occured while trying to create an ENet server host." << endl;
        return false;
    }
    else
    {
        return true;
    }
}

void fnet::Server::Stop()
{
    enet_host_destroy( server );
}

vector<pair<unsigned int, string>> fnet::Server::Update( const double elapsedTime )
{
    //update time out timer for each client
    for ( auto client : clients )
    {
        client.UpdateTimeOutTimer( elapsedTime );
    }

    vector< pair<unsigned int, string> > received;

    //check for received packets
    ENetEvent event;

    //iterate over  each received packet
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
                cerr << "New client connected." << endl;
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
                cerr << "Client disconnected." << endl;
                int clientIndex = GetClientIndexFromID( event.peer->connectID );
                if ( clientIndex != -1 )
                {
                    clients.erase( clients.begin() + clientIndex );
                }
                else
                {
                    cerr << "Server::Update() - Could not find ID among connected clients." << endl;
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
    for ( auto it = clients.begin(); it != clients.end(); ++it )
    {
        if ( it->GetTimeOutTimer() >= timeOutLimit )
        {
            enet_peer_disconnect( it->GetPeerRef(), 0 );
            clients.erase( it );
        }
    }

    return received;
}

fnet::Server::Server( const double timeOutLimit )
    :   server( NULL ),
        timeOutLimit( timeOutLimit )
{
}

fnet::Server::~Server()
{
    delete server;
}
