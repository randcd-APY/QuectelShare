#ifndef _QL_GPIO_H_
#define _QL_GPIO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int Gpio_Max_Number(void);
int Gpio_Valid(uint32_t gpio);
int QL_Gpio_Set_Config(uint32_t gpio, uint32_t cfg_val);
int QL_Gpio_Get_Config(uint32_t gpio, uint32_t *gpio_cfg);
int QL_Gpio_Set_Level(uint32_t gpio, uint32_t level);
int QL_Gpio_Get_Level(uint32_t gpio, uint32_t *level);

#ifdef __cplusplus
}
#endif

#endif
