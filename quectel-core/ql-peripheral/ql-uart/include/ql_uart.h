#ifndef _QL_UART_H_
#define _QL_UART_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int QL_Uart_Init_Port(const char *port_name, const char *port_property);
int QL_Uart_Write_Port(const char *write_buf, uint32_t write_len);
int QL_Uart_Read_Port(char *read_buf);
void QL_Uart_Close_Port(void);

#ifdef __cplusplus
}
#endif

#endif
