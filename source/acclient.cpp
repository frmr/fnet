#include "windows.h"
#include <iostream>

#include "acclient.h"

using std::cout;
using std::endl;

bool ac::acClient::Connect( const string &serverIP, const int serverPort )
{
    client = enet_host_create( NULL, 1, 2, 0, 0 );  //SEPARATE METHOD TO SET THE LAST TWO VALUES

    if ( client == NULL )
    {
        cout << "acClient::Connect() - An error occured while trying to create an ENet server host." << endl;
        return false;
    }

    enet_address_set_host( &address, serverIP.c_str() );
    address.port = serverPort;

    peer = enet_host_connect( client, &address, 2, 0 );

    if ( peer == NULL )
    {
        cout << "acClient::Connect() - No available peers for initiating an ENet connection." << endl;
        return false;
    }

    attemptConnection = true;
}

void ac::acClient::Disconnect()
{
    if ( connected )
    {
        enet_peer_disconnect ( peer, 0 );
        connected = false;
        cout << "acClient::Disconnect() - Disconnected from server." << endl;
    }
    else
    {
        cout << "acClient::Disconnect() - No connected server from which to disconnect." << endl;
    }
}

void ac::acClient::Send( const string &message ) const
{
    if ( connected )
    {
        ENetPacket *packet = enet_packet_create( message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send( peer, 0, packet );
    }
    else
    {
        cout << "acClient::Send() - Unable to send message unless connected to a server." << endl;
    }
}

vector<string> ac::acClient::Update( const double elapsedTime )
{
    //check for server time out
    timeOutTimer += elapsedTime;

    if ( timeOutTimer >= timeOutLimit )
    {
        cout << "acClient::Update() - Server timed out." << endl;

        if ( connected )
        {
            Disconnect();
        }
        else
        {
            attemptConnection = false;
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
                    //send name
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
        }
    }

    return received;
}

ac::acClient::acClient( const double serverTimeOutLimit )
    : attemptConnection( false ),
      connected( false ),
      timeOutLimit( serverTimeOutLimit ),
      timeOutTimer( 0.0 )
{
}

ac::acClient::~acClient()
{
    enet_host_destroy( client );
    enet_deinitialize();
}
