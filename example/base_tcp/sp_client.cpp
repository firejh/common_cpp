#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <netinet/tcp.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <errno.h>
 
#define MYPORT  19091
#define BUFFER_SIZE 1024
const char* SERVER_IP = "192.168.101.176";

static inline int pollFd(int fd, int events, int ms) {  
    pollfd fds[1];    
    fds[0].fd = fd;   
    fds[0].events = events;   
    fds[0].revents = 0;   
    int ret = ::poll(fds, 1, ms);    
    if (ret > 0) {    
        return fds[0].revents;    
    } 
    return ret;  
}   

static inline int checkAliveness(int fd) { 
    char buf[1];  
    int ret;  
    ret = pollFd(fd, POLLIN | POLLPRI, 0);   
    std::cout << "pollFd ret = " << ret << "\n";
    if (0 < ret) {    
        int num = ::recv(fd, buf, 1, MSG_PEEK); 
        if (num == 0) { 
            return -1;    
        }   
        if (num == -1) {    
            int errnum = errno;   
            if (errnum != EINTR && errnum != EAGAIN && errnum != EWOULDBLOCK) {   
                return -1;  
            } 
        }   
    } 
    return 0;    
}

int main()
{
    ///定义sockfd
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    
    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  ///服务器ip

    //设置超时
    struct timeval time_out;
    time_out.tv_sec = 5;
    time_out.tv_usec = 0;
    int ret = setsockopt(sock_cli, SOL_SOCKET, SO_SNDTIMEO, &time_out, sizeof(time_out));
    if (ret < 0) {
        perror("setsockopt SO_SENDTIMEO");
        exit(1);
    }
    //设置缓冲区
    int send_buf = 256;
    ret = setsockopt(sock_cli, SOL_SOCKET, SO_SNDBUF, (const char*)&send_buf, sizeof(int));
    if (ret < 0) {
        perror("setsockopt SO_SNDBUF");
        exit(1);
    }
    //keepalive
    /*
    int keep_alive = 1;
    int keepidle = 5;
    int keepinterval = 5;
    int keepcount = 3;
    ret = setsockopt(sock_cli, SOL_SOCKET, SO_KEEPALIVE, (void*)&keep_alive, sizeof(keep_alive));
    if (ret < 0) {
        exit(1);
    }
    setsockopt(sock_cli, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle, sizeof(keepidle));
    setsockopt(sock_cli, SOL_TCP, TCP_KEEPINTVL, (void*)&keepinterval, sizeof(keepinterval));
    setsockopt(sock_cli, SOL_TCP, TCP_KEEPCNT, (void*)&keepcount, sizeof(keepcount));
*/
    printf("连接%s:%d\n",SERVER_IP,MYPORT);
    ///连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    //非阻塞
    int liResult = fcntl(sock_cli, F_GETFL);
    liResult = liResult|O_NONBLOCK;
    if (fcntl(sock_cli, F_SETFL, liResult) < 0) {
        perror("noblock");
        exit(1);
    }

    //epoll  ctrl
    struct epoll_event ev, events[20];
    int epfd;
    epfd = epoll_create(256);
    ev.data.fd = sock_cli;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
    //ev.events = EPOLLIN;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sock_cli, &ev);//注册可读
    std::cout << "epoll_ctl ret " << ret << std::endl;

    printf("服务器连接成功\n");
    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];
    
    //等待断网
    fgets(sendbuf, sizeof(sendbuf), stdin);
    send(sock_cli, sendbuf, strlen(sendbuf), 0); //发送
    while (1) {//发送数据后看所有的读时间
        int nfds = epoll_wait(epfd, events, 20, 500);
        for (int i=0; i < nfds; i++) {
            std::cout << "event " << events[i].events << std::endl;
            if (events[i].events & EPOLLIN) {
                ret = recv(sock_cli, recvbuf, sizeof(recvbuf),0); ///接收
                std::cout << "recv ret = " << ret << ", " << recvbuf << "\n";
            }
        }
        sleep(1);
        int nu = checkAliveness(sock_cli);
        std::cout << "checkAliveness ret = " << nu << "\n";
        std::cout << "event loop\n";
    }

    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
        //获取发送缓冲区已经占用大小
        int send_buf_size = 0;
        ioctl(sock_cli, SIOCOUTQ, &send_buf_size);
        printf("向服务器发送数据：%s\n, 当前缓冲区使用 %d",sendbuf, send_buf_size);
        int ret = send(sock_cli, sendbuf, strlen(sendbuf),0); ///发送
        std::cout << "send ret = " << ret << std::endl;
        if(strcmp(sendbuf,"exit\n")==0)
            break;
        if (strcmp(sendbuf,"loop\n")==0) {
            while (1) {
                int ret = send(sock_cli, sendbuf, strlen(sendbuf),0); ///发送
                char str[1024];
                if (ret < 0) {
                    std::cout << "send ret = " << ret << std::endl;
                }
            }
        }

        sleep(1);
        ret = recv(sock_cli, recvbuf, sizeof(recvbuf),0); ///接收
        std::cout << "recv ret = " << ret << std::endl;
        //printf("从服务器接收数据：%s\n",recvbuf);
        
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock_cli);
    return 0;
}
