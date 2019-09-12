#ifndef QL_WIFI
#define QL_WIFI

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int QL_WIFI_init();//初始化，创建连接

void QL_WIFI_connect(char *ssid,char *psk);

int QL_WIFI_disconnect();

int QL_WIFI_reconnect();

int QL_WIFI_scan();

int QL_WIFI_remove_network(int id);

int QL_WIFI_list_network(char *buf,size_t len);

int QL_WIFI_save_config();

int QL_WIFI_abort_scan();

int QL_WIFI_status(char *type,char *result,size_t len);

void QL_WIFI_set_scan_results_listener(void (*on_result)(char *results));

void QL_WIFI_set_event_listener(void (*on_event)(char *results));

int QL_WIFI_uninit();

int QL_WIFI_select_network(int network_id); 

int QL_WIFI_enableNetwork();

int QL_WIFI_disableNetwork();

int QL_WIFI_select_enable(int network_id);

int QL_WIFI_select_disable(int network_id);

#ifdef __cplusplus
}
#endif

#endif
