/*-----------------------------------------------------------------------------------------------*/
/**
  @file ql_cell_locator.h
  @brief Common API
*/
/*-----------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd. All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
-------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------
  EDIT HISTORY
  This section contains comments describing changes made to the file.
  Notice that changes are listed in reverse chronological order.
  $Header: $
  when       who          what, where, why
  --------   ---          ----------------------------------------------------------
  20190321   baron.qian  Created .
-------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
 extern "C" {
#endif
#ifndef __QL_CELL_LOCATOR_H__
#define __QL_CELL_LOCATOR_H__

/**/
#define LOCATOR_MAX_ADDRESS_SIZE (256)
#define LOCATOR_MAX_ERR_MSG_SIZE (256)


typedef enum {
    Q_LOC_ADDR_UTF8 = 0,
    Q_LOC_ADDR_GBK,
    Q_LOC_ADDR_ASCII,
    Q_LOC_ADDR_NUM,
}QUECLOCATOR_CHARSET;

typedef struct {
    int err_code;
    char err_msg[LOCATOR_MAX_ERR_MSG_SIZE];
} ql_cell_err;

typedef struct {
    /*longtitude*/
    double lon;
    /*latitude*/
    double lat;
    /*accuracy*/
    unsigned short accuracy;
    /*address charset, not support yet*/
    QUECLOCATOR_CHARSET charset;
    /*address info, not support yet*/
    unsigned char addrinfo[LOCATOR_MAX_ADDRESS_SIZE];
    /*address length, not support yet*/
    unsigned short addrlen;
    /*result*/
    ql_cell_err err;
} ql_cell_resp;


/*-----------------------------------------------------------------------------------------------*/
/**
  @brief cell locator init function, when you want to use cell locator, call this function first!
  @return if success return 0, else return -1
  */
/*-----------------------------------------------------------------------------------------------*/
int ql_cell_locator_init();


/*-----------------------------------------------------------------------------------------------*/
/**
  @brief set locator query server and port, server length must be less than 255 bytes.
  @param[in] server the query server ip address
  @param[in] port the query server port 
  @return if success return 0, else return -1
  */
/*-----------------------------------------------------------------------------------------------*/
int ql_cell_locator_set_server(const char *server, unsigned short port);

/*-----------------------------------------------------------------------------------------------*/
/**
  @brief set locator query timeout, the value must between 1-300 [seconds]
  @param[in] timeout value of query timeout
  @return if success return 0, else return -1
  */
/*-----------------------------------------------------------------------------------------------*/
int ql_cell_locator_set_timeout(unsigned short timeout);


/*-----------------------------------------------------------------------------------------------*/
/**
  @brief set locator query token, token length must be 16 bytes. the token Used to verify that 
         the client accessing the service is valid.
  @param[in] token string of token which want to be setted.
  @param[in] length of token string.
  @return if success return 0, else return -1
  */
/*-----------------------------------------------------------------------------------------------*/
int ql_cell_locator_set_token(const char *token, int len);

/*-----------------------------------------------------------------------------------------------*/
/**
  @brief perform cell locator query 
  @param[out] resp include query result or set the error_msg and error_code
  @return if success return 0, else return -1
  */
/*-----------------------------------------------------------------------------------------------*/
int ql_cell_locator_perform(ql_cell_resp *resp);


#endif /* !__QL_CELL_LOCATOR_H__ */
#ifdef __cplusplus
} 
#endif
