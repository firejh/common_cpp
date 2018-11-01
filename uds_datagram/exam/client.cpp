#include <stdio.h>
#include <unistd.h>
#include <string>
#include <uds_datagram.h>

UDSDatagramClient*     uds_datagram_client;
bool running;

class ClientCallBack : public UDSDatagramCallback {
public:
    ClientCallBack();
    virtual ~ClientCallBack();

private:
    virtual void onRecv(const char * peer_sock_file, char* buf, int buf_len);
    virtual void onError(int error_code);
};

ClientCallBack::ClientCallBack() {}
ClientCallBack::~ClientCallBack() {}

void ClientCallBack::onRecv(const char * peer_sock_file, char* buf, int buf_len) {
    if (NULL == peer_sock_file) {
        printf("onRecv peer sock file error.\n");
        return;
    }

    if (NULL == buf) {
        printf("onRecv buf error.\n");
        return;
    }

    printf("onRecv %s:%.*s\n", peer_sock_file, buf_len, buf);
}

void ClientCallBack::onError(int error_code) {
    printf("on uds datagram error:%d\n", error_code);
}

int main(int argc, char *argv[]) {
    const char* local_file = "/tmp/uds_client_test";
    const char* sock_file = "/tmp/uds_test";
    // if (argc != 3) {
    //     printf("Usage:\r\t%s bind_sock_file socket_file\n", argv[0]);
    //     return 1;
    // }
    if (argc == 3) {
        local_file = argv[1];
        sock_file = argv[2];
    }

    ClientCallBack call_back;
    uds_datagram_client = createUDSDatagramClient();
    if (NULL == uds_datagram_client) {
        return 2;
    }

    if (0 != uds_datagram_client->open(local_file, sock_file, &call_back)) {
        destroyUDSDatagramClient(uds_datagram_client);
        return 3;
    }

    printf("uds datagram client is running...\n");
    running = true;
    while (running) {
        usleep(100*1000);
        uds_datagram_client->send_data(std::string("hello, server."));
    }

    return 0;
}
