#include "unp.h"
#define MAXFILES    20
#define SERV    "80"
struct file{
    char *f_name;
    char *f_host;
    int   f_fd;
    int   f_flags;
}file[MAXLINE];
#define F_CONNECTING 1    //状态：建立连接中
#define F_READING    2    //状态：连接已建立，开始读数据
#define F_DONE       4    //状态：已完成

#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"

int nconn,nfiles,nlefttoconn,nlefttoread,maxfd;
fd_set rset,wset;

void home_page(const char *,const char *);
void start_connect(struct file *);
void write_get_cmd(struct file *);