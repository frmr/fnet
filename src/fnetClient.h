#ifndef FNET_CLIENT_H
#define FNET_CLIENT_H

#include <string>
#include <vector>

#include "enet/enet_enet.h"

using std::string;
using std::vector;

namespace fnet
{
    class Client
    {
    private:
        ENetAddress     address;
        ENetHost*       client;
        ENetPeer*       peer;

        bool            attemptConnection;
        bool            connected;

        const double    timeOutLimit;
        double          timeOutTimer;

    public:
        bool            Connect( const string& serverIp, const int serverPort );
        void            Disconnect();
        int             Ping() const;
        void            Send( const string& message, const bool reliable ) const;
        vector<string>  Update( const double elapsedTime );

    public:
        Client( const double timeOutLimit );
        ~Client();
    };
}

#endif //FNET_CLIENT_H
