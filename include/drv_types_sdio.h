// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __DRV_TYPES_SDIO_H__
#define __DRV_TYPES_SDIO_H__

/* SDIO Header Files */
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>

#define RTW_SDIO_CLK_33M	33000000
#define RTW_SDIO_CLK_40M	40000000
#define RTW_SDIO_CLK_80M	80000000
#define RTW_SDIO_CLK_160M	160000000

typedef struct sdio_data {
	u8  func_number;

	u8  tx_block_mode;
	u8  rx_block_mode;
	u32 block_transfer_len;

	struct sdio_func	*func;
	_thread_hdl_ sys_sdio_irq_thd;
	unsigned int clock;
	unsigned int timing;
	u8	sd3_bus_mode;
} SDIO_DATA, *PSDIO_DATA;

#define dvobj_to_sdio_func(d)	((d)->intf_data.func)

#define RTW_SDIO_ADDR_CMD52_BIT		(1<<17)
#define RTW_SDIO_ADDR_CMD52_GEN(a)	(a | RTW_SDIO_ADDR_CMD52_BIT)
#define RTW_SDIO_ADDR_CMD52_CLR(a)	(a&~RTW_SDIO_ADDR_CMD52_BIT)
#define RTW_SDIO_ADDR_CMD52_CHK(a)	(a&RTW_SDIO_ADDR_CMD52_BIT ? 1 : 0)

#define RTW_SDIO_ADDR_F0_BIT		(1<<18)
#define RTW_SDIO_ADDR_F0_GEN(a)		(a | RTW_SDIO_ADDR_F0_BIT)
#define RTW_SDIO_ADDR_F0_CLR(a)		(a&~RTW_SDIO_ADDR_F0_BIT)
#define RTW_SDIO_ADDR_F0_CHK(a)		(a&RTW_SDIO_ADDR_F0_BIT ? 1 : 0)

#endif
