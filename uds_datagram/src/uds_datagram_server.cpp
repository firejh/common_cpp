#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <iostream>

using namespace std;

#include "uds_datagram_server.h"

UDSDatagramServerImpl::UDSDatagramServerImpl() {
    this->sock_file = NULL;
    this->call_back = NULL;
    this->sock = -1;
    this->thread_count = 0;
    this->running = false;
}

UDSDatagramServerImpl::~UDSDatagramServerImpl() {
}

int UDSDatagramServerImpl::open(const char * sock_file, UDSDatagramCallback* callback) {
    if (NULL == sock_file || 0 == sock_file[0] || NULL == callback) {
        return uds_ec_param_error;
    }

    size_t sock_file_len = strlen(sock_file);
    if (MAX_FILE_PATH_LEN < sock_file_len) {
        return uds_ec_param_error;
    }

    this->sock_file = (char*)malloc(sock_file_len + 1);
    if (NULL == this->sock_file) {
        return uds_ec_no_memory;
    }

    strcpy(this->sock_file, sock_file);
    this->call_back = callback;

    int ret = 0;
    pthread_attr_t thread_attr;
    memset(&thread_attr, 0, sizeof(thread_attr));

    ret = pthread_attr_init(&thread_attr);
    if (0 != ret) {
        return uds_ec_system_error;
    }

    ret = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (0 != ret) {
        return uds_ec_system_error;
    }

    __sync_add_and_fetch(&this->thread_count, 1);
    pthread_t thread_id;
    ret = pthread_create(&thread_id, &thread_attr, &UDSDatagramServerImpl::work_thread, this);
    if (0 != ret) {
        __sync_sub_and_fetch(&this->thread_count, 1);
        return uds_ec_system_error;
    }

    this->running = true;

    return 0;
}

void UDSDatagramServerImpl::close() {
    this->running = false;

    // waitting for work thread stop
    while (this->thread_count > 0) {
        usleep(100);
    }

    // clean resource
    if (-1 != this->sock) {
        ::close(this->sock);
    }

    if (NULL != this->sock_file) {
        free(this->sock_file);
    }
}

int UDSDatagramServerImpl::send_data(const char * peer_sock_file, const char* data, int data_len) {
    if (NULL == data || 0 == data_len) {
        return uds_ec_param_error;
    }
    if (NULL == peer_sock_file || 0 == peer_sock_file[0]) {
        return uds_ec_param_error;
    }
    if (MAX_FILE_PATH_LEN < strlen(peer_sock_file)) {
        return uds_ec_param_error;
    }

    if (-1 == this->sock) {
        return uds_ec_unopen;
    }

    struct sockaddr_un peer_addr;
    memset(&peer_addr, 0, sizeof(struct sockaddr_un));
    peer_addr.sun_family = AF_UNIX;
    strcpy(peer_addr.sun_path, peer_sock_file);

    return sendto(this->sock, data, data_len, 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr));
}

int UDSDatagramServerImpl::send_data(const char * peer_sock_file, const std::string & data) {
    return this->send_data(peer_sock_file, data.data(), data.size());
}

void* UDSDatagramServerImpl::work_thread(void * arg) {
    UDSDatagramServerImpl * _this = (UDSDatagramServerImpl*)arg;
    if (NULL != _this) {
        _this->work_thread();
    }
    __sync_sub_and_fetch(&_this->thread_count, 1);
    return NULL;
}

void UDSDatagramServerImpl::work_thread() {
    this->sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (-1 == this->sock) {
        if (NULL != this->call_back) {
            this->call_back->onError(errno);
        }
        return;
    }

    // set recv timeout. 500 microsecond
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500;
    if (0 != setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))) {
        if (NULL != this->call_back) this->call_back->onError(errno);
        return;
    }

    // set socket rcv buffer size
    // https://stackoverflow.com/questions/4729315/what-is-the-max-size-of-af-unix-datagram-message-that-can-be-sent-in-linux
    // it looks like AF_UNIX sockets don't support scatter/gather on current Linux. it is a fixed size 130688 B
    // 也就是说单个 datagram 包最大长度是 130688 B。

    // bind addr
    struct sockaddr_un bind_addr;
    memset(&bind_addr, 0, sizeof(struct sockaddr_un));
    bind_addr.sun_family = AF_UNIX;
    strcpy(bind_addr.sun_path, this->sock_file);

    // 删除文件，防止有残留数据
    unlink(this->sock_file);
    int ret = bind(this->sock, (struct sockaddr*)&bind_addr, sizeof(bind_addr));
    if (-1 == ret) {
        if (NULL != this->call_back) this->call_back->onError(errno);
        return;
    }

    char * recv_buf = (char*)malloc(MAX_DATA_LEN);
    if (NULL == recv_buf) {
        if (NULL != this->call_back) {
            this->call_back->onError(uds_ec_no_memory);
        }
        return;
    }

    struct sockaddr_un peer_addr;
    memset(&peer_addr, 0, sizeof(struct sockaddr_un));
    socklen_t sock_len;
    ssize_t recv_byte_count = 0;
    while (this->running) {
        // bug fix(AL, 20180719): 下面这行代码如果不存在， 则 linux 系统的 recvfrom api 依然能
        // 获取到 peer 的地址， 但是在 mac 系统下就无法获取 peer 地址。
        sock_len = sizeof(peer_addr);
        recv_byte_count = recvfrom(this->sock, recv_buf, MAX_DATA_LEN, 0,
                                    (struct sockaddr *)&peer_addr, &sock_len);
        if (-1 == recv_byte_count) {
            if (errno == EWOULDBLOCK) {
                continue;     // timeout still work
            }
            if (NULL != this->call_back) {
                this->call_back->onError(errno); // error stop work
            }
            continue;
            //break;
        }
        // cout << "peer addr:" << peer_addr.sun_path << ", addr len:" << sock_len << endl;

        if (NULL != this->call_back) {
            this->call_back->onRecv(peer_addr.sun_path, recv_buf, recv_byte_count);
        }
    }

    free(recv_buf);
}
