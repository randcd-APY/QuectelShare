/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_bt_platform.h"
#include "csr_app_lib.h"
#include "csr_bt_file.h"
#include "csr_bt_av_app_util.h"

/* Various constants */
#define FILEDUMP_PRN        "filedump: "

/* Instance data for filedump filter */
typedef struct
{
    av2filter_t     *filter;
    FILE            *file;
    char            *name;
} filedump_instance_t;

/* Filter entry prototypes */
static CsrBool filedump_init(void **instance, av2filter_t *filter, av2instance_t *unused);
static CsrBool filedump_deinit(void **instance);
static CsrBool filedump_open(void **instance);
static CsrBool filedump_close(void **instance);
static CsrBool filedump_start(void **instance);
static CsrBool filedump_stop(void **instance);
static CsrBool filedump_process(CsrUint8 index,
                               av2filter_t **chain,
                               CsrBool freeData,
                               void *data,
                               CsrUint32 length);
static CsrUint8 *filedump_get_config(void **instance,
                                    CsrUint8 *conf_len);
static CsrBtAvResult filedump_set_config(void **instance,
                                       CsrUint8 *conf,
                                       CsrUint8 conf_len);
static CsrUint8 *filedump_get_caps(void **instance,
                                  CsrUint8 *cap_len);
static CsrUint8 *filedump_remote_caps(void **instance,
                                     CsrUint8 *caps,
                                     CsrUint8 caps_len,
                                     CsrUint8 *conf_len);
static void filedump_qos_update(void **instance,
                                CsrUint16 qosLevel);
static void filedump_menu(void **instance,
                          CsrUint8 *num_options);
static CsrBool filedump_setup(void **instance,
                             CsrUint8 index,
                             char *value);
static char* filedump_filename(void **instance);


/* Filter structure setup for filedump decoder */
av2filter_t filter_filedump =
{
    "csr_raw_file_dump",
    FILTER_PASS,
    AV_NO_MEDIA,
    AV_NO_CODEC,
    0,
    NULL,
    filedump_init,
    filedump_deinit,
    filedump_open,
    filedump_close,
    filedump_start,
    filedump_stop,
    filedump_process,
    filedump_get_config,
    filedump_set_config,
    filedump_get_caps,
    filedump_remote_caps,
    filedump_qos_update,
    filedump_menu,
    filedump_setup,
    filedump_filename
};

/* Initialise filedump filter */
static CsrBool filedump_init(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    filedump_instance_t *inst;
    *instance = CsrPmemZalloc(sizeof(filedump_instance_t));
    inst = (filedump_instance_t*)*instance;

    /* Set default filename */
    inst->name = CsrStrDup(av2inst->dump_filename);

    return TRUE;
}

/* Deinitialise filedump filter */
static CsrBool filedump_deinit(void **instance)
{
    filedump_instance_t *inst;
    inst = (filedump_instance_t*)*instance;

    /* Free instance */
    if(inst->file)
    {
        CsrBtFclose(inst->file);
        inst->file = NULL;
    }
    if(inst->name)
    {
        CsrPmemFree(inst->name);
        inst->name = NULL;
    }
    CsrPmemFree(inst);
    *instance = NULL;

    return TRUE;
}

/* Open filedump with current configuration */
static CsrBool filedump_open(void **instance)
{
    filedump_instance_t *inst;
    CsrBool result;
    inst = (filedump_instance_t*)*instance;

    result = FALSE;
    if(inst->name != NULL)
    {
        inst->file = CsrBtFopen(inst->name, "wb");

        if(inst->file != NULL)
        {
            result = TRUE;
        }
    }

    return result;
}

/* Close filedump configuration */
static CsrBool filedump_close(void **instance)
{
    filedump_instance_t *inst;
    inst = (filedump_instance_t*)*instance;

    if(inst->file)
    {
        CsrBtFclose(inst->file);
        inst->file = NULL;
    }

    return TRUE;
}

/* Start filedump streaming */
static CsrBool filedump_start(void **instance)
{
    /* Not required */
    return TRUE;
}

/* Stop filedump streaming */
static CsrBool filedump_stop(void **instance)
{
    /* Not required */
    return TRUE;
}

/* Process filedump data: Empty windows buffers */
static CsrBool filedump_process(CsrUint8 index,
                               av2filter_t **chain,
                               CsrBool freeData,
                               void *data,
                               CsrUint32 length)
{
    filedump_instance_t *inst;
    CsrUint32 wr;
    inst = (filedump_instance_t*)(chain[index]->f_instance);

    /* Write data to file */
    if(length > 0)
    {
        wr = CsrBtFwrite(data,
                       1,
                       length,
                       inst->file);
        if(wr == 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,FILEDUMP_PRN "file write error, code %i\n",
                   ferror(inst->file));
        }
        else if(wr != length)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,FILEDUMP_PRN "%i bytes received, but only %i written\n",
                   length, wr);
        }
    }

    /* Free data and trigger next stage even though data has been consumed */
    if(freeData && data)
    {
        CsrPmemFree(data);
    }

    /* Pass data on to next stage */
    return chain[index+1]->f_process((CsrUint8)(index+1),
                                     chain,
                                     FALSE,
                                     NULL,
                                     0);
}

/* Return filedump configuration */
static CsrUint8 *filedump_get_config(void **instance,
                                   CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Set filedump configuration */
static CsrBtAvResult filedump_set_config(void **instance,
                                      CsrUint8 *conf,
                                      CsrUint8 conf_len)
{
    /* We do not support this feature */
    return CSR_BT_AV_ACCEPT;
}

/* Return filedump service capabilities */
static CsrUint8 *filedump_get_caps(void **instance,
                                 CsrUint8 *cap_len)
{
    /* We do not support this feature */
    *cap_len = 0;
    return NULL;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *filedump_remote_caps(void **instance,
                                    CsrUint8 *caps,
                                    CsrUint8 caps_len,
                                    CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Sink buffer level update */
static void filedump_qos_update(void **instance,
                                CsrUint16 qosLevel)
{
    /* Not supported */
}

/* Print menu options to screen */
static void filedump_menu(void **instance,
                         CsrUint8 *num_options)
{
    filedump_instance_t *inst;
    inst = (filedump_instance_t*)*instance;

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  0) Change filename (%s)\n", inst->name);
    *num_options = 1;
}

/* Set user option */
static CsrBool filedump_setup(void **instance,
                            CsrUint8 index,
                            char *value)
{
    filedump_instance_t *inst;
    int i;
    inst = (filedump_instance_t*)*instance;

    switch(index)
    {
        case 0:
            {
                CsrPmemFree(inst->name);
                inst->name = value;
                value = NULL;
                i = 1;
            }
            break;

        default:
            i = 0;
            break;
    }

    CsrPmemFree(value);

    return (i > 0);
}


static char* filedump_filename(void **instance)
{
    filedump_instance_t *inst = NULL;
    inst = (filedump_instance_t *)*instance;

    return inst->name;
}

