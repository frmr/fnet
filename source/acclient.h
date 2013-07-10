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

        bool            attemptConnection;
        bool            connected;

        const double    timeOutLimit;
        double          timeOutTimer;

    public:
        bool            Connect( const string &serverIP, const int serverPort );
        void            Disconnect();
        void            Send( const string &message ) const;
        vector<string>  Update( const double elapsedTime );

    public:
        acClient( const double timeOutLimit );
        ~acClient();
    };
}
