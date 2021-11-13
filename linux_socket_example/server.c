
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

#define MAX_ACCEPT 5
#define BUFFER_SIZE 1024
int listen_socketFd,maxfd;
int client_fd[MAX_ACCEPT+1];
struct sockaddr_in client_addr[MAX_ACCEPT+1];
char buffer[BUFFER_SIZE];

int getSocketFd(void){
    return listen_socketFd;
}
int handleReceiveData(unsigned char *data,unsigned int len){
    printf("[");
    for(unsigned int i=0;i<len;i++){
        printf("%02x ",data[i]);
    }
    printf("]\r\n");
    return 0;
}

int socketSever_Init_select(void)
{
    struct sockaddr_in serv_addr;
    int stat,tr;

    printf("socketSeverInit\n");
    // New socket
    listen_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socketFd < 0) 
    {
       printf("ERROR opening socket\n");
       return -1;
    }
 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(778);

    stat = bind(listen_socketFd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if ( stat < 0) 
    {
      printf("ERROR on binding: %s\n", strerror( errno ) );
      exit(1);
    }
    //will have 5 pending open client requests
    listen(listen_socketFd,5); 
    // Set the fd to none blocking
    fcntl(listen_socketFd, F_SETFL, O_NONBLOCK);
    // Set the socket option SO_REUSEADDR to reduce the chance of a 
    // "Address Already in Use" error on the bind
    setsockopt(listen_socketFd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int));

  	printf("init nonblock listen_socketFd=%d\n",listen_socketFd);
    for(int i = 0; i < MAX_ACCEPT; i ++){
        client_fd[i] = -1;
        memset(&client_addr[i], 0x0, sizeof(struct sockaddr_in));
    }
    for(;;)
    {
        //Set select option
        fd_set fdr;
        struct timeval timeout;
        FD_ZERO(&fdr);

        //把监听fd放到数组，后面好遍历
        client_fd[0] = listen_socketFd;
        //添加客户端fd
        for(int i = 0; i < MAX_ACCEPT; i ++){
            printf("client_fd[%d]=%d\r\n",i,client_fd[i]);
            if(client_fd[i]!=-1){
                FD_SET(client_fd[i], &fdr);
                if(client_fd[i] > maxfd){
                    maxfd = client_fd[i];
                }
            }
        }
        //select阻塞监听10s，若超时select返回0，有事件发生select返回大于0，出错select返回小于0
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        int rc = select(maxfd + 1, &fdr, NULL, NULL, &timeout);
        printf("rc is: %d\n", rc);
        /*select调用失败*/
        if (rc < 0) {
            fprintf(stderr, "listening error:%s\n", strerror(errno));
            close(listen_socketFd);
            exit(1);
        }
        /*连接超时*/
        if (rc == 0) {
            /*can do some things*/
        }
        /*rc > 0 有数据，select需要自己在fdr中找到有事件的fd，epoll/poll会直接给出有事件的fd*/
        for(int m=0;m < MAX_ACCEPT+1;m++){
            if(FD_ISSET(client_fd[m], &fdr)){
                if(client_fd[m] == listen_socketFd){
                    printf("accept new client\r\n");
                    //接受一个到server_socket代表的socket的一个连接
                    //如果没有连接请求,就等待到有连接请求--这是accept函数的特性
                    // accept函数返回一个新的socket,这个socket(new_server_socket)用于同连接到的客户的通信
                    // new_server_socket代表了服务器和客户端之间的一个通信通道
                    // accept函数把连接到的客户端信息填写到客户端的socket地址结构client_addr中
                    socklen_t length = sizeof (client_addr);

                    int new_server_socket = accept(listen_socketFd, (struct sockaddr *) &client_addr[m], &length);
                    if (new_server_socket != 0){
                        printf("new_server_socket=%d\r\n",new_server_socket);
                        fcntl(new_server_socket, F_SETFL, O_NONBLOCK);
                        for(int n=0;n<MAX_ACCEPT+1;n++){
                            if(client_fd[n] == -1){
                                client_fd[n] = new_server_socket;
                                break;
                            }
                        }
                    }
                }else{
                    printf("receive data\r\n");
                    //receive data
                    bzero(buffer, BUFFER_SIZE);
                    int readlength = (int)read(client_fd[m], buffer,1024);
                    if (readlength < 0)
                    {
                        printf("Server Recieve Data Failed!\n");
                        //关闭listen socket
                        for(int n=0;n<MAX_ACCEPT+1;n++){
                            if(client_fd[n]== client_fd[m]){
                                client_fd[n] = -1;
                                break;
                            }
                        }
                        close(client_fd[m]);
                    }else{
                        handleReceiveData((unsigned char*)buffer,readlength);
                    }
                }
            }
        }
    }//for(;;)
    //关闭与客户端的连接
    close(listen_socketFd);

    return 0;
}

int socketSever_Init_poll(void)
{
    struct sockaddr_in serv_addr;
    int stat,tr;

    printf("socketSeverInit\n");
    // New socket
    listen_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socketFd < 0) 
    {
       printf("ERROR opening socket\n");
       return -1;
    }
 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	  serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(778);

    stat = bind(listen_socketFd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if ( stat < 0) 
    {
      printf("ERROR on binding: %s\n", strerror( errno ) );
      exit(1);
    }
    //will have 5 pending open client requests
    listen(listen_socketFd,5); 
    // Set the fd to none blocking
    fcntl(listen_socketFd, F_SETFL, O_NONBLOCK);
    // Set the socket option SO_REUSEADDR to reduce the chance of a 
    // "Address Already in Use" error on the bind
    setsockopt(listen_socketFd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int));

  	printf("init nonblock listen_socketFd=%d\n",listen_socketFd);
    for(int i = 0; i < MAX_ACCEPT; i ++){
        client_fd[i] = -1;
        memset(&client_addr[i], 0x0, sizeof(struct sockaddr_in));
    }
    for(;;)
    {
        //epoll init
        struct pollfd *pollFds = (struct pollfd *)malloc(  ((MAX_ACCEPT + 1) * sizeof( struct pollfd )) );
        if(pollFds){
            //把监听fd放到数组，后面好遍历
            client_fd[0] = listen_socketFd;
            //add fd to poll list
            for(int i = 0; i < MAX_ACCEPT+1; i ++){
                //printf("client_fd[%d]=%d\r\n",i,client_fd[i]);
                if(client_fd[i]!=-1){
                    pollFds[i].fd = client_fd[i];
                    pollFds[i].events = POLLIN |POLLHUP|POLLNVAL|POLLERR|POLLRDHUP;
                }
            }
            //poll阻塞监听是ms级别的，10s，若超时poll返回0，有事件发生poll返回大于0
            int rc = poll(pollFds, (MAX_ACCEPT+1), 1000);
            /*监听超时返回*/
            if (rc == 0) {
                /*can do some things*/
            }else{
                printf("poll rc is: %d\n", rc);
                /*rc > 0 有数据，这里select/poll都一样，需要自己在监听list中找到有事件的fd*/
                for(int Idx=0; Idx < (MAX_ACCEPT+1); Idx++)
                {
                    if(pollFds[Idx].revents) 
                    {
                        /**/
                        if(pollFds[Idx].fd == listen_socketFd){
                            printf("accept new client\r\n");
                            //接受一个到server_socket代表的socket的一个连接
                            //如果没有连接请求,就等待到有连接请求--这是accept函数的特性
                            // accept函数返回一个新的socket,这个socket(new_server_socket)用于同连接到的客户的通信
                            // new_server_socket代表了服务器和客户端之间的一个通信通道
                            // accept函数把连接到的客户端信息填写到客户端的socket地址结构client_addr中
                            socklen_t length = sizeof (client_addr);

                            int new_server_socket = accept(listen_socketFd, (struct sockaddr *) &client_addr[Idx], &length);
                            if (new_server_socket != 0){
                                printf("new_server_socket=%d\r\n",new_server_socket);
                                fcntl(new_server_socket, F_SETFL, O_NONBLOCK);
                                for(int n=0;n<MAX_ACCEPT+1;n++){
                                    if(client_fd[n] == -1){
                                        client_fd[n] = new_server_socket;
                                                break;
                                    }
                                }
                            }
                        }else{
                            printf("receive data\r\n");
                            //receive data
                            bzero(buffer, BUFFER_SIZE);
                            int readlength = (int)read(client_fd[Idx], buffer,1024);
                            if (readlength < 0)
                            {
                                printf("Server Recieve Data Failed!\n");
                                //关闭listen socket
                                for(int n=0;n<MAX_ACCEPT+1;n++){
                                    if(client_fd[n]== client_fd[Idx]){
                                        client_fd[n] = -1;
                                        break;
                                    }
                                }
                                close(client_fd[Idx]);
                            }else{
                                handleReceiveData((unsigned char*)buffer,readlength);
                            }
                        }
                    }
                }
            }
            free(pollFds);
        }
    }//for(;;)
    //关闭与客户端的连接
    close(listen_socketFd);
    return 0;
}

int socketSeverInit_epoll(void)
{
    struct sockaddr_in serv_addr;
    int stat,tr=0;

    printf("socketSeverInit\n");
    // New socket
    listen_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socketFd < 0) 
    {
       printf("ERROR opening socket\n");
       return -1;
    }
 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(778);

    stat = bind(listen_socketFd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if ( stat < 0) 
    {
      printf("ERROR on binding: %s\n", strerror( errno ) );
      exit(1);
    }
    //will have 5 pending open client requests
    listen(listen_socketFd,5); 
    // Set the fd to none blocking
    fcntl(listen_socketFd, F_SETFL, O_NONBLOCK);
    // Set the socket option SO_REUSEADDR to reduce the chance of a 
    // "Address Already in Use" error on the bind
    setsockopt(listen_socketFd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int));

  	printf("init nonblock listen_socketFd=%d\n",listen_socketFd);
    for(int i = 0; i < MAX_ACCEPT; i ++){
        client_fd[i] = -1;
        memset(&client_addr[i], 0x0, sizeof(struct sockaddr_in));
    }
    //epoll init
    struct epoll_event ev,events[100];
	int epollFd;

	epollFd=epoll_create(MAX_ACCEPT+1);
	if(epollFd==-1)
	{
		perror("epoll_create");
		return false;
	}
    client_fd[0] = listen_socketFd;
    //set option to epoll event
    //epoll的监控列表不可以重复添加，所以不可以放到for(;;)循环里面
    ev.events=EPOLLIN|EPOLLRDHUP;
    ev.data.fd=listen_socketFd;
    if(epoll_ctl(epollFd,EPOLL_CTL_ADD,listen_socketFd,&ev)==-1)
    {
        perror("epoll_ctl:ServerFd");
        exit(1);
    }
    for(;;)
    {
        //epoll_wait阻塞监听是ms级别的，timeout=-1表示一直阻塞到有event到来，
        //返回值：0表示超时返回，-1表示发生错误返回，>0 返回发生event的fd的个数
        //int epoll_wait(int epfd,struct epoll_event * events,int maxevents,int timeout)
        int rc =epoll_wait(epollFd,events,MAX_EVENTS,10);
        /*监听超时返回*/
        if (rc == 0) {
            /*can do some things*/
        }else if(rc == -1){
            perror("epoll_wait");
            close(listen_socketFd);
            exit(1);
        }else{
            printf("epoll rc is: %d\n", rc);
            /*rc > 0 有数据，这里select/poll/epoll都一样，需要自己在监听list中找到有事件的fd
            不同的是，epoll在events只返回发生事件的fd。
            */
            for(int Idx=0; Idx < rc; Idx++)
            {
                /**/
                if(events[Idx].data.fd == listen_socketFd){
                    printf("accept new client\r\n");
                    //接受一个到server_socket代表的socket的一个连接
                    //如果没有连接请求,就等待到有连接请求--这是accept函数的特性
                    // accept函数返回一个新的socket,这个socket(new_server_socket)用于同连接到的客户的通信
                    // new_server_socket代表了服务器和客户端之间的一个通信通道
                    // accept函数把连接到的客户端信息填写到客户端的socket地址结构client_addr中
                    socklen_t length = sizeof (client_addr);

                    int new_server_socket = accept(listen_socketFd, (struct sockaddr *) &client_addr[Idx], &length);
                    if (new_server_socket != 0){
                        printf("new_server_socket=%d\r\n",new_server_socket);
                        fcntl(new_server_socket, F_SETFL, O_NONBLOCK);
                        //epoll的监控列表不可以重复添加,来一个新的就添加一个，所以不可以放到for(;;)循环里面
                        ev.events=EPOLLIN|EPOLLRDHUP;
                        ev.data.fd=new_server_socket;
                        if(epoll_ctl(epollFd,EPOLL_CTL_ADD,new_server_socket,&ev)==-1)
                        {
                            perror("epoll_ctl:ServerFd");
                            exit(1);
                        }
                        for(int n=0;n<MAX_ACCEPT+1;n++){
                            if(client_fd[n] == -1){
                                client_fd[n] = new_server_socket;
                                break;
                            }
                        }
                    }
                }else{
                    printf("receive data\r\n");
                    //receive data
                    bzero(buffer, BUFFER_SIZE);
                    int readlength = (int)read(events[Idx].data.fd, buffer,1024);
                    if (readlength < 0)
                    {
                        printf("Server Recieve Data Failed!\n");
                        //关闭listen socket
                        for(int n=0;n<MAX_ACCEPT+1;n++){
                            if(client_fd[n]== events[Idx].data.fd){
                                client_fd[n] = -1;
                                break;
                            }
                        }
                        /*删除要关闭的fd*/
                        ev.events=EPOLLIN|EPOLLRDHUP;
                        ev.data.fd=events[Idx].data.fd;
                        if(epoll_ctl(epollFd,EPOLL_CTL_DEL,events[Idx].data.fd,&ev)==-1)
                        {
                            perror("epoll_ctl:ServerFd");
                            exit(1);
                        }
                        close(events[Idx].data.fd);
                    }else{
                        handleReceiveData((unsigned char*)buffer,readlength);
                    }
                }
            }
        }
    }//for(;;)
    //关闭与客户端的连接
    close(listen_socketFd);
    return 0;
}
void* main(void*arg)
{
    socketSever_Init_select();
    return NULL;
}
