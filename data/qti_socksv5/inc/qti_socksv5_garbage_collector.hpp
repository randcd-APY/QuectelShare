#ifndef QTI_SOCKSV5_GARBAGE_COLLECTOR_HPP_
#define QTI_SOCKSV5_GARBAGE_COLLECTOR_HPP_

/*==========================================================================

  FILE:  qti_socksv5_garbage_collector.hpp

  SERVICES:

  SOCKSv5 header file for dead socket pair garbage collection.

==========================================================================*/

/*==========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

==========================================================================*/

/*==========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------------------------
  03/20/17   jt         SOCKSv5 support.
==========================================================================*/

#include <iostream>
#include <map>
#include <mutex>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace QC_SOCKSv5_Garbage_Collector
{
  void collectGarbage(std::map<int, int>* sock_pair_map);
  void shutdownAllConnections(std::map<int, int>* sock_pair_map);
}

extern std::mutex cli_req_sock_pair_map_mutex;

#endif
