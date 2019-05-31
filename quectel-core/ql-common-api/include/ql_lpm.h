/*
 *@file     ql_lpm.h
 *@date     2018-08-30
 *@author   gale
 *@brief    the function is suitable for this product: 
 *		 __________                 ________
 *		| Quectel  |<--wakeupin----|Customer| 
 *		| 4G module|               |  MCU   |
 *		|__________|---wakeupout-->|________|
 *@Default:
 *          EC20xx: wakeupin pin62(input), rising to wakeup 4G module; wakeupout pin5  (output), rising to wakeup mcu.
 *          AG35xx: wakeupin pin61(input), rising to wakeup 4G module; wakeupout pin147(output), rising to wakeup mcu.
*/

#ifndef __QL_LPM_H__
#define __QL_LPM_H__

typedef enum 
{
    E_QL_LPM_FALLING	= 0, /* Falling, Means wakeupin falling to wakeup the module, or wakeupout falling to wakeup mcu. */
    E_QL_LPM_RISING	= 1, /* Rising, Means  wakeupin rising to wakeup the module,  or wakeupout rising to wakeup  mcu. */
}ql_lpm_edge_t;

typedef int ql_lpm_pin_t;

typedef struct{
	ql_lpm_pin_t wakeupin_pin;
	ql_lpm_edge_t wakeupin_edge;
}ql_lpm_wakeupin_data_t;

typedef struct{
	ql_lpm_pin_t wakeupout_pin;
	ql_lpm_edge_t wakeupout_edge;
}ql_lpm_wakeupout_data_t;

typedef struct{
	ql_lpm_wakeupin_data_t wakeupin;
	ql_lpm_wakeupout_data_t wakeupout;
}QL_Lpm_Cfg_T;

/* Callback function registered via QL_Lpm_Init();
 * It will be called when wakeup_in state changed.
 * lpm_edge: report the wakeup_in state.
*/
typedef void (*QL_Lpm_Handler_T)
(
	ql_lpm_edge_t lpm_edge
);

/* Init QL LPM Init function.
 * Para:
 * 	ql_lpm_handler: the callback handler.
 * 	ql_lpm_cfg:	NULL --> use the default value of quectel;
 *			ql_lpm_cfg --> fill in the structer(Not Recommend).
 * Return value:
 *	0  --> success
 * 	-1 --> failed
*/
int QL_Lpm_Init(QL_Lpm_Handler_T ql_lpm_handler, QL_Lpm_Cfg_T *ql_lpm_cfg);

/* Deinit QL LPM function
 * Rmmod ql_lpm kmod, remove lpm handler.
 * Return value:
 * 	0  --> success
 *	-1 --> failed 
*/
int QL_Lpm_Deinit();

#endif // __QL_LPM_H__
