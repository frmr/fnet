#include "acclient.h"

void ac::acClient::Ping()
{
    //get system time
    //Send() it to the server
}

bool ac::acClient::Connect( const string &serverIP, const int serverPort )
{
    client = enet_host_create( NULL, 1, 2, 57600/8, 14400/8 );  //SEPARATE METHOD TO SET THE LAST TWO VALUES

    if ( client == NULL )
    {
        cout << "acClient::Connect() - An error occured while trying to create an ENet server host." << endl;
        return false;
    }

    enet_address_set_host( &address, serverIP.c_str() );
    address.port = 1234;

    peer = enet_host_connect( client, &address, 2, 0 );

    if ( peer == NULL )
    {
        cout << "acClient::Connect() - No available peers for initiating an ENet connection." << endl;
        return false;
    }

    attemptConnection = true;
}

void acClient::Disconnect()
{

}

vector<string> acClient::Update( const double elapsedTime )
{
    if ( connected )
    {
        serverPingTimer += elapsedTime;
        if ( serverPingTimer > serverPingInterval )
        {
            Ping();
            while ( serverPingTimer > serverPingInterval )
            {
                serverPingTimer -= serverPingInterval;
            }
        }
    }

    vector<string> received;

    //check for received packets
    //if you get a ping back, set serverPing
}

acClient::acClient( const double serverPingInterval, const double serverTimeOutLimit )
    : attemptConnection( false ),
      connected( false ),
      serverPing( 0.0 ),
      serverPingInterval( serverPingInterval ),
      serverPingTimer( 0.0 ),
      serverTimeOutLimit( serverTimeOutLimit ),
      serverTimeOutTimer( 0.0 )
{

}
