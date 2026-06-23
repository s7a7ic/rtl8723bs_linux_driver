/// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef	__ODM_PRECOMP_H__
#define __ODM_PRECOMP_H__

#include "phydm_types.h"
#include "phydm_features.h"

/* 2 Config Flags and Structs - defined by each ODM type */

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	#define __PACK
	#define __WLAN_ATTRIB_PACK__
#endif

/* 2 OutSrc Header Files */

#include "phydm.h"
#include "phydm_hwconfig.h"
#include "phydm_phystatus.h"
#include "phydm_debug.h"
#include "phydm_regdefine11ac.h"
#include "phydm_regdefine11n.h"
#include "phydm_interface.h"
#include "phydm_reg.h"

#if (DM_ODM_SUPPORT_TYPE & ODM_CE) && !defined(DM_ODM_CE_MAC80211)

void
phy_set_tx_power_limit(
	struct PHY_DM_STRUCT	*p_dm,
	u8	*regulation,
	u8	*band,
	u8	*bandwidth,
	u8	*rate_section,
	u8	*rf_path,
	u8	*channel,
	u8	*power_limit
);

enum hal_status
rtw_phydm_fw_iqk(
	struct PHY_DM_STRUCT	*p_dm,
	u8 clear,
	u8 segment
);

enum hal_status
rtw_phydm_cfg_phy_para(
	struct PHY_DM_STRUCT	*p_dm,
	enum phydm_halmac_param config_type,
	u32 offset,
	u32 data,
	u32 mask,
	enum rf_path e_rf_path,
	u32 delay_time
);

#endif

#if (RTL8723B_SUPPORT == 1)
	#include "rtl8723b/halhwimg8723b_mac.h"
	#include "rtl8723b/halhwimg8723b_rf.h"
	#include "rtl8723b/halhwimg8723b_bb.h"
	#include "rtl8723b/phydm_regconfig8723b.h"
	#include "rtl8723b/phydm_rtl8723b.h"
	#include "rtl8723b/hal8723breg.h"
	#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
		#include "halrf/rtl8723b/halrf_8723b_ce.h"
		#include "rtl8723b_hal.h"
	#endif
#endif

#endif /* __ODM_PRECOMP_H__ */
