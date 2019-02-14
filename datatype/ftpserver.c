#ifndef __FTPSERVER_H__  
#define __FTPSERVER_H__  
#ifdef __cplusplus  
extern "C" {  
#endif  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>          /* See NOTES */  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <pthread.h>  
struct data {  
    char ip[100];  
    short port;  
    char user[100];  
    char pass[100];  
    char url[100];  
    int sock;  
    pthread_t pid;  
    FILE *fp;  
};  
  
void server_init(struct data *data);  
void server_set(struct data *data, const char *addr, short port, const char *user, const char *pass, const char *rooturl);  
int server_start(struct data *data);  
void server_stop(struct data *data);  
  
#ifdef __cplusplus  
}  
#endif  
#endif // __FTPSERVER_H__  
  
  
  
  
//ftpserver.c  
////////////////////////////////////////  
#include <sys/stat.h>  
#include <sys/types.h>  
#include <fcntl.h>  
#include <dirent.h>  
#include <sys/stat.h>  
#include <signal.h>  
#include <sys/wait.h>  
//#include "ftpserver.h"  
#ifdef __cplusplus  
extern "C" {  
#endif  
  
struct ftp_server {  
    int ofs;  
    int port_ip;  
    short port_port;  
    int c_sock;  
    int d_sock;  
    int data_sock;  
    int c_addr;  
    short c_port;  
    int d_addr;  
    short d_port;  
    int islogin;  
    char rename[256];  
    char send_buf[256];  
    char user[100];  
    char pass[100];  
    void *data;  
};  
  
static pid_t pid_arr[256];  
  
static void sig_fun(int sig)  
{  
    int status, i;  
    pid_t pid = wait(&status);  
    for (i = 0; i < sizeof(pid_arr) / sizeof(pid_arr[0]); i++) {  
        if (pid_arr[i] == pid) {  
            pid_arr[i] = 0;  
        }  
    }  
}  
  
static void close_tenor(void)  
{  
    int i;  
    for (i = 0; i < sizeof(pid_arr) / sizeof(pid_arr[0]); i++) {  
        if (pid_arr[i]) {  
            kill(pid_arr[i], SIGINT);  
            pid_arr[i] = 0;  
        }  
    }  
}  
  
static char *my_split(char *src, const char *key, int *len, char **data)  
{  
    char *pos, *p;  
    if (src) *data = src;  
    pos = strstr(*data, key);  
    if (!pos) {  
        *len = 0;  
        return *data;  
    } else {  
        *len = pos - *data;  
        p = *data;  
        while (!memcmp(pos, key, strlen(key))) {  
            pos += strlen(key);  
        }  
        *data = pos;  
        return p;  
    }  
}  
  
  
  
static int ftp_server_user(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
    if (!strcmp(server->user, start)) {  
        sprintf(server->send_buf, "331 Password required\r\n");  
        server->islogin = 1;  
    } else {  
        sprintf(server->send_buf, "332 notfound user\r\n");  
        server->islogin = 0;  
    }  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_pass(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    if (!strcmp(server->pass, start)) {  
        sprintf(server->send_buf, "230 User logged in..\r\n");  
        if (server->islogin == 1)  
            server->islogin = 2;  
        else  
            server->islogin = 0;  
    } else {  
        sprintf(server->send_buf, "331 Need password login\r\n");  
        server->islogin = 0;  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_syst(struct ftp_server *server, const char *cmd)  
{  
    printf("%s", "215 UNIX emulated by FileZilla\r\n");  
    send(server->c_sock, "215 UNIX emulated by FileZilla\r\n", strlen("215  UNIX emulated by FileZilla\r\n"), 0);  
    return 0;  
}  
  
static int ftp_server_pwd(struct ftp_server *server, const char *cmd)  
{  
    char buf[128] = {};  
    getcwd(buf, sizeof(buf) - 1);  
    sprintf(server->send_buf, "257 \"%s\" is current directory.\r\n", buf);  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_type(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
    if (!strncmp(start, "I", 1)) {  
        sprintf(server->send_buf, "200 Type set to I.\r\n");  
    } else {  
        sprintf(server->send_buf, "500 : command not understood.\r\n");  
    }  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_pasv(struct ftp_server *server, const char *cmd)  
{  
    int ret;  
    struct sockaddr_in addr;  
  
    server->port_ip = 0;  
    server->d_sock = socket(AF_INET, SOCK_STREAM , 0);  
    if (server->d_sock == -1) {  
        return -1;  
    }  
    struct timeval timeout = {6, 0};  
    setsockopt(server->d_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));  
    //设置接收超时  
    setsockopt(server->d_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));  
  
    addr.sin_addr.s_addr = server->d_addr;  
    addr.sin_port = 0;  
    addr.sin_family = AF_INET;  
  
    int opt=SO_REUSEADDR;  
    setsockopt(server->d_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  
  
    ret = bind(server->d_sock, (struct sockaddr *)&addr, sizeof(addr));  
    if (ret != 0) {  
        return -1;  
    }  
  
    struct sockaddr_in localaddr;  
    socklen_t locallen = sizeof(localaddr);  
    getsockname(server->d_sock, (struct sockaddr *)&localaddr, & locallen);  
    ret = listen(server->d_sock, 10);  
    if (ret == -1) {  
        close(server->d_sock);  
        return -1;  
    }  
  
    short port =  localaddr.sin_port;  
    sprintf(server->send_buf, "227 Entering Passive Mode (%s,%d,%d)\r\n", inet_ntoa(localaddr.sin_addr),  
            0xff & (port),  
            0xff & (port >> 8));  
    int i;  
    for (i = 0; i < strlen(server->send_buf); i++) {  
        if (server->send_buf[i] == '.')  
            server->send_buf[i] = ',';  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_port(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    char *pos, *end;  
    pos = start;  
    int i;  
    for (i = 0, end = pos; i < 4; i++) {  
        end = strstr(end, ",");  
        if (end) *end = '.';  
    }  
    *end = 0;  
  
    server->port_ip = inet_addr(pos);  
    pos = ++end;  
    end = strstr(pos, ",");  
        if (end) *end = 0;  
    server->port_port = atoi(pos);  
    server->port_port <<= 8;  
    server->port_port |= atoi(end + 1);  
    server->port_port = htons(server->port_port);  
  
    printf("%s",  "200 Port command successful.\r\n");  
    send(server->c_sock, "200 Port command successful.\r\n",  
         strlen("200 Port command successful.\r\n"), 0);  
  
    return 0;  
}  
  
static int ftp_server_get_dsock(struct ftp_server *server)  
{  
    struct sockaddr_in inaddr;  
    socklen_t socklen = sizeof(inaddr);  
    if (server->port_ip == 0) {  
        server->data_sock = accept(server->d_sock, (struct sockaddr *)&inaddr, &socklen);  
        if (server->data_sock == -1) return -1;  
    } else {  
        server->data_sock = socket(AF_INET, SOCK_STREAM, 0);  
        if (server->data_sock == -1) return -1;  
        inaddr.sin_addr.s_addr = server->port_ip;  
        inaddr.sin_port = server->port_port;  
        inaddr.sin_family = AF_INET;  
        struct timeval timeout = {30, 0};  
        setsockopt(server->data_sock , SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));  
        int ret = connect(server->data_sock, (struct sockaddr *)&inaddr, socklen);  
        if (ret == -1) {  
            close(server->data_sock);  
            server->data_sock = -1;  
            return -1;  
        }  
    }  
    return 0;  
}  
  
static void stat_test(const char *name, char *out)  
{  
    struct stat s = {};  
    char buf[100] = {};  
    int res = stat(name, &s);  
  
    if (-1 == res) {  
        return;  
    }  
    if (S_ISDIR(s.st_mode)) {  
        out[0] = 'd';  
    } else {  
        out[0] = '-';  
    }  
    out[1] = 0;  
    int i;  
    int num = s.st_mode & 07777;  
    strcat(out, "rwxrwxrwx");  
    for (i = 0; i < 9; i++) {  
        if (!(num & (0x100 >> i))) {  
            out[i + 1] = '-';  
        }  
    }  
    strcat(out, " ");  
    sprintf(buf, "%3d", s.st_nlink);  
    strcat(out, buf);  
    strcat(out, " root root ");  
    sprintf(buf, "%6d", (int)s.st_size);  
    strcat(out, buf);  
    strcat(out, " ");  
    time_t t = time(NULL);  
    int year = gmtime(&t)->tm_year;  
    int year2 = gmtime(&s.st_mtime)->tm_year;  
    if (year == year2) {  
        strftime(buf, sizeof(buf), "%h %d %R", gmtime(&s.st_mtime));  
    } else {  
        strftime(buf, sizeof(buf), "%h %d %G", gmtime(&s.st_mtime));  
    }  
    strcat(out, buf);  
    strcat(out, " ");  
    strcat(out, name);  
    strcat(out, "\r\n");  
}  
  
  
  
static int ftp_server_list(struct ftp_server *server, const char *cmd)  
{  
    if (server->d_sock == -1) return -1;  
    printf("%s", "150 Opening BINARY mode data connection for /bin/ls.\r\n");  
    send(server->c_sock, "150 Opening BINARY mode data connection for /bin/ls.\r\n",  
         strlen("150 Opening BINARY mode data connection for /bin/ls.\r\n"), 0);  
  
  
    DIR *dir = NULL;  
    struct dirent *ent = NULL;  
    char buf[256] = {};  
    dir  = opendir(".");  
    if (!dir) {  
        return -1;  
    }  
  
    if (-1 == ftp_server_get_dsock(server)) {  
        return -1;  
    }  
    while ((ent = readdir(dir))) {  
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;  
        stat_test(ent->d_name, buf);  
        //printf("%s", buf);  
        send(server->data_sock, buf, strlen(buf), 0);  
    }  
    closedir(dir);  
  
    close(server->data_sock);  
    if (server->d_sock != -1) close(server->d_sock);  
  
    printf("%s",  "226 Transfer complete.\r\n");  
    send(server->c_sock, "226 Transfer complete.\r\n",  
         strlen("226 Transfer complete.\r\n"), 0);  
    return 0;  
}  
  
static int ftp_server_stor(struct ftp_server *server, const char *cmd)  
{  
    if (server->d_sock == -1) return -1;  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    sprintf(server->send_buf, "150 Opening BINARY mode data connection.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    int fd;  
    int len;  
    char buf[256] = {};  
  
    if (-1 == ftp_server_get_dsock(server)) {  
        sprintf(server->send_buf, "553 net err\r\n");  
        printf("%s", server->send_buf);  
        send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
        if (server->d_sock != -1) close(server->d_sock);  
        return -1;  
    }  
  
  
    fd = open(start, O_CREAT  | O_TRUNC | O_WRONLY, 0664);  
    if (fd == -1) {  
        sprintf(server->send_buf, "553 : Permission denied\r\n");  
    } else {  
        while ((len = recv(server->data_sock, buf, sizeof(buf), 0)) > 0) {  
            write(fd, buf, len);  
        }  
        close(fd);  
    }  
  
    close(server->data_sock);  
    if (server->d_sock != -1) close(server->d_sock);  
  
    sprintf(server->send_buf, "226 Transfer complete.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_rest(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
    server->ofs = atoi(start);  
    printf("%s",  "200 complete.\r\n");  
    send(server->c_sock, "200 complete.\r\n",  
         strlen("200 complete.\r\n"), 0);  
    return 0;  
}  
  
static int ftp_server_appe(struct ftp_server *server, const char *cmd)  
{  
    if (server->d_sock == -1) return -1;  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    sprintf(server->send_buf, "150 Opening BINARY mode data connection.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    int fd;  
    int len;  
    char buf[256] = {};  
  
    if (-1 == ftp_server_get_dsock(server)) {  
        sprintf(server->send_buf, "553 net err\r\n");  
        printf("%s", server->send_buf);  
        send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
        if (server->d_sock != -1) close(server->d_sock);  
        return -1;  
    }  
  
  
    fd = open(start,   O_APPEND | O_WRONLY);  
    if (fd == -1) {  
        sprintf(server->send_buf, "553 : Permission denied\r\n");  
    } else {  
        while ((len = recv(server->data_sock, buf, sizeof(buf), 0)) > 0) {  
            write(fd, buf, len);  
        }  
        close(fd);  
    }  
  
    close(server->data_sock);  
    if (server->d_sock != -1) close(server->d_sock);  
  
    sprintf(server->send_buf, "226 Transfer complete.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_retr(struct ftp_server *server, const char *cmd)  
{  
    if (server->d_sock == -1) return -1;  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    sprintf(server->send_buf, "150 Opening BINARY mode data connection for.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    int fd;  
    int len, ret;  
    char buf[256] = {};  
  
    if (-1 == ftp_server_get_dsock(server)) {  
        sprintf(server->send_buf, "553 net err\r\n");  
        printf("%s", server->send_buf);  
        send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
        if (server->d_sock != -1) close(server->d_sock);  
        return -1;  
    }  
  
  
    fd = open(start, O_RDONLY);  
    if (fd == -1) {  
        sprintf(server->send_buf, "553 : Permission denied\r\n");  
    } else {  
        lseek(fd, server->ofs, SEEK_SET);  
        while ((len = read(fd, buf, sizeof(buf))) > 0) {  
            ret = send(server->data_sock, buf, len, 0);  
            if (ret < 0)  
                break;  
        }  
        server->ofs = 0;  
        close(fd);  
    }  
  
    close(server->data_sock);  
    if (server->d_sock != -1) close(server->d_sock);  
  
    sprintf(server->send_buf, "226 Transfer complete.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_size(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    int ret;  
    struct stat info;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
    ret = stat(start, &info);  
    if (ret) {  
        sprintf(server->send_buf, "550 : not a plain file.\r\n");  
    } else {  
        sprintf(server->send_buf, "213 %u\r\n", (unsigned int)info.st_size);  
    }  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_cwd(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    int ret;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
    ret = chdir(start);  
    if (ret == -1) {  
        sprintf(server->send_buf, "550 : No such file or directory.\r\n");  
    } else {  
        sprintf(server->send_buf, "250 CWD command successful.\r\n");  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_cdup(struct ftp_server *server, const char *cmd)  
{  
    int ret;  
    ret = chdir("..");  
    if (ret == -1) {  
        sprintf(server->send_buf, "550 CDUP No such file or directory.\r\n");  
    } else {  
        sprintf(server->send_buf, "250 CDUP command successful.\r\n");  
    }  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_mkd(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    int ret;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    ret = access(start, F_OK);  
    if (!ret) {  
        sprintf(server->send_buf, "550 : File exists.\r\n");  
    } else {  
        ret = mkdir(start, 0775);  
        if (ret == -1) {  
            sprintf(server->send_buf, "550 : Permission denied.\r\n");  
        } else {  
            sprintf(server->send_buf, "257 MKD command successful.\r\n");  
        }  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_rmd(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    int ret;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    ret = access(start, F_OK);  
    if (ret) {  
        sprintf(server->send_buf, "550 : No such file or directory.\r\n");  
    } else {  
  
        ret = rmdir(start);  
        if (ret == -1) {  
            sprintf(server->send_buf, "550 : Permission denied.\r\n");  
        } else {  
            sprintf(server->send_buf, "250 RMD command successful.\r\n");  
        }  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_dele(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    int ret;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    ret = access(start, F_OK);  
    if (ret) {  
        sprintf(server->send_buf, "550 : No such file or directory.\r\n");  
    } else {  
  
        ret = remove(start);  
        if (ret == -1) {  
            sprintf(server->send_buf, "550 : Permission denied.\r\n");  
        } else {  
            sprintf(server->send_buf, "250 DELE command successful.\r\n");  
        }  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_utf8(struct ftp_server *server, const char *cmd)  
{  
    sprintf(server->send_buf, "200  No such file or directory.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_feat(struct ftp_server *server, const char *cmd)  
{  
    sprintf(server->send_buf, "211-Features\r\n SIZE \r\n UTF8\r\n211 End\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_site_chmod(struct ftp_server *server, const char *cmd)  
{  
    char name[256] = {};  
    mode_t mode;  
    char *start;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start = strstr(++start, " ");  
    if (!start) return -1;  
    start++;  
  
    printf("%s\n", start);  
    sscanf(start, "%o%s", (int *)&mode, name);  
    printf("%d %s\n", (int)mode, name);  
  
    chmod(name, mode);  
  
    sprintf(server->send_buf, "200 ok.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
static int ftp_server_rnfr(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    int ret;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
  
    ret = access(start, F_OK);  
    if (ret) {  
        sprintf(server->send_buf, "550 : No such file or directory.\r\n");  
    } else {  
        strcpy(server->rename, start);  
        sprintf(server->send_buf, "200 command successful.\r\n");  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_rnto(struct ftp_server *server, const char *cmd)  
{  
    char *start;  
    int ret;  
    start = strstr(cmd, " ");  
    if (!start) return -1;  
    start++;  
    ret = rename(server->rename, start);  
    if (ret) {  
        sprintf(server->send_buf, "550 : rename err.\r\n");  
    } else {  
        sprintf(server->send_buf, "200 command successful.\r\n");  
    }  
  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
static int ftp_server_nocmd(struct ftp_server *server, const char *cmd)  
{  
    sprintf(server->send_buf, "500 : command not understood.\r\n");  
    printf("%s", server->send_buf);  
    send(server->c_sock, server->send_buf, strlen(server->send_buf), 0);  
    return 0;  
}  
  
  
struct cmd_t {  
    const char *cmd;  
    int (*func)(struct ftp_server *ftp, const char *cmd);  
};  
  
static struct cmd_t cmd_table[] = {  
    [0] = {  
        .cmd = "not found cmd",  
        .func = ftp_server_nocmd  
    },  
    [1] = {  
        .cmd = "PASS",  
        .func = ftp_server_pass  
    },  
    [2] = {  
        .cmd = "SYST",  
        .func = ftp_server_syst  
    },  
    [3] = {  
        .cmd = "USER",  
        .func = ftp_server_user  
    },  
    [4] = {  
        .cmd = "PWD",  
        .func = ftp_server_pwd  
    },  
    [5] = {  
        .cmd = "TYPE",  
        .func = ftp_server_type  
    },  
    [6] = {  
        .cmd = "PASV",  
        .func = ftp_server_pasv  
    },  
    [7] = {  
        .cmd = "LIST",  
        .func = ftp_server_list  
    },  
    [8] = {  
        .cmd = "CWD",  
        .func = ftp_server_cwd  
    },  
    [9] = {  
        .cmd = "CDUP",  
        .func = ftp_server_cdup  
    },  
    [10] = {  
        .cmd = "MKD",  
        .func = ftp_server_mkd  
    },  
    [11] = {  
        .cmd = "RMD",  
        .func = ftp_server_rmd  
    },  
    [12] = {  
        .cmd = "DELE",  
        .func = ftp_server_dele  
    },  
    [13] = {  
        .cmd = "STOR",  
        .func = ftp_server_stor  
    },  
    [14] = {  
        .cmd = "RETR",  
        .func = ftp_server_retr  
    },  
    [15] = {  
        .cmd = "SIZE",  
        .func = ftp_server_size  
    },  
    [16] = {  
        .cmd = "UTF8",  
        .func = ftp_server_utf8  
    },  
    [17] = {  
        .cmd = "FEAT",  
        .func = ftp_server_feat  
    },  
    [18] = {  
        .cmd = "SITE CHMOD",  
        .func = ftp_server_site_chmod  
    },  
    [19] = {  
        .cmd = "RNTO",  
        .func = ftp_server_rnto  
  
    },  
    [20] = {  
        .cmd = "RNFR",  
        .func = ftp_server_rnfr  
    },  
    [21] = {  
        .cmd = "APPE",  
        .func = ftp_server_appe  
    },  
    [22] = {  
        .cmd = "REST",  
        .func = ftp_server_rest  
    },  
    [23] = {  
        .cmd = "PORT",  
        .func = ftp_server_port  
    }  
};  
struct cmd_t *find_cmd(char *cmd)  
{  
    int i, len;  
    char *end, *pos = NULL, *p;  
    end = strstr(cmd, "\r\n");  
    if (!end)  
        end = strstr(cmd, "\n");  
    if (!end) return NULL;  
    *end = 0;  
    p = my_split(cmd, " ", &len, &pos);  
  
    for (i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i++) {  
        if (!strncmp(cmd_table[i].cmd, p, strlen(cmd_table[i].cmd))) {  
            return cmd_table + i;  
        }  
    }  
    return cmd_table;  
};  
  
static void *pthread_func(void *data)  
{  
    int ret;  
    char buf[256] = {};  
    struct cmd_t *cmd;  
    struct ftp_server *server = data;  
    send(server->c_sock, "220 ftp\r\n", strlen("220 ftp\r\n"), 0);  
    while (1) {  
        ret = recv(server->c_sock, buf, sizeof(buf) - 1, 0);  
        printf("%s", buf);  
        if (ret <= 0)  
            break;  
        cmd = find_cmd(buf);  
        if (cmd != cmd_table + 1 && cmd != cmd_table + 3) {  
            if (server->islogin != 2)  
                continue;  
        }  
        if (cmd)  
            cmd->func(server, buf);  
        memset(buf, 0, sizeof(buf));  
    }  
  	printf("ftp client break now\n");
    close(server->c_sock);  
    free(server);  
    return NULL;  
}  
  
  
int ftp_start(int sock, const char *local_addr, const char *user, const char *pass)  
{  
    struct ftp_server *server = malloc(sizeof(struct ftp_server));  
    memset(server, 0, sizeof(struct ftp_server));  
    strcpy(server->user, user);  
    strcpy(server->pass, pass);  
    server->c_sock = sock;  
    server->d_addr = inet_addr(local_addr);  
    struct timeval timeout = {30, 0};  
    setsockopt(sock , SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));  
    setsockopt(sock , SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));  
    //pthread_create(&pid, NULL, pthread_func, (void *)server);  
    pthread_func((void *)server);  
    return 0;  
}  
  
static void thread_exit_handler(int sig)  
{  
    printf("this signal is %d \n", sig);  
    pthread_exit(0);  
}  
#define BUFF_SIZE 1024

 char *http_res_tmpl = "HTTP/1.1 200 OK\r\n"  
		 "Server: Cleey's Server V1.0\r\n"	
		 "Accept-Ranges: bytes\r\n"  
		 "Content-Length: %d\r\n"  
		 "Connection: close\r\n"  
		 "Content-Type: %s\r\n\r\n";  
   
 void http_send(int sock_client,char *content){  
		 char HTTP_HEADER[BUFF_SIZE],HTTP_INFO[BUFF_SIZE];	
		 int len = strlen(content);  
		 sprintf(HTTP_HEADER,http_res_tmpl,len,"text/html");  
		 len = sprintf(HTTP_INFO,"%s%s",HTTP_HEADER,content);  
   
		 write(sock_client,HTTP_INFO,len);	
 }	
 char g_html[] = {
	 "<html>\r\n"
	"<form action=\"check.php\" method=\"post\">\r\n"
"用户名:<input type=\"text\" name=\"user\">\r\n"
"密码:<input type=\"password\" name=\"pw\">\r\n"
"<input type=\"submit\" value=\"提交\">\r\n"
"</form>\r\n"
 "</html>\r\n"
 }; 

 void html_test(int fd)
{
	http_send(fd, g_html);
}
void *server_pthread(void *data)  
{  
    struct data  *p = data;  
    int lis_sock = p->sock;  
    int client;  
    struct sockaddr_in cli_addr;  
    socklen_t len;  
    len = sizeof(cli_addr);  
  
  	printf("start listen %d\n", lis_sock);
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old);  
    while (1) {  
        client = accept(lis_sock, (struct sockaddr *)&cli_addr, &len);  
		printf("accept %d\n", client);
        if (client == -1)  
            break;  
  		
  		printf("new client .... %d\n", client);
		html_test(client);
        int i;  
        for (i = 0; i < sizeof(pid_arr) / sizeof(pid_arr[0]); i++) {  
            if (!pid_arr[i]) {  
                pid_arr[i] = fork();  
                break;  
            }  
        }  
  
        if (i == sizeof(pid_arr) / sizeof(pid_arr[0])) {  
            close(client);  
            continue;  
        }  
  
        if (pid_arr[i] < 0) {  
            close(client);  
            pid_arr[i] = 0;  
            continue;  
        } else if (pid_arr[i] == 0) {  
            ftp_start(client, p->ip, p->user, p->pass);  
            close(lis_sock);  
            exit(0);  
        } else if (pid_arr[i] > 0) {  
            close(client);  
        }  
    }  
    close(lis_sock);  
    return NULL;  
}  
  
void server_init(struct data *data)  
{  
    memset(data, 0, sizeof(struct data));  
    data->fp = fopen("/home/ftp.dat", "rb+");  
    if (!data->fp) {  
        data->fp = fopen("/home/ftp.dat", "wb+");  
        if (data->fp) {  
            fprintf(data->fp, "%hd %s %s %s\n", 80, "ftp", "1234", "/");  
        }  
        data->port = 80;  
        strcpy(data->user, "admin");  
        strcpy(data->pass, "123456");  
        strcpy(data->url, "/");  
        return;  
    }  
    fscanf(data->fp, "%d%s%s%s", &data->port, data->user, data->pass, data->url);  
}  
  
void server_set(struct data *data, const char *addr, short port, const char *user, const char *pass, const char *rooturl)  
{  
    strcpy(data->ip, addr);  
    strcpy(data->user, user);  
    strcpy(data->pass, pass);  
    strcpy(data->url, rooturl);  
    data->port = port;  
}  

int server_start(struct data *data)  
{  
    int lis_sock, ret;  
    struct sockaddr_in lis_addr;  
  	/*
    if (chroot(data->url) == -1) {  
        ret = chdir(data->url);  
    } else {  
        ret = chdir("/");  
    }  
    if (ret == -1)  
        return ret;  */
  	printf("-----\n");
    lis_sock = socket(AF_INET, SOCK_STREAM, 0);  
    if (lis_sock == -1) {  
        perror("socket");  
        return -1;  
    }  
  	bzero(&lis_addr, sizeof(lis_addr));
    //lis_addr.sin_addr.s_addr = inet_addr(data->ip);  
    lis_addr.sin_family = AF_INET;
    lis_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    lis_addr.sin_port = htons(data->port);  
    lis_addr.sin_family = AF_INET;  
  
    int opt=SO_REUSEADDR;  
    setsockopt(lis_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  
    ret = bind(lis_sock, (struct sockaddr*)&lis_addr, sizeof(lis_addr));  
    if (ret == -1) {  
        perror("bind");  
        printf("failed\n");  
        return -1;  
    }  
    printf("started\n");  
    listen(lis_sock, 10);  
    data->sock = lis_sock;  
  
    signal(SIGCHLD, sig_fun);  
    signal(SIGQUIT, thread_exit_handler);  
    pthread_create(&data->pid, NULL, server_pthread, (void *)data);  
  	printf("pid = %d\n", data->pid);
    if (data->fp) {  
        fseek(data->fp, 0, 0);  
        fprintf(data->fp, "%hd %s %s %s\n", data->port, data->user, data->pass, data->url);  
    }  
  
    return 0;  
}  
  
void server_stop(struct data *data)  
{  
    if (data->fp) {  
        fclose(data->fp);  
        data->fp = NULL;  
    }  
  
    close_tenor();  
  
    if (data->pid) {  
        //pthread_cancel(data->pid);  
        pthread_kill(data->pid, SIGQUIT);  
        shutdown(data->sock,  SHUT_RDWR);  
        close(data->sock);  
        data->sock = 0;  
        data->pid = 0;  
    }  
}  

#ifdef __cplusplus  
}  
#endif 


int main(void)
{
	struct data ftp_data;
	memset(&ftp_data, 0, sizeof(ftp_data));
	strcpy(ftp_data.ip, "192.168.17.130");
	ftp_data.port = 8000;
	strcpy(ftp_data.user, "admin");
	strcpy(ftp_data.pass, "123456");

	//server_init(&ftp_data);
	if(server_start(&ftp_data) != 0){
		printf("start ftp server fail\n");
		return -1;
	}
	while(1){
		sleep(1);
	}
	return 0;
}
