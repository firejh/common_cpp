#include <stdlib.h>
#include <unistd.h>
#include <cerrno>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "uds_datagram_client.h"

UDSDatagramClientImpl::UDSDatagramClientImpl() {
    this->server_sock_file = NULL;
    this->bind_sock_file = NULL;
    this->call_back = NULL;
    this->sock = -1;
    this->thread_count = 0;
    this->running = false;
}

UDSDatagramClientImpl::~UDSDatagramClientImpl() {
}

int UDSDatagramClientImpl::open(const char * bind_sock_file, const char * sock_file, UDSDatagramCallback* callback) {
    if (NULL == sock_file || 0 == sock_file[0] || NULL == sock_file || 0 == sock_file[0] || NULL == callback) {
        return uds_ec_param_error;
    }

    size_t sock_file_len = strlen(sock_file);
    size_t bind_file_len = strlen(bind_sock_file);
    if (MAX_FILE_PATH_LEN < sock_file_len || MAX_FILE_PATH_LEN < bind_file_len) {
        return uds_ec_param_error;
    }

    this->server_sock_file = (char*)malloc(sock_file_len + 1);
    if (NULL == this->server_sock_file) {
        return uds_ec_no_memory;
    }
    strcpy(this->server_sock_file, sock_file);

    this->bind_sock_file = (char*)malloc(bind_file_len + 1);
    if (NULL == this->bind_sock_file) {
        return uds_ec_no_memory;
    }
    strcpy(this->bind_sock_file, bind_sock_file);

    this->call_back = callback;

    pthread_attr_t thread_attr;
    memset(&thread_attr, 0, sizeof(thread_attr));

    int ret = pthread_attr_init(&thread_attr);
    if (0 != ret) {
        return uds_ec_system_error;
    }

    ret = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (0 != ret) {
        return uds_ec_system_error;
    }

    __sync_add_and_fetch(&this->thread_count, 1);
    pthread_t thread_id;
    ret = pthread_create(&thread_id, &thread_attr, &UDSDatagramClientImpl::work_thread, this);
    if (0 != ret) {
        __sync_sub_and_fetch(&this->thread_count, 1);
        return uds_ec_system_error;
    }

    this->running = true;

    return 0;
}

void UDSDatagramClientImpl::close() {
    this->running = false;

    // waitting for work thread stop
    while (this->thread_count > 0) {
        usleep(100);
    }

    // clean resource
    if (-1 != this->sock) ::close(this->sock);
    if (NULL != this->server_sock_file) free(this->server_sock_file);
}

int UDSDatagramClientImpl::send_data(const char* data, int data_len) {
    if (NULL == data || 0 == data_len)
        return uds_ec_param_error;
    if (-1 == this->sock)
        return uds_ec_unopen;

    struct sockaddr_un peer_addr;
    memset(&peer_addr, 0, sizeof(struct sockaddr_un));
    peer_addr.sun_family = AF_UNIX;
    strcpy(peer_addr.sun_path, this->server_sock_file);

    return sendto(this->sock, data, data_len, 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr));
}

int UDSDatagramClientImpl::send_data(const std::string & data) {
    return this->send_data(data.data(), data.size());
}

void* UDSDatagramClientImpl::work_thread(void * arg) {
    UDSDatagramClientImpl * _this = (UDSDatagramClientImpl*)arg;
    if (NULL != _this) {
        _this->work_thread();
    }
    __sync_sub_and_fetch(&_this->thread_count, 1);
    return NULL;
}

void UDSDatagramClientImpl::work_thread() {
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
        if (NULL != this->call_back) {
            this->call_back->onError(errno);
        }
        return;
    }

    // bind addr
    struct sockaddr_un bind_addr;
    memset(&bind_addr, 0, sizeof(struct sockaddr_un));
    bind_addr.sun_family = AF_UNIX;
    strcpy(bind_addr.sun_path, this->bind_sock_file);

    // 删除文件，防止有残留数据
    unlink(this->bind_sock_file);
    int ret = bind(this->sock, (struct sockaddr*)&bind_addr, sizeof(bind_addr));
    if (-1 == ret) {
        if (NULL != this->call_back) {
            this->call_back->onError(errno);
        }
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
    int recv_byte_count = 0;

    while (this->running) {
        sock_len = sizeof(peer_addr);
        recv_byte_count = recvfrom(this->sock, recv_buf, MAX_DATA_LEN, 0, (struct sockaddr *)&peer_addr, &sock_len);
        if (-1 == recv_byte_count) {
            if ((errno == EAGAIN) || (errno == ENOBUFS) || (errno == EINTR)) {
                continue;     // timeout still work
            }
            if (NULL != this->call_back) {
                this->call_back->onError(errno); // error stop work
            }
            break;
        }

        if (NULL != this->call_back) {
            this->call_back->onRecv(peer_addr.sun_path, recv_buf, recv_byte_count);
        }
    }

    free(recv_buf);
}
