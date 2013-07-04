#include <string>
#include <vector>

using std::string;
using std::vector;

namespace ac
{
    class acServer
    {
    private:
        struct ClientInfo
        {
            uint16_t        id;
            string          ip;
            string          name;
        };

    private:
        ENetAddress         address;
        ENetHost*           server;

        vector<ClientInfo>  clients;

        const double        pingInterval;
        double              pingTimer;

        const double        timeOutInterval;
        double              timeOutTimer;

    private:
        void                CalculatePing( string pingMessage );
        void                PingClients() const;

    public:
        void                Send( const string &message ) const;
        void                Start();
        void                Stop();
        vector<string>      Update( const double elapsedTime );

    public:
        acServer( const double pingInterval, const double serverTimeOutLimit );
        ~acServer();
    };
}
