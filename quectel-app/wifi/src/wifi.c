#include <ql_wifi.h>
#include <stdio.h>

void main(int argc,char *argv[]){
    if(argc != 3){ 
        printf("Invalid parameter!.\n");
        printf("example : WIFI_TEST ssid password.\n");
        return;
    }else{
        printf("ssid:%s,psk:%s.\n",argv[1],argv[2]);
    }   
    int ret = QL_WIFI_init();
    if(ret == 0){ 
        printf("wpa_supplicant service open succed!.\n");
    }else{
        printf("wpa_supplicant service open failed.\n");
    }   

    QL_WIFI_connect(argv[1],argv[2]);
} 
