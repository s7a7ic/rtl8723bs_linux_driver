// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef _RTL8723B_SRESET_H_
#define _RTL8723B_SRESET_H_

#include <rtw_sreset.h>

#ifdef DBG_CONFIG_ERROR_DETECT
	extern void rtl8723b_sreset_xmit_status_check(_adapter *padapter);
	extern void rtl8723b_sreset_linked_status_check(_adapter *padapter);
#endif
#endif
