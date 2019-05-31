/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "config_file_handling.h"

void config_file_read_intialize_variables_home_security(char *config_filename,home_security_config_struct *home_security_config)
{
	printf ("Configuration File to be read = %s\n",config_filename);

	char *search = "=";
	char *key;
	char *value;

	FILE *fp;	/*file pointer variable */
	fp=fopen(config_filename,"r"); /* open a file for reading */

	if (fp == NULL)
	{
		fprintf(stderr, "Can't open file %s!\n", config_filename);
		exit(1);
	}
	if(fp!= NULL )
	{
		char line [128];
		while (fgets(line,sizeof line,fp) != NULL) /* read a line */
		{
			key = strtok(line, search); /*Key will point to the part before the = */
			value=strtok(NULL, search); /*Value will point to the part after = */

			if(key == NULL || value == NULL){
				continue;
			}

			if (strcmp(key,"ServerID")== 0)
				strcpy(home_security_config->host_name,value);
			else if (strcmp(key,"Phone")== 0)
				strcpy(home_security_config->phone_number,value);
			else if (strcmp(key,"WaitDuration")== 0)
				home_security_config->wait_time=atoi(value);
			else if (strcmp(key,"Port")== 0)
				home_security_config->port=atoi(value);
		}
	}
	fclose(fp); /* close the file */
}