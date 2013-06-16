#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::end;
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
        double          serverPing;

        const double    serverPingInterval;
        const double    serverPingTimer;

        const double    serverTimeOutLimit;
        double          serverTimeOutTimer;

    private:
        void            Ping();

    public:
        bool            Connect( const string &serverIP, const int serverPort );
        void            Disconnect();
        void            Send( const string &message ) const;
        vector<string>  Update( const double elapsedTime );

    public:
        acClient( const double serverPingInterval, const double serverTimeOutLimit );
        ~acClient();
    }
}
