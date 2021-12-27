
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
// #include "socket_server.h"
// #include "Device.h"
// #include "tuya_iot_com_api.h"

#define MAX_ACCEPT 5
#define BUFFER_SIZE 1024
#define MAX_SIZE 500
#define BUFF_SIZE 1000
#define MAX_EVENTS 100
#define TY_MATTER_UDP_SERVER_PORT  12121
#define TY_MATTER_UDP_SEND_Z3_PORT 12122

int listen_socketFd,maxfd,udpSendfd;
int client_fd[MAX_ACCEPT+1];
struct sockaddr_in client_addr[MAX_ACCEPT+1];
char buffer[BUFFER_SIZE];


int getSocketFd(void){
    return listen_socketFd;
}
int sendData(unsigned char *data,unsigned int len){
    struct sockaddr_in serv_addr;

    printf("udp sendData\n");
    // New socket
    udpSendfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSendfd < 0) 
    {
       printf("ERROR opening socket\n");
       return -1;
    }
 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.1.16");
    serv_addr.sin_port = htons(TY_MATTER_UDP_SEND_Z3_PORT);

    //回显不用自己新开socket直接用udp server的就可以，下面用udpSendfd或者listen_socketFd都可以
    //sendto(udpSendfd,data,len,0,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
    sendto(listen_socketFd,data,len,0,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
    return 0;
}
int handleReceiveData(int srcFd,unsigned char *data,unsigned int len){
    printf("[");
    for(unsigned int i=0;i<len;i++){
        printf("%02x ",data[i]);
    }
    printf("]\r\n");
    sendData(data,len);
    if(data[0]==0xaa){
        // message_process(srcFd,data,len);
    }
    return 0;
}
int udp_blocking_sever_Init(void)
{
    struct sockaddr_in serv_addr;
    struct sockaddr_in src_addr ={};//udp获取对端ip地址
    socklen_t addr_len = sizeof(struct sockaddr_in);

    int stat,tr=0;

    printf("udp socketSeverInit\n");
    // New socket
    listen_socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listen_socketFd < 0) 
    {
       printf("ERROR opening socket\n");
       return -1;
    }
 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(TY_MATTER_UDP_SERVER_PORT);
    // Set the socket option SO_REUSEADDR to reduce the chance of a 
    // "Address Already in Use" error on the bind
    setsockopt(listen_socketFd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int));

    //udp需要绑定,不需要监听和connect
    stat = bind(listen_socketFd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if ( stat < 0) 
    {
      printf("ERROR on binding: %s\n", strerror( errno ) );
      exit(1);
    }

  	printf("init nonblock listen_socketFd=%d\n",listen_socketFd);
    for(int i = 0; i < MAX_ACCEPT; i ++){
        client_fd[i] = -1;
        memset(&client_addr[i], 0x0, sizeof(struct sockaddr_in));
    }

    for(;;)
    {
        //blocking receive data
        bzero(buffer, BUFFER_SIZE);
        ssize_t readlength = recvfrom(listen_socketFd, buffer,1024,0,(struct sockaddr *)&src_addr,&addr_len);
        printf("receive data:%d\r\n",(int)readlength);
        if (readlength == 0)
        {
            printf("Server Recieve Data Failed!\n");
        }else if(readlength < 0){
            perror("non-blocking return \r\n");
        }else{
            char ipAddress[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(src_addr.sin_addr.s_addr), ipAddress, INET_ADDRSTRLEN);
            printf("The Source IP address is: %s\n", ipAddress);

            handleReceiveData(listen_socketFd,(unsigned char*)buffer,(int)readlength);
        }
    }//for(;;)
    //关闭与客户端的连接
    printf("can not run here\r\n");
    close(listen_socketFd);
    return 0;
}
void main(void)
{
    udp_blocking_sever_Init();
    return NULL;
}
