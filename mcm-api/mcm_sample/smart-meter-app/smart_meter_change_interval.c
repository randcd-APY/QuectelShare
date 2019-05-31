/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "smart_meter_app.h"
/* variables used for shared memory segment */
int shmid;
key_t key;
char *shm;
key = 2601;

int main()
{
    int new_time_interval;
    printf ("Enter value of new time interval in seconds \n");
    scanf ("%d", &new_time_interval);

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0)     /*Locate the segment*/
    {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)    /* attach segment to data space */
    {
        perror("shmat");
        exit(1);
    }

    printf("Current time interval = %d\n",atoi(shm));
    snprintf(shm,sizeof(shm),"%d",new_time_interval);    /* write new time interval to shared memory */
    printf("\nNew time interval is set to %d seconds\n",atoi(shm));

    return 0;
}
