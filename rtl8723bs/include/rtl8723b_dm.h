// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTL8723B_DM_H__
#define __RTL8723B_DM_H__
/* ************************************************************
 * Description:
 *
 * This file is for 8723B dynamic mechanism only
 *
 *
 * ************************************************************ */

/* ************************************************************
 * structure and define
 * ************************************************************ */

/* ************************************************************
 * function prototype
 * ************************************************************ */

void rtl8723b_init_dm_priv(PADAPTER padapter);
void rtl8723b_deinit_dm_priv(PADAPTER padapter);

void rtl8723b_InitHalDm(PADAPTER padapter);
void rtl8723b_HalDmWatchDog(PADAPTER padapter);
#endif
