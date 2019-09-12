/**
 * @file gpio_test.c
 * @brief Quectel SC20 Module about gpio example.
 * 
 * @note 
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ql_gpio/ql_gpio.h>

int main(int argc, char *argv[])
{
	char buf[64];
	uint32_t gpio_number, cfg_val, inout_val;
	uint32_t dir_index, drv_strength_index, bias_index;
	char *dir[2] = { "in", "out" };
	char *drv_strength[8] = {
		"2mA",  "4mA",  "6mA",  "8mA",
		"10mA", "12mA", "14mA", "16mA",
	};
	char *bias[4] = { "no-pull", "weak-pull-down", "weak-keeper", "weak-pull-up" };

	if (argc < 2 || argc > 4) {
		fprintf(stderr, "usage: %s <gpio_number> [gpio_cfg] [gpio_level]\n", argv[0]);
		return -1;
	}

	sscanf(argv[1], "%d", &gpio_number);
	if (!Gpio_Valid(gpio_number)) {
		fprintf(stderr, "%s: invalid gpio number: %d\n", argv[0], gpio_number);
		return -1;
	}

	fprintf(stdout, "gpio number: %d\n", gpio_number);

	if (argc == 2) {          // gpio status test
		QL_Gpio_Get_Config(gpio_number, &cfg_val);
		QL_Gpio_Get_Level(gpio_number, &inout_val);
		dir_index = (cfg_val >> 9) & 1;
		drv_strength_index = (cfg_val >> 6) & 7;
		bias_index = cfg_val & 3;

		memset(buf, 0, sizeof(buf));
		fprintf(stdout, "%s:%s:%s:%d\n",
			dir[dir_index],
			drv_strength[drv_strength_index],
			bias[bias_index],
			inout_val);
	} else if (argc == 3) {
		sscanf(argv[2], "%x", &cfg_val);
		fprintf(stdout, "gpio configuration: %#x\n", cfg_val);
		QL_Gpio_Set_Config(gpio_number, cfg_val);
	} else if (argc == 4) {   // gpio output test
		sscanf(argv[2], "%x", &cfg_val);
		sscanf(argv[3], "%x", &inout_val);
		fprintf(stdout, "gpio configuration: %#x, out level: %d\n",
			cfg_val, inout_val);
		QL_Gpio_Set_Config(gpio_number, cfg_val);
		QL_Gpio_Set_Level(gpio_number, !!inout_val);
	}

	return 0;
}
