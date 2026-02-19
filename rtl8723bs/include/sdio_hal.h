// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __SDIO_HAL_H__
#define __SDIO_HAL_H__

void sd_int_dpc(PADAPTER padapter);
u8 rtw_set_hal_ops(_adapter *padapter);

#ifdef CONFIG_RTL8188E
void rtl8188es_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8723B
void rtl8723bs_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8821A
void rtl8821as_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8192E
void rtl8192es_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8703B
void rtl8703bs_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8723D
void rtl8723ds_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8188F
void rtl8188fs_set_hal_ops(PADAPTER padapter);
#endif

#endif /* __SDIO_HAL_H__ */
