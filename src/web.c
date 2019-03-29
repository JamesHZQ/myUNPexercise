#include "web.h"
int main(int argc,char **argv){

    int i,fd, n,maxnconn,flags,error;
    char buf[MAXLINE];
    fd_set rs,ws;
    if(argc<5)
        err_quit("usage: web <#conns> <hostname> <homepage> <file1>...");
    maxnconn = atoi(argv[1]);                      //确定最大连接数
    nfiles = min(argc-4,MAXFILES);                 //确定有所少文件需要读
    for(i=0;i<nfiles;i++){
        file[i].f_name = argv[i+4];                //提取文件名
        file[i].f_host = argv[2];                  //提取文件所在域名或IP地址
        file[i].f_flags = 0;                       //初始化文件标志
    }
    printf("nfiles = %d\n",nfiles);
    home_page(argv[2],argv[3]);                    //
    FD_ZERO(&rset);                                //初始化读描述符集
    FD_ZERO(&wset);                                //初始化写描述符集
    maxfd = -1;                                    //最大描述符设为-1
    nlefttoread = nlefttoconn = nfiles;            //待读取文件数，待连接文件数初始化为总文件数
    nconn = 0;                                     //打开连接数初始化为0
    while(nlefttoread>0){                          //如果还有文件未读
        while(nconn<maxnconn&&nlefttoconn>0){      //如果打开连接数小于最大连接数并且还有文件等待连接
            for(i=0;i<nfiles;i++)                  //遍历所有文件
                if(file[i].f_flags==0)             //发现未处理文件，跳出循环
                    break;
            if(i==nfiles)
                err_quit("nlefttoconn = %d but nothing found",nlefttoconn);
            start_connect(&file[i]);               //使用非阻塞connect连接，若未连接成功，将文件套接字加入读/写描述符集
            nconn++;                               //已占用连接数+1
            nlefttoconn--;                         //文件待连接数-1
        }
        rs=rset;
        ws = wset;
        n=Select(maxfd+1,&rs,&ws,NULL,NULL);       //select等待其中某些描述符可读或可写
        for(i=0;i<nfiles;i++){                     //遍历所有文件
            flags=file[i].f_flags;
            if(flags==0||flags&F_DONE)             //文件未建立连接或已完成下载任务就跳过,选择状态为CONNECTING或READING的文件
                continue;
            fd=file[i].f_fd;                       //提取文件套接字描述符
            if(flags & F_CONNECTING && (FD_ISSET(fd,&rs)||FD_ISSET(fd,&ws))){           //如果文件处于连接中状态，且文件套接字可读或可写
                n= sizeof(error);
                if(getsockopt(fd,SOL_SOCKET,SO_ERROR,&error,&n)<0||error!=0){           //连接是否成功？
                    err_ret("nonbloking connect failed for %s",file[i].f_name);
                }
                printf("connection established for %s\n",file[i].f_name);               //连接建立成功
                FD_CLR(fd,&wset);
                write_get_cmd(&file[i]);                                                //发送GET请求
            }else if(flags&F_READING&&FD_ISSET(fd,&rs)){                                //如果文件处于读取数据状态，且其套接字可读
                if((n=Read(fd,buf,sizeof(buf)))==0){                                    //如果文件已经读完数据关闭套接字并设其状态为F_DONE
                printf("end-of-file on %s\n",file[i].f_name);
                Close(fd);
                file[i].f_flags = F_DONE;
                FD_CLR(fd,&rset);
                nconn--;                                                                //已占用连接数-1
                nlefttoread--;                                                          //文件待连接数+1
            } else
                printf("read %d bytes from %s\n",n,file[i].f_name);
            }
        }
    }
    exit(0);
}