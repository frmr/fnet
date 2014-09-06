#include <iostream>

#include "fnetClient.h"

using std::cout;
using std::endl;

bool fnet::Client::Connect( const string& serverIp, const int serverPort )
{
    if ( !connected )
    {
        client = enet_host_create( NULL, 1, 2, 0, 0 );  //separate method to set the last two values

        if ( client == NULL )
        {
            cout << "Client::Connect() - An error occured while trying to create an ENet server host." << endl;
            return false;
        }

        enet_address_set_host( &address, serverIp.c_str() );
        address.port = serverPort;

        peer = enet_host_connect( client, &address, 2, 0 );

        if ( peer == NULL )
        {
            cout << "Client::Connect() - No available peers for initiating an ENet connection." << endl;
            return false;
        }

        attemptConnection = true;
    }
    else
    {
        cout << "Connect() - Unable to request connection to server: Already connected to a server." << endl;
        return false;
    }
    return true;
}

int fnet::Client::Ping() const
{
    return peer->roundTripTime;
}

void fnet::Client::Disconnect()
{
    if ( connected )
    {
        enet_peer_disconnect ( peer, 0 );
        connected = false;
        cout << "Client::Disconnect() - Disconnected from server." << endl;
    }
    else
    {
        cout << "Client::Disconnect() - No connected server from which to disconnect." << endl;
    }
}

void fnet::Client::Send( const string& message, const bool reliable ) const
{
    if ( connected )
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

        enet_peer_send( peer, 0, packet );
    }
    else
    {
        cout << "Client::Send() - Unable to send message unless connected to a server." << endl;
    }
}

vector<string> fnet::Client::Update( const double elapsedTime )
{
    if ( connected || attemptConnection )
    {
        //check for server time out
        timeOutTimer += elapsedTime;

        if ( timeOutTimer >= timeOutLimit )
        {
            cout << "Client::Update() - Server timed out." << endl;

            if ( connected )
            {
                Disconnect();
            }
            else
            {
                attemptConnection = false;
            }
        }
    }

    vector<string> received;

    //check for received packets
    ENetEvent event;
    while ( enet_host_service( client, &event, 0 ) )
    {
        timeOutTimer = 0.0;

        switch( event.type )
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                if ( attemptConnection )
                {
                    cout << "Connected to server." << endl;
                    connected = true;
                    attemptConnection = false;
                }
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                if ( connected )
                {
                    string message ( (char*) event.packet->data, event.packet->dataLength );
                    received.push_back( message );
                    enet_packet_destroy( event.packet );
                }

                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                if ( connected )
                {
                    cout << "Disconnected from server." << endl;
                    connected = false;
                    event.peer->data = NULL; //reset client's information
                }

                break;
            }

            case ENET_EVENT_TYPE_NONE:
            {
                break;
            }
        }
    }

    return received;
}

fnet::Client::Client( const double serverTimeOutLimit )
    :   attemptConnection( false ),
        connected( false ),
        timeOutLimit( serverTimeOutLimit ),
        timeOutTimer( 0.0 )
{
    enet_initialize();
}

fnet::Client::~Client()
{
    enet_deinitialize();
    delete client;
    delete peer;
}
