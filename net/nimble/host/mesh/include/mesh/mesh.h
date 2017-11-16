/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_H
#define __BT_MESH_H

#include <stddef.h>
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"

#include "glue.h"
#include "access.h"
#include "main.h"
#include "cfg_srv.h"
#include "health_srv.h"

#if MYNEWT_VAL(BLE_MESH_GATT_PROXY)
#include "proxy.h"
#endif

#endif /* __BT_MESH_H */
