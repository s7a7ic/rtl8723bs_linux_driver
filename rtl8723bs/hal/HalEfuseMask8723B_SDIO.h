// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/

/******************************************************************************
*                           MSDIO.TXT
******************************************************************************/

u2Byte EFUSE_GetArrayLen_MP_8723B_MSDIO(VOID);

VOID EFUSE_GetMaskArray_MP_8723B_MSDIO(IN OUT pu1Byte Array);

/* TC: Test Chip, MP: MP Chip */
BOOLEAN EFUSE_IsAddressMasked_MP_8723B_MSDIO( IN u2Byte Offset);
