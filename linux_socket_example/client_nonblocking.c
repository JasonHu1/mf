
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <errno.h>

int client_blocking_init(void*arg){

    unsigned char rbuf[1024]={0};
    int fd = 0;
    struct sockaddr_in  addr;
    fd_set fdr, fdw;
    struct timeval timeout;
    int err = 0;
    int errlen = sizeof(err);
    fd = socket(AF_INET,SOCK_STREAM,0);
    if (fd < 0) {
        fprintf(stderr, "create socket failed,error:%s.\n", strerror(errno));
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(777);
    inet_pton(AF_INET, "192.168.1.16", &addr.sin_addr);
    

    /*阻塞情况下linux系统默认超时时间为75s
     connect返回值：0表示连接成功，-1表示连接失败
    */
    int rc = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (rc != 0) {
        fprintf(stderr, "connect failed, error:%s.\n", strerror(errno));
        return -1;
    }
    printf("connect successfully\r\n");
    for(;;){
       memset(rbuff,0,1024);
       /*
        recv返回值<0 出错,     =0 对方调用了close API来关闭连接,       >0 接收到的数据大小
        阻塞模式下recv会一直阻塞直到接收到数据，非阻塞模式下如果没有数据就会返回，不会阻塞着读，因此需要循环读取
       */
       int readSize = recv(fd,rbuff,1024,0);
       printf("readSize=%d\r\n",readSize);
       if(readSize == 0){
           printf("peer close the socket\r\n");
           close(fd);
       }else if(readSize < 0){
           perror("read data error,");
       }else{
           for(int i=0;i<readSize;i++){
               printf("%02x ",rbuff[i]);
           }
           printf("\r\n");
       }
    }
    return 0;
}

int client_nonblock_init(void*arg){

    unsigned char rbuf[1024]={0};
    int fd = 0;
    struct sockaddr_in  addr;
    fd_set fdr, fdw;
    struct timeval timeout;
    int err = 0;
    int errlen = sizeof(err);
    fd = socket(AF_INET,SOCK_STREAM,0);
    if (fd < 0) {
        fprintf(stderr, "create socket failed,error:%s.\n", strerror(errno));
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(777);
    inet_pton(AF_INET, "192.168.1.16", &addr.sin_addr);
    /*设置套接字为非阻塞*/
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        fprintf(stderr, "Get flags error:%s\n", strerror(errno));
        close(fd);
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
        fprintf(stderr, "Set flags error:%s\n", strerror(errno));
        close(fd);
        return -1;
    }

    /*阻塞情况下linux系统默认超时时间为75s*/
    int rc = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (rc != 0) {
        if (errno == EINPROGRESS) {
            printf("Doing connection.\n");
            /*正在处理连接*/
            FD_ZERO(&fdr);
            FD_ZERO(&fdw);
            FD_SET(fd, &fdr);
            FD_SET(fd, &fdw);
            //select阻塞监听10s，若超时select返回0，有事件发生select返回大于0，出错select返回小于0
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
            rc = select(fd + 1, &fdr, &fdw, NULL, &timeout);
            printf("rc is: %d\n", rc);
            /*select调用失败*/
            if (rc < 0) {
                fprintf(stderr, "connect error:%s\n", strerror(errno));
                close(fd);
                return -1;
            }

            /*连接超时*/
            if (rc == 0) {
                fprintf(stderr, "Connect timeout.\n");
                close(fd);
                return -1;
            }

            /*[1] 当连接成功建立时，描述符变成可写,rc=1*/
            if (rc == 1 && FD_ISSET(fd, &fdw)) {
                printf("Connect success\n");
               goto _connect_success;
            }

            /*[2] 当连接建立遇到错误时，描述符变为即可读，也可写，rc=2 遇到这种情况，可调用getsockopt函数*/
            if (rc == 2) {
                if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
                    fprintf(stderr, "getsockopt(SO_ERROR): %s", strerror(errno));
                    close(fd);
                    return -1;
                }

                if (err) {
                    errno = err;
                    fprintf(stderr, "connect error:%s\n", strerror(errno));
                    close(fd);
                    return -1;
                }
            }
            printf("111");
        }

        fprintf(stderr, "connect failed, error:%s.\n", strerror(errno));
        return -1;
    }
_connect_success:
    for(;;){
        /*正在处理连接*/
        FD_ZERO(&fdr);
        FD_SET(fd, &fdr);
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        rc = select(fd + 1, &fdr, NULL, NULL, &timeout);
        printf("rc is: %d\n", rc);
        /*select调用失败*/
        if (rc < 0) {
            fprintf(stderr, "connect error:%s\n", strerror(errno));
            close(fd);
            exit(-1);
        }

        /*[1] 当连接成功建立时，描述符变成可写,rc=1*/
        if (rc == 1 && FD_ISSET(fd, &fdr)) {
           printf("read success\n");
           memset(rbuff,0,1024);
           int readSize = recv(fd,rbuff,1024,0);
           printf("readSize=%d\r\n",readSize);
           for(int i=0;i<readSize;i++){
               printf("%02x ",rbuff[i]);
           }
           printf("\r\n");
        }
   }

    return 0;
}


int main(void){
    client_nonblock_init(NULL);
}