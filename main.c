#include "unp.h"
/****server begin****/
//void sig_chld(int signo){
//    pid_t pid;
//    int stat;
//    pid=wait(&stat);
//    printf("child %d teminated\n",pid);
//    return;
//}
/*
void
str_echo(int sockfd){
    char line[MAXLINE];
    FILE *fpin,*fpout;
    fpin = Fdopen(sockfd,"r");
    fpout = Fdopen(sockfd,"w");
    while(Fgets(line,MAXLINE,fpin) != NULL)
        Fputs(line,fpout);
} 

int main(int argc,char**argv){
    int i,maxi,maxfd,listenfd,connfd,sockfd;
    int nready,client[FD_SETSIZE];
    ssize_t n;
    fd_set rset,allset;
    char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr,servaddr;

    listenfd = Socket(AF_INET,SOCK_STREAM,0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd,(SA*) &servaddr, sizeof(servaddr));

    Listen(listenfd,LISTENQ);

    maxfd = listenfd;
    maxi = -1;
    for(i=0;i<FD_SETSIZE;i++)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);
    for(;;){
        rset = allset;
        nready = Select(maxfd+1,&rset,NULL,NULL,NULL);

        if(FD_ISSET(listenfd,&rset)){
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd,(SA*)&cliaddr,&clilen);
            for(i=0;i<FD_SETSIZE;i++)
                if(client[i]<0){
                    client[i] = connfd;
                    break;
                }
            if(i==FD_SETSIZE)
                err_quit("too many clients");
            FD_SET(connfd,&allset);
            if(connfd>maxfd)
                maxfd = connfd;
            if(i>maxi)
                maxi=i;
            if(--nready<=0)
                continue;
        }
        for(i=0;i<=maxi;i++){
            if((sockfd=client[i])<0)
                continue;
            if(FD_ISSET(sockfd,&rset)){
                if((n=Read(sockfd,buf,MAXLINE))==0){
                    Close(sockfd);
                    FD_CLR(sockfd,&allset);
                    client[i]=-1;
                } else
                    Writen(sockfd,buf,n);
		if(--nready<=0)
		    break;
            }
        }
    }

}
*/
/****server end*****/



/****client begin****/

void str_cli(FILE* fp,int sockfd){
    int maxfdp1,stdineof;
    fd_set rset;
    char buf[MAXLINE];
    int n;
    stdineof=0;
    FD_ZERO(&rset);
    for(;;){
        if(stdineof == 0)
            FD_SET(fileno(stdin),&rset);
        FD_SET(sockfd,&rset);
        maxfdp1 = max(fileno(fp),sockfd)+1;
        Select(maxfdp1,&rset,NULL,NULL,NULL);
        if(FD_ISSET(sockfd,&rset)){
            if((n=Read(sockfd,buf,MAXLINE))==0){
                if(stdineof == 1)
                    return;
                else
                    err_quit("str_cli server terminated premmaturely");
            }
            Write(fileno(stdout),buf,n);
        }
        if(FD_ISSET(fileno(fp),&rset)){
            if((n=Read(fileno(fp),buf,MAXLINE))==0){
                stdineof =1;
                Shutdown(sockfd,SHUT_WR);
                FD_CLR(fileno(fp),&rset);
                continue;
            }
            Writen(sockfd,buf,n);
        }

    }
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

/****client end*****/
