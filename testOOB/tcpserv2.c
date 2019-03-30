#include "unp.h"
//
// Created by john on 3/30/19.
//
int sockatmark(int fd){
    int flag;
    if(ioctl(fd,SIOCATMARK,&flag)<0)
        return -1;
    return (flag!=0);
}
int main(int argc,char **argv){
    int listenfd, connfd, n,on=1;
    char buff[100];
    if(argc==2)
        listenfd = Tcp_listen(NULL,argv[1],NULL);
    else if(argc==3)
        listenfd = Tcp_listen(argv[1],argv[2],NULL);
    else
        err_quit("usage: tcprecv02 [<host>] <port#>");
    Setsockopt(listenfd,SOL_SOCKET,SO_OOBINLINE,&on,sizeof(on));
    connfd = Accept(listenfd,NULL,NULL);
    sleep(5);
    for(;;){
        if(sockatmark(connfd))
            printf("at OOB mark\n");
        if((n=Read(connfd,buff,sizeof(buff)-1))==0){
            printf("recevied EOF\n");
            exit(0);
        }
        buff[n]=0;
        printf("read %d bytes: %s\n", n, buff);
    }
}
