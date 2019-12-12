/*
 *      Copyright:  (C) 2019 quectel
 *                  All rights reserved.
 *
 *       Filename:  btdamo.c
 *    Description:  从NV获取BT地址，在设置BT的MAC. 
 *                 
 *        Version:  1.0.0(2019年11月27日)
 *         Author:  Peeta <peeta.chen@quectel.com>
 *      ChangeLog:  1, Release initial version on "2019年11月27日 18时43分59秒"
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ql-mcm-api/ql_in.h>

#define ATC_REQ_CMD_MAX_LEN     100
#define ATC_RESP_CMD_MAX_LEN    100
#define HCISMD_SET_FILE         "/sys/module/hci_smd/parameters/hcismd_set"

int main (int argc, char **argv)
{
    int i = 0;
    int ret = 0;
    int offset = 0;
    FILE *fp = NULL;
    char *temp = NULL;
    char mac[12] = {0};
    char macstr[17] = {0};
    char bdaddrStr[50] = {0};
    static atc_client_handle_type h_atc = 0;
    char atc_cmd_req[ATC_REQ_CMD_MAX_LEN] = {0};
    char atc_cmd_resp[ATC_RESP_CMD_MAX_LEN] = {0};

    fp = fopen(HCISMD_SET_FILE, "w");
    if (fp == NULL) {
        return -1;
    }

    if (fputs("1", fp) < 0) {
        perror("fputs 1");
    }

    fflush(fp);

    //hciconfig hci0 up
    pid_t status = system("hciconfig hci0 up");
    if (status == -1) {
        perror("hciconfig hci0 up");
    } else {
        if(WIFEXITED(status)){
            if(0 == WEXITSTATUS(status)){
                printf("hciconfig hci0 up successfully\n");
            } else {
                printf("hciconfig hci0 up failed %d \n",WEXITSTATUS(status));
            }
        } else
            printf("hciconfig hci0 up exit code %d \n",WEXITSTATUS(status));
    }


    ret = QL_ATC_Client_Init(&h_atc);
    printf("QL_ATC_Client_Init ret=%d with h_atc=0x%x\n", ret, h_atc);
    if(ret != E_QL_OK){
        return ret;
    }

    memset(atc_cmd_req,  0, sizeof(atc_cmd_req));
    memset(atc_cmd_resp, 0, sizeof(atc_cmd_resp));

    strcpy((char *)atc_cmd_req, "AT+QNVR=447,0");
    ret = QL_ATC_Send_Cmd(h_atc, atc_cmd_req, atc_cmd_resp, ATC_RESP_CMD_MAX_LEN);
    printf("QL_ATC_Send_Cmd \"%s\" ret=%d with resp=\n%s\n", atc_cmd_req, ret, atc_cmd_resp);
    if (ret < 0) {
        return ret;
    }

    ret = QL_ATC_Client_Deinit(h_atc);
    printf("QL_ATC_Client_Deinit ret=%d\n", ret);
    if (ret < 0) {
        return ret;
    }

    temp = strtok(atc_cmd_resp, "\r\n");
    while(temp)
    {
        //printf("%s\n",temp);
        if (strstr(temp, "+QNVR:") != NULL) {
            //printf ("temp:(%s)\n", temp);
            sscanf(temp, "+QNVR:%*[^0-9,A-Z]%[0-9,a-z,A-Z]", mac);
        }
        temp = strtok(NULL,"\n");
    }

    printf("mac:(%s)\n", mac);

    for (i = 0; i < 5; i++) {
        offset += sprintf(macstr + offset, "%c%c:", mac[i*2],mac[i*2 + 1]);
        //printf("macstr:(%s) offset = %d\n", macstr, offset);
    }
    offset += sprintf(macstr + offset, "%c%c", mac[i*2],mac[i*2 + 1]);

    printf("macstr:(%s)\n", macstr);

    //bdaddr -i hci0 5D:C7:68:3D:87:34
    sprintf(bdaddrStr, "bdaddr -i hci0 %s", macstr);

    status = system(bdaddrStr);
    if (status == -1) {
        perror("bdaddr");
    } else {
        if(WIFEXITED(status)){
            if(0 == WEXITSTATUS(status)){
                printf("bdaddr -i hci0 successfully\n");
            } else {
                printf("bdaddr -i hci0 failed %d \n",WEXITSTATUS(status));
            }
        } else
            printf("bdaddr -i hci0 exit code %d \n",WEXITSTATUS(status));
    }

    fseek(fp, 0, SEEK_SET);
    if (fputs("0", fp) < 0) {
        perror("fputs 0");
    }

    fflush(fp);

    sleep(1);

    fseek(fp, 0, SEEK_SET);
    if (fputs("1", fp) < 0) {
        perror("fputs 1");
    }
    fflush(fp);

    fclose(fp);

    return 0;
} /* ----- End of main() ----- */

