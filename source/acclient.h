#include <iostream>
#include <string>
#include <vector>

#include "enet/enet_enet.h"

using std::string;
using std::vector;

namespace ac
{
    class acClient
    {
    private:
        ENetAddress     address;
        ENetHost*       client;
        ENetPeer*       peer;
        ENetEvent       event;

        bool            attemptConnection;
        bool            connected;
        uint16_t        serverPing;

        const double    serverPingInterval;
        double          serverPingTimer;

        const double    serverTimeOutLimit;
        double          serverTimeOutTimer;

    private:
        void            CalculatePing( string pingMessage );
        void            PingServer() const;

    public:
        bool            Connect( const string &serverIP, const int serverPort );
        void            Disconnect();
        void            Send( const string &message ) const;
        vector<string>  Update( const double elapsedTime );

    public:
        acClient( const double serverPingInterval, const double serverTimeOutLimit );
        ~acClient();
    };
}