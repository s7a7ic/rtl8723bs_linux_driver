// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "../phydm_precomp.h"

#if (RTL8723B_SUPPORT == 1)

s8
odm_CCKRSSI_8723B(
	u8	LNA_idx,
	u8	VGA_idx
)
{
	s8	rx_pwr_all = 0x00;
	switch (LNA_idx) {
	/* 46  53 73 95 201301231630 */
	/* 46 53 77 99 201301241630 */

	case 6:
		rx_pwr_all = -40 - (2 * VGA_idx);
		break;
	case 4:
		rx_pwr_all = -20 - (2 * VGA_idx);
		break;
	case 1:
		rx_pwr_all = 0 - (2 * VGA_idx);
		break;
	case 0:
		rx_pwr_all = 10 - (2 * VGA_idx);
		break;
	default:
		/*rx_pwr_all = -53+(2*(31-VGA_idx));*/
		/*dbg_print("wrong LNA index\n");*/
		break;

	}
	return	rx_pwr_all;
}

#endif /* end if RTL8723B */
