#ifndef FRMR_SERVER_H
#define FRMR_SERVER_H

#include <string>
#include <vector>
#include <utility>

#include "enet/enet_enet.h"

using std::string;
using std::vector;
using std::pair;

namespace frmr
{
    class frmrServer
    {
    private:
        class ClientInfo
        {
        private:
            ENetPeer*   peer;
            double      timeOutTimer;

        public:
            ENetPeer*   GetPeerRef() const;
            double      GetTimeOutTimer() const;
            void        ResetTimeOutTimer();
            void        UpdateTimeOutTimer( const double elapsedTime );

        public:
            ClientInfo( ENetPeer* const peer );
            ~ClientInfo();
        };

    private:
        ENetHost*           server;
        string              name;
        vector<ClientInfo>  clients;
        const double        timeOutLimit;

    private:
        int                 GetClientIndexFromID( const unsigned int id ) const;

    public:
        void                                    Broadcast( const string &message, const bool reliable ) const;
        unsigned int                            Ping( const unsigned int client ) const;
        void                                    Send( const unsigned int client, const string &message, const bool reliable ) const;
        void                                    SetName( const string &newName );
        bool                                    Start( const int port );
        void                                    Stop();
        vector< pair<unsigned int, string> >    Update( const double elapsedTime );

    public:
        frmrServer( const double timeOutLimit );
        ~frmrServer();
    };
}

#endif
