/***********************************************************************
 * tftp_utils.h
 *
 * Short description
 * Copyright (c) 2014,2018 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header$ $DateTime$ $Author$

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-12-26   rp    Store TFTP packets in RAM for debug
2014-07-28   rp    Add debug_info to measure timings.
2014-06-04   rp    Create

===========================================================================*/

#ifndef __TFTP_UTILS_H__
#define __TFTP_UTILS_H__

#include "tftp_config_i.h"
#include "tftp_comdef.h"

#define TFTP_MIN(x,y)  ((x) < (y) ? (x) : (y))
#define TFTP_MAX(x,y)  ((x) > (y) ? (x) : (y))

void tftp_utils_init (void);

uint16 tftp_htons (uint16 hostshort);
uint16 tftp_ntohs (uint16 netshort);

uint64 tftp_timetick_get (void);
uint64 tftp_timetick_diff_in_usec (uint64 start_time, uint64 end_time);


#endif /* not __TFTP_UTILS_H__ */
