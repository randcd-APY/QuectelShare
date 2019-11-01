#ifndef __QL_IN_H__
#define __QL_IN_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>                                                                                                                                                              
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <memory.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <dirent.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <malloc.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <locale.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

typedef  uint32_t           uint32;
#include "mcm_sms_v01.h"
#include "mcm_atcop_v01.h"
#include "mcm_client_v01.h"
#include "mcm_client.h"
#include "mcm_dm_v01.h"
#include "mcm_nw_v01.h"
#include "mcm_voice_v01.h"
#include "mcm_mobileap_v01.h"
#include "mcm_data_v01.h"
#include "mcm_sim_v01.h"
#include "mcm_loc_v01.h"

#include "ql_error.h"
#include "ql_at.h"
#include "ql_vcall.h"
#include "ql_mcm.h"
#include "ql_mcm_data.h"
#include "ql_mcm_mobap.h"
#include "ql_mcm_sim.h"
#include "ql_mcm_nw.h"
#include "ql_mcm_atc.h"
#include "ql_mcm_voice.h"
#include "ql_mcm_dm.h"
#include "ql_utils.h"
#include "ql_mcm_dev.h"
#include "ql_mcm_gps.h"
#ifdef __cplusplus
}
#endif

#endif	//__QL_OE_H__
