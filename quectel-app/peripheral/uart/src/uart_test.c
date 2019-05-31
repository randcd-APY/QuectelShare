/**
 * @file uart_test.c
 * @brief Quectel SC20 Module about uart example.
 * 
 * @note 
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "ql_uart.h"

int main(int argc, char *argv[])
{
	int ret;
	int read_bytes;
	uint8_t buf[4096];

	if (argc != 3) {
		fprintf(stderr, "usage: %s <port_name> <port_property>\n", argv[0]);
		return -1;
	}

	ret = QL_Uart_Init_Port(argv[1], argv[2]);
	if (ret) {
		fprintf(stderr, "%s: QL_Uart_Init_Port failed\n", argv[0]);
		return -1;
	}

	while (1) {
		read_bytes = QL_Uart_Read_Port(buf);
		if (read_bytes < 0) {
			fprintf(stderr, "%s: QL_Uart_Read_Port failed\n", argv[0]);
			return -1;
		}
		fprintf(stdout, "%s: QL_Uart_Read_Port return %d bytes\n", argv[0], read_bytes);
		ret = QL_Uart_Write_Port(buf, read_bytes);
		if (ret) {
			fprintf(stderr, "%s: QL_Uart_Write_Port failed\n", argv[0]);
			return -1;
		}
		fprintf(stdout, "%s: QL_Uart_Write_Port write %d bytes\n", argv[0], read_bytes);
	}

	return 0;
}
