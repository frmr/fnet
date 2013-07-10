#include <string>
#include <vector>
#include <utility>

#include "enet/enet_enet.h"

using std::string;
using std::vector;
using std::pair;

namespace ac
{
    class acServer
    {
    private:
        class ClientInfo
        {
        private:
            string      name;
            ENetPeer*   peer;
            double      timeOutTimer;

        public:
            ENetPeer*   GetPeerRef() const;
            double      GetTimeOutTimer() const;
            void        SetName( const string &name );
            void        ResetTimeOutTimer();
            void        UpdateTimeOutTimer( const double elapsedTime );

        public:
            ClientInfo( ENetPeer* const peer );
            ~ClientInfo();
        };

    private:
        ENetHost*           server;
        vector<ClientInfo>  clients;
        const double        timeOutLimit;

    private:
        int                 GetClientIndexFromID( const unsigned int id ) const;

    public:
        void                                    Broadcast( const string &message ) const;
        unsigned int                            Ping( const unsigned int client ) const;
        void                                    Send( const unsigned int client, const string &message ) const;
        bool                                    Start( const int port );
        void                                    Stop();
        vector< pair<unsigned int, string> >    Update( const double elapsedTime );

    public:
        acServer( const double timeOutLimit );
        ~acServer();
    };
}
