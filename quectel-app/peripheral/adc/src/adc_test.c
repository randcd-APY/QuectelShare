#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <unistd.h>
#include "ql_adc.h"

int main(int argc, char *argv[])
{
	int ret;
	int adc_val;

	ret = QL_Get_Adc_Voltage(&adc_val);
 	if (ret < 0) {
		fprintf(stderr, "%s: QL_Get_Adc_Voltage failed\n", argv[0]);
		return -1;
	} else
		printf("adc voltage %d uV\n",adc_val);

	return 0;
}
