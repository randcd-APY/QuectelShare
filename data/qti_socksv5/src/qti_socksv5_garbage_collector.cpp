/*==========================================================================

  FILE:  qti_socksv5_garbage_collector.cpp

  SERVICES:

  Garbage collector for QC_SOCKSv5 Proxy on dead socket pairs notified
  via TCP keepalive and SIGPIPE

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

#include "qti_socksv5_garbage_collector.hpp"
#include "qti_socksv5_log_msg.h"

namespace QC_SOCKSv5_Garbage_Collector
{

  /*==========================================================================
    FUNCTION collectGarbage
  ==========================================================================*/
  /*!
  @brief
    spawns pthread to collect dead socket pairs

  @parameters
    ptr to the socket_pair resource

  @return
  */
  /*========================================================================*/
  void collectGarbage(std::map<int, int>* sock_pair_map)
  {
    unsigned char buffer;

    if(NULL == sock_pair_map)
    {
      LOG_MSG_INFO1("sock_pair_map is NULL!", 0, 0, 0);
      return;
    }

    if(cli_req_sock_pair_map_mutex.try_lock())
    {
      LOG_MSG_INFO1("Collecting garbage sockets...", 0, 0, 0);

      //iterate through socket pair list and close all dead sockets
      for(std::map<int, int>::iterator it = sock_pair_map->begin();
          it != sock_pair_map->end(); ++it)
      {
        //check the recv queue
        if((recv(it->first, &buffer, 1, MSG_DONTWAIT | MSG_PEEK) <= 0) ||
           (recv(it->second, &buffer, 1, MSG_DONTWAIT | MSG_PEEK) <= 0))
        {
          if(close(it->first))
          {
            LOG_MSG_INFO1("error with close: %s", strerror(errno), 0, 0);
          }
          if(close(it->second))
          {
            LOG_MSG_INFO1("error with close: %s", strerror(errno), 0, 0);
          }
          sock_pair_map->erase(it);
        }
      }
      cli_req_sock_pair_map_mutex.unlock();
    } else {
      LOG_MSG_INFO1("Deferring garbage collection to a later time\n", 0, 0, 0);
    }

    return;
  }

  /*==========================================================================
    FUNCTION shutdownAllConnections
  ==========================================================================*/
  /*!
  @brief
    spawns pthread to close all socket pairs regardless if dead connection
    or not

  @parameters
    ptr to the socket_pair resource

  @return
  */
  /*========================================================================*/
  void shutdownAllConnections(std::map<int, int>* sock_pair_map)
  {
    if(NULL == sock_pair_map)
    {
      LOG_MSG_INFO1("sock_pair_map is NULL!", 0, 0, 0);
      return;
    }

    //iterate through socket pair list and close all dead sockets
    cli_req_sock_pair_map_mutex.lock();
    for(std::map<int, int>::iterator it = sock_pair_map->begin();
        it != sock_pair_map->end(); ++it)
    {
      if(close(it->first))
      {
        LOG_MSG_INFO1("error with close: %s", strerror(errno), 0, 0);
      }
      if(close(it->second))
      {
        LOG_MSG_INFO1("error with close: %s", strerror(errno), 0, 0);
      }

      sock_pair_map->erase(it);
    }
    cli_req_sock_pair_map_mutex.unlock();

    LOG_MSG_INFO1("Garbage collector: closed all socket pairs", 0, 0, 0);
  }
}
