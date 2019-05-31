/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef QAPI_HOST_COMMON_H
#define QAPI_HOST_COMMON_H

#define QDEV(n,qapiFunc,...) _##qapiFunc(n, ##__VA_ARGS__)

#endif /* QAPI_HOST_COMMON_H */