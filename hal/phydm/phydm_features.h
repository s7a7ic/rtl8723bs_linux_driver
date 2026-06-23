/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#ifndef	__PHYDM_FEATURES_H__
#define __PHYDM_FEATURES

#define ODM_DC_CANCELLATION_SUPPORT		(ODM_RTL8188F | ODM_RTL8710B)
#define ODM_RECEIVER_BLOCKING_SUPPORT	(ODM_RTL8188E | ODM_RTL8192E)

#define PHYDM_LA_MODE_SUPPORT			0

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	#include	"phydm_features_ce.h"
#endif

#endif
