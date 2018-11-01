#include <stdio.h>
#include <unistd.h>
#include <string>
#include <uds_datagram.h>

UDSDatagramServer*     uds_datagram_server;
bool running;

class ServerCallBack : public UDSDatagramCallback {
public:
    ServerCallBack();
    virtual ~ServerCallBack();

private:
    virtual void onRecv(const char * peer_sock_file, char* buf, int buf_len);
    virtual void onError(int error_code);
};

ServerCallBack::ServerCallBack() {}
ServerCallBack::~ServerCallBack() {}

void ServerCallBack::onRecv(const char * peer_sock_file, char* buf, int buf_len) {
    if (NULL == peer_sock_file) {
        printf("onRecv peer sock file error.\n");
        return;
    }

    if (NULL == buf) {
        printf("onRecv buf error.\n");
        return;
    }

    printf("onRecv %s:%.*s\n", peer_sock_file, buf_len, buf);
    int ret = uds_datagram_server->send_data(peer_sock_file, std::string("hello, client."));
    if (0 >= ret) {
        printf("send data error(%d). peer:%s\n", ret, peer_sock_file);
    }
}

void ServerCallBack::onError(int error_code) {
    printf("on uds datagram error:%d\n", error_code);
}

int main(int argc, char *argv[]) {
    const char* sock_file = "/tmp/uds_test";
    // if (argc != 2) {
    //     printf("Usage:\r\t%s socket_file\n", argv[0]);
    //     return 1;
    // }
    if (argc == 2) {
        sock_file = argv[1];
    }

    ServerCallBack call_back;
    uds_datagram_server = createUDSDatagramServer();
    if (NULL == uds_datagram_server) {
        return 2;
    }

    if (0 != uds_datagram_server->open(sock_file, &call_back)) {
        destroyUDSDatagramServer(uds_datagram_server);
        return 3;
    }

    printf("uds datagram server is running...\n");
    running = true;
    while (running) {
        usleep(100*1000);
    }

    return 0;
}
