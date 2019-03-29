#include "web.h"
void start_connect(struct file *fptr){
    int fd, flags, n;
    struct addrinfo *ai;
    ai=Host_serv(fptr->f_host,SERV, 0, SOCK_STREAM);
    fd = Socket(ai->ai_family,ai->ai_socktype,ai->ai_protocol); //得到socket套接字
    fptr->f_fd = fd;                                            //设置fptr的f_fd
    printf("star_connect for %s ,fd %d\n",fptr->f_name,fd);
    flags = Fcntl(fd,F_GETFL,0);                                //将socket描述符设为非阻塞
    Fcntl(fd,F_SETFL,flags|O_NONBLOCK);
    if((n=connect(fd,ai->ai_addr,ai->ai_addrlen))<0){           //发起建立非阻塞connect连接
        if(errno != EWOULDBLOCK)
            err_sys("nonbloking connect error");
        fptr->f_flags = F_CONNECTING;                           //如不能马上建立连接，connect返回EW。。。的错误，将fptr的状态设为建立连接中
        FD_SET(fd,&rset);                                       //将套接字fd添加到读/写描述符集
        FD_SET(fd,&wset);
        if(fd>maxfd)                                            //如果套接字fd大于当前最大描述符，将最大描述符更新为fd
            maxfd = fd;
    }else if(n>=0)                                              //如果立即建立连接
        write_get_cmd(fptr);                                    //发送GET命令到服务器
}