// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifdef CONFIG_RTL8723B
#ifndef _FW_HEADER_8723B_H
#define _FW_HEADER_8723B_H

#ifdef LOAD_FW_HEADER_FROM_DRIVER

#if (DM_ODM_SUPPORT_TYPE & (ODM_CE))
extern u8 array_mp_8723b_fw_nic[32272];
extern u32 array_length_mp_8723b_fw_nic;
extern u8 array_mp_8723b_fw_wowlan[27432];
extern u32 array_length_mp_8723b_fw_wowlan;
#endif
#endif /* end of LOAD_FW_HEADER_FROM_DRIVER*/

#endif
#endif /* end of HWIMG_SUPPORT*/
