/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 */
#ifndef _LIBABCTL_H_
#define _LIBABCTL_H_
/*
 *  libabctl_getBootSlot returns current slot number from which device booted.
 *  Return values should be interpreted as follows:
 *    0: When device booted from slot suffixed "_a".
 *    1: When device booted from slot suffixed "_b".
 *   -1: If system does not support A/B, or in case of any error.
 */
int libabctl_getBootSlot();


/*
 * libabctl_getSuccessStatus() gets Success status for the slot specified.
 *  Return values should be interpreted as follows:
 *    0: The Success flag is NOT set for given slot.
 *    1: The Success flag is set for given slot.
 *   -1: If system does not support A/B, or in case of any error.
 */
int libabctl_getSuccessStatus(unsigned int slot);


/*
 * libabctl_SetBootSuccess marks the success flag for the slot number specified.
 * from which device booted..
 *  Return values should be interpreted as follows:
 *    0: The Success flag was set successfully.
 *   -1: If system does not support A/B, or in case of any error.
 */
int libabctl_SetBootSuccess();


/*
 * libabctl_getActiveStatus() gets active status for the slot specified.
 *  Return values should be interpreted as follows:
 *    0: The slot is NOT Active.
 *    1: The slot is Active.
 *   -1: If system does not support A/B, or in case of any error.
 */
int libabctl_getActiveStatus(unsigned int slot);


/*
 * libabctl_setActive() marks the specified slot as active and
 * sets the priotity and retry count to highest, clears the success and unbootable flags.
 *  Return values should be interpreted as follows:
 *    0: The Success flag was set successfully.
 *   -1: If system does not support A/B, or in case of any error.
 */
int libabctl_setActive(unsigned int slot);

/*
 * libabctl_setUnbootable() marks the specified slot as unbootable.
 *  Return values should be interpreted as follows:
 *    0: The Success flag was set successfully.
 *   -1: If system does not support A/B, or in case of any error.
 */
int libabctl_setUnbootable(unsigned int slot);


void libabctl_debugPrints(int do_prints);
#endif //_LIBABCTL_H_
