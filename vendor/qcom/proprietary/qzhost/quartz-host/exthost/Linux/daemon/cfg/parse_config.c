/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse_config.h"
#include "iotd_context.h"

// test config struct, it may link to daemon_context_t
cfg_ini g_cfg_ini;

//Entry
REG_TABLE_ENTRY system_group_entry[] =
{
    REG_VARIABLE( CONFIG_SYSTEM_DEVICE_NUM_NAME, PARAM_Integer,
            config_system_t, num_device,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_DEVICE_NUM_DEFAULT,
            CONFIG_SYSTEM_DEVICE_NUM_MIN,
            CONFIG_SYSTEM_DEVICE_NUM_MAX),

    REG_VARIABLE( CONFIG_SYSTEM_INTERFACE_NUM_NAME, PARAM_Integer,
            config_system_t, num_interface,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_INTERFACE_NUM_DEFAULT,
            CONFIG_SYSTEM_INTERFACE_NUM_MIN,
            CONFIG_SYSTEM_INTERFACE_NUM_MAX),

    REG_VARIABLE( CONFIG_SYSTEM_CLIENT_NUM_NAME, PARAM_Integer,
            config_system_t, num_clients,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_CLIENT_NUM_DEFAULT,
            CONFIG_SYSTEM_CLIENT_NUM_MIN,
            CONFIG_SYSTEM_CLIENT_NUM_MAX),

    REG_VARIABLE( CONFIG_SYSTEM_BUFFER_NUM_NAME, PARAM_Integer,
            config_system_t, num_buffer,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_BUFFER_NUM_DEFAULT,
            CONFIG_SYSTEM_BUFFER_NUM_MIN,
            CONFIG_SYSTEM_BUFFER_NUM_MAX),

    REG_VARIABLE( CONFIG_SYSTEM_PWD_GPIO_NAME, PARAM_Integer,
            config_system_t, pwd_gpio,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_PWD_GPIO_DEFAULT,
            CONFIG_SYSTEM_PWD_GPIO_MIN,
            CONFIG_SYSTEM_PWD_GPIO_MAX),

    REG_VARIABLE( CONFIG_SYSTEM_FORCE_RESET_NAME, PARAM_Integer,
            config_system_t, force_reset,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_FORCE_RESET_DEFAULT,
            CONFIG_SYSTEM_FORCE_RESET_MIN,
            CONFIG_SYSTEM_FORCE_RESET_MAX),

    REG_VARIABLE( CONFIG_SYSTEM_HEART_BEAT_NAME, PARAM_Integer,
            config_system_t, heart_beat_enable,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_HEART_BEAT_DEFAULT,
            CONFIG_SYSTEM_HEART_BEAT_MIN,
            CONFIG_SYSTEM_HEART_BEAT_MAX),
            
    REG_VARIABLE( CONFIG_SYSTEM_HEART_BEAT_INTERVAL_NAME, PARAM_Integer,
            config_system_t, heart_beat_interval,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_HEART_BEAT_INTERVAL_DEFAULT,
            CONFIG_SYSTEM_HEART_BEAT_INTERVAL_MIN,
            CONFIG_SYSTEM_HEART_BEAT_INTERVAL_MAX),

    REG_VARIABLE( CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_NAME, PARAM_Integer,
            config_system_t, throughput_test_enable,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_MIN,
            CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_MAX),            

    REG_VARIABLE( CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_NAME, PARAM_Integer,
            config_system_t, throughput_test_mode,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_MIN,
            CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_MAX),            
            
    REG_VARIABLE( CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_NAME, PARAM_Integer,
            config_system_t, throughput_test_interval,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_MIN,
            CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_MAX),               
            
    REG_VARIABLE( CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_NAME, PARAM_Integer,
            config_system_t, throughput_test_packet_len,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_DEFAULT,
            CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_MIN,
            CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_MAX),      

    REG_VARIABLE( CONFIG_SYSTEM_DBG_LVL_NAME, PARAM_Integer,
            config_system_t, dbg_lvl,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_SYSTEM_DBG_LVL_DEFAULT,
            CONFIG_SYSTEM_DBG_LVL_MIN,
            CONFIG_SYSTEM_DBG_LVL_MAX),        
};

REG_TABLE_ENTRY interface_group_entry_A[] =
{
    REG_VARIABLE( CONFIG_INTERFACE_ENABLE_NAME, PARAM_Integer,
            config_interface_t, enable,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_ENABLE,
            CONFIG_INTERFACE_ENABLE_MIN,
            CONFIG_INTERFACE_ENABLE_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_TYPE_NAME, PARAM_Integer,
            config_interface_t, type,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_TYPE_UART,
            CONFIG_INTERFACE_TYPE_MIN,
            CONFIG_INTERFACE_TYPE_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_DEVICE_ID_NAME, PARAM_Integer,
            config_interface_t, device_id,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_DEVICE_ID_DEFAULT,
            CONFIG_INTERFACE_DEVICE_ID_MIN,
            CONFIG_INTERFACE_DEVICE_ID_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_SERVICE_Q_NUM_NAME, PARAM_Integer,
            config_interface_t, num_service_q,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_SERVICE_Q_NUM_DEFAULT,
            CONFIG_INTERFACE_SERVICE_Q_NUM_MIN,
            CONFIG_INTERFACE_SERVICE_Q_NUM_MAX),

    REG_VARIABLE_STRING( CONFIG_INTERFACE_NAME, PARAM_String,
            config_interface_t, name,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_INTERFACE_NAME_UART_DEFAULT),

    REG_VARIABLE( CONFIG_INTERFACE_SPEED_NAME, PARAM_Integer,
            config_interface_t, speed,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_SPEED_DEFAULT,
            CONFIG_INTERFACE_SPEED_MIN,
            CONFIG_INTERFACE_SPEED_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_FLOW_CONTROL_NAME, PARAM_Integer,
            config_interface_t, flow_control,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_FLOW_CONTROL_ENABLE,
            CONFIG_INTERFACE_FLOW_CONTROL_MIN,
            CONFIG_INTERFACE_FLOW_CONTROL_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_INTR_GPIO_NAME, PARAM_Integer,
            config_interface_t, intr_gpio,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_INTR_GPIO_DEFAULT,
            CONFIG_INTERFACE_INTR_GPIO_MIN,
            CONFIG_INTERFACE_INTR_GPIO_MAX),
            
    REG_VARIABLE( CONFIG_INTERFACE_BLOCK_SIZE_NAME, PARAM_Integer,
            config_interface_t, block_size,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_BLOCK_SIZE_DEFAULT,
            CONFIG_INTERFACE_BLOCK_SIZE_MIN,
            CONFIG_INTERFACE_BLOCK_SIZE_MAX),

    REG_VARIABLE_ARRAY( CONFIG_INTERFACE_QID_NAME, PARAM_IntegerArray,
            config_interface_t, qid,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_INTERFACE_QID_DEFAULT,
            CONFIG_INTERFACE_QID_ARRAY_NUM),

};

REG_TABLE_ENTRY interface_group_entry_B[] =
{
    //Disable other interface by default
    REG_VARIABLE( CONFIG_INTERFACE_ENABLE_NAME, PARAM_Integer,
            config_interface_t, enable,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_DISABLE,
            CONFIG_INTERFACE_ENABLE_MIN,
            CONFIG_INTERFACE_ENABLE_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_TYPE_NAME, PARAM_Integer,
            config_interface_t, type,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_TYPE_SPI,
            CONFIG_INTERFACE_TYPE_MIN,
            CONFIG_INTERFACE_TYPE_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_DEVICE_ID_NAME, PARAM_Integer,
            config_interface_t, device_id,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_DEVICE_ID_DEFAULT,
            CONFIG_INTERFACE_DEVICE_ID_MIN,
            CONFIG_INTERFACE_DEVICE_ID_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_SERVICE_Q_NUM_NAME, PARAM_Integer,
            config_interface_t, num_service_q,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_SERVICE_Q_NUM_DEFAULT,
            CONFIG_INTERFACE_SERVICE_Q_NUM_MIN,
            CONFIG_INTERFACE_SERVICE_Q_NUM_MAX),

    REG_VARIABLE_STRING( CONFIG_INTERFACE_NAME, PARAM_String,
            config_interface_t, name,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_INTERFACE_NAME_SPI_DEFAULT),

    REG_VARIABLE( CONFIG_INTERFACE_SPEED_NAME, PARAM_Integer,
            config_interface_t, speed,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_SPEED_DEFAULT,
            CONFIG_INTERFACE_SPEED_MIN,
            CONFIG_INTERFACE_SPEED_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_FLOW_CONTROL_NAME, PARAM_Integer,
            config_interface_t, flow_control,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_FLOW_CONTROL_ENABLE,
            CONFIG_INTERFACE_FLOW_CONTROL_MIN,
            CONFIG_INTERFACE_FLOW_CONTROL_MAX),

    REG_VARIABLE( CONFIG_INTERFACE_INTR_GPIO_NAME, PARAM_Integer,
            config_interface_t, intr_gpio,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_INTR_GPIO_DEFAULT,
            CONFIG_INTERFACE_INTR_GPIO_MIN,
            CONFIG_INTERFACE_INTR_GPIO_MAX),
            
    REG_VARIABLE( CONFIG_INTERFACE_BLOCK_SIZE_NAME, PARAM_Integer,
            config_interface_t, block_size,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_INTERFACE_BLOCK_SIZE_DEFAULT,
            CONFIG_INTERFACE_BLOCK_SIZE_MIN,
            CONFIG_INTERFACE_BLOCK_SIZE_MAX),

    REG_VARIABLE_ARRAY( CONFIG_INTERFACE_QID_NAME, PARAM_IntegerArray,
            config_interface_t, qid,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_INTERFACE_QID_DEFAULT,
            CONFIG_INTERFACE_QID_ARRAY_NUM),

};

REG_TABLE_ENTRY qapi_group_entry[] =
{
    REG_VARIABLE_STRING( CONFIG_IPC_SERVER_NAME, PARAM_String,
            config_service_t, ipc_server,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_IPC_SERVER_QAPI_NAME_DEFAULT),

    REG_VARIABLE( CONFIG_IPC_MSG_NUMBER_NAME, PARAM_Integer,
            config_service_t, msg_num,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_MIN,
            CONFIG_IPC_MSG_NUMBER_MAX),

    REG_VARIABLE( CONFIG_IPC_MSG_SIZE_NAME, PARAM_Integer,
            config_service_t, msg_sz,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_SIZE_DEFAULT,
            CONFIG_IPC_MSG_SIZE_MIN,
            CONFIG_IPC_MSG_SIZE_MAX),

    REG_VARIABLE( CONFIG_IPC_SERVICE_Q_NUMBER_NAME, PARAM_Integer,
            config_service_t, num_service_q,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_MIN,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),

    REG_VARIABLE_ARRAY( CONFIG_SERVICE_Q_ID_NAME, PARAM_IntegerArray,
            config_service_t, qid,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_QAPI_Q_ID_DEFAULT,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),
};

REG_TABLE_ENTRY diag_group_entry[] =
{
    REG_VARIABLE_STRING( CONFIG_IPC_SERVER_NAME, PARAM_String,
            config_service_t, ipc_server,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_IPC_SERVER_DIAG_NAME_DEFAULT),

    REG_VARIABLE( CONFIG_IPC_MSG_NUMBER_NAME, PARAM_Integer,
            config_service_t, msg_num,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_MIN,
            CONFIG_IPC_MSG_NUMBER_MAX),

    REG_VARIABLE( CONFIG_IPC_MSG_SIZE_NAME, PARAM_Integer,
            config_service_t, msg_sz,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_SIZE_DEFAULT,
            CONFIG_IPC_MSG_SIZE_MIN,
            CONFIG_IPC_MSG_SIZE_MAX),
    REG_VARIABLE( CONFIG_IPC_SERVICE_Q_NUMBER_NAME, PARAM_Integer,
            config_service_t, num_service_q,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_MIN,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),

    REG_VARIABLE_ARRAY( CONFIG_SERVICE_Q_ID_NAME, PARAM_IntegerArray,
            config_service_t, qid,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_DIAG_Q_ID_DEFAULT,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),
};

REG_TABLE_ENTRY data_group_entry[] =
{
    REG_VARIABLE_STRING( CONFIG_IPC_SERVER_NAME, PARAM_String,
            config_service_t, ipc_server,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_IPC_SERVER_NAME_DEFAULT),

    REG_VARIABLE( CONFIG_IPC_MSG_NUMBER_NAME, PARAM_Integer,
            config_service_t, msg_num,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_MIN,
            CONFIG_IPC_MSG_NUMBER_MAX),

    REG_VARIABLE( CONFIG_IPC_MSG_SIZE_NAME, PARAM_Integer,
            config_service_t, msg_sz,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_SIZE_DEFAULT,
            CONFIG_IPC_MSG_SIZE_MIN,
            CONFIG_IPC_MSG_SIZE_MAX),

    REG_VARIABLE( CONFIG_IPC_SERVICE_Q_NUMBER_NAME, PARAM_Integer,
            config_service_t, num_service_q,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_MIN,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),

    REG_VARIABLE_ARRAY( CONFIG_SERVICE_Q_ID_NAME, PARAM_IntegerArray,
            config_service_t, qid,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_DATA_Q_ID_DEFAULT,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),
};

REG_TABLE_ENTRY mgmt_group_entry[] =
{
    REG_VARIABLE_STRING( CONFIG_IPC_SERVER_NAME, PARAM_String,
            config_service_t, ipc_server,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_IPC_SERVER_NAME_DEFAULT),

    REG_VARIABLE( CONFIG_IPC_MSG_NUMBER_NAME, PARAM_Integer,
            config_service_t, msg_num,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_DEFAULT,
            CONFIG_IPC_MSG_NUMBER_MIN,
            CONFIG_IPC_MSG_NUMBER_MAX),

    REG_VARIABLE( CONFIG_IPC_MSG_SIZE_NAME, PARAM_Integer,
            config_service_t, msg_sz,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_MSG_SIZE_DEFAULT,
            CONFIG_IPC_MSG_SIZE_MIN,
            CONFIG_IPC_MSG_SIZE_MAX),

    REG_VARIABLE( CONFIG_IPC_SERVICE_Q_NUMBER_NAME, PARAM_Integer,
            config_service_t, num_service_q,
            VAR_FLAGS_OPTIONAL | VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_DEFAULT,
            CONFIG_IPC_SERVICE_Q_NUMBER_MIN,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),

    REG_VARIABLE_ARRAY( CONFIG_SERVICE_Q_ID_NAME, PARAM_IntegerArray,
            config_service_t, qid,
            VAR_FLAGS_OPTIONAL,
            (void *)CONFIG_MGMT_Q_ID_DEFAULT,
            CONFIG_SERVICE_Q_ID_ARRAY_NUM),
};


//Group
REG_TABLE_GROUP g_registry_group[] =
{
    //system
    REG_Group( CONFIG_GROUP_SYSTEM_NAME,
            cfg_ini, config_system,
            VAR_FLAGS_OPTIONAL,
            system_group_entry ),

    //interface A
    REG_Group( CONFIG_GROUP_INTERFACE_NAME,
            cfg_ini, config_interface[0],
            VAR_FLAGS_OPTIONAL,
            interface_group_entry_A ),

    //interface B
    REG_Group( CONFIG_GROUP_INTERFACE_NAME,
            cfg_ini, config_interface[1],
            VAR_FLAGS_OPTIONAL,
            interface_group_entry_B ),

    //QAPI
    REG_Group( CONFIG_GROUP_QAPI_NAME,
            cfg_ini, config_qapi,
            VAR_FLAGS_OPTIONAL,
            qapi_group_entry ),

    //DIAG
    REG_Group( CONFIG_GROUP_DIAG_NAME,
            cfg_ini, config_diag,
            VAR_FLAGS_OPTIONAL,
            diag_group_entry ),

    //DATA
    REG_Group( CONFIG_GROUP_DATA_NAME,
            cfg_ini, config_data,
            VAR_FLAGS_OPTIONAL,
            data_group_entry ),

    //MGMT
    REG_Group( CONFIG_GROUP_MGMT_NAME,
            cfg_ini, config_mgmt,
            VAR_FLAGS_OPTIONAL,
            mgmt_group_entry ),
};


// look for space. Ascii values to look are -
// 0x09 == horizontal tab
// 0x0a == Newline ("\n")
// 0x0b == vertical tab
// 0x0c == Newpage or feed form.
// 0x0d == carriage return (CR or "\r")
// Null ('\0') should not considered as space.
#define i_isspace(ch)  (((ch) >= 0x09 && (ch) <= 0x0d) || (ch) == ' ')

/*
 * This function trims any leading and trailing white spaces
 */
static char *i_trim(char *str)
{
    char *ptr;

    if(*str == '\0') return str;

    /* Find the first non white-space*/
    for (ptr = str; i_isspace(*ptr); ptr++);
    if (*ptr == '\0')
        return str;

    /* This is the new start of the string*/
    str = ptr;

    /* Find the last non white-space*/
    ptr += strlen(ptr) - 1;
    for (; ptr != str && i_isspace(*ptr); ptr--);
    /* Null terminate the following character */
    ptr[1] = '\0';

    return str;
}

/*
 * This function returns a pointer to the character after the occurence
 * of a new line character. It also modifies the original string by replacing
 * the '\n' character or '#' with the null character.
 * Function returns NULL if no new line character was found before end of
 * string was reached
 */
static char* get_next_line(char* str)
{
    char c;
    char *pound = NULL;

    if( str == NULL || *str == '\0') {
        return NULL;
    }

    c = *str;
    while(c != '\n'  && c != '\0' && c != 0xd)  {
        if (c == '#' && pound == NULL) pound = str;
        str = str + 1;
        c = *str;
    }

    if (pound) *pound = '\0';

    if (c == '\0' ) {
        return NULL;
    }
    else
    {
        *str = '\0';
        return (str+1);
    }

    return NULL;
}

static int apply_config_ini(tCfgIniGroup* iniGroup, unsigned int group_entries,
        tCfgIniEntry* iniTable, unsigned int table_entries, cfg_ini* target);

int parse_config_ini(cfg_ini *config_target, char* cfgFile)
{
    FILE *fp = NULL;
    char *buffer, *line, *name, *pTemp, *value;
    int status = 0;
    unsigned long size, i=0, j=0;
    static tCfgIniGroup cfgIniGroup[MAX_CFG_INI_GROUPS];
    static tCfgIniEntry cfgIniTable[MAX_CFG_INI_ITEMS];

    if(cfgFile){
        IOTD_LOG(LOG_TYPE_CRIT,"IOTD: Opening config file %s\n",cfgFile);
        fp = fopen(cfgFile, CONFIG_FILE_MODE);
        if (fp == NULL)
        {
            IOTD_LOG(LOG_TYPE_CRIT,"ERROR: Unable to open user provided config file\n");
            return -1;
        }
    }else {
        IOTD_LOG(LOG_TYPE_CRIT,"IOTD: Opening default config file %s\n",CONFIG_FILE);

        fp = fopen(CONFIG_FILE , CONFIG_FILE_MODE);
        if (fp == NULL)
        {
            IOTD_LOG(LOG_TYPE_CRIT, "ERROR: Failed to open default config file\n");
            return -1;
        }
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    //check size
    if (size >= CONFIG_FILE_MAX_SIZE)
    {
        IOTD_LOG(LOG_TYPE_CRIT, "config file size exceeds the limit\n");
        goto fail;
    }

    buffer = (char*)malloc(size+1);
    if (buffer == NULL)
    {
        IOTD_LOG(LOG_TYPE_CRIT, "buffer malloc failure, size = %lu\n", size);
        goto fail;
    }
    memset(buffer, 0, size+1);
    pTemp = buffer;

    if(fread(buffer, size, 1, fp) > size)
    {
        IOTD_LOG(LOG_TYPE_CRIT, "read file size not matched\n");
        free(pTemp);
        goto fail;
    }

    while (buffer != NULL)
    {
        line = get_next_line(buffer);
        buffer = i_trim(buffer);

        if(strlen((char*)buffer) == 0 || *buffer == '#')  {
            buffer = line;
            continue;
        }
        else if(strncmp(buffer, "END", 3) == 0 ) {
            break;
        }
        else if(*buffer == '[') {
            buffer ++;
            name = buffer;
            while(*buffer != ']' && *buffer !='\0')
                buffer++;
            if(*buffer == ']') {
                *buffer = '\0';
                i_trim(name);
                if(strlen (name) != 0) {
                    if (i >= MAX_CFG_INI_GROUPS) {
                        IOTD_LOG(LOG_TYPE_INFO, "%s: Number of groups in %s > %d\n",__func__, CONFIG_FILE, MAX_CFG_INI_GROUPS);
                        status = -1;
                        break;
                    }
                    cfgIniGroup[i].name = name;
                    cfgIniGroup[i].count = 0;
                    cfgIniGroup[i++].start_idx = j;
                }
            }
            else
                IOTD_LOG(LOG_TYPE_CRIT, "Error: unreconginzed format config value, lost ']'\n");
        }
        else
        {
            name = buffer;
            while(*buffer != '=' && *buffer != '\0')
                buffer++;
            if(*buffer != '\0') {
                *buffer++ = '\0';
                i_trim(name);
                if(strlen (name) != 0) {
                    buffer = i_trim(buffer);
                    if(strlen(buffer)>0) {
                        value = buffer;
                        while (*buffer != '\0')
                            buffer++;
                        *buffer = '\0';
                        if(i == 0) {
                            IOTD_LOG(LOG_TYPE_INFO, "add [Group Name] before group\n");
                            status = -1;
                            break;
                        }
                        cfgIniTable[j].name= name;
                        cfgIniTable[j++].value= value;
                        cfgIniGroup[i-1].count++;
                        if(j >= MAX_CFG_INI_ITEMS) {
                            IOTD_LOG(LOG_TYPE_INFO, "%s: Number of items in %s > %d\n",__func__, CONFIG_FILE, MAX_CFG_INI_ITEMS);
                            status = -1;
                            break;
                        }
                    }
                }
            }
        }
        buffer = line;
    }

    if (status == 0) //0: SUCCESS, -1: FAIL
    {
        status = apply_config_ini(cfgIniGroup, i, cfgIniTable, j, config_target);
    }

    //close config file and free buffer
    fclose(fp);
    free(pTemp);

    return status;

fail:
    fclose(fp);
    return -1;
}

static int find_cfg_item (tCfgIniEntry* iniTable, unsigned long entries,
        char *name, char** value)
{
    unsigned long i;

    for (i = 0; i < entries; i++) {
        if (strcmp(iniTable[i].name, name) == 0) {
            *value = iniTable[i].value;
            IOTD_LOG(LOG_TYPE_INFO, "Found %s entry for Name=[%s] Value=[%s]\n",
                    CONFIG_FILE, name, *value);
            return 1;
        }
    }

    return 0;
}

static long find_cfg_group (tCfgIniGroup* iniGroup, unsigned long entries,
        char *name, unsigned long bitmap)
{
    unsigned long i;

    for (i = 0; i < entries; i++) {
        if ((strcmp(iniGroup[i].name, name) == 0) && !(bitmap & (1<<i))) {
            IOTD_LOG(LOG_TYPE_INFO, "Found %s group for Name=[%s]\n",
                    CONFIG_FILE, name);
            return i;
        }
    }

    return -1;
}

static int apply_config_entry(REG_TABLE_GROUP* pRegGroup, tCfgIniEntry* iniTable,
        unsigned int table_entries, cfg_ini* target)
{
    unsigned long idx, len_value_str, cbOutString;
    void *pField;
    char *value_str = NULL;
    int isMatch = 0, ret_status = 0, array_num = 0, array_size = 0;
    unsigned long value;
    long svalue;
    REG_TABLE_ENTRY *pRegEntry = pRegGroup->TableEntry;
    unsigned int cRegEntryNum = pRegGroup->EntryNumber;
    char *rv, *pStart, *pEnd, data[16];
    void *pStructBase = (void *)target + pRegGroup->GroupOffset;

    for ( idx = 0; idx < cRegEntryNum;  idx++, pRegEntry++)
    {
        pField = pStructBase + pRegEntry->VarOffset;
        //Range Check
        if(iniTable != NULL && table_entries > 0 )
        {
            isMatch = find_cfg_item(iniTable, table_entries, pRegEntry->RegName, &value_str);
        }

        if ((!isMatch) && ( pRegEntry->Flags & VAR_FLAGS_REQUIRED ))
        {
            IOTD_LOG(LOG_TYPE_INFO, "failed to read a required entry %s\n", pRegEntry->RegName);
            ret_status = -1;
            break;
        }

        // If successfully read from the registry, use the value read.
        // If not, use the default value.
        if ( ( PARAM_Integer    == pRegEntry->RegType ) ||
                ( PARAM_HexInteger == pRegEntry->RegType ) )
        {
            if ( isMatch && (PARAM_Integer == pRegEntry->RegType)) {
                //is strtoul() safe as kstrtou32?
                value = strtoul(value_str, &rv, 10);
                if(*rv != '\0')
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s invalid. Enforcing default\n",
                            __func__, pRegEntry->RegName);
                    value = pRegEntry->VarDefault;
                }
            }
            else if ( isMatch && (PARAM_HexInteger == pRegEntry->RegType)) {
                value = strtoul(value_str, &rv, 16);
                if(*rv != '\0')
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg paramter %s invalid. Enforcing default\n",
                            __func__, pRegEntry->RegName);
                    value = pRegEntry->VarDefault;
                }
            }
            else {
                value = pRegEntry->VarDefault;
                IOTD_LOG(LOG_TYPE_INFO, "%s: Not matched, use default value %lu for [%s]\n", __func__,pRegEntry->VarDefault, pRegEntry->RegName);
            }

            //Range Check
            if ( pRegEntry->Flags & VAR_FLAGS_RANGE_CHECK )
            {
                if ( value > pRegEntry->VarMax )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s > allowed Maximum [%lu > %lu]. Enforcing Maximum\n",
                            __func__, pRegEntry->RegName, value, pRegEntry->VarMax );
                    value = pRegEntry->VarMax;
                }

                if ( value < pRegEntry->VarMin )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s < allowed Minimum [%lu < %lu]. Enforcing Minimum\n",
                            __func__, pRegEntry->RegName, value, pRegEntry->VarMin);
                    value = pRegEntry->VarMin;
                }
            }
            else if ( pRegEntry->Flags & VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT )
            {
                if ( value > pRegEntry->VarMax )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s > allowed Maximum [%lu > %lu]. Enforcing Default= %lu\n",
                            __func__, pRegEntry->RegName, value, pRegEntry->VarMax, pRegEntry->VarDefault  );
                    value = pRegEntry->VarDefault;
                }

                if ( value < pRegEntry->VarMin )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s < allowed Minimum [%lu < %lu]. Enforcing Default= %lu\n",
                            __func__, pRegEntry->RegName, value, pRegEntry->VarMin, pRegEntry->VarDefault  );
                    value = pRegEntry->VarDefault;
                }
            }

            // Move the variable into the output field.
            memcpy( pField, &value, pRegEntry->VarSize );
        }
        else if ( PARAM_SignedInteger == pRegEntry->RegType )
        {
            if (isMatch)
            {
                svalue = strtol(value_str, &rv, 10);
                if(*rv != '\0')
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s invalid. Enforcing Default\n",
                            __func__, pRegEntry->RegName);
                    svalue = (int)pRegEntry->VarDefault;
                }
            }
            else
            {
                svalue = (int)pRegEntry->VarDefault;
            }

            // If this parameter needs range checking, do it here.
            if ( pRegEntry->Flags & VAR_FLAGS_RANGE_CHECK )
            {
                if ( svalue > pRegEntry->VarMax )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s > allowed Maximum"
                            "[%ld > %ld]. Enforcing Maximum\n", __func__,
                            pRegEntry->RegName, svalue, pRegEntry->VarMax );
                    svalue = pRegEntry->VarMax;
                }

                if ( svalue < pRegEntry->VarMin )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s < allowed Minimum "
                            "[%ld < %ld]. Enforcing Minimum\n",  __func__,
                            pRegEntry->RegName, svalue, pRegEntry->VarMin);
                    svalue = pRegEntry->VarMin;
                }
            }
            // If this parameter needs range checking, do it here.
            else if ( pRegEntry->Flags & VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT )
            {
                if ( svalue > pRegEntry->VarMax )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s > allowed Maximum "
                            "[%ld > %ld]. Enforcing Default= %ld\n",
                            __func__, pRegEntry->RegName, svalue,
                            pRegEntry->VarMax,
                            pRegEntry->VarDefault  );
                    svalue = pRegEntry->VarDefault;
                }

                if ( svalue < pRegEntry->VarMin )
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Reg Parameter %s < allowed Minimum "
                            "[%ld < %ld]. Enforcing Default= %ld\n",
                            __func__, pRegEntry->RegName, svalue,
                            pRegEntry->VarMin,
                            pRegEntry->VarDefault);
                    svalue = pRegEntry->VarDefault;
                }
            }

            // Move the variable into the output field.
            memcpy( pField, &svalue, pRegEntry->VarSize );
        }
        // Handle string parameters
        else if ( PARAM_String == pRegEntry->RegType )
        {
            //IOTD_LOG(LOG_TYPE_INFO, "RegName = %s, VarOffset %u VarSize %u VarDefault %s\n",
            //    pRegEntry->RegName, pRegEntry->VarOffset, pRegEntry->VarSize, (char*)pRegEntry->VarDefault);
            if ( isMatch )
            {
                len_value_str = strlen(value_str);

                if(len_value_str > (pRegEntry->VarSize - 1)) {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: Invalid Value=[%s] specified for Name=[%s] in %s\n",
                            __func__, value_str, pRegEntry->RegName, CONFIG_FILE);
                    cbOutString = MIN( strlen( (char *)pRegEntry->VarDefault ), pRegEntry->VarSize - 1 );
                    memcpy( pField, (void *)(pRegEntry->VarDefault), cbOutString );
                    ( (char *)pField )[ cbOutString ] = '\0';
                }
                else
                {
                    memcpy( pField, (void *)(value_str), len_value_str);
                    ( (char *)pField )[ len_value_str ] = '\0';
                }
            }
            else
            {
                // Failed to read the string parameter from the registry.  Use the default.
                cbOutString = MIN( strlen( (char *)pRegEntry->VarDefault ), pRegEntry->VarSize - 1 );
                memcpy( pField, (void *)(pRegEntry->VarDefault), cbOutString );
                ( (char *)pField )[ cbOutString ] = '\0';
                IOTD_LOG(LOG_TYPE_INFO, "NO match string : %s, use default value [%s]\n",pRegEntry->RegName, (char*)pRegEntry->VarDefault);
            }
        }
        // Handle array parameters
        else if ( PARAM_IntegerArray == pRegEntry->RegType )
        {
            array_num = pRegEntry->VarNum;
            array_size = pRegEntry->VarSize/array_num;
            memset(pField,0xFF,pRegEntry->VarSize);
            if ( isMatch && strlen(value_str))
            {
                pStart = value_str;
                pEnd = value_str;
            }
            else
            {
                if(strlen((char *)pRegEntry->VarDefault) == 0)
                {
                    IOTD_LOG(LOG_TYPE_INFO, "%s: can't set NULL value to an array\n",__func__);
                    return -1;
                }
                pStart = (char *)pRegEntry->VarDefault;
                pEnd = pStart;
            }
            while(*pEnd != '\0' && array_num > 0)
            {
                pEnd ++;
                if(*pEnd == ',' || *pEnd == '\0')
                {
                    array_num--;
                    len_value_str = pEnd - pStart;
                    if (len_value_str > sizeof(data) - 1)
                    {
                        IOTD_LOG(LOG_TYPE_INFO, "%s: Error: data is too large to handle\n",__func__);
                        return -1;
                    }
                    if (len_value_str == 0)
                    {
                        IOTD_LOG(LOG_TYPE_INFO, "%s: Not allow this array format\n",__func__);
                        return -1;
                    }
                    memset(data,0,sizeof(data));
                    memcpy(data, pStart, len_value_str);
                    data[len_value_str] = '\0';
                    i_trim(data);
                    value = strtoul(data, &rv, 0);
                    if(*rv != '\0')
                    {
                        IOTD_LOG(LOG_TYPE_INFO, "%s: Error: Reg paramter %s invalid.\n",
                                __func__, pRegEntry->RegName);
                        return -1;
                    }
                    memcpy( pField, &value, array_size);
                    if (*pEnd == '\0') break;
                    pField += array_size;
                    pStart = pEnd + 1;
                }
            }
            if(*pEnd != '\0')
            {
                IOTD_LOG(LOG_TYPE_INFO, "%s: Waring: too many input values, ignored\n",__func__);
            }
        }
        else
        {
            IOTD_LOG(LOG_TYPE_INFO, "%s: Unknown param type for name[%s] in registry table\n",
                    __func__, pRegEntry->RegName);
        }
    }

    return ret_status;
}


static int apply_config_ini(tCfgIniGroup* iniGroup, unsigned int group_entries,
        tCfgIniEntry* iniTable, unsigned int table_entries, cfg_ini* target)
{
    unsigned long idx;
    int match_idx = -1, ret_status = 0;
    REG_TABLE_GROUP *pRegGroup = g_registry_group;
    unsigned long cRegGroupNum = sizeof(g_registry_group) / sizeof( g_registry_group[ 0 ]);
    unsigned long bitmap = 0;

    // sanity test
    if (cRegGroupNum > MAX_CFG_INI_GROUPS)
    {
        IOTD_LOG(LOG_TYPE_INFO, "%s: ERROR: MAX_CFG_INI_GROUPS is not enough, must be at least %lu\n",__func__, cRegGroupNum);
    }
    if (MAX_CFG_INI_GROUPS > (sizeof(bitmap) * 8))
    {
        IOTD_LOG(LOG_TYPE_INFO, "%s: ERROR: the bits of bitmap should cover MAX_CFG_INI_GROUPS\n",__func__);
    }

    for ( idx = 0; idx < cRegGroupNum;  idx++, pRegGroup++)
    {
        match_idx = find_cfg_group(iniGroup, group_entries, pRegGroup->GroupName, bitmap);

        if ( (match_idx == -1) && ( pRegGroup->Flags & VAR_FLAGS_REQUIRED ))
        {
            IOTD_LOG(LOG_TYPE_INFO, "failed to read a required group %s\n", pRegGroup->GroupName);
            ret_status = -1;
            break;
        }

        if ( match_idx == -1)
        {
            //use default value for this group
            ret_status = apply_config_entry (pRegGroup, NULL, 0, target);
        }
        else
        {
            ret_status = apply_config_entry (pRegGroup, iniTable + iniGroup[match_idx].start_idx,
                    iniGroup[match_idx].count, target);
        }

        if (ret_status != 0)
        {
            IOTD_LOG(LOG_TYPE_INFO, "Error\n");
            break;
        }

        bitmap |= (1<<idx);
    }

    return ret_status;
}

static void print_config_ini(cfg_ini* config_target)
{
    int i,j,array_num,array_size;
    char *source=NULL;
    char cvalue[32];
    long svalue;
    unsigned long uvalue;
    REG_TABLE_GROUP *pRegGroup = g_registry_group;
    unsigned int cRegGroupNum = sizeof(g_registry_group) / sizeof( g_registry_group[ 0 ]);
    REG_TABLE_ENTRY *pRegEntry = NULL;
    unsigned int cRegEntryNum = 0;

    //sanity test
    if ( config_target == NULL || pRegGroup == NULL )
    {
        IOTD_LOG(LOG_TYPE_CRIT, "%s: check failed\n",__func__);
        return;
    }
    IOTD_LOG(LOG_TYPE_INFO, "########## Dump config ini ##########i\n");
    for( i = 0; i < cRegGroupNum; i++, pRegGroup++)
    {
        IOTD_LOG(LOG_TYPE_INFO, "[%s]\n",pRegGroup->GroupName);
        cRegEntryNum = pRegGroup->EntryNumber;
        pRegEntry = pRegGroup->TableEntry;
        for( j = 0; j < cRegEntryNum; j++, pRegEntry++)
        {
            source = (void *)config_target + pRegGroup->GroupOffset + pRegEntry->VarOffset;
            if (pRegEntry->VarSize > (MAX_TOTAL_SQ_NUM * sizeof(int)))
            {
                IOTD_LOG(LOG_TYPE_CRIT, "please increase value buffer\n");
                return;
            }
            switch (pRegEntry->RegType)
            {
                case PARAM_Integer:
                    memcpy(&uvalue, source, pRegEntry->VarSize);
                    IOTD_LOG(LOG_TYPE_INFO, "%s=%lu\n",pRegEntry->RegName,uvalue);
                    break;

                case PARAM_HexInteger:
                    memcpy(&uvalue, source, pRegEntry->VarSize);
                    IOTD_LOG(LOG_TYPE_INFO, "%s=0x%lx\n",pRegEntry->RegName,uvalue);
                    break;

                case PARAM_SignedInteger:
                    memcpy(&svalue, source, pRegEntry->VarSize);
                    IOTD_LOG(LOG_TYPE_INFO, "%s=0x%ld\n",pRegEntry->RegName, svalue);
                    break;

                case PARAM_String:
                    memcpy(cvalue, source, pRegEntry->VarSize);
                    IOTD_LOG(LOG_TYPE_INFO, "%s=%s\n",pRegEntry->RegName, cvalue);
                    break;

                case PARAM_IntegerArray:
                    IOTD_LOG(LOG_TYPE_INFO, "%s=",pRegEntry->RegName);
                    array_num = pRegEntry->VarNum;
                    array_size = pRegEntry->VarSize / array_num;
                    while(array_num > 1)
                    {
                        memcpy(&uvalue, source, array_size);
                        if(uvalue < 255)
                            IOTD_LOG(LOG_TYPE_INFO, "%lu,", uvalue);
                        source += array_size;
                        array_num --;
                    }
                    if (array_num > 0)
                    {
                        memcpy(&uvalue, source, array_size);
                        if(uvalue < 255)
                            IOTD_LOG(LOG_TYPE_INFO, "%lu", uvalue);
                    }
                    IOTD_LOG(LOG_TYPE_INFO, "\n");
                    break;

                default:
                    IOTD_LOG(LOG_TYPE_CRIT, "%s,can't recognize this type\n",__func__);
            }
        }
    }
}

static int validate_config_ini(cfg_ini* config_target)
{
    unsigned int  interface_enable = CONFIG_INTERFACE_DISABLE;
    int i;
    if( config_target == NULL)
    {
        IOTD_LOG(LOG_TYPE_CRIT, "%s: check fail\n",__func__);
        return IOTD_ERROR;
    }
    
    if( config_target->config_system.num_interface < 1 )
    {
        IOTD_LOG(LOG_TYPE_CRIT, "Error: no interface defined\n");
        return IOTD_ERROR;
    }
    
    // check if any interface is enabled
    for( i = 0; i < config_target->config_system.num_interface; i++ )
    {
        interface_enable |= config_target->config_interface[i].enable;
    }

    if( interface_enable == CONFIG_INTERFACE_DISABLE)
    {
        IOTD_LOG(LOG_TYPE_CRIT, "Error: all interfaces are disabled\n");
        return IOTD_ERROR;
    }
    return IOTD_OK;
}

int cfg_init(void* cxt)
{
    IOTD_CXT_T* pCxt = (IOTD_CXT_T*)cxt;

    memset(&(pCxt->cfg), 0, sizeof(cfg_ini));
    if(parse_config_ini(&(pCxt->cfg), pCxt->cfg_file) == 0)
    {

        return validate_config_ini(&(pCxt->cfg));
    }else {
        /*Something went wrong while parsing config file, dump it out*/
        print_config_ini(&(pCxt->cfg));
        return IOTD_ERROR;
    }
}
