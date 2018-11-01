#include "uds_datagram.h"
#include "uds_datagram_server.h"
#include "uds_datagram_client.h"

UDSDatagramServer* createUDSDatagramServer() {
    return new UDSDatagramServerImpl();
}

void destroyUDSDatagramServer(UDSDatagramServer* udsds) {
    if (NULL != udsds) delete udsds;
}

UDSDatagramClient* createUDSDatagramClient() {
    return new UDSDatagramClientImpl();
}

void destroyUDSDatagramClient(UDSDatagramClient* udsdc) {
    if (NULL != udsdc) delete udsdc;
}
