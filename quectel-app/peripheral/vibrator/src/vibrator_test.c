#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <unistd.h>
#include "ql_vibrator.h"

int main(int argc, char **argv)
{
	int ret;
	int time = 0;

	sscanf(argv[1], "%d", &time);
	fprintf(stdout, "%: set vibrator %d ms\n", argv[0], time);
	ret = QL_Set_Vibrator(time);
	if (ret < 0) {
		fprintf(stderr, "%s: QL_Set_Vibrator failed\n", argv[0]);
		return -1;
	}
	fprintf(stdout, "%: set vibrator succeeded\n", argv[0]);

    return 0;
}
