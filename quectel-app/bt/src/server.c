#include <stdio.h>
#include <string.h>
#include <ql_bt.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

char BT_LIST[100][100];
int MAX = 0;
int rfcomm_fd = -1;
int client_rfcomm_fd = -1;

void scan_result(const char *ap,const char *address){
    if(ap == NULL || address == NULL){
        return;
    }
    if(MAX == 0){
        sprintf(BT_LIST[MAX],"ap:%s address:%s",ap,address);
        MAX = MAX + 1;
        return;
    }

    char temp[100];
    memset(temp,'\0',sizeof(temp));
    sprintf(temp,"ap:%s address:%s",ap,address);
    for(int i = 0;i < MAX;i ++){
        if(!strcmp(BT_LIST[i],temp)){
            return;
        }
    }
    sprintf(BT_LIST[MAX],"ap:%s address:%s",ap,address);
    MAX = MAX + 1;
}

void pair_result(const char *remote_address){
    printf("remote_address : %s\n",remote_address);
}
static void *recv_client_socket(void *arg){
    printf("server running start recv.\n");
    while(1){
        char buff[1024];
        memset(buff,'\0',sizeof(buff));
        int n = recv(client_rfcomm_fd,buff,sizeof(buff),0);
        printf("recvie client data : %s length : %d \n",buff,n);
    }
}

static void *send_client_socket(void *argc){
    printf("server running start send\n");
    while(1){
        char data[100];
        memset(data,'\0',sizeof(data));
        printf("please input send data:\n");
        scanf("%s",data);
        printf("server send data:%s,length:%d \n",data,strlen(data));
        if(send(client_rfcomm_fd,data,strlen(data),0) < 0){
            printf("server send msg error: %s(errno: %d)\n", strerror(errno), errno);
        }else{
            printf("server send success.\n\n");
        }
    }   
}

#define MAXLINE   256
static void *recv_socket(void *arg){
    printf("lxzha recv_socket thread running\n");
    while(1){
        int sockfd, n;
        struct sockaddr_in dstaddr;
        char buff[MAXLINE], buff2[MAXLINE];
        struct msghdr msgsent, msgrecvd;
        struct cmsghdr cmsg, *cmsgtmp;
        struct iovec iov, iov2;
        const int on = 1;
        char control[CMSG_SPACE(64)]; 
        int flags = MSG_NOSIGNAL;

        msgrecvd.msg_name = &dstaddr;
        msgrecvd.msg_control = control;
        msgrecvd.msg_controllen = sizeof(control);
        iov2.iov_base = (void *)buff2;
        iov2.iov_len = MAXLINE;
        msgrecvd.msg_iov = &iov2;
        msgrecvd.msg_iovlen = 1;
        msgrecvd.msg_flags = 0;

        n = recvmsg(rfcomm_fd,&msgrecvd,flags);
        cmsgtmp = CMSG_FIRSTHDR(&msgrecvd);

        printf("n : %d\n",n);
        int mFd = -1;
        if(n >= 10){
            memcpy(&mFd,CMSG_DATA(cmsgtmp),sizeof mFd);
            printf("recive fd : %d.\n",mFd);
            client_rfcomm_fd = mFd;

            pthread_t thread_id;
            int ret = pthread_create(&thread_id, NULL, recv_client_socket, NULL);
            ret = pthread_create(&thread_id, NULL, send_client_socket, NULL);
            if(ret < 0){
                printf("pthread_create failed.\n");
            }
        }
        sleep(1);
    }    
}


int main(int argc,char *argv[]){
    int ret = -1;

    ret = QL_HAL_load();
    if(ret == 0){
        QL_BT_set_scan_cb(scan_result);
        QL_BT_set_pair_cb(pair_result);
        printf("QL_HAL_load Successful.\n");
        QL_BT_init();
        QL_BT_enable();
        sleep(3);
        rfcomm_fd = QL_BT_socket_listen("socket");
        pthread_t thread_id;
        ret = pthread_create(&thread_id, NULL, recv_socket, NULL);
        printf("wait connecting \n");
        sleep(1000);
    }else{
        printf("QL_HAL_load ret : %d \n",ret);
        return ret;
    }

    return 0;
}
