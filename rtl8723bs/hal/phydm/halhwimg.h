// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#pragma once
#ifndef __INC_HW_IMG_H
#define __INC_HW_IMG_H

/* 2011/03/15 MH Add for different IC HW image file selection. code size consideration. */
#if RT_PLATFORM == PLATFORM_LINUX

	#if (DEV_BUS_TYPE == RT_SDIO_INTERFACE)
		/* For 8723 */
		#define	RTL8723E_HWIMG_SUPPORT					0
		#define	RTL8723U_HWIMG_SUPPORT					0
		#define	RTL8723S_HWIMG_SUPPORT					1
	#endif

#endif

#endif /* __INC_HW_IMG_H */
