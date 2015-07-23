#ifndef FNET_H
#define FNET_H

#include "fnetServer.h"
#include "fnetClient.h"

namespace fnet
{
    void Start()
    {
        enet_initialize();
    }

    void Stop()
    {
        enet_deinitialize();
    }
}

#endif //FNET_H
