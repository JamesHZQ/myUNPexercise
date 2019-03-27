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

char* gf_time(void){
    struct timeval tv;
    static char str[30];
    char *ptr;
    if(gettimeofday(&tv,NULL)<0)
        err_sys("gettimeofday error");
    ptr = ctime(&tv.tv_sec);
    strcpy(str,&ptr[11]);

    snprintf(str+8,sizeof(str)-8,".%06ld",tv,tv.tv_usec);

    return(str);
}
void str_cli(FILE *fp,int sockfd){
    int maxfdp1,val,stdineof;
    ssize_t n,nwritten;
    fd_set rset,wset;
    char to[MAXLINE],fr[MAXLINE];
    char *toiptr,*tooptr,*friptr,*froptr;
    val = Fcntl(sockfd,F_GETFL,0);
    Fcntl(sockfd, F_SETFL,val|O_NONBLOCK);
    val = Fcntl(STDIN_FILENO,F_GETFL,0);
    Fcntl(STDIN_FILENO,F_SETFL,val|O_NONBLOCK);
    val = Fcntl(STDOUT_FILENO,F_GETFL,0);
    Fcntl(STDOUT_FILENO,F_SETFL,val|O_NONBLOCK);

    toiptr = tooptr = to;
    friptr = froptr = fr;

    stdineof = 0;

    maxfdp1 = max(max(STDOUT_FILENO,STDIN_FILENO),sockfd)+1;
    for(;;){
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if(stdineof==0&&toiptr<&to[MAXLINE])
            FD_SET(STDIN_FILENO,&rset);
        if(friptr<&fr[MAXLINE])
            FD_SET(sockfd,&rset);
        if(tooptr!=toiptr)
            FD_SET(sockfd,&wset);
        if(froptr!=friptr)
            FD_SET(STDOUT_FILENO,&wset);
        Select(maxfdp1,&rset,&wset,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&rset)){
            if((n=read(STDIN_FILENO,toiptr,&to[MAXLINE]-toiptr))<0){
                if(errno!=EWOULDBLOCK)
                    err_sys("read error on stdin");
            }else if(n==0){
                fprintf(stderr,"%s:EOF on stdin\n",gf_time());
                stdineof = 1;
                if(tooptr == toiptr)
                    Shutdown(sockfd,SHUT_WR);
            }else{
                fprintf(stderr,"%s: read %d bytes from stdin\n",gf_time(),n);
                toiptr+=n;
		FD_SET(sockfd,&wset);
            }
        }
        if(FD_ISSET(sockfd,&rset)){
            if((n=read(sockfd,friptr,&fr[MAXLINE]-friptr))<0){
                if(errno!=EWOULDBLOCK)
                    err_sys("read error on socket");
            }else if(n==0){
                fprintf(stderr,"%s: EOF on socket\n",gf_time());
                if(stdineof)
                    return ;
                else
                    err_quit("str_cli:server terminated prematurely");
            }else{
                fprintf(stderr, "%s:read %d bytes from socket\n",gf_time(),n);
                friptr += n;
                FD_SET(STDOUT_FILENO,&wset);
            }
        }
        if(FD_ISSET(STDOUT_FILENO,&wset)&&((n=friptr-froptr)>0)){
            if((nwritten = write(STDOUT_FILENO,froptr,n))<0){
                if(errno!=EWOULDBLOCK)
                    err_sys("write error to stdout");
            }else{
                fprintf(stderr,"%s:wrote %d bytes to stdout\n",gf_time(),nwritten);
                froptr += nwritten;
                if(froptr == friptr)
                    froptr = friptr = fr;
            }
        }

        if(FD_ISSET(sockfd,&wset)&&((n=toiptr-tooptr)>0)){
            if((nwritten = write(sockfd,tooptr,n))<0){
                if(errno!=EWOULDBLOCK)
                    err_sys("write error to socket");
            }else{
                fprintf(stderr,"%s:wrote %d bytes to socket\n",gf_time(),nwritten);
                tooptr += nwritten;
                if(tooptr==toiptr){
                    toiptr = tooptr = to;
                    if(stdineof)
                        Shutdown(sockfd,SHUT_WR);
                }
            }
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
