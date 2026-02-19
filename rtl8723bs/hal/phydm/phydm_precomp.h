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
#include "halrf/halrf_features.h"

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#include "Precomp.h"		/* We need to include mp_precomp.h due to batch file setting. */
#else
	#define		TEST_FALG___		1
#endif

/* 2 Config Flags and Structs - defined by each ODM type */

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	#include "../8192cd_cfg.h"
	#include "../odm_inc.h"

	#include "../8192cd.h"
	#include "../8192cd_util.h"
	#ifdef _BIG_ENDIAN_
		#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_BIG
	#else
		#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_LITTLE
	#endif

	#include "../8192cd_headers.h"
	#include "../8192cd_debug.h"

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	#ifdef DM_ODM_CE_MAC80211
		#include "../wifi.h"
		#include "rtl_phydm.h"
	#endif
	#define __PACK
	#define __WLAN_ATTRIB_PACK__
#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#include "mp_precomp.h"
	#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_LITTLE
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

#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	#define RTL8703B_SUPPORT		0
	#define RTL8188F_SUPPORT		0
	#define RTL8723D_SUPPORT		0
#endif

#if (RTL8723B_SUPPORT == 1)
	#include "rtl8723b/halhwimg8723b_mac.h"
	#include "rtl8723b/halhwimg8723b_rf.h"
	#include "rtl8723b/halhwimg8723b_bb.h"
	#include "rtl8723b/phydm_regconfig8723b.h"
	#include "rtl8723b/phydm_rtl8723b.h"
	#include "rtl8723b/hal8723breg.h"
	#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		#include "halrf/rtl8723b/halrf_8723b_win.h"
	#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
		#include "halrf/rtl8723b/halrf_8723b_ce.h"
		#include "rtl8723b/halhwimg8723b_mp.h"
		#include "rtl8723b_hal.h"
	#else
		#include "halrf/rtl8723b/halrf_8723b_ap.h"
	#endif
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_CE) && defined(DM_ODM_CE_MAC80211)
#include "../halmac/halmac_reg2.h"

#define	LDPC_HT_ENABLE_RX			BIT(0)
#define	LDPC_HT_ENABLE_TX			BIT(1)
#define	LDPC_HT_TEST_TX_ENABLE			BIT(2)
#define	LDPC_HT_CAP_TX				BIT(3)

#define	STBC_HT_ENABLE_RX			BIT(0)
#define	STBC_HT_ENABLE_TX			BIT(1)
#define	STBC_HT_TEST_TX_ENABLE			BIT(2)
#define	STBC_HT_CAP_TX				BIT(3)


#define	LDPC_VHT_ENABLE_RX			BIT(0)
#define	LDPC_VHT_ENABLE_TX			BIT(1)
#define	LDPC_VHT_TEST_TX_ENABLE			BIT(2)
#define	LDPC_VHT_CAP_TX				BIT(3)

#define	STBC_VHT_ENABLE_RX			BIT(0)
#define	STBC_VHT_ENABLE_TX			BIT(1)
#define	STBC_VHT_TEST_TX_ENABLE			BIT(2)
#define	STBC_VHT_CAP_TX				BIT(3)
#endif

#endif /* __ODM_PRECOMP_H__ */
