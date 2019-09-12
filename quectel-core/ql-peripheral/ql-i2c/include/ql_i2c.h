#ifndef _QL_I2C_H_
#define _QL_I2C_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int QL_I2c_Set_Bus_Number(uint32_t num);
int QL_I2c_Set_Slave_Addr(uint8_t addr);
int QL_I2c_Read_Write(uint8_t *write_buf, uint32_t write_len,
	uint8_t *read_buf, uint32_t read_len);

#ifdef __cplusplus
}
#endif

#endif
