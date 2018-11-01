#ifndef __UDS_DATAGRAM_SERVER_H__
#define __UDS_DATAGRAM_SERVER_H__

#include "uds_datagram.h"

class UDSDatagramServerImpl : public UDSDatagramServer {
public:
    UDSDatagramServerImpl();
    virtual ~UDSDatagramServerImpl();

public:
    // max length of sock_file is 64 character
    int open(const char * sock_file, UDSDatagramCallback* callback);
    void close();

    int send_data(const char * peer_sock_file, const char* data, int data_len);
    int send_data(const char * peer_sock_file, const std::string & data);

private:
    static void* work_thread(void * arg);
    void work_thread();

private:
    char*                   sock_file;
    UDSDatagramCallback*    call_back;
    int                     sock;
    int                     thread_count;
    bool                    running;
};

#endif /* __UDS_DATAGRAM_SERVER_H__ */
