/**
 * @file i2c_test.c
 * @brief Quectel SC20 Module about i2c example.
 * 
 * @note 
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "ql_i2c.h"

static int bus_number = 0;
static uint8_t slave_addr = 0;
static uint8_t wr_buf[1024] = { 0 };
static uint8_t rd_buf[1024] = { 0 };
static int read_len = 0, write_len = 0;

static const struct option lopts[] = {
	{ "help",      optional_argument, 0, 'h' },
	{ "busnum",    required_argument, 0, 'B' },
	{ "slaveaddr", required_argument, 0, 'S' },
	{ "write",     optional_argument, 0, 'W' },
	{ "read",      optional_argument, 0, 'R' },
};

static void print_usage(const char *prog)
{
	fprintf(stdout, "Usage: %s [-BSWR]\n", prog);
	puts("  -h --help      display help\n"
	     "  -B --busnum    i2c bus number\n"
	     "  -S --slaveaddr i2c slave address\n"
	     "  -W --write     i2c write data\n"
	     "  -R --read      i2c read data length\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	int c;
	char *data;
	int i, index = 0;

	while (1) {
		c = getopt_long(argc, argv, "B:S:W:R:", lopts, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'B':
			bus_number = strtoul(optarg, NULL, 0);
			break;
		case 'S':
			slave_addr = strtoul(optarg, NULL, 0);
			break;
		case 'W':
			while (data = strsep(&optarg, ":"))
				wr_buf[index++] = strtoul(data, NULL, 16);
			write_len = index;
			break;
		case 'R':
			read_len = strtoul(optarg, NULL, 0);
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}

	fprintf(stdout, "%s: bus_number [%d], slave_addr [%#02x]\n",
		argv[0], bus_number, slave_addr);

	fprintf(stdout, "%s: write data length [ %d ]\n", argv[0], index);
	fprintf(stdout, "%s: write data [", argv[0]);
	for (i = 0; i < index; i++)
		fprintf(stdout, " %02x", wr_buf[i]);
	fprintf(stdout, " ]\n");
	fprintf(stdout, "%s: read data length [ %d ]\n", argv[0], read_len);
}


int main(int argc, char *argv[])
{
	int i;
	int ret;

	parse_opts(argc, argv);

	ret = QL_I2c_Set_Bus_Number(bus_number);
	if (ret) {
		fprintf(stderr, "%s: QL_I2c_Set_Bus_Number error, ret [%d]\n", argv[0], ret);
		return -1;
	}

	ret = QL_I2c_Set_Slave_Addr(slave_addr);
	if (ret) {
		fprintf(stderr, "%s: QL_I2c_Set_Slave_Addr error, ret [%d]\n", argv[0], ret);
		return -1;
	}

	ret = QL_I2c_Read_Write(wr_buf, write_len, rd_buf, read_len);
	if (ret) {
		fprintf(stderr, "%s: QL_I2c_Read_Write error, ret [%d]\n", argv[0], ret);
		return -1;
	}

	if (read_len) {
		fprintf(stdout, "%s: read data [", argv[0]);
		for (i = 0; i < read_len; i++)
			fprintf(stdout, " %02x", rd_buf[i]);
		fprintf(stdout, " ]\n");
	}

	fprintf(stdout, "%s: exit succeeded\n", argv[0]);

	return 0;
}
