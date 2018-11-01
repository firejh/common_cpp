#ifndef __UDS_DATAGRAM_CLIENT_H__
#define __UDS_DATAGRAM_CLIENT_H__

#include "uds_datagram.h"

class UDSDatagramClientImpl : public UDSDatagramClient {
public:
    UDSDatagramClientImpl();
    virtual ~UDSDatagramClientImpl();

public:
    // max length of sock_file is 64 character
    int open(const char * bind_sock_file, const char * server_sock_file, UDSDatagramCallback* callback);
    void close();

    int send_data(const char* data, int data_len);
    int send_data(const std::string & data);

private:
    static void* work_thread(void * arg);
    void work_thread();

private:
    char*                   server_sock_file;
    char*                   bind_sock_file;
    UDSDatagramCallback*    call_back;
    int                     sock;
    int                     thread_count;
    bool                    running;
};

#endif /* __UDS_DATAGRAM_CLIENT_H__ */
