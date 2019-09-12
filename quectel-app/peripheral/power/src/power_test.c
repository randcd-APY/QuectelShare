#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <unistd.h>

#include <ql_power/ql_power.h>

int main(int argc, char *argv[])
{
	int ret;
	char buf[2048];

	memset(buf, 0, sizeof(buf));
	ret = QL_Power_Info(argv[1], buf);
	if (ret < 0) {
		fprintf(stderr, "%s: QL_Power_Info failed\n", argv[0]);
		return -1;
	}
	fprintf(stdout, "%s\n", buf);

    return 0;
}
