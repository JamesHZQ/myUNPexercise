#include "unpthread.h"
/*client begin*/
/*void *copyto(void *);
static int sockfd;
static FILE*fp;

void str_cli(FILE*fp_arg,int sockfd_arg){
    char recvline[MAXLINE];
    pthread_t tid;
    sockfd = sockfd_arg;
    fp=fp_arg;

    Pthread_create(&tid,NULL,copyto,NULL);
    while(Readline(sockfd,recvline,MAXLINE)>0)
        Fputs(recvline,stdout);
}
void *
copyto(void *arg){
    char sendline[MAXLINE];
    while(Fgets(sendline,MAXLINE,fp)!=NULL)
        Writen(sockfd,sendline,strlen(sendline));
    Shutdown(sockfd,SHUT_WR);

    return NULL;
}

int main(int argc,char **argv){
    int sockfd;
    struct sockaddr_in servaddr;
    if(argc!=2)
        err_quit("usage:tcpcli<IPaddress>");
    sockfd=Socket(AF_INET,SOCK_STREAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    Connect(sockfd,(SA*)&servaddr, sizeof(servaddr));
    str_cli(stdin,sockfd);
    exit(0);
}
*/
/*client end*/
/*server begin*/
void str_echo(int sockfd){
    ssize_t n;
    char buf[MAXLINE];
    again:
    while((n=read(sockfd,buf,MAXLINE))>0)
        Writen(sockfd,buf,n);
    if(n<0&&errno==EINTR)
        goto again;
    else if (n<0)
        err_sys("str_echo:read error");
}

static void *doit(void *);
int main(int argc,char**argv){
    int listenfd,connfd;
    pthread_t tid;
    socklen_t addrlen,len;
    struct sockaddr *cliaddr;

    if(argc==2)
        listenfd = Tcp_listen(NULL,argv[1],&addrlen);
    else if(argc==3)
        listenfd = Tcp_listen(argv[1],argv[2],&addrlen);
    else
        err_quit("usage: proc [<host>] <port#>");

    cliaddr = Malloc(addrlen);

    for(;;){
        len=addrlen;
        connfd = Accept(listenfd,cliaddr,&len);
        Pthread_create(&tid,NULL,&doit,(void*)connfd);
    }
}
static void *doit(void *arg){
    Pthread_detach(pthread_self());
    str_echo((int)arg);
    Close((int) arg);
    return (NULL);
}

/*server end*/
