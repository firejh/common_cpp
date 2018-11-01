#ifndef __UDS_DATAGRAM_H__
#define __UDS_DATAGRAM_H__

#include <string>

#define MAX_DATA_LEN					(1024*1024)         // 1M，可以调整
#define MAX_FILE_PATH_LEN				(256)				// 64

enum UDSErrorCode {
	uds_ec_base = 1000000,
	uds_ec_no_memory,
	uds_ec_system_error,
	uds_ec_unopen,
	uds_ec_param_error
};

class UDSDatagramCallback {
public:
    virtual ~UDSDatagramCallback() {}
	virtual void onRecv(const char * peer_sock_file, char* buf, int buf_len) = 0;
	virtual void onError(int error_code) = 0;
};

class UDSDatagramServer {
public:
    virtual ~UDSDatagramServer() {}
	// max length of sock_file is 64 character
	virtual int open(const char * sock_file, UDSDatagramCallback* callback) = 0;
	virtual void close() = 0;

	virtual int send_data(const char * peer_sock_file, const char* data, int data_len) = 0;
	virtual int send_data(const char * peer_sock_file, const std::string & data) = 0;
};

class UDSDatagramClient {
public:
    virtual ~UDSDatagramClient() {}
	// max length of sock_file is 64 character
	virtual int open(const char * bind_sock_file, const char * server_sock_file, UDSDatagramCallback* callback) = 0;
	virtual void close() = 0;

	virtual int send_data(const char* data, int data_len) = 0;
	virtual int send_data(const std::string & data) = 0;
};

UDSDatagramServer* createUDSDatagramServer();
void destroyUDSDatagramServer(UDSDatagramServer* udsds);

UDSDatagramClient* createUDSDatagramClient();
void destroyUDSDatagramClient(UDSDatagramClient* udsdc);

#endif /* __UDS_DATAGRAM_H__ */
