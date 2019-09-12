#include <wifi/ql_wifi.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>


void on_result(char *buff){
    printf("thisi is wifi list:\n");
    printf("\n%s\n",buff);
}

void on_event(char *event){
    printf("\nrecieve event:%s.\n",event);
}

void main(int argc,char *argv[]){
    int ret = -1;
    char ssid[30];
    char psk[30];

    printf("welcome come wifi test program.\n");
    ret = QL_WIFI_init();
    QL_WIFI_set_scan_results_listener(on_result);
    QL_WIFI_set_event_listener(on_event);
    if(ret == 0){
        printf("it's glad that wifi init is successful.\n");
    }
    printf("now check wifi status.\n");
    char buff[4096];
    memset(buff,0,4096);
    QL_WIFI_status(NULL,buff,4096);
    printf("now status : %s.\n",buff);
    sleep(3);
    printf("now start scanning wifi list,please wait.\n");
    QL_WIFI_scan();
    sleep(3);
    printf("please input you want to connect wifi ssid:");
    scanf("%s",ssid);
    printf("\n");
    printf("please input wifi password:");
    scanf("%s",psk);
    printf("now I begin connect wifi,please wait.\n");
    QL_WIFI_connect(ssid,psk);
    sleep(3);
    printf("please choose save password,yes or not?\nplease input:");
    char temp[30];
    memset(temp,0,30);
    scanf("%s",temp);
    if(strcmp(temp,"yes") == 0){
        QL_WIFI_save_config();
        printf("save password successful.\n"); 
    }
    printf("next we will check wifi network list.\n");
    memset(buff,0,4096);
    QL_WIFI_list_network(buff,4096);
    printf("%s\n",buff);
    printf("please choose delete network,yes or not?\nplease input:");
    memset(temp,0,30);
    scanf("%s",temp);
    if(strcmp(temp,"yes") == 0){
        QL_WIFI_remove_network(0);
        QL_WIFI_save_config();
    }
    sleep(3);
    printf("I already delete wifi id 0,and we again check wifi network list.\n");
    QL_WIFI_list_network(buff,4096);
    printf("%s\n",buff);
    QL_WIFI_disconnect();
    printf("start choose first network.\n");
    QL_WIFI_select_network(0);
    sleep(3);
    QL_WIFI_uninit();
}
