#include <string>
#include <vector>

#include "enet/enet_enet.h"

using std::string;
using std::vector;

namespace frmr
{
    class frmrClient
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
        int             Ping() const;
        void            Send( const string &message ) const;
        vector<string>  Update( const double elapsedTime );

    public:
        frmrClient( const double timeOutLimit );
        ~frmrClient();
    };
}
