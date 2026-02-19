// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __PCI_HAL_H__
#define __PCI_HAL_H__

#ifdef CONFIG_RTL8188E
	void rtl8188ee_set_hal_ops(_adapter *padapter);
#endif

#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
	void rtl8812ae_set_hal_ops(_adapter *padapter);
#endif

#if defined(CONFIG_RTL8192E)
	void rtl8192ee_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8723B
	void rtl8723be_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8723D
	void rtl8723de_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8814A
	void rtl8814ae_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8822B
	void rtl8822be_set_hal_ops(PADAPTER padapter);
#endif

u8 rtw_set_hal_ops(_adapter *padapter);

#endif /* __PCIE_HAL_H__ */
