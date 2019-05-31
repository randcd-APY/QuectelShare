/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <syslog.h>
#include "iotd_context.h"
#include "mml.h"
#include "dataManager.h"
#include "bufpool.h"
#include "unistd.h"
#include "errno.h"
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/time.h>

extern int32_t dataManager_find_index_by_name(void* cxt, char* name);

uint8_t ipv6_addr[16];

char* tapPacket;
uint8_t mac[6]={0x12,0x34,0xff,0xab,0xaa,0xbb};
void create_interface(void* pCxt, int type, char* name, uint8_t devId)
{
    char id[2];

    while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_INIT_CMD;
    /*Leave 1 byte reserved and 4 bytes for iface ID*/
    tapPacket[IOTD_HEADER_LEN + 6] = type; //tap/tun
    tapPacket[IOTD_HEADER_LEN+13] = strlen(name);
    strncpy(&tapPacket[IOTD_HEADER_LEN+14],name,strlen(name));
    memcpy(&tapPacket[IOTD_HEADER_LEN+7], mac,6);

    *(uint16_t*)tapPacket = 10 + 19 + strlen(name); //total len to HTC header

    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id

    mml_enqueue(pCxt,tapPacket, IOTD_DIRECTION_RX);

    snprintf(id, 2, "%d",devId);
    strncat(name, id, 1);
}

void del_interface(void* pCxt, int type, char* name, uint8_t devId)
{
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index;

    index = dataManager_find_index_by_name(pDMCxt, name);

    printf("deleting %d\n",index);

    while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_DESTROY_CMD;
    /*Leave 1 byte reserved and 4 bytes for iface ID*/
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

    *(uint16_t*)tapPacket = 10 + 6; //total len to HTC header

    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id

    mml_enqueue(pCxt,tapPacket, IOTD_DIRECTION_RX);
}


void set_flags(void* pCxt, char* name, uint8_t devId)
{
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

     while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_LINK_STATE_CMD;
    tapPacket[IOTD_HEADER_LEN+6] = DM_IF_UP;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

    *(uint16_t*)tapPacket = 10 + 7; //total len to HTC header

    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);

}

void query_prefix(void* pCxt, char* name, uint8_t devId)
{
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

  while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_QUERY_PREFIX_CMD;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

    *(uint16_t*)tapPacket = 10 + 7; //total len to HTC header


    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);

}
//extern void rtlink_test();
extern void reinit_modules(void* pCxt);

void set_ip_test(void* pCxt, char* name, char* ip, int prefixLen, uint8_t devId)
{
	    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

      while((tapPacket = buf_alloc(64)) == NULL){

        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_ADD_IP;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

    inet_pton(AF_INET6, ip, ipv6_addr);
	memcpy(&tapPacket[IOTD_HEADER_LEN+6], ipv6_addr, 16);
	memcpy(&tapPacket[IOTD_HEADER_LEN+6+16], (char*)&prefixLen, 4);
	*(uint16_t*)tapPacket = 10 + 6 + 16 + sizeof(prefixLen); //total len to HTC header

    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);
}

void del_ip_test(void* pCxt, char* name, char* ip, uint8_t devId)
{
	DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

      while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_DEL_IP;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

	*(uint16_t*)tapPacket = 10 + 6 ; //total len to HTC header


    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);
}

void set_mcast_test(void* pCxt, char* name, char* address, uint8_t devId)
{
	    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

      while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_ADD_MCAST;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);
	
    inet_pton(AF_INET6, address, ipv6_addr);
	memcpy(&tapPacket[IOTD_HEADER_LEN+6], ipv6_addr, 16);
	*(uint16_t*)tapPacket = 10 + 6 + 16; //total len to HTC header


    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);
}

void del_mcast_test(void* pCxt, char* name, char* address, uint8_t devId)
{
	DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

      while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_DEL_MCAST;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);
	
	*(uint16_t*)tapPacket = 10 + 6; //total len to HTC header


    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);
}

void set_route_test(void* pCxt, char* name, char* route, uint8_t devId)
{
	    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

      while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_ADD_ROUTE;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

    inet_pton(AF_INET6, route, ipv6_addr);
	memcpy(&tapPacket[IOTD_HEADER_LEN+6], ipv6_addr, 16);
	*(uint32_t*)(tapPacket +IOTD_HEADER_LEN+6+16)=64;
	*(uint16_t*)tapPacket = 10 + 6 + 16 + 4; //total len to HTC header


    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);
}


void prefix_match_test(void* pCxt, char* name, char* ip, uint8_t devId)
{
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

      while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }
    memset(ipv6_addr, 0, 16); 
   inet_pton(AF_INET6, ip, ipv6_addr);
	memcpy(&tapPacket[IOTD_HEADER_LEN+6], ipv6_addr, 16);
    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_PREFIX_MATCH;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

	*(uint16_t*)tapPacket = 10 + 6 +16; //total len to HTC header


    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);
}

void del_route_test(void* pCxt, char* name, uint8_t devId)
{
	    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int8_t index = dataManager_find_index_by_name(pDMCxt, name);

      while((tapPacket = buf_alloc(64)) == NULL){
        sleep(1000);
    }

    tapPacket[IOTD_HEADER_LEN] = DM_INTERFACE_DEL_ROUTE;
    memcpy(&tapPacket[IOTD_HEADER_LEN+2], (char*)&(pDMCxt->interface[index].id),4);

	*(uint16_t*)tapPacket = 10 + 6; //total len to HTC header


    tapPacket[HTC_HEADER_LEN] = devId<<4;
    tapPacket[HTC_HEADER_LEN] |= 0x02;  //MML sq id
    mml_enqueue(pCxt, tapPacket, IOTD_DIRECTION_RX);
}


int once = 0;
void run_unit_tests(void* pCxt)
{
    if(once == 0){
     //   reinit_modules(pCxt);
#if 1
       char brName[32] = "br\0";   
       char thName[32] = "th\0";

        create_interface(pCxt,0, brName, 1);
        sleep(1);
        set_flags(pCxt, brName, 1);
        sleep(2);
        
       create_interface(pCxt,1, thName, 1);
      // set_flags(pCxt, "br0");

	set_ip_test(pCxt,brName, "2000::200a", 64, 1);
	sleep(2);
	set_ip_test(pCxt,brName, "2001:03:01::200f", 32, 1);
	sleep(2);

	set_ip_test(pCxt,thName, "2009::2002", 64, 1);
	sleep(2);

        query_prefix(pCxt, brName, 0);
        prefix_match_test(pCxt,brName,"2000:0000::11:ff", 1);
        sleep(1);   
	set_mcast_test(pCxt,brName, "ff02::1:3", 1);
	sleep(1);
	set_mcast_test(pCxt,thName, "ff02::3:3", 1);
	sleep(1);
	set_route_test(pCxt,brName, "2005::1:3", 1);
	sleep(1);
	set_route_test(pCxt,thName, "2009::1:3", 1);
	sleep(1);		
	del_mcast_test(pCxt,brName,NULL, 0);
	sleep(1);
	del_mcast_test(pCxt,thName,NULL, 1);
	sleep(1);
	del_route_test(pCxt, brName, 0);
	sleep(1);
	del_route_test(pCxt, thName, 1);
	sleep(1);
	del_ip_test(pCxt, brName, NULL, 0);

        del_interface(pCxt,0,brName, 0);
        sleep(2);
	del_ip_test(pCxt, thName, NULL, 1);
        del_interface(pCxt,0,thName, 1);
	sleep(5);
#endif
        once = 0;
    }
}

#ifdef GPIO_PROFILING
int tog_fd;
setupGPIOToggle(int io)
{
    export_gpio("48");
    set_gpio_direction("48", "out");

    tog_fd = open_gpio_value_file("48", 1);
    
}

void toggleGPIO()
{
    static uint8_t i = 0;
    uint8_t val1 = '1', val2 = '0';
 
    if(i == 0){
     write(tog_fd, &val1, 1);
     i = 1;
    } else {
       write(tog_fd, &val2, 1);
       i = 0;
    }
}
#endif

uint32_t gettstamp()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_usec;
}

