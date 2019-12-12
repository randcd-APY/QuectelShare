#ifndef ALPHA_QL_BLUETOOTH
#define ALPHA_QL_BLUETOOTH

#ifdef __cplusplus
extern "C" {
#endif

int QL_HAL_load();

void QL_BT_init();

void QL_BT_enable();

void QL_BT_disable();

void QL_BT_scan();

void QL_BT_unscan();

void QL_BT_pair(char *remote_address);

int QL_BT_socket_connect(char *remote_address);

int QL_BT_socket_listen(char *server_name);

void QL_BT_set_scan_cb(void (*p)(const char *ap,const char*address));

void QL_BT_set_pair_cb(void (*p)(const char *address));

void QL_BT_set_recv_cb(void (*p)(const char *data));

#ifdef __cplusplus
}
#endif


#endif
