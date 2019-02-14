#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __LINUX__
#ifdef __LINUX__
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

typedef enum
{
    SOCKET_TYPE_TCP = 0,
    SOCKET_TYPE_UDP,
    SOCKET_TYPE_ICMP,
    SOCKET_TYPE_UNIX,
    SOCKET_TYPE_NETLINK
}SOCKET_TYPE;
typedef struct socket_info_s
{
	char ip[16];
	unsigned short port;
	SOCKET_TYPE socket_type;
	int socket_fd;
	void *data;
	int len;
}socket_info_t;
struct socket_fops_struct
{
	int (*connect)(socket_info_t *socket_info);
	int (*recv)(socket_info_t *socket_info, unsigned char *data, unsigned int len);
	int (*send)(socket_info_t *socket_info, unsigned char *data, unsigned int len);
	int (*disconnect)(int fd);
};
struct ftp_fops_struct
{
	socket_info_t socket_cmd;
	socket_info_t socket_data;
	char user[20];
	char pwd[20];
	char mode[5];
	struct socket_fops_struct *socket_fops;
};
static int net_cfg(void)
{
	return 0;
}
//return:0,success;-1:fail; 1:connecting
static int socket_connect(socket_info_t *socket_info)
{
	int sock;
#ifdef __LINUX__
    struct sockaddr_in serv_addr;
    struct timeval timeout = {10, 0};

	printf("connect....%s:%d\n", socket_info->ip, socket_info->port);
	sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        printf("sock() error");
		return -1;
	}

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(socket_info->ip);
    serv_addr.sin_port = htons(socket_info->port);

	  
    setsockopt(sock , SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));  
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		printf("connect fail:%s:%d\n", socket_info->ip, socket_info->port);
		return -1;
    }
    socket_info->socket_fd = sock;
    
	return 0;
#else
	int ret;
	
	ret = msg_post(MSG_TYPE_SOCKET, EX_MSG_TYPE_SOCKET_CREATE, &socket_info, sizeof(socket_info));
	if(ret < 0){
		return -1;
	}
	else if(ret == 0){
		return 0;
	}
	return 1;
#endif
}
/*********************************************************************************
  *Function:  
  * Description:  socket 发送数据
  *Calls:  
  *Called By:  
  *Input:  
  *Output:  
  *Return:   success:发送数据长度;	error:-1;	busy:-2
  *Others: 
**********************************************************************************/
int socket_send(socket_info_t *socket_info, unsigned char *data, unsigned int len)
{ 
#ifdef __LINUX__
	struct sockaddr_in address;
	int ret = -1;
	fd_set set;
	struct timeval tv;

	if((data == NULL) || (len == 0)){
		return -1;
	}
	if(socket_info->socket_fd < 0){
		return -2;
	}
	FD_ZERO(&set);
       FD_SET(socket_info->socket_fd, &set);
	tv.tv_sec = 5;
	tv.tv_usec = 1;
	ret = select(socket_info->socket_fd + 1, NULL, &set, NULL, &tv);
	if (ret == -1){
		perror("select()");
		ret = -1;
	}
	else if (ret){
		if(socket_info->socket_type ==  SOCKET_TYPE_TCP){
			ret = send(socket_info->socket_fd, data, len, 0);
		}
		else if(socket_info->socket_type == SOCKET_TYPE_UDP){
			bzero(&address,sizeof(address));  
			address.sin_family=AF_INET;  
			address.sin_addr.s_addr=inet_addr(socket_info->ip);
			address.sin_port=htons(socket_info->port);
			ret = sendto(socket_info->socket_fd, data, len, 0, (struct sockaddr *)&address, sizeof(address));
		}
		else if(socket_info->socket_type ==  SOCKET_TYPE_UNIX){
			ret = write(socket_info->socket_fd, data, len);
		}
		else
			ret = -2;
	}
	else{
		ret = -2;
	}
	return ret; 
#else
	int ret;

	socket_info->data = data;
	socket_info->len =  len;
	ret = msg_post(MSG_TYPE_SOCKET, EX_MSG_TYPE_SOCKET_WRITE, &socket_info, sizeof(socket_info));
	if(ret < 0){
		return -1;
	}
	else if(ret == 0){
		return 0;
	}
	return 1;
#endif
}
/*********************************************************************************
  *Function:  
  * Description: socket 接收数据
  *Calls:  
  *Called By:  
  *Input:  
  *Output:  
  *Return:  success:接收数据长度;	error:-1;	busy:-2
  *Others: 
**********************************************************************************/
int socket_recv(socket_info_t *socket_info, unsigned char *data, unsigned int len)
{
#ifdef __LINUX__
	struct sockaddr_in sin;
	int sin_len;
	int ret = -1;
#if 1
	fd_set set;
	struct timeval tv;

	if(socket_info->socket_fd < 0){
		return -1;
	}
	FD_ZERO(&set);
       FD_SET(socket_info->socket_fd, &set);
	tv.tv_sec = 10;
	tv.tv_usec = 1;
	ret = select(socket_info->socket_fd + 1, &set, NULL, NULL, &tv);
	if (ret == -1){
		perror("select()");
		ret = -1;
	}
	else if (ret){
		if(socket_info->socket_type == SOCKET_TYPE_TCP){
			ret = recv(socket_info->socket_fd, data, len, 0);
		}
		else if(socket_info->socket_type == SOCKET_TYPE_UDP){
			bzero(&sin,sizeof(sin));  
			sin.sin_family = AF_INET;  
			sin.sin_addr.s_addr = inet_addr(socket_info->ip);  
			sin.sin_port = htons(socket_info->port);  
			sin_len = sizeof(sin);

			ret = recvfrom(socket_info->socket_fd, data, len, 0,(struct sockaddr *)&sin,(socklen_t*)&sin_len);
		}
		else
			ret = -2;
	}
	else{
		ret = -2;
	}
#else
	if(socket_info->socket_fd < 0){
		return -1;
	}
	if(socket_info->socket_type == SOCKET_TYPE_TCP){
		ret = recv(socket_fd, data, len, 0);
	}
	else if(socket_info->socket_type == SOCKET_TYPE_UDP){
		bzero(&sin,sizeof(sin));  
		sin.sin_family = AF_INET;  
		sin.sin_addr.s_addr = inet_addr(socket_info->ip);  
		sin.sin_port = htons(socket_info->port);  
		sin_len = sizeof(sin);

		ret = recvfrom(socket_info->socket_fd, data, len, 0,(struct sockaddr *)&sin,(socklen_t*)&sin_len);
	}
	else if(socket_info->socket_type == SOCKET_TYPE_UNIX){
		ret = read(socket_info->socket_fd, data, len);
	}
	else
		ret = -2;
#endif

	return ret;
#else
	int ret;

	socket_info->data = data;
	socket_info->len =  len;
	ret = msg_post(MSG_TYPE_SOCKET, EX_MSG_TYPE_SOCKET_WRITE, &socket_info, sizeof(socket_info));
	if(ret < 0){
		return -1;
	}
	else if(ret == 0){
		return 0;
	}
	return 1;
#endif
}
static int socket_disconnect(int fd)
{
	if(fd < 0)
		return -1;
	close(fd);
	return 0;
}
struct socket_fops_struct g_socket_fops = {
	.connect = socket_connect,
	.send = socket_send,
	.recv = socket_recv,
	.disconnect = socket_disconnect
};
int ftp_cmd(struct ftp_fops_struct *ftp_ops, char *cmd, char *ack, int size)
{
	unsigned char buf[30] = {0};
	
	if(ftp_ops->socket_cmd.socket_fd < 0){
		return -1;
	}
	sprintf(buf, "%s\r\n", cmd);
	if(ftp_ops->socket_fops->send(&ftp_ops->socket_cmd, buf, strlen(buf)) <= 0){
		return -1;
	}
	if(ack != NULL){
		if(ftp_ops->socket_fops->recv(&ftp_ops->socket_cmd, ack, size) > 0){
			return 0;
		}
	}
	else{
		return 0;
	}
	return -1;
}
int ftp_pasv(struct ftp_fops_struct *ftp_ops)
{
	char buf[50] = {0};
	int tmp[6] = {0};
	char *p;
	
	if(ftp_cmd(ftp_ops, "PASV", buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
		memset(tmp, 0, sizeof(tmp));
		if(strncmp((char *)buf, (char *)"227", 3) == 0){
			p = strstr((char *)buf, (char *)"(");
			if(p != NULL){
				p++;
				sscanf(p, "%d,%d,%d,%d,%d,%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
			}
		}
		ftp_ops->socket_data.socket_fd = -1;
		ftp_ops->socket_data.port = tmp[4] * 256 + tmp[5];
		memset(ftp_ops->socket_data.ip, 0, sizeof(ftp_ops->socket_data.ip));
		strcpy(ftp_ops->socket_data.ip, ftp_ops->socket_cmd.ip);
		ftp_ops->socket_data.socket_type = ftp_ops->socket_cmd.socket_type;
		printf("pasv port:%d\n", ftp_ops->socket_data.port);
		return 0;
	}
	else{
		printf("set pasv fail\n");
		return -1;
	}
	return 0;
}
int ftp_list(struct ftp_fops_struct *ftp_ops, char *list, int size)
{
	int ret = -1;
	char ack[100] = {0};
	int i, offset, len;

	if(ftp_pasv(ftp_ops) != 0){
		return -1;
	}
	if(ftp_ops->socket_fops->connect(&ftp_ops->socket_data) < 0){
		return -1;
	}
	if(ftp_cmd(ftp_ops, "LIST -a", ack, sizeof(ack)) == 0){
		printf("%s\n", ack);
		offset = 0;
		for(i = 0; i < 2; i++){
			len = ftp_ops->socket_fops->recv(&ftp_ops->socket_data, &list[offset], size - offset);
			if(len > 0){
				offset += len;
				ret = 0;
			}
			if(offset >= size)
				break;
		}
		
	}
	ftp_ops->socket_fops->disconnect(ftp_ops->socket_data.socket_fd);
	ftp_ops->socket_data.socket_fd = -1;
	return ret;
}
//mode is "PASV" or "PORT"
int ftp_login(struct ftp_fops_struct *ftp_ops)
{
	unsigned char cmd[20] = {0};
	unsigned char buf[512] = {0};
	
	if(ftp_ops->socket_fops->connect(&ftp_ops->socket_cmd) < 0){
		return -1;
	}
	if(ftp_ops->socket_fops->recv(&ftp_ops->socket_cmd, buf, sizeof(buf)) > 0){
		printf("%s\n", buf);
	}
	printf("connect success\n");
	memset(buf, 0, sizeof(buf));
	memset(cmd, 0, sizeof(cmd));
	printf("sending user.....\n");
	sprintf(cmd,"USER %s",ftp_ops->user);
	if(ftp_cmd(ftp_ops, cmd, buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
	}
	else{
		printf("sending user fail\n");
		goto FTP_LOGIN_IN_FAIL;
	}
	memset(buf, 0, sizeof(buf));
	memset(cmd, 0, sizeof(cmd));
	printf("sending pass.....%s\n", ftp_ops->pwd);
	sprintf(cmd,"PASS %s",ftp_ops->pwd);
	if(ftp_cmd(ftp_ops, cmd, buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
	}
	else{
		printf("sending password fail\n");
		goto FTP_LOGIN_IN_FAIL;
	}
	memset(buf, 0, sizeof(buf));
	if(ftp_cmd(ftp_ops, "SYST", buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));
	if(ftp_cmd(ftp_ops, "FEAT", buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));
	if(ftp_cmd(ftp_ops, "PWD", buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));
	if(ftp_cmd(ftp_ops, "TYPE A", buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
	}
	return 0;
FTP_LOGIN_IN_FAIL:
	ftp_ops->socket_fops->disconnect(ftp_ops->socket_cmd.socket_fd);
	ftp_ops->socket_cmd.socket_fd = -1;
	
	return -1;
}
int ftp_logout(struct ftp_fops_struct *ftp_ops)
{
	if(ftp_ops->socket_cmd.socket_fd < 0){
		return -1;
	}
	ftp_ops->socket_fops->disconnect(ftp_ops->socket_cmd.socket_fd);
	if(ftp_ops->socket_cmd.socket_fd != -1){
		ftp_ops->socket_fops->disconnect(ftp_ops->socket_cmd.socket_fd);
	}
	return 0;
}
int ftp_download(struct ftp_fops_struct *ftp_ops, char *filename)
{
	unsigned char buf[512] = {0};
	unsigned char cmd[128] = {0};
	int len;
	FILE *fp;

	if(ftp_list(ftp_ops, buf, sizeof(buf)) == 0){
		printf("%s\n", buf);
		if(strstr((char *)buf, filename) != NULL){
#ifdef __LINUX__
			//fp = fopen(filename, "w+");
			fp = fopen("./1.txt", "w+");
			if(fp != NULL){
				if(ftp_pasv(ftp_ops) != 0){
					return -1;
				}
				if(ftp_ops->socket_fops->connect(&ftp_ops->socket_data) < 0){
					printf("connect data port fail\n");
					return -1;
				}
				memset(buf, 0, sizeof(buf));
				sprintf(cmd, "RETR %s", filename);
				if(ftp_cmd(ftp_ops, cmd, buf, sizeof(buf)) == 0){
					printf("%s\n", buf);
					len = ftp_ops->socket_fops->recv(&ftp_ops->socket_data, buf, sizeof(buf));
					while(len > 0){
						fwrite(buf, len, 1, fp);
						len = ftp_ops->socket_fops->recv(&ftp_ops->socket_data, buf, sizeof(buf));
					}
				}
				fclose(fp);
				ftp_ops->socket_fops->disconnect(ftp_ops->socket_data.socket_fd);
				return 0;
			}
#else
			if(ftp_pasv(ftp_ops) != 0){
					return -1;
			}
			if(ftp_ops->socket_fops->connect(&ftp_ops->socket_data) < 0){
				printf("connect data port fail\n");
				return -1;
			}
			memset(buf, 0, sizeof(buf));
			sprintf(cmd, "RETR %s", filename);
			if(ftp_cmd(ftp_ops, cmd, buf, sizeof(buf)) == 0){
				printf("%s\n", buf);
				len = ftp_ops->socket_fops->recv(&ftp_ops->socket_data, buf, sizeof(buf));
				while(len > 0){
					//save
					len = ftp_ops->socket_fops->recv(&ftp_ops->socket_data, buf, sizeof(buf));
				}
			}
			ftp_ops->socket_fops->disconnect(ftp_ops->socket_data.socket_fd);
			return 0;
#endif
		}
		else{
			printf("can not find %s\n", filename);
		}
	}

	return -1;	
}
struct ftp_fops_struct ftp_fops;
int upgrade_from_ftp(void)
{
	static unsigned char upgrade_step = 0;
	static unsigned int timeout = 0;
	int ret = 1;

	timeout++;
	switch(upgrade_step){
	case 0:
		memset(&ftp_fops, 0, sizeof(struct ftp_fops_struct));
		ftp_fops.socket_fops = &g_socket_fops;
		strcpy(ftp_fops.socket_cmd.ip, "192.167.1.140");
		ftp_fops.socket_cmd.port = 21;
		ftp_fops.socket_cmd.socket_type = SOCKET_TYPE_TCP;
		ftp_fops.socket_cmd.socket_fd = -1;
		ftp_fops.socket_data.socket_fd = -1;
		ftp_fops.socket_cmd.socket_type = SOCKET_TYPE_TCP;
		strcpy(ftp_fops.user, "admin");
		strcpy(ftp_fops.pwd, "123456");
		strcpy(ftp_fops.mode, "PASV");
		upgrade_step = 1;
	case 1:
		ret = ftp_login(&ftp_fops);
		if(ret < 0){
			ret = -1;
		}
		else if(ret == 0){
			upgrade_step = 2;
		}
		break;
	case 2:
		ret = ftp_download(&ftp_fops, "readme.txt");
		if((ret > 0) && (timeout > 120)){
			ret = -1;
		}
		if(ret <= 0){
			ftp_logout(&ftp_fops);
			upgrade_step = 0;
		}
		if(ret == 0){
			//upgrade
			
		}
		break;
	}
	return 1;
	
}
int main(int argc, char *argv[])
{	
	int ret = 0;
	
	while(1){
		ret = upgrade_from_ftp();
		if(ret < 0){
			printf("download fail\n");
			break;
		}
		else if(ret == 0){
			printf("download sucess\n");
			break;
		}
		sleep(1);
	}

    return 0;
}

