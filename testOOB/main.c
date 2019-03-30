#include "unp.h"
int main(int argc, char **argv){
    int sockfd;
    if(argc != 3)
        err_quit("usage:tcpsend <host> <port#>");
    sockfd = Tcp_connect(argv[1],argv[2]);

    Write(sockfd,"123",3);
    printf("wrote 3 bytes of normal data\n");
    //sleep(1);

    Send(sockfd,"4",1,MSG_OOB);
    printf("wrote 1 bytes of OOB data\n");
    //sleep(1);

    Write(sockfd,"5",1);
    printf("wrote 1 bytes of normal data\n");
    //sleep(1);

//    Send(sockfd,"7",1,MSG_OOB);
//    printf("wrote 2 bytes of OOB data\n");
//    sleep(1);
//
//    Write(sockfd,"89",2);
//    printf("wrote 2 bytes of normal data\n");
//    sleep(1);

    exit(0);
}
//int listenfd,connfd;
//void sig_urg(int);
//int main(int argc,char **argv){
//    int n;
//    char buff[100];
//    if(argc==2)
//        listenfd = Tcp_listen(NULL,argv[1],NULL);
//    else if(argc==3)
//        listenfd = Tcp_listen(argv[1],argv[2],NULL);
//    else
//        err_quit("usage:[<host>] <port#>");
//    connfd = Accept(listenfd,NULL,NULL);
//    Signal(SIGURG,sig_urg);
//    Fcntl(connfd,F_SETOWN,getpid());
//    for(;;){
//        if((n=Read(connfd,buff,sizeof(buff)-1))==0){
//            printf("recevied EOF\n");
//            exit(0);
//        }
//        buff[n] = 0;
//        printf("read %d bytes: %s\n",n,buff);
//    }
//}
//void sig_urg(int signo){
//    int n;
//    char buff[100];
//
//    printf("SIGURG recived\n");
//    n=Recv(connfd,buff,sizeof(buff)-1,MSG_OOB);
//    buff[n]=0;
//    printf("read %d OOB byte: %s\n",n,buff);
//}
