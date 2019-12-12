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

void recv_result(const char *data){
    printf("data --> %s\n",data);
}

static void *recv_socket(void *arg){
    printf("running start recv.\n");
    while(1){
        char data[100];
        memset(data,'\0',sizeof(data));
        int n = 0;
        n = recv(rfcomm_fd,data,sizeof(data),0);
        printf("recv server data : %s length : %d\n",data,n);
    }
}

static void *send_socket(void *argc){
    printf("client running start send\n");
    while(1){
        char data[100];
        memset(data,'\0',sizeof(data));
        printf("please input send data:\n");
        scanf("%s",data);
        printf("client send data:%s,length:%d \n",data,strlen(data));
        if(send(rfcomm_fd,data,strlen(data),0) < 0){
            printf("client send msg error: %s(errno: %d)\n", strerror(errno), errno);
        }else{
            printf("client send success.\n\n");
        }
    }
}
int main(int argc,char *argv[]){
    int ret = -1;

    ret = QL_HAL_load();
    if(ret == 0){
        QL_BT_set_recv_cb(recv_result);
        QL_BT_set_scan_cb(scan_result);
        QL_BT_set_pair_cb(pair_result);
        printf("QL_HAL_load Successful.\n");
        QL_BT_init();
        QL_BT_enable();
        sleep(10);
        for(int i = 0;i <= MAX;i ++){
            printf("%s\n",BT_LIST[i]);
        }
        int pos = 0;
        printf("please input pair device number:");
        scanf("%d",&pos);
        printf("choose pair device : %s\n",BT_LIST[pos]);
        char result[100];
        memset(result,'\0',sizeof(result));
        char *temp = strstr(strstr(BT_LIST[pos], "address"),":");
        for(int i = 0;i < strlen(temp);i ++){
            if(i == 0){
                continue;
            }
            result[i - 1] = temp[i];
        }
        char remote[100];
        memset(remote,'\0',sizeof(remote));
        memcpy(remote,result,sizeof(result));
        QL_BT_pair(result);
        sleep(10);
        printf("alpha start connected %s.\n",remote);
        rfcomm_fd = QL_BT_socket_connect(remote);
        pthread_t thread_id;
        ret = pthread_create(&thread_id, NULL, send_socket, NULL);
        ret = pthread_create(&thread_id, NULL, recv_socket, NULL);
        sleep(300000);
    }else{
        printf("QL_HAL_load ret : %d \n",ret);
        return ret;
    }

    return 0;
}
