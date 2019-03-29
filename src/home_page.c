#include "web.h"
void home_page(const char *host,const char *fname){
    int fd,n;
    char line[MAXLINE];
    fd=Tcp_connect(host,SERV);                              //建立与服务器的连接
    n=snprintf(line,sizeof(line),GET_CMD,fname);            //设置line为GET请求（下载文件fname）
    Writen(fd,line,n);                                      //将line写入套接字描述符，给服务器发送GET请求
    for(;;){                                                /*begin读取服务器应答，不做反应*/
        if((n=Read(fd,line,MAXLINE))==0)
            break;
        printf("read %d bytes of home page \n",n);
    }
    printf("end-of-file on home page\n");                   /*end*/
    Close(fd);                                              //关闭连接
}