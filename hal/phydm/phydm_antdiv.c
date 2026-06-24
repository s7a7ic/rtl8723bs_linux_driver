// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "phydm_precomp.h"

/* ******************************************************
 * when antenna test utility is on or some testing need to disable antenna diversity
 * call this function to disable all ODM related mechanisms which will switch antenna.
 * ****************************************************** */
void
odm_stop_antenna_switch_dm(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	/* disable ODM antenna diversity */
	p_dm->support_ability &= ~ODM_BB_ANT_DIV;
	odm_ant_div_on_off(p_dm, ANTDIV_OFF);
	odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("STOP Antenna Diversity\n"));
}

void
phydm_enable_antenna_diversity(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	p_dm->support_ability |= ODM_BB_ANT_DIV;
	p_dm->antdiv_select = 0;
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("AntDiv is enabled & Re-Init AntDiv\n"));
	odm_antenna_diversity_init(p_dm);
}

void
odm_set_ant_config(
	void	*p_dm_void,
	u8		ant_setting	/* 0=A, 1=B, 2=C, .... */
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	if (p_dm->support_ic_type == ODM_RTL8723B) {
		if (ant_setting == 0)		/* ant A*/
			odm_set_bb_reg(p_dm, 0x948, MASKDWORD, 0x00000000);
		else if (ant_setting == 1)
			odm_set_bb_reg(p_dm, 0x948, MASKDWORD, 0x00000280);
	} else if (p_dm->support_ic_type == ODM_RTL8723D) {
		if (ant_setting == 0)		/* ant A*/
			odm_set_bb_reg(p_dm, 0x948, MASKLWORD, 0x0000);
		else if (ant_setting == 1)
			odm_set_bb_reg(p_dm, 0x948, MASKLWORD, 0x0280);
	}
}

/* ****************************************************** */


void
odm_sw_ant_div_rest_after_link(
	void		*p_dm_void
)
{
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _sw_antenna_switch_		*p_dm_swat_table = &p_dm->dm_swat_table;
	struct phydm_fat_struct		*p_dm_fat_table = &p_dm->dm_fat_table;
	u32	i;

	if (p_dm->ant_div_type == S0S1_SW_ANTDIV) {

		p_dm_swat_table->try_flag = SWAW_STEP_INIT;
		p_dm_swat_table->rssi_trying = 0;
		p_dm_swat_table->double_chk_flag = 0;
		p_dm_fat_table->rx_idle_ant = MAIN_ANT;

		for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
			phydm_antdiv_reset_statistic(p_dm, i);
	}
	
#endif	
}

void
odm_ant_div_on_off(
	void		*p_dm_void,
	u8		swch
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;

	if (p_dm_fat_table->ant_div_on_off != swch) {
		if (p_dm->ant_div_type == S0S1_SW_ANTDIV)
			return;

		if (p_dm->support_ic_type & ODM_N_ANTDIV_SUPPORT) {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("(( Turn %s )) N-Series HW-AntDiv block\n", (swch == ANTDIV_ON) ? "ON" : "OFF"));
			odm_set_bb_reg(p_dm, 0xc50, BIT(7), swch);
			odm_set_bb_reg(p_dm, 0xa00, BIT(15), swch);

		} else if (p_dm->support_ic_type & ODM_AC_ANTDIV_SUPPORT) {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("(( Turn %s )) AC-Series HW-AntDiv block\n", (swch == ANTDIV_ON) ? "ON" : "OFF"));
			if (p_dm->support_ic_type & ODM_RTL8812) {
				odm_set_bb_reg(p_dm, 0xc50, BIT(7), swch); /* OFDM AntDiv function block enable */
				odm_set_bb_reg(p_dm, 0xa00, BIT(15), swch); /* CCK AntDiv function block enable */
			} else {
				odm_set_bb_reg(p_dm, 0x8D4, BIT(24), swch); /* OFDM AntDiv function block enable */

				if ((p_dm->cut_version >= ODM_CUT_C) && (p_dm->support_ic_type == ODM_RTL8821) && (p_dm->ant_div_type != S0S1_SW_ANTDIV)) {
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("(( Turn %s )) CCK HW-AntDiv block\n", (swch == ANTDIV_ON) ? "ON" : "OFF"));
					odm_set_bb_reg(p_dm, 0x800, BIT(25), swch);
					odm_set_bb_reg(p_dm, 0xA00, BIT(15), swch); /* CCK AntDiv function block enable */
				} else if (p_dm->support_ic_type == ODM_RTL8821C) {
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("(( Turn %s )) CCK HW-AntDiv block\n", (swch == ANTDIV_ON) ? "ON" : "OFF"));
					odm_set_bb_reg(p_dm, 0x800, BIT(25), swch);
					odm_set_bb_reg(p_dm, 0xA00, BIT(15), swch); /* CCK AntDiv function block enable */
				}
			}
		}
	}
	p_dm_fat_table->ant_div_on_off = swch;

}

void
odm_tx_by_tx_desc_or_reg(
	void		*p_dm_void,
	u8			swch
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;
	u8 enable;

	if (p_dm_fat_table->b_fix_tx_ant == NO_FIX_TX_ANT)
		enable = (swch == TX_BY_DESC) ? 1 : 0;
	else
		enable = 0;/*Force TX by Reg*/

	if (p_dm->ant_div_type != CGCS_RX_HW_ANTDIV) {
		if (p_dm->support_ic_type & ODM_N_ANTDIV_SUPPORT)
			odm_set_bb_reg(p_dm, 0x80c, BIT(21), enable);
		else if (p_dm->support_ic_type & ODM_AC_ANTDIV_SUPPORT)
			odm_set_bb_reg(p_dm, 0x900, BIT(18), enable);

		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[AntDiv] TX_Ant_BY (( %s ))\n", (enable == TX_BY_DESC) ? "DESC" : "REG"));
	}
}

#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
void
phydm_antdiv_reset_statistic(
	void	*p_dm_void,
	u32	macid
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct		*p_dm_fat_table = &p_dm->dm_fat_table;

	p_dm_fat_table->main_ant_sum[macid] = 0;
	p_dm_fat_table->aux_ant_sum[macid] = 0;
	p_dm_fat_table->main_ant_cnt[macid] = 0;
	p_dm_fat_table->aux_ant_cnt[macid] = 0;
	p_dm_fat_table->main_ant_sum_cck[macid] = 0;
	p_dm_fat_table->aux_ant_sum_cck[macid] = 0;
	p_dm_fat_table->main_ant_cnt_cck[macid] = 0;
	p_dm_fat_table->aux_ant_cnt_cck[macid] = 0;
}

void
phydm_fast_training_enable(
	void		*p_dm_void,
	u8			swch
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	u8			enable;

	if (swch == FAT_ON)
		enable = 1;
	else
		enable = 0;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Fast ant Training_en = ((%d))\n", enable));

	if (p_dm->support_ic_type == ODM_RTL8188E) {
		odm_set_bb_reg(p_dm, 0xe08, BIT(16), enable);	/*enable fast training*/
		/**/
	} else if (p_dm->support_ic_type == ODM_RTL8192E) {
		odm_set_bb_reg(p_dm, 0xB34, BIT(28), enable);	/*enable fast training (path-A)*/
		/*odm_set_bb_reg(p_dm, 0xB34, BIT(29), enable);*/	/*enable fast training (path-B)*/
	} else if (p_dm->support_ic_type & (ODM_RTL8821 | ODM_RTL8822B)) {
		odm_set_bb_reg(p_dm, 0x900, BIT(19), enable);	/*enable fast training */
		/**/
	}
}

void
phydm_keep_rx_ack_ant_by_tx_ant_time(
	void		*p_dm_void,
	u32		time
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	/* Timming issue: keep Rx ant after tx for ACK ( time x 3.2 mu sec)*/
	if (p_dm->support_ic_type & ODM_N_ANTDIV_SUPPORT) {

		odm_set_bb_reg(p_dm, 0xE20, BIT(23) | BIT(22) | BIT(21) | BIT(20), time);
		/**/
	} else if (p_dm->support_ic_type & ODM_AC_ANTDIV_SUPPORT) {

		odm_set_bb_reg(p_dm, 0x818, BIT(23) | BIT(22) | BIT(21) | BIT(20), time);
		/**/
	}
}

void
odm_update_rx_idle_ant(
	void		*p_dm_void,
	u8		ant
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct			*p_dm_fat_table = &p_dm->dm_fat_table;
	u32			default_ant, optional_ant, value32, default_tx_ant;

	if (p_dm_fat_table->rx_idle_ant != ant) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] rx_idle_ant =%s\n", (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"));

		if (!(p_dm->support_ic_type & ODM_RTL8723B))
			p_dm_fat_table->rx_idle_ant = ant;

		if (ant == MAIN_ANT) {
			default_ant   =  ANT1_2G;
			optional_ant =  ANT2_2G;
		} else {
			default_ant  =   ANT2_2G;
			optional_ant =  ANT1_2G;
		}

		if (p_dm_fat_table->b_fix_tx_ant != NO_FIX_TX_ANT)
			default_tx_ant = (p_dm_fat_table->b_fix_tx_ant == FIX_TX_AT_MAIN) ? 0 : 1;
		else
			default_tx_ant = default_ant;

		if (p_dm->support_ic_type & ODM_N_ANTDIV_SUPPORT) {
			if (p_dm->support_ic_type == ODM_RTL8192E) {
				odm_set_bb_reg(p_dm, 0xB38, BIT(5) | BIT(4) | BIT(3), default_ant); /* Default RX */
				odm_set_bb_reg(p_dm, 0xB38, BIT(8) | BIT(7) | BIT(6), optional_ant); /* Optional RX */
				odm_set_bb_reg(p_dm, 0x860, BIT(14) | BIT(13) | BIT(12), default_ant); /* Default TX */
			}
#if (RTL8723B_SUPPORT == 1)
			else if (p_dm->support_ic_type == ODM_RTL8723B) {

				value32 = odm_get_bb_reg(p_dm, 0x948, 0xFFF);

				if (value32 != 0x280)
					odm_update_rx_idle_ant_8723b(p_dm, ant, default_ant, optional_ant);
				else
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: Fail to set RX antenna due to 0x948 = 0x280\n"));
			}
#endif

			else { /*8188E & 8188F*/

				odm_set_bb_reg(p_dm, 0x864, BIT(5) | BIT(4) | BIT(3), default_ant);		/*Default RX*/
				odm_set_bb_reg(p_dm, 0x864, BIT(8) | BIT(7) | BIT(6), optional_ant);	/*Optional RX*/
				odm_set_bb_reg(p_dm, 0x860, BIT(14) | BIT(13) | BIT(12), default_tx_ant);	/*Default TX*/
			}
		} else if (p_dm->support_ic_type & ODM_AC_ANTDIV_SUPPORT) {
			u16	value16 = odm_read_2byte(p_dm, ODM_REG_TRMUX_11AC + 2);
			/*  */
			/* 2014/01/14 MH/Luke.Lee Add direct write for register 0xc0a to prevnt */
			/* incorrect 0xc08 bit0-15 .We still not know why it is changed. */
			/*  */
			value16 &= ~(BIT(11) | BIT(10) | BIT(9) | BIT(8) | BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3));
			value16 |= ((u16)default_ant << 3);
			value16 |= ((u16)optional_ant << 6);
			value16 |= ((u16)default_ant << 9);
			odm_write_2byte(p_dm, ODM_REG_TRMUX_11AC + 2, value16);
#if 0
			odm_set_bb_reg(p_dm, ODM_REG_TRMUX_11AC, BIT(21) | BIT20 | BIT19, default_ant);	 /* Default RX */
			odm_set_bb_reg(p_dm, ODM_REG_TRMUX_11AC, BIT(24) | BIT23 | BIT22, optional_ant); /* Optional RX */
			odm_set_bb_reg(p_dm, ODM_REG_TRMUX_11AC, BIT(27) | BIT26 | BIT25, default_ant);	 /* Default TX */
#endif
		}

		if (p_dm->support_ic_type & (ODM_RTL8821C | ODM_RTL8822B | ODM_RTL8814A)) {
			odm_set_mac_reg(p_dm, 0x6D8, 0x7, default_tx_ant);		/*PathA Resp Tx*/
			/**/
		} else if (p_dm->support_ic_type == ODM_RTL8188E) {
			odm_set_mac_reg(p_dm, 0x6D8, BIT(7) | BIT(6), default_tx_ant);		/*PathA Resp Tx*/
			/**/
		} else {
			odm_set_mac_reg(p_dm, 0x6D8, BIT(10) | BIT(9) | BIT(8), default_tx_ant);	/*PathA Resp Tx*/
			/**/
		}

	} else { /* p_dm_fat_table->rx_idle_ant == ant */
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Stay in Ori-ant ]  rx_idle_ant =%s\n", (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"));
		p_dm_fat_table->rx_idle_ant = ant;
	}
}

void
phydm_set_antdiv_val(
	void			*p_dm_void,
	u32			*val_buf,
	u8			val_len
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	if (val_len != 1) {
		PHYDM_DBG(p_dm, ODM_COMP_API, ("[Error][antdiv]Need val_len=1\n"));
		return;
	}
	
	odm_update_rx_idle_ant(p_dm, (u8)(*val_buf));
}

void
odm_update_tx_ant(
	void		*p_dm_void,
	u8		ant,
	u32		mac_id
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;
	u8	tx_ant;

	if (p_dm_fat_table->b_fix_tx_ant != NO_FIX_TX_ANT)
		ant = (p_dm_fat_table->b_fix_tx_ant == FIX_TX_AT_MAIN) ? MAIN_ANT : AUX_ANT;

	if (p_dm->ant_div_type == CG_TRX_SMART_ANTDIV)
		tx_ant = ant;
	else {
		if (ant == MAIN_ANT)
			tx_ant = ANT1_2G;
		else
			tx_ant = ANT2_2G;
	}

	p_dm_fat_table->antsel_a[mac_id] = tx_ant & BIT(0);
	p_dm_fat_table->antsel_b[mac_id] = (tx_ant & BIT(1)) >> 1;
	p_dm_fat_table->antsel_c[mac_id] = (tx_ant & BIT(2)) >> 2;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Set TX-DESC value]: mac_id:(( %d )),  tx_ant = (( %s ))\n", mac_id, (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"));
	/* PHYDM_DBG(p_dm,DBG_ANT_DIV,("antsel_tr_mux=(( 3'b%d%d%d ))\n",p_dm_fat_table->antsel_c[mac_id] , p_dm_fat_table->antsel_b[mac_id] , p_dm_fat_table->antsel_a[mac_id] )); */

}

#if (RTL8723B_SUPPORT == 1)
void
odm_trx_hw_ant_div_init_8723b(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***8723B AntDiv_Init =>  ant_div_type=[CG_TRX_HW_ANTDIV(DPDT)]\n"));

	/* Mapping Table */
	odm_set_bb_reg(p_dm, 0x914, MASKBYTE0, 0);
	odm_set_bb_reg(p_dm, 0x914, MASKBYTE1, 1);

	/* OFDM HW AntDiv Parameters */
	odm_set_bb_reg(p_dm, 0xCA4, 0x7FF, 0xa0); /* thershold */
	odm_set_bb_reg(p_dm, 0xCA4, 0x7FF000, 0x00); /* bias */

	/* CCK HW AntDiv Parameters */
	odm_set_bb_reg(p_dm, 0xA74, BIT(7), 1); /* patch for clk from 88M to 80M */
	odm_set_bb_reg(p_dm, 0xA0C, BIT(4), 1); /* do 64 samples */

	/* BT Coexistence */
	odm_set_bb_reg(p_dm, 0x864, BIT(12), 0); /* keep antsel_map when GNT_BT = 1 */
	odm_set_bb_reg(p_dm, 0x874, BIT(23), 0); /* Disable hw antsw & fast_train.antsw when GNT_BT=1 */

	/* Output Pin Settings */
	odm_set_bb_reg(p_dm, 0x870, BIT(8), 0);

	odm_set_bb_reg(p_dm, 0x948, BIT(6), 0); /* WL_BB_SEL_BTG_TRXG_anta,  (1: HW CTRL  0: SW CTRL) */
	odm_set_bb_reg(p_dm, 0x948, BIT(7), 0);

	odm_set_mac_reg(p_dm, 0x40, BIT(3), 1);
	odm_set_mac_reg(p_dm, 0x38, BIT(11), 1);
	odm_set_mac_reg(p_dm, 0x4C,  BIT(24) | BIT(23), 2); /* select DPDT_P and DPDT_N as output pin */

	odm_set_bb_reg(p_dm, 0x944, BIT(0) | BIT(1), 3); /* in/out */
	odm_set_bb_reg(p_dm, 0x944, BIT(31), 0);

	odm_set_bb_reg(p_dm, 0x92C, BIT(1), 0); /* DPDT_P non-inverse */
	odm_set_bb_reg(p_dm, 0x92C, BIT(0), 1); /* DPDT_N inverse */

	odm_set_bb_reg(p_dm, 0x930, 0xF0, 8); /* DPDT_P = ANTSEL[0] */
	odm_set_bb_reg(p_dm, 0x930, 0xF, 8); /* DPDT_N = ANTSEL[0] */

	/* 2 [--For HW Bug setting] */
	if (p_dm->ant_type == ODM_AUTO_ANT)
		odm_set_bb_reg(p_dm, 0xA00, BIT(15), 0); /* CCK AntDiv function block enable */

}



void
odm_s0s1_sw_ant_div_init_8723b(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _sw_antenna_switch_		*p_dm_swat_table = &p_dm->dm_swat_table;
	struct phydm_fat_struct		*p_dm_fat_table = &p_dm->dm_fat_table;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***8723B AntDiv_Init => ant_div_type=[ S0S1_SW_AntDiv]\n"));

	/* Mapping Table */
	odm_set_bb_reg(p_dm, 0x914, MASKBYTE0, 0);
	odm_set_bb_reg(p_dm, 0x914, MASKBYTE1, 1);

	/* Output Pin Settings */
	/* odm_set_bb_reg(p_dm, 0x948, BIT6, 0x1); */
	odm_set_bb_reg(p_dm, 0x870, BIT(9) | BIT(8), 0);

	p_dm_fat_table->is_become_linked  = false;
	p_dm_swat_table->try_flag = SWAW_STEP_INIT;
	p_dm_swat_table->double_chk_flag = 0;

	/* 2 [--For HW Bug setting] */
	odm_set_bb_reg(p_dm, 0x80C, BIT(21), 0); /* TX ant  by Reg */

}

void
odm_update_rx_idle_ant_8723b(
	void			*p_dm_void,
	u8			ant,
	u32			default_ant,
	u32			optional_ant
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct			*p_dm_fat_table = &p_dm->dm_fat_table;
	struct _ADAPTER		*p_adapter = p_dm->adapter;
	u8			count = 0;
	/*u8			u1_temp;*/
	/*u8			h2c_parameter;*/

	if ((!p_dm->is_linked) && (p_dm->ant_type == ODM_AUTO_ANT)) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: Fail to set RX antenna due to no link\n"));
		return;
	}

#if 0
	/* Send H2C command to FW */
	/* Enable wifi calibration */
	h2c_parameter = true;
	odm_fill_h2c_cmd(p_dm, ODM_H2C_WIFI_CALIBRATION, 1, &h2c_parameter);

	/* Check if H2C command sucess or not (0x1e6) */
	u1_temp = odm_read_1byte(p_dm, 0x1e6);
	while ((u1_temp != 0x1) && (count < 100)) {
		ODM_delay_us(10);
		u1_temp = odm_read_1byte(p_dm, 0x1e6);
		count++;
	}
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: H2C command status = %d, count = %d\n", u1_temp, count));

	if (u1_temp == 0x1) {
		/* Check if BT is doing IQK (0x1e7) */
		count = 0;
		u1_temp = odm_read_1byte(p_dm, 0x1e7);
		while ((!(u1_temp & BIT(0)))  && (count < 100)) {
			ODM_delay_us(50);
			u1_temp = odm_read_1byte(p_dm, 0x1e7);
			count++;
		}
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: BT IQK status = %d, count = %d\n", u1_temp, count));

		if (u1_temp & BIT(0)) {
			odm_set_bb_reg(p_dm, 0x948, BIT(6), 0x1);
			odm_set_bb_reg(p_dm, 0x948, BIT(9), default_ant);
			odm_set_bb_reg(p_dm, 0x864, BIT(5) | BIT4 | BIT3, default_ant);	/* Default RX */
			odm_set_bb_reg(p_dm, 0x864, BIT(8) | BIT7 | BIT6, optional_ant);	/* Optional RX */
			odm_set_bb_reg(p_dm, 0x860, BIT(14) | BIT13 | BIT12, default_ant); /* Default TX */
			p_dm_fat_table->rx_idle_ant = ant;

			/* Set TX AGC by S0/S1 */
			/* Need to consider Linux driver */
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
			rtw_hal_set_tx_power_level(p_adapter, *p_dm->p_channel);
#endif

			/* Set IQC by S0/S1 */
			odm_set_iqc_by_rfpath(p_dm, default_ant);
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: Success to set RX antenna\n"));
		} else
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: Fail to set RX antenna due to BT IQK\n"));
	} else
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: Fail to set RX antenna due to H2C command fail\n"));

	/* Send H2C command to FW */
	/* Disable wifi calibration */
	h2c_parameter = false;
	odm_fill_h2c_cmd(p_dm, ODM_H2C_WIFI_CALIBRATION, 1, &h2c_parameter);
#else

	odm_set_bb_reg(p_dm, 0x948, BIT(6), 0x1);
	odm_set_bb_reg(p_dm, 0x948, BIT(9), default_ant);
	odm_set_bb_reg(p_dm, 0x864, BIT(5) | BIT(4) | BIT(3), default_ant);      /*Default RX*/
	odm_set_bb_reg(p_dm, 0x864, BIT(8) | BIT(7) | BIT(6), optional_ant);     /*Optional RX*/
	odm_set_bb_reg(p_dm, 0x860, BIT(14) | BIT(13) | BIT(12), default_ant);    /*Default TX*/
	p_dm_fat_table->rx_idle_ant = ant;

	/* Set TX AGC by S0/S1 */
	/* Need to consider Linux driver */
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	rtw_hal_set_tx_power_level(p_adapter, *p_dm->p_channel);
#endif

	/* Set IQC by S0/S1 */
	odm_set_iqc_by_rfpath(p_dm, default_ant);
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Update Rx-Idle-ant ] 8723B: Success to set RX antenna\n"));

#endif
}

boolean
phydm_is_bt_enable_8723b(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	u32			bt_state;
	/*u32			reg75;*/

	/*reg75 = odm_get_bb_reg(p_dm, 0x74, BIT8);*/
	/*odm_set_bb_reg(p_dm, 0x74, BIT8, 0x0);*/
	odm_set_bb_reg(p_dm, 0xa0, BIT(24) | BIT(25) | BIT(26), 0x5);
	bt_state = odm_get_bb_reg(p_dm, 0xa0, (BIT(3) | BIT(2) | BIT(1) | BIT(0)));
	/*odm_set_bb_reg(p_dm, 0x74, BIT8, reg75);*/

	if ((bt_state == 4) || (bt_state == 7) || (bt_state == 9) || (bt_state == 13))
		return true;
	else
		return false;
}
#endif /* #if (RTL8723B_SUPPORT == 1) */

#ifdef ODM_EVM_ENHANCE_ANTDIV
void
phydm_evm_sw_antdiv_init(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;

	/*EVM enhance AntDiv method init----------------------------------------------------------------------*/
	p_dm_fat_table->EVM_method_enable = 0;
	p_dm_fat_table->fat_state = NORMAL_STATE_MIAN;
	p_dm_fat_table->fat_state_cnt = 0;
	p_dm_fat_table->pre_antdiv_rssi = 0;

	p_dm->antdiv_intvl = 30;
	p_dm->antdiv_train_num = 2;
	odm_set_bb_reg(p_dm, 0x910, 0x3f, 0xf);
	p_dm->antdiv_evm_en = 1;
	/*p_dm->antdiv_period=1;*/
	p_dm->evm_antdiv_period = 3;
	p_dm->stop_antdiv_rssi_th = 3;
	p_dm->stop_antdiv_tp_th = 80;
	p_dm->antdiv_tp_period = 3;
	p_dm->stop_antdiv_tp_diff_th = 5;
}

void
odm_evm_fast_ant_reset(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;

	p_dm_fat_table->EVM_method_enable = 0;
	odm_ant_div_on_off(p_dm, ANTDIV_ON);
	p_dm_fat_table->fat_state = NORMAL_STATE_MIAN;
	p_dm_fat_table->fat_state_cnt = 0;
	p_dm->antdiv_period = 0;
	odm_set_mac_reg(p_dm, 0x608, BIT(8), 0);
}


void
odm_evm_enhance_ant_div(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	u32	main_rssi, aux_rssi ;
	u32	main_crc_utility = 0, aux_crc_utility = 0, utility_ratio = 1;
	u32	main_evm, aux_evm, diff_rssi = 0, diff_EVM = 0;
	u32	main_2ss_evm[2], aux_2ss_evm[2];
	u32	main_1ss_evm, aux_1ss_evm;
	u32	main_2ss_evm_sum, aux_2ss_evm_sum;
	u8	score_EVM = 0, score_CRC = 0;
	u8	rssi_larger_ant = 0;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;
	u32	value32, i;
	boolean main_above1 = false, aux_above1 = false;
	boolean force_antenna = false;
	struct cmn_sta_info	*p_sta;
	u32	antdiv_tp_main_avg, antdiv_tp_aux_avg;
	u8	curr_rssi, rssi_diff;
	u32	tp_diff;
	u8	tp_diff_return = 0, tp_return = 0, rssi_return = 0;
	u8	target_ant_evm_1ss, target_ant_evm_2ss;
	u8	decision_evm_ss;
	u8	next_ant;

	p_dm_fat_table->target_ant_enhance = 0xFF;

	if ((p_dm->support_ic_type & ODM_EVM_ENHANCE_ANTDIV_SUPPORT_IC)) {
		if (p_dm->is_one_entry_only) {
			/* PHYDM_DBG(p_dm,DBG_ANT_DIV, ("[One Client only]\n")); */
			i = p_dm->one_entry_macid;
			p_sta = p_dm->p_phydm_sta_info[i];

			main_rssi = (p_dm_fat_table->main_ant_cnt[i] != 0) ? (p_dm_fat_table->main_ant_sum[i] / p_dm_fat_table->main_ant_cnt[i]) : 0;
			aux_rssi = (p_dm_fat_table->aux_ant_cnt[i] != 0) ? (p_dm_fat_table->aux_ant_sum[i] / p_dm_fat_table->aux_ant_cnt[i]) : 0;

			if ((main_rssi == 0 && aux_rssi != 0 && aux_rssi >= FORCE_RSSI_DIFF) || (main_rssi != 0 && aux_rssi == 0 && main_rssi >= FORCE_RSSI_DIFF))
				diff_rssi = FORCE_RSSI_DIFF;
			else if (main_rssi != 0 && aux_rssi != 0)
				diff_rssi = (main_rssi >= aux_rssi) ? (main_rssi - aux_rssi) : (aux_rssi - main_rssi);

			if (main_rssi >= aux_rssi)
				rssi_larger_ant = MAIN_ANT;
			else
				rssi_larger_ant = AUX_ANT;

			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Main_Cnt=(( %d )), main_rssi=(( %d ))\n", p_dm_fat_table->main_ant_cnt[i], main_rssi));
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Aux_Cnt=(( %d )), aux_rssi=(( %d ))\n", p_dm_fat_table->aux_ant_cnt[i], aux_rssi));

			if (((main_rssi >= evm_rssi_th_high || aux_rssi >= evm_rssi_th_high) || (p_dm_fat_table->EVM_method_enable == 1))
			/* && (diff_rssi <= FORCE_RSSI_DIFF + 1) */
			) {
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("> TH_H || EVM_method_enable==1\n"));

				if (((main_rssi >= evm_rssi_th_low) || (aux_rssi >= evm_rssi_th_low))) {
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("> TH_L, fat_state_cnt =((%d))\n", p_dm_fat_table->fat_state_cnt));

					/*Traning state: 0(alt) 1(ori) 2(alt) 3(ori)============================================================*/
					if (p_dm_fat_table->fat_state_cnt < ((p_dm->antdiv_train_num)<<1)) {

						if (p_dm_fat_table->fat_state_cnt == 0) {
							/*Reset EVM 1SS Method */
							p_dm_fat_table->main_ant_evm_sum[i] = 0;
							p_dm_fat_table->aux_ant_evm_sum[i] = 0;
							p_dm_fat_table->main_ant_evm_cnt[i] = 0;
							p_dm_fat_table->aux_ant_evm_cnt[i] = 0;
							/*Reset EVM 2SS Method */
							p_dm_fat_table->main_ant_evm_2ss_sum[i][0] = 0;
							p_dm_fat_table->main_ant_evm_2ss_sum[i][1] = 0;
							p_dm_fat_table->aux_ant_evm_2ss_sum[i][0] = 0;
							p_dm_fat_table->aux_ant_evm_2ss_sum[i][1] = 0;
							p_dm_fat_table->main_ant_evm_2ss_cnt[i] = 0;
							p_dm_fat_table->aux_ant_evm_2ss_cnt[i] = 0;
							#if 0
							/*Reset TP Method */
							p_dm_fat_table->antdiv_tp_main = 0;
							p_dm_fat_table->antdiv_tp_aux = 0;
							p_dm_fat_table->antdiv_tp_main_cnt = 0;
							p_dm_fat_table->antdiv_tp_aux_cnt = 0;
							#endif
							/*Reset CRC Method */
							p_dm_fat_table->main_crc32_ok_cnt = 0;
							p_dm_fat_table->main_crc32_fail_cnt = 0;
							p_dm_fat_table->aux_crc32_ok_cnt = 0;
							p_dm_fat_table->aux_crc32_fail_cnt = 0;

							#ifdef SKIP_EVM_ANTDIV_TRAINING_PATCH
							if ((*p_dm->p_band_width == CHANNEL_WIDTH_20) && (p_sta->mimo_type == RF_2T2R)) {
								/*1. Skip training: RSSI*/
								/*PHYDM_DBG(pDM_Odm,DBG_ANT_DIV, ("TargetAnt_enhance=((%d)), RxIdleAnt=((%d))\n", pDM_FatTable->TargetAnt_enhance, pDM_FatTable->RxIdleAnt));*/
								curr_rssi = (u8)((p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? main_rssi : aux_rssi);
								rssi_diff = (curr_rssi > p_dm_fat_table->pre_antdiv_rssi) ? (curr_rssi - p_dm_fat_table->pre_antdiv_rssi) : (p_dm_fat_table->pre_antdiv_rssi - curr_rssi);

								PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[1] rssi_return, curr_rssi=((%d)), pre_rssi=((%d))\n", curr_rssi, p_dm_fat_table->pre_antdiv_rssi));

								p_dm_fat_table->pre_antdiv_rssi = curr_rssi;
								if ((rssi_diff < (p_dm->stop_antdiv_rssi_th)) && (curr_rssi != 0))
									rssi_return = 1;

								/*2. Skip training: TP Diff*/
								tp_diff = (p_dm->rx_tp > p_dm_fat_table->pre_antdiv_tp) ? (p_dm->rx_tp  - p_dm_fat_table->pre_antdiv_tp) : (p_dm_fat_table->pre_antdiv_tp - p_dm->rx_tp);

								PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[2] tp_diff_return, curr_tp=((%d)), pre_tp=((%d))\n", p_dm->rx_tp, p_dm_fat_table->pre_antdiv_tp));
								p_dm_fat_table->pre_antdiv_tp = p_dm->rx_tp;
								if ((tp_diff < (u32)(p_dm->stop_antdiv_tp_diff_th)  && (p_dm->rx_tp != 0)))
									tp_diff_return = 1;

								PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[3] tp_return, curr_rx_tp=((%d))\n", p_dm->rx_tp));
								/*3. Skip training: TP*/
								if (p_dm->rx_tp >= (u32)(p_dm->stop_antdiv_tp_th))
									tp_return = 1;

								PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[4] Return {rssi, tp_diff, tp} = {%d, %d, %d}\n", rssi_return, tp_diff_return, tp_return));
								/*4. Joint Return Decision*/
								if (tp_return) {
									if (tp_diff_return || rssi_diff) {

										PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***Return EVM SW AntDiv\n"));
										return;
									}
								}
							}
							#endif

							p_dm_fat_table->EVM_method_enable = 1;
							odm_ant_div_on_off(p_dm, ANTDIV_OFF);
							p_dm->antdiv_period = p_dm->evm_antdiv_period;
							odm_set_mac_reg(p_dm, 0x608, BIT(8), 1); /*RCR accepts CRC32-Error packets*/

						}


					p_dm_fat_table->fat_state_cnt++;
					next_ant = (p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
					odm_update_rx_idle_ant(p_dm, next_ant);
					odm_set_timer(p_dm, &p_dm->evm_fast_ant_training_timer, p_dm->antdiv_intvl); //ms

					}
					/*Decision state: 4==============================================================*/
					else {

						p_dm_fat_table->fat_state_cnt = 0;
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Decisoin state ]\n"));

						/* 3 [CRC32 statistic] */
						#if 0
						if ((p_dm_fat_table->main_crc32_ok_cnt > ((p_dm_fat_table->aux_crc32_ok_cnt) << 1)) || ((diff_rssi >= 40) && (rssi_larger_ant == MAIN_ANT))) {
							p_dm_fat_table->target_ant_crc32 = MAIN_ANT;
							force_antenna = true;
							PHYDM_DBG(p_dm, DBG_ANT_DIV, ("CRC32 Force Main\n"));
						} else if ((p_dm_fat_table->aux_crc32_ok_cnt > ((p_dm_fat_table->main_crc32_ok_cnt) << 1)) || ((diff_rssi >= 40) && (rssi_larger_ant == AUX_ANT))) {
							p_dm_fat_table->target_ant_crc32 = AUX_ANT;
							force_antenna = true;
							PHYDM_DBG(p_dm, DBG_ANT_DIV, ("CRC32 Force Aux\n"));
						} else
						#endif
						{
							if (p_dm_fat_table->main_crc32_fail_cnt <= 5)
								p_dm_fat_table->main_crc32_fail_cnt = 5;

							if (p_dm_fat_table->aux_crc32_fail_cnt <= 5)
								p_dm_fat_table->aux_crc32_fail_cnt = 5;

							if (p_dm_fat_table->main_crc32_ok_cnt > p_dm_fat_table->main_crc32_fail_cnt)
								main_above1 = true;

							if (p_dm_fat_table->aux_crc32_ok_cnt > p_dm_fat_table->aux_crc32_fail_cnt)
								aux_above1 = true;

							if (main_above1 == true && aux_above1 == false) {
								force_antenna = true;
								p_dm_fat_table->target_ant_crc32 = MAIN_ANT;
							} else if (main_above1 == false && aux_above1 == true) {
								force_antenna = true;
								p_dm_fat_table->target_ant_crc32 = AUX_ANT;
							} else if (main_above1 == true && aux_above1 == true) {
								main_crc_utility = ((p_dm_fat_table->main_crc32_ok_cnt) << 7) / p_dm_fat_table->main_crc32_fail_cnt;
								aux_crc_utility = ((p_dm_fat_table->aux_crc32_ok_cnt) << 7) / p_dm_fat_table->aux_crc32_fail_cnt;
								p_dm_fat_table->target_ant_crc32 = (main_crc_utility == aux_crc_utility) ? (p_dm_fat_table->pre_target_ant_enhance) : ((main_crc_utility >= aux_crc_utility) ? MAIN_ANT : AUX_ANT);

								if (main_crc_utility != 0 && aux_crc_utility != 0) {
									if (main_crc_utility >= aux_crc_utility)
										utility_ratio = (main_crc_utility << 1) / aux_crc_utility;
									else
										utility_ratio = (aux_crc_utility << 1) / main_crc_utility;
								}
							} else if (main_above1 == false && aux_above1 == false) {
								if (p_dm_fat_table->main_crc32_ok_cnt == 0)
									p_dm_fat_table->main_crc32_ok_cnt = 1;
								if (p_dm_fat_table->aux_crc32_ok_cnt == 0)
									p_dm_fat_table->aux_crc32_ok_cnt = 1;

								main_crc_utility = ((p_dm_fat_table->main_crc32_fail_cnt) << 7) / p_dm_fat_table->main_crc32_ok_cnt;
								aux_crc_utility = ((p_dm_fat_table->aux_crc32_fail_cnt) << 7) / p_dm_fat_table->aux_crc32_ok_cnt;
								p_dm_fat_table->target_ant_crc32 = (main_crc_utility == aux_crc_utility) ? (p_dm_fat_table->pre_target_ant_enhance) : ((main_crc_utility <= aux_crc_utility) ? MAIN_ANT : AUX_ANT);

								if (main_crc_utility != 0 && aux_crc_utility != 0) {
									if (main_crc_utility >= aux_crc_utility)
										utility_ratio = (main_crc_utility << 1) / (aux_crc_utility);
									else
										utility_ratio = (aux_crc_utility << 1) / (main_crc_utility);
								}
							}
						}
						odm_set_mac_reg(p_dm, 0x608, BIT(8), 0);/* NOT Accept CRC32 Error packets. */
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("MAIN_CRC: Ok=((%d)), Fail = ((%d)), Utility = ((%d))\n", p_dm_fat_table->main_crc32_ok_cnt, p_dm_fat_table->main_crc32_fail_cnt, main_crc_utility));
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("AUX__CRC: Ok=((%d)), Fail = ((%d)), Utility = ((%d))\n", p_dm_fat_table->aux_crc32_ok_cnt, p_dm_fat_table->aux_crc32_fail_cnt, aux_crc_utility));
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***1.TargetAnt_CRC32 = ((%s))\n", (p_dm_fat_table->target_ant_crc32 == MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));

						/* 3 [EVM statistic] */
						/*1SS EVM*/
						main_1ss_evm = (p_dm_fat_table->main_ant_evm_cnt[i] != 0) ? (p_dm_fat_table->main_ant_evm_sum[i] / p_dm_fat_table->main_ant_evm_cnt[i]) : 0;
						aux_1ss_evm = (p_dm_fat_table->aux_ant_evm_cnt[i] != 0) ? (p_dm_fat_table->aux_ant_evm_sum[i] / p_dm_fat_table->aux_ant_evm_cnt[i]) : 0;
						target_ant_evm_1ss = (main_1ss_evm == aux_1ss_evm) ? (p_dm_fat_table->pre_target_ant_enhance) : ((main_1ss_evm >= aux_1ss_evm) ? MAIN_ANT : AUX_ANT);

						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Cnt = ((%d)), Main1ss_EVM= ((  %d ))\n", p_dm_fat_table->main_ant_evm_cnt[i], main_1ss_evm));
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Cnt = ((%d)), Aux_1ss_EVM = ((  %d ))\n", p_dm_fat_table->main_ant_evm_cnt[i], aux_1ss_evm));

						/*2SS EVM*/
						main_2ss_evm[0] = (p_dm_fat_table->main_ant_evm_2ss_cnt[i] != 0) ? (p_dm_fat_table->main_ant_evm_2ss_sum[i][0] / p_dm_fat_table->main_ant_evm_2ss_cnt[i]) : 0;
						main_2ss_evm[1] = (p_dm_fat_table->main_ant_evm_2ss_cnt[i] != 0) ? (p_dm_fat_table->main_ant_evm_2ss_sum[i][1] / p_dm_fat_table->main_ant_evm_2ss_cnt[i]) : 0;
						main_2ss_evm_sum = main_2ss_evm[0] + main_2ss_evm[1];

						aux_2ss_evm[0] = (p_dm_fat_table->aux_ant_evm_2ss_cnt[i] != 0) ? (p_dm_fat_table->aux_ant_evm_2ss_sum[i][0] / p_dm_fat_table->aux_ant_evm_2ss_cnt[i]) : 0;
						aux_2ss_evm[1] = (p_dm_fat_table->aux_ant_evm_2ss_cnt[i] != 0) ? (p_dm_fat_table->aux_ant_evm_2ss_sum[i][1] / p_dm_fat_table->aux_ant_evm_2ss_cnt[i]) : 0;
						aux_2ss_evm_sum = aux_2ss_evm[0] + aux_2ss_evm[1];

						target_ant_evm_2ss = (main_2ss_evm_sum == aux_2ss_evm_sum) ? (p_dm_fat_table->pre_target_ant_enhance) : ((main_2ss_evm_sum >= aux_2ss_evm_sum) ? MAIN_ANT : AUX_ANT);

						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Cnt = ((%d)), Main2ss_EVM{A,B,Sum} = {%d, %d, %d}\n",
							p_dm_fat_table->main_ant_evm_2ss_cnt[i], main_2ss_evm[0], main_2ss_evm[1], main_2ss_evm_sum));
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Cnt = ((%d)), Aux_2ss_EVM{A,B,Sum} = {%d, %d, %d}\n",
							p_dm_fat_table->aux_ant_evm_2ss_cnt[i], aux_2ss_evm[0], aux_2ss_evm[1], aux_2ss_evm_sum));

						if ((main_2ss_evm_sum + aux_2ss_evm_sum) != 0) {
							decision_evm_ss = 2;
							main_evm = main_2ss_evm_sum;
							aux_evm = aux_2ss_evm_sum;
							p_dm_fat_table->target_ant_evm = target_ant_evm_2ss;
						} else {
							decision_evm_ss = 1;
							main_evm = main_1ss_evm;
							aux_evm = aux_1ss_evm;
							p_dm_fat_table->target_ant_evm = target_ant_evm_1ss;
						}

						if ((main_evm == 0 || aux_evm == 0))
							diff_EVM = 100;
						else if (main_evm >= aux_evm)
							diff_EVM = main_evm - aux_evm;
						else
							diff_EVM = aux_evm - main_evm;

						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***2.TargetAnt_EVM((%d-ss)) = ((%s))\n", decision_evm_ss, (p_dm_fat_table->target_ant_evm == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"));


						//3 [TP statistic]
						antdiv_tp_main_avg = (p_dm_fat_table->antdiv_tp_main_cnt != 0) ? (p_dm_fat_table->antdiv_tp_main / p_dm_fat_table->antdiv_tp_main_cnt) : 0;
						antdiv_tp_aux_avg = (p_dm_fat_table->antdiv_tp_aux_cnt != 0) ? (p_dm_fat_table->antdiv_tp_aux / p_dm_fat_table->antdiv_tp_aux_cnt) : 0;
						p_dm_fat_table->target_ant_tp = (antdiv_tp_main_avg == antdiv_tp_aux_avg) ? (p_dm_fat_table->pre_target_ant_enhance) : ((antdiv_tp_main_avg >= antdiv_tp_aux_avg) ? MAIN_ANT : AUX_ANT);

						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Cnt = ((%d)), Main_TP = ((%d))\n", p_dm_fat_table->antdiv_tp_main_cnt, antdiv_tp_main_avg));
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Cnt = ((%d)), Aux_TP = ((%d))\n", p_dm_fat_table->antdiv_tp_aux_cnt, antdiv_tp_aux_avg));
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***3.TargetAnt_TP = ((%s))\n", (p_dm_fat_table->target_ant_tp == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"));

						/*Reset TP Method */
						p_dm_fat_table->antdiv_tp_main = 0;
						p_dm_fat_table->antdiv_tp_aux = 0;
						p_dm_fat_table->antdiv_tp_main_cnt = 0;
						p_dm_fat_table->antdiv_tp_aux_cnt = 0;

						/* 2 [ Decision state ] */
						if (p_dm_fat_table->target_ant_evm == p_dm_fat_table->target_ant_crc32) {
							PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Decision type 1, CRC_utility = ((%d)), EVM_diff = ((%d))\n", utility_ratio, diff_EVM));

							if ((utility_ratio < 2 && force_antenna == false) && diff_EVM <= 30)
								p_dm_fat_table->target_ant_enhance = p_dm_fat_table->pre_target_ant_enhance;
							else
								p_dm_fat_table->target_ant_enhance = p_dm_fat_table->target_ant_evm;
						}
						#if 0
						else if ((diff_EVM <= 50 && (utility_ratio > 4 && force_antenna == false)) || (force_antenna == true)) {
							PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Decision type 2, CRC_utility = ((%d)), EVM_diff = ((%d))\n", utility_ratio, diff_EVM));
							p_dm_fat_table->target_ant_enhance = p_dm_fat_table->target_ant_crc32;
						}
						#endif
						else if (diff_EVM >= 20) {
							PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Decision type 3, CRC_utility = ((%d)), EVM_diff = ((%d))\n", utility_ratio, diff_EVM));
							p_dm_fat_table->target_ant_enhance = p_dm_fat_table->target_ant_evm;
						} else if (utility_ratio >= 6 && force_antenna == false) {
							PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Decision type 4, CRC_utility = ((%d)), EVM_diff = ((%d))\n", utility_ratio, diff_EVM));
							p_dm_fat_table->target_ant_enhance = p_dm_fat_table->target_ant_crc32;
						} else {
							PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Decision type 5, CRC_utility = ((%d)), EVM_diff = ((%d))\n", utility_ratio, diff_EVM));

							if (force_antenna == true)
								score_CRC = 2;
							else if (utility_ratio >= 5) /*>2.5*/
								score_CRC = 2;
							else if (utility_ratio >= 4) /*>2*/
								score_CRC = 1;
							else
								score_CRC = 0;

							if (diff_EVM >= 15)
								score_EVM = 3;
							else if (diff_EVM >= 10)
								score_EVM = 2;
							else if (diff_EVM >= 5)
								score_EVM = 1;
							else
								score_EVM = 0;

							if (score_CRC > score_EVM)
								p_dm_fat_table->target_ant_enhance = p_dm_fat_table->target_ant_crc32;
							else if (score_CRC < score_EVM)
								p_dm_fat_table->target_ant_enhance = p_dm_fat_table->target_ant_evm;
							else
								p_dm_fat_table->target_ant_enhance = p_dm_fat_table->pre_target_ant_enhance;
						}
						p_dm_fat_table->pre_target_ant_enhance = p_dm_fat_table->target_ant_enhance;

						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** 4.TargetAnt_enhance = (( %s ))******\n", (p_dm_fat_table->target_ant_enhance == MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));


					}
				} else { /* RSSI< = evm_rssi_th_low */
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ <TH_L: escape from > TH_L ]\n"));
					odm_evm_fast_ant_reset(p_dm);
				}
			} else {
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[escape from> TH_H || EVM_method_enable==1]\n"));
				odm_evm_fast_ant_reset(p_dm);
			}
		} else {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[multi-Client]\n"));
			odm_evm_fast_ant_reset(p_dm);
		}
	}
}

void
odm_evm_fast_ant_training_callback(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("******AntDiv_Callback******\n"));
	odm_hw_ant_div(p_dm);
}
#endif

void
odm_hw_ant_div(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	u32	i, min_max_rssi = 0xFF,  ant_div_max_rssi = 0, max_rssi = 0, local_max_rssi;
	u32	main_rssi, aux_rssi, mian_cnt, aux_cnt;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;
	u8	rx_idle_ant = p_dm_fat_table->rx_idle_ant, target_ant = 7;
	struct phydm_dig_struct	*p_dig_t = &p_dm->dm_dig_table;
	struct cmn_sta_info	*p_sta;

	if (!p_dm->is_linked) { /* is_linked==False */
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[No Link!!!]\n"));

		if (p_dm_fat_table->is_become_linked == true) {
			odm_ant_div_on_off(p_dm, ANTDIV_OFF);
			odm_update_rx_idle_ant(p_dm, MAIN_ANT);
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);
			p_dm->antdiv_period = 0;

			p_dm_fat_table->is_become_linked = p_dm->is_linked;
		}
		return;
	} else {
		if (p_dm_fat_table->is_become_linked == false) {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Linked !!!]\n"));
			odm_ant_div_on_off(p_dm, ANTDIV_ON);
			/*odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_DESC);*/

			/* else if(p_dm->support_ic_type == ODM_RTL8723B ||p_dm->support_ic_type == ODM_RTL8812) */
			/* odm_set_bb_reg(p_dm, 0xA00, BIT(15), 0); */ /* CCK AntDiv function disable */

			p_dm_fat_table->is_become_linked = p_dm->is_linked;

			if (p_dm->support_ic_type == ODM_RTL8723B && p_dm->ant_div_type == CG_TRX_HW_ANTDIV) {
				odm_set_bb_reg(p_dm, 0x930, 0xF0, 8); /* DPDT_P = ANTSEL[0]   */ /* for 8723B AntDiv function patch.  BB  Dino  130412 */
				odm_set_bb_reg(p_dm, 0x930, 0xF, 8); /* DPDT_N = ANTSEL[0] */
			}

			/* 2 BDC Init */

#ifdef ODM_EVM_ENHANCE_ANTDIV
			odm_evm_fast_ant_reset(p_dm);
#endif
		}
	}

	if (*(p_dm_fat_table->p_force_tx_ant_by_desc) == false) {
		if (p_dm->is_one_entry_only == true)
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);
		else
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_DESC);
	}

#ifdef ODM_EVM_ENHANCE_ANTDIV
	if (p_dm->antdiv_evm_en == 1) {
		odm_evm_enhance_ant_div(p_dm);
		if (p_dm_fat_table->fat_state_cnt != 0)
			return;
	} else
		odm_evm_fast_ant_reset(p_dm);
#endif

	/* 2 BDC mode Arbitration */

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		p_sta = p_dm->p_phydm_sta_info[i];
		if (is_sta_active(p_sta)) {
			/* 2 Caculate RSSI per Antenna */
			if ((p_dm_fat_table->main_ant_cnt[i] != 0) || (p_dm_fat_table->aux_ant_cnt[i] != 0)) {
				mian_cnt = p_dm_fat_table->main_ant_cnt[i];
				aux_cnt = p_dm_fat_table->aux_ant_cnt[i];
				main_rssi = (mian_cnt != 0) ? (p_dm_fat_table->main_ant_sum[i] / mian_cnt) : 0;
				aux_rssi = (aux_cnt != 0) ? (p_dm_fat_table->aux_ant_sum[i] / aux_cnt) : 0;
				target_ant = (mian_cnt == aux_cnt) ? p_dm_fat_table->rx_idle_ant : ((mian_cnt >= aux_cnt) ? MAIN_ANT : AUX_ANT); /*Use counter number for OFDM*/

			} else {	/*CCK only case*/
				mian_cnt = p_dm_fat_table->main_ant_cnt_cck[i];
				aux_cnt = p_dm_fat_table->aux_ant_cnt_cck[i];
				main_rssi = (mian_cnt != 0) ? (p_dm_fat_table->main_ant_sum_cck[i] / mian_cnt) : 0;
				aux_rssi = (aux_cnt != 0) ? (p_dm_fat_table->aux_ant_sum_cck[i] / aux_cnt) : 0;
				target_ant = (main_rssi == aux_rssi) ? p_dm_fat_table->rx_idle_ant : ((main_rssi >= aux_rssi) ? MAIN_ANT : AUX_ANT); /*Use RSSI for CCK only case*/
			}

			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** Client[ %d ] : Main_Cnt = (( %d ))  ,  CCK_Main_Cnt = (( %d )) ,  main_rssi= ((  %d ))\n", i, p_dm_fat_table->main_ant_cnt[i], p_dm_fat_table->main_ant_cnt_cck[i], main_rssi));
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** Client[ %d ] : Aux_Cnt   = (( %d ))  , CCK_Aux_Cnt   = (( %d )) ,  aux_rssi = ((  %d ))\n", i, p_dm_fat_table->aux_ant_cnt[i], p_dm_fat_table->aux_ant_cnt_cck[i], aux_rssi));
			/* PHYDM_DBG(p_dm,DBG_ANT_DIV, ("*** MAC ID:[ %d ] , target_ant = (( %s ))\n", i ,( target_ant ==MAIN_ANT)?"MAIN_ANT":"AUX_ANT")); */

			local_max_rssi = (main_rssi > aux_rssi) ? main_rssi : aux_rssi;
			/* 2 Select max_rssi for DIG */
			if ((local_max_rssi > ant_div_max_rssi) && (local_max_rssi < 40))
				ant_div_max_rssi = local_max_rssi;
			if (local_max_rssi > max_rssi)
				max_rssi = local_max_rssi;

			/* 2 Select RX Idle Antenna */
			if ((local_max_rssi != 0) && (local_max_rssi < min_max_rssi)) {
				rx_idle_ant = target_ant;
				min_max_rssi = local_max_rssi;
			}

#ifdef ODM_EVM_ENHANCE_ANTDIV
			if (p_dm->antdiv_evm_en == 1) {
				if (p_dm_fat_table->target_ant_enhance != 0xFF) {
					target_ant = p_dm_fat_table->target_ant_enhance;
					rx_idle_ant = p_dm_fat_table->target_ant_enhance;
				}
			}
#endif

			/* 2 Select TX Antenna */
			if (p_dm->ant_div_type != CGCS_RX_HW_ANTDIV) {
				{
					odm_update_tx_ant(p_dm, target_ant, i);
				}
			}
		}

		phydm_antdiv_reset_statistic(p_dm, i);
	}

	/* 2 Set RX Idle Antenna & TX Antenna(Because of HW Bug ) */
	odm_update_rx_idle_ant(p_dm, rx_idle_ant);

	/* 2 BDC Main Algorithm */

	if (ant_div_max_rssi == 0)
		p_dig_t->ant_div_rssi_max = p_dm->rssi_min;
	else
		p_dig_t->ant_div_rssi_max = ant_div_max_rssi;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***AntDiv End***\n\n"));
}

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY

void
odm_s0s1_sw_ant_div_reset(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _sw_antenna_switch_		*p_dm_swat_table	= &p_dm->dm_swat_table;
	struct phydm_fat_struct		*p_dm_fat_table		= &p_dm->dm_fat_table;

	p_dm_fat_table->is_become_linked  = false;
	p_dm_swat_table->try_flag = SWAW_STEP_INIT;
	p_dm_swat_table->double_chk_flag = 0;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("odm_s0s1_sw_ant_div_reset(): p_dm_fat_table->is_become_linked = %d\n", p_dm_fat_table->is_become_linked));
}

void
odm_s0s1_sw_ant_div(
	void			*p_dm_void,
	u8			step
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _sw_antenna_switch_			*p_dm_swat_table = &p_dm->dm_swat_table;
	struct phydm_fat_struct			*p_dm_fat_table = &p_dm->dm_fat_table;
	u32			i, min_max_rssi = 0xFF, local_max_rssi, local_min_rssi;
	u32			main_rssi, aux_rssi;
	u8			high_traffic_train_time_u = 0x32, high_traffic_train_time_l = 0, train_time_temp;
	u8			low_traffic_train_time_u = 200, low_traffic_train_time_l = 0;
	u8			rx_idle_ant = p_dm_swat_table->pre_antenna, target_ant, next_ant = 0;
	struct cmn_sta_info		*p_entry = NULL;
	u32			value32;
	u32			main_ant_sum;
	u32			aux_ant_sum;
	u32			main_ant_cnt;
	u32			aux_ant_cnt;


	if (!p_dm->is_linked) { /* is_linked==False */
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[No Link!!!]\n"));
		if (p_dm_fat_table->is_become_linked == true) {
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);
			if (p_dm->support_ic_type == ODM_RTL8723B) {

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Set REG 948[9:6]=0x0\n"));
				odm_set_bb_reg(p_dm, 0x948, (BIT(9) | BIT(8) | BIT(7) | BIT(6)), 0x0);
			}
			p_dm_fat_table->is_become_linked = p_dm->is_linked;
		}
		return;
	} else {
		if (p_dm_fat_table->is_become_linked == false) {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Linked !!!]\n"));

			if (p_dm->support_ic_type == ODM_RTL8723B) {
				value32 = odm_get_bb_reg(p_dm, 0x864, BIT(5) | BIT(4) | BIT(3));

#if (RTL8723B_SUPPORT == 1)
				if (value32 == 0x0)
					odm_update_rx_idle_ant_8723b(p_dm, MAIN_ANT, ANT1_2G, ANT2_2G);
				else if (value32 == 0x1)
					odm_update_rx_idle_ant_8723b(p_dm, AUX_ANT, ANT2_2G, ANT1_2G);
#endif

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("8723B: First link! Force antenna to  %s\n", (value32 == 0x0 ? "MAIN" : "AUX")));
			}
			p_dm_fat_table->is_become_linked = p_dm->is_linked;
		}
	}

	if (*(p_dm_fat_table->p_force_tx_ant_by_desc) == false) {
		if (p_dm->is_one_entry_only == true)
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);
		else
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_DESC);
	}

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[%d] { try_flag=(( %d )), step=(( %d )), double_chk_flag = (( %d )) }\n",
		__LINE__, p_dm_swat_table->try_flag, step, p_dm_swat_table->double_chk_flag));

	/* Handling step mismatch condition. */
	/* Peak step is not finished at last time. Recover the variable and check again. */
	if (step != p_dm_swat_table->try_flag) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[step != try_flag]    Need to Reset After Link\n"));
		odm_sw_ant_div_rest_after_link(p_dm);
	}

	if (p_dm_swat_table->try_flag == SWAW_STEP_INIT) {

		p_dm_swat_table->try_flag = SWAW_STEP_PEEK;
		p_dm_swat_table->train_time_flag = 0;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[set try_flag = 0]  Prepare for peek!\n\n"));
		return;

	} else {

		/* 1 Normal state (Begin Trying) */
		if (p_dm_swat_table->try_flag == SWAW_STEP_PEEK) {

			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("TxOkCnt=(( %llu )), RxOkCnt=(( %llu )), traffic_load = (%d))\n", p_dm->cur_tx_ok_cnt, p_dm->cur_rx_ok_cnt, p_dm->traffic_load));

			if (p_dm->traffic_load == TRAFFIC_HIGH) {
				train_time_temp = p_dm_swat_table->train_time ;

				if (p_dm_swat_table->train_time_flag == 3) {
					high_traffic_train_time_l = 0xa;

					if (train_time_temp <= 16)
						train_time_temp = high_traffic_train_time_l;
					else
						train_time_temp -= 16;

				} else if (p_dm_swat_table->train_time_flag == 2) {
					train_time_temp -= 8;
					high_traffic_train_time_l = 0xf;
				} else if (p_dm_swat_table->train_time_flag == 1) {
					train_time_temp -= 4;
					high_traffic_train_time_l = 0x1e;
				} else if (p_dm_swat_table->train_time_flag == 0) {
					train_time_temp += 8;
					high_traffic_train_time_l = 0x28;
				}
				
				if (p_dm->support_ic_type == ODM_RTL8188F) {
					if (p_dm->support_interface == ODM_ITRF_SDIO)
						high_traffic_train_time_l += 0xa;
				}

				/* PHYDM_DBG(p_dm,DBG_ANT_DIV, ("*** train_time_temp = ((%d))\n",train_time_temp)); */

				/* -- */
				if (train_time_temp > high_traffic_train_time_u)
					train_time_temp = high_traffic_train_time_u;

				else if (train_time_temp < high_traffic_train_time_l)
					train_time_temp = high_traffic_train_time_l;

				p_dm_swat_table->train_time = train_time_temp; /*10ms~200ms*/

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("train_time_flag=((%d)), train_time=((%d))\n", p_dm_swat_table->train_time_flag, p_dm_swat_table->train_time));

			} else if ((p_dm->traffic_load == TRAFFIC_MID) || (p_dm->traffic_load == TRAFFIC_LOW)) {

				train_time_temp = p_dm_swat_table->train_time ;

				if (p_dm_swat_table->train_time_flag == 3) {
					low_traffic_train_time_l = 10;
					if (train_time_temp < 50)
						train_time_temp = low_traffic_train_time_l;
					else
						train_time_temp -= 50;
				} else if (p_dm_swat_table->train_time_flag == 2) {
					train_time_temp -= 30;
					low_traffic_train_time_l = 36;
				} else if (p_dm_swat_table->train_time_flag == 1) {
					train_time_temp -= 10;
					low_traffic_train_time_l = 40;
				} else {

					train_time_temp += 10;
					low_traffic_train_time_l = 50;
				}

				if (p_dm->support_ic_type == ODM_RTL8188F) {
					if (p_dm->support_interface == ODM_ITRF_SDIO)
						low_traffic_train_time_l += 10;
				}

				/* -- */
				if (train_time_temp >= low_traffic_train_time_u)
					train_time_temp = low_traffic_train_time_u;

				else if (train_time_temp <= low_traffic_train_time_l)
					train_time_temp = low_traffic_train_time_l;

				p_dm_swat_table->train_time = train_time_temp; /*10ms~200ms*/

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("train_time_flag=((%d)) , train_time=((%d))\n", p_dm_swat_table->train_time_flag, p_dm_swat_table->train_time));

			} else {
				p_dm_swat_table->train_time = 0xc8; /*200ms*/

			}

			/* ----------------- */

			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Current min_max_rssi is ((%d))\n", p_dm_fat_table->min_max_rssi));

			/* ---reset index--- */
			if (p_dm_swat_table->reset_idx >= RSSI_CHECK_RESET_PERIOD) {

				p_dm_fat_table->min_max_rssi = 0;
				p_dm_swat_table->reset_idx = 0;
			}
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("reset_idx = (( %d ))\n", p_dm_swat_table->reset_idx));

			p_dm_swat_table->reset_idx++;

			/* ---double check flag--- */
			if ((p_dm_fat_table->min_max_rssi > RSSI_CHECK_THRESHOLD) && (p_dm_swat_table->double_chk_flag == 0)) {
				PHYDM_DBG(p_dm, DBG_ANT_DIV, (" min_max_rssi is ((%d)), and > %d\n",
					p_dm_fat_table->min_max_rssi, RSSI_CHECK_THRESHOLD));

				p_dm_swat_table->double_chk_flag = 1;
				p_dm_swat_table->try_flag = SWAW_STEP_DETERMINE;
				p_dm_swat_table->rssi_trying = 0;

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Test the current ant for (( %d )) ms again\n", p_dm_swat_table->train_time));
				odm_update_rx_idle_ant(p_dm, p_dm_fat_table->rx_idle_ant);
				odm_set_timer(p_dm, &(p_dm_swat_table->phydm_sw_antenna_switch_timer), p_dm_swat_table->train_time); /*ms*/
				return;
			}

			next_ant = (p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? AUX_ANT : MAIN_ANT;

			p_dm_swat_table->try_flag = SWAW_STEP_DETERMINE;

			if (p_dm_swat_table->reset_idx <= 1)
				p_dm_swat_table->rssi_trying = 2;
			else
				p_dm_swat_table->rssi_trying = 1;

			odm_s0s1_sw_ant_div_by_ctrl_frame(p_dm, SWAW_STEP_PEEK);
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[set try_flag=1]  Normal state:  Begin Trying!!\n"));

		} else if ((p_dm_swat_table->try_flag == SWAW_STEP_DETERMINE) && (p_dm_swat_table->double_chk_flag == 0)) {

			next_ant = (p_dm_fat_table->rx_idle_ant  == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
			p_dm_swat_table->rssi_trying--;
		}

		/* 1 Decision state */
		if ((p_dm_swat_table->try_flag == SWAW_STEP_DETERMINE) && (p_dm_swat_table->rssi_trying == 0)) {

			boolean is_by_ctrl_frame = false;
			u64	pkt_cnt_total = 0;

			for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
				p_entry = p_dm->p_phydm_sta_info[i];
				if (is_sta_active(p_entry)) {
					/* 2 Caculate RSSI per Antenna */

					main_ant_sum = (u32)p_dm_fat_table->main_ant_sum[i] + (u32)p_dm_fat_table->main_ant_sum_cck[i];
					aux_ant_sum = (u32)p_dm_fat_table->aux_ant_sum[i] + (u32)p_dm_fat_table->aux_ant_sum_cck[i];
					main_ant_cnt = (u32)p_dm_fat_table->main_ant_cnt[i] + (u32)p_dm_fat_table->main_ant_cnt_cck[i];
					aux_ant_cnt = (u32)p_dm_fat_table->aux_ant_cnt[i] + (u32)p_dm_fat_table->aux_ant_cnt_cck[i];

					main_rssi = (main_ant_cnt != 0) ? (main_ant_sum / main_ant_cnt) : 0;
					aux_rssi = (aux_ant_cnt != 0) ? (aux_ant_sum / aux_ant_cnt) : 0;

					if (p_dm_fat_table->main_ant_cnt[i] <= 1 && p_dm_fat_table->main_ant_cnt_cck[i] >= 1)
						main_rssi = 0;

					if (p_dm_fat_table->aux_ant_cnt[i] <= 1 && p_dm_fat_table->aux_ant_cnt_cck[i] >= 1)
						aux_rssi = 0;

					target_ant = (main_rssi == aux_rssi) ? p_dm_swat_table->pre_antenna : ((main_rssi >= aux_rssi) ? MAIN_ANT : AUX_ANT);
					local_max_rssi = (main_rssi >= aux_rssi) ? main_rssi : aux_rssi;
					local_min_rssi = (main_rssi >= aux_rssi) ? aux_rssi : main_rssi;

					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***  CCK_counter_main = (( %d ))  , CCK_counter_aux= ((  %d ))\n", p_dm_fat_table->main_ant_cnt_cck[i], p_dm_fat_table->aux_ant_cnt_cck[i]));
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***  OFDM_counter_main = (( %d ))  , OFDM_counter_aux= ((  %d ))\n", p_dm_fat_table->main_ant_cnt[i], p_dm_fat_table->aux_ant_cnt[i]));
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***  Main_Cnt = (( %d ))  , main_rssi= ((  %d ))\n", main_ant_cnt, main_rssi));
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("***  Aux_Cnt   = (( %d ))  , aux_rssi = ((  %d ))\n", aux_ant_cnt, aux_rssi));
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** MAC ID:[ %d ] , target_ant = (( %s ))\n", i, (target_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT"));

					/* 2 Select RX Idle Antenna */

					if (local_max_rssi != 0 && local_max_rssi < min_max_rssi) {
						rx_idle_ant = target_ant;
						min_max_rssi = local_max_rssi;
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** local_max_rssi-local_min_rssi = ((%d))\n", (local_max_rssi - local_min_rssi)));

						if ((local_max_rssi - local_min_rssi) > 8) {
							if (local_min_rssi != 0)
								p_dm_swat_table->train_time_flag = 3;
							else {
								if (min_max_rssi > RSSI_CHECK_THRESHOLD)
									p_dm_swat_table->train_time_flag = 0;
								else
									p_dm_swat_table->train_time_flag = 3;
							}
						} else if ((local_max_rssi - local_min_rssi) > 5)
							p_dm_swat_table->train_time_flag = 2;
						else if ((local_max_rssi - local_min_rssi) > 2)
							p_dm_swat_table->train_time_flag = 1;
						else
							p_dm_swat_table->train_time_flag = 0;

					}

					/* 2 Select TX Antenna */
					if (target_ant == MAIN_ANT)
						p_dm_fat_table->antsel_a[i] = ANT1_2G;
					else
						p_dm_fat_table->antsel_a[i] = ANT2_2G;

				}
				phydm_antdiv_reset_statistic(p_dm, i);
				pkt_cnt_total += (main_ant_cnt + aux_ant_cnt);
			}

			if (p_dm_swat_table->is_sw_ant_div_by_ctrl_frame) {
				odm_s0s1_sw_ant_div_by_ctrl_frame(p_dm, SWAW_STEP_DETERMINE);
				is_by_ctrl_frame = true;
			}

			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Control frame packet counter = %d, data frame packet counter = %llu\n",
				p_dm_swat_table->pkt_cnt_sw_ant_div_by_ctrl_frame, pkt_cnt_total));

			if (min_max_rssi == 0xff || ((pkt_cnt_total < (p_dm_swat_table->pkt_cnt_sw_ant_div_by_ctrl_frame >> 1)) && p_dm->phy_dbg_info.num_qry_beacon_pkt < 2)) {
				min_max_rssi = 0;
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Check RSSI of control frame because min_max_rssi == 0xff\n"));
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("is_by_ctrl_frame = %d\n", is_by_ctrl_frame));

				if (is_by_ctrl_frame) {
					main_rssi = (p_dm_fat_table->main_ant_ctrl_frame_cnt != 0) ? (p_dm_fat_table->main_ant_ctrl_frame_sum / p_dm_fat_table->main_ant_ctrl_frame_cnt) : 0;
					aux_rssi = (p_dm_fat_table->aux_ant_ctrl_frame_cnt != 0) ? (p_dm_fat_table->aux_ant_ctrl_frame_sum / p_dm_fat_table->aux_ant_ctrl_frame_cnt) : 0;

					if (p_dm_fat_table->main_ant_ctrl_frame_cnt <= 1 && p_dm_fat_table->cck_ctrl_frame_cnt_main >= 1)
						main_rssi = 0;

					if (p_dm_fat_table->aux_ant_ctrl_frame_cnt <= 1 && p_dm_fat_table->cck_ctrl_frame_cnt_aux >= 1)
						aux_rssi = 0;

					if (main_rssi != 0 || aux_rssi != 0) {
						rx_idle_ant = (main_rssi == aux_rssi) ? p_dm_swat_table->pre_antenna : ((main_rssi >= aux_rssi) ? MAIN_ANT : AUX_ANT);
						local_max_rssi = (main_rssi >= aux_rssi) ? main_rssi : aux_rssi;
						local_min_rssi = (main_rssi >= aux_rssi) ? aux_rssi : main_rssi;

						if ((local_max_rssi - local_min_rssi) > 8)
							p_dm_swat_table->train_time_flag = 3;
						else if ((local_max_rssi - local_min_rssi) > 5)
							p_dm_swat_table->train_time_flag = 2;
						else if ((local_max_rssi - local_min_rssi) > 2)
							p_dm_swat_table->train_time_flag = 1;
						else
							p_dm_swat_table->train_time_flag = 0;

						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Control frame: main_rssi = %d, aux_rssi = %d\n", main_rssi, aux_rssi));
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("rx_idle_ant decided by control frame = %s\n", (rx_idle_ant == MAIN_ANT ? "MAIN" : "AUX")));
					}
				}
			}

			p_dm_fat_table->min_max_rssi = min_max_rssi;
			p_dm_swat_table->try_flag = SWAW_STEP_PEEK;

			if (p_dm_swat_table->double_chk_flag == 1) {
				p_dm_swat_table->double_chk_flag = 0;

				if (p_dm_fat_table->min_max_rssi > RSSI_CHECK_THRESHOLD) {

					PHYDM_DBG(p_dm, DBG_ANT_DIV, (" [Double check] min_max_rssi ((%d)) > %d again!!\n",
						p_dm_fat_table->min_max_rssi, RSSI_CHECK_THRESHOLD));

					odm_update_rx_idle_ant(p_dm, rx_idle_ant);

					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[reset try_flag = 0] Training accomplished !!!]\n\n\n"));
					return;
				} else {
					PHYDM_DBG(p_dm, DBG_ANT_DIV, (" [Double check] min_max_rssi ((%d)) <= %d !!\n",
						p_dm_fat_table->min_max_rssi, RSSI_CHECK_THRESHOLD));

					next_ant = (p_dm_fat_table->rx_idle_ant  == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
					p_dm_swat_table->try_flag = SWAW_STEP_PEEK;
					p_dm_swat_table->reset_idx = RSSI_CHECK_RESET_PERIOD;
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[set try_flag=0]  Normal state:  Need to tryg again!!\n\n\n"));
					return;
				}
			} else {
				if (p_dm_fat_table->min_max_rssi < RSSI_CHECK_THRESHOLD)
					p_dm_swat_table->reset_idx = RSSI_CHECK_RESET_PERIOD;

				p_dm_swat_table->pre_antenna = rx_idle_ant;
				odm_update_rx_idle_ant(p_dm, rx_idle_ant);
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[reset try_flag = 0] Training accomplished !!!]\n\n\n"));
				return;
			}

		}

	}

	/* 1 4.Change TRX antenna */

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("rssi_trying = (( %d )),    ant: (( %s )) >>> (( %s ))\n",
		p_dm_swat_table->rssi_trying, (p_dm_fat_table->rx_idle_ant  == MAIN_ANT ? "MAIN" : "AUX"), (next_ant == MAIN_ANT ? "MAIN" : "AUX")));

	odm_update_rx_idle_ant(p_dm, next_ant);

	/* 1 5.Reset Statistics */

	p_dm_fat_table->rx_idle_ant  = next_ant;

	if (p_dm->support_ic_type == ODM_RTL8188F) {
		if (p_dm->support_interface == ODM_ITRF_SDIO) {

			ODM_delay_us(200);
			
			if (p_dm_fat_table->rx_idle_ant == MAIN_ANT) {
				p_dm_fat_table->main_ant_sum[0] = 0;
				p_dm_fat_table->main_ant_cnt[0] = 0;
				p_dm_fat_table->main_ant_sum_cck[0] = 0;
				p_dm_fat_table->main_ant_cnt_cck[0] = 0;	
			} else {
				p_dm_fat_table->aux_ant_sum[0] = 0;
				p_dm_fat_table->aux_ant_cnt[0] = 0;
				p_dm_fat_table->aux_ant_sum_cck[0] = 0;
				p_dm_fat_table->aux_ant_cnt_cck[0] = 0;	
			}	
		}	
	}

	/* 1 6.Set next timer   (Trying state) */

	PHYDM_DBG(p_dm, DBG_ANT_DIV, (" Test ((%s)) ant for (( %d )) ms\n", (next_ant == MAIN_ANT ? "MAIN" : "AUX"), p_dm_swat_table->train_time));
	odm_set_timer(p_dm, &(p_dm_swat_table->phydm_sw_antenna_switch_timer), p_dm_swat_table->train_time); /*ms*/
}


#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
void odm_sw_antdiv_workitem_callback(void *p_context)
{
	struct _ADAPTER *
	p_adapter = (struct _ADAPTER *)p_context;
	HAL_DATA_TYPE
	*p_hal_data = GET_HAL_DATA(p_adapter);

	/*dbg_print("SW_antdiv_Workitem_Callback");*/
	odm_s0s1_sw_ant_div(&p_hal_data->odmpriv, SWAW_STEP_DETERMINE);
}

void odm_sw_antdiv_callback(void *function_context)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)function_context;
	struct _ADAPTER	*padapter = p_dm->adapter;
	if (padapter->net_closed == true)
		return;

#if 0 /* Can't do I/O in timer callback*/
	odm_s0s1_sw_ant_div(p_dm, SWAW_STEP_DETERMINE);
#else
	rtw_run_in_thread_cmd(padapter, odm_sw_antdiv_workitem_callback, padapter);
#endif
}

#endif

void odm_s0s1_sw_ant_div_by_ctrl_frame(
	void			*p_dm_void,
	u8			step
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _sw_antenna_switch_	*p_dm_swat_table = &p_dm->dm_swat_table;
	struct phydm_fat_struct		*p_dm_fat_table = &p_dm->dm_fat_table;

	switch (step) {
	case SWAW_STEP_PEEK:
		p_dm_swat_table->pkt_cnt_sw_ant_div_by_ctrl_frame = 0;
		p_dm_swat_table->is_sw_ant_div_by_ctrl_frame = true;
		p_dm_fat_table->main_ant_ctrl_frame_cnt = 0;
		p_dm_fat_table->aux_ant_ctrl_frame_cnt = 0;
		p_dm_fat_table->main_ant_ctrl_frame_sum = 0;
		p_dm_fat_table->aux_ant_ctrl_frame_sum = 0;
		p_dm_fat_table->cck_ctrl_frame_cnt_main = 0;
		p_dm_fat_table->cck_ctrl_frame_cnt_aux = 0;
		p_dm_fat_table->ofdm_ctrl_frame_cnt_main = 0;
		p_dm_fat_table->ofdm_ctrl_frame_cnt_aux = 0;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("odm_S0S1_SwAntDivForAPMode(): Start peek and reset counter\n"));
		break;
	case SWAW_STEP_DETERMINE:
		p_dm_swat_table->is_sw_ant_div_by_ctrl_frame = false;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("odm_S0S1_SwAntDivForAPMode(): Stop peek\n"));
		break;
	default:
		p_dm_swat_table->is_sw_ant_div_by_ctrl_frame = false;
		break;
	}
}

void odm_antsel_statistics_of_ctrl_frame(
	void			*p_dm_void,
	u8			antsel_tr_mux,
	u32			rx_pwdb_all

)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;

	if (antsel_tr_mux == ANT1_2G) {
		p_dm_fat_table->main_ant_ctrl_frame_sum += rx_pwdb_all;
		p_dm_fat_table->main_ant_ctrl_frame_cnt++;
	} else {
		p_dm_fat_table->aux_ant_ctrl_frame_sum += rx_pwdb_all;
		p_dm_fat_table->aux_ant_ctrl_frame_cnt++;
	}
}

void
odm_s0s1_sw_ant_div_by_ctrl_frame_process_rssi(
	void			*p_dm_void,
	void			*p_phy_info_void,
	void			*p_pkt_info_void
	/*	struct phydm_phyinfo_struct*		p_phy_info, */
	/*	struct phydm_perpkt_info_struct*		p_pktinfo */
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_phyinfo_struct	*p_phy_info = (struct phydm_phyinfo_struct *)p_phy_info_void;
	struct phydm_perpkt_info_struct	*p_pktinfo = (struct phydm_perpkt_info_struct *)p_pkt_info_void;
	struct _sw_antenna_switch_	*p_dm_swat_table = &p_dm->dm_swat_table;
	struct phydm_fat_struct		*p_dm_fat_table = &p_dm->dm_fat_table;
	boolean		is_cck_rate;

	if (!(p_dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (p_dm->ant_div_type != S0S1_SW_ANTDIV)
		return;

	/* In try state */
	if (!p_dm_swat_table->is_sw_ant_div_by_ctrl_frame)
		return;

	/* No HW error and match receiver address */
	if (!p_pktinfo->is_to_self)
		return;

	p_dm_swat_table->pkt_cnt_sw_ant_div_by_ctrl_frame++;
	is_cck_rate = ((p_pktinfo->data_rate >= DESC_RATE1M) && (p_pktinfo->data_rate <= DESC_RATE11M)) ? true : false;

	if (is_cck_rate) {
		p_dm_fat_table->antsel_rx_keep_0 = (p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? ANT1_2G : ANT2_2G;

		if (p_dm_fat_table->antsel_rx_keep_0 == ANT1_2G)
			p_dm_fat_table->cck_ctrl_frame_cnt_main++;
		else
			p_dm_fat_table->cck_ctrl_frame_cnt_aux++;

		odm_antsel_statistics_of_ctrl_frame(p_dm, p_dm_fat_table->antsel_rx_keep_0, p_phy_info->rx_mimo_signal_strength[RF_PATH_A]);
	} else {
		p_dm_fat_table->antsel_rx_keep_0 = (p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? ANT1_2G : ANT2_2G;
		
		if (p_dm_fat_table->antsel_rx_keep_0 == ANT1_2G)
			p_dm_fat_table->ofdm_ctrl_frame_cnt_main++;
		else
			p_dm_fat_table->ofdm_ctrl_frame_cnt_aux++;

		odm_antsel_statistics_of_ctrl_frame(p_dm, p_dm_fat_table->antsel_rx_keep_0, p_phy_info->rx_pwdb_all);
	}
}

#endif /* #if (RTL8723B_SUPPORT == 1) || (RTL8821A_SUPPORT == 1) */

void odm_set_next_mac_addr_target(void *p_dm_void)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct			*p_dm_fat_table = &p_dm->dm_fat_table;
	struct cmn_sta_info	*p_entry;
	u32			value32, i;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("odm_set_next_mac_addr_target() ==>\n"));

	if (p_dm->is_linked) {
		for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {

			if ((p_dm_fat_table->train_idx + 1) == ODM_ASSOCIATE_ENTRY_NUM)
				p_dm_fat_table->train_idx = 0;
			else
				p_dm_fat_table->train_idx++;

			p_entry = p_dm->p_phydm_sta_info[p_dm_fat_table->train_idx];

			if (is_sta_active(p_entry)) {

				/*Match MAC ADDR*/
				value32 = (p_entry->mac_addr[5] << 8) | p_entry->mac_addr[4];

				odm_set_mac_reg(p_dm, 0x7b4, 0xFFFF, value32);/*0x7b4~0x7b5*/

				value32 = (p_entry->mac_addr[3] << 24) | (p_entry->mac_addr[2] << 16) | (p_entry->mac_addr[1] << 8) | p_entry->mac_addr[0];

				odm_set_mac_reg(p_dm, 0x7b0, MASKDWORD, value32);/*0x7b0~0x7b3*/

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("p_dm_fat_table->train_idx=%d\n", p_dm_fat_table->train_idx));

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Training MAC addr = %x:%x:%x:%x:%x:%x\n",
					p_entry->mac_addr[5], p_entry->mac_addr[4], p_entry->mac_addr[3], p_entry->mac_addr[2], p_entry->mac_addr[1], p_entry->mac_addr[0]));

				break;
			}
		}
	}

}

#if (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))

void
odm_fast_ant_training(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;

	u32	max_rssi_path_a = 0, pckcnt_path_a = 0;
	u8	i, target_ant_path_a = 0;
	boolean	is_pkt_filter_macth_path_a = false;

	if (!p_dm->is_linked) { /* is_linked==False */
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[No Link!!!]\n"));

		if (p_dm_fat_table->is_become_linked == true) {
			odm_ant_div_on_off(p_dm, ANTDIV_OFF);
			phydm_fast_training_enable(p_dm, FAT_OFF);
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);
			p_dm_fat_table->is_become_linked = p_dm->is_linked;
		}
		return;
	} else {
		if (p_dm_fat_table->is_become_linked == false) {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Linked!!!]\n"));
			p_dm_fat_table->is_become_linked = p_dm->is_linked;
		}
	}

	if (*(p_dm_fat_table->p_force_tx_ant_by_desc) == false) {
		if (p_dm->is_one_entry_only == true)
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);
		else
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_DESC);
	}


	if (p_dm->support_ic_type == ODM_RTL8188E)
		odm_set_bb_reg(p_dm, 0x864, BIT(2) | BIT(1) | BIT(0), ((p_dm->fat_comb_a) - 1));

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("==>odm_fast_ant_training()\n"));

	/* 1 TRAINING STATE */
	if (p_dm_fat_table->fat_state == FAT_TRAINING_STATE) {
		/* 2 Caculate RSSI per Antenna */

		/* 3 [path-A]--------------------------- */
		for (i = 0; i < (p_dm->fat_comb_a); i++) { /* i : antenna index */
			if (p_dm_fat_table->ant_rssi_cnt[i] == 0)
				p_dm_fat_table->ant_ave_rssi[i] = 0;
			else {
				p_dm_fat_table->ant_ave_rssi[i] = p_dm_fat_table->ant_sum_rssi[i] / p_dm_fat_table->ant_rssi_cnt[i];
				is_pkt_filter_macth_path_a = true;
			}

			if (p_dm_fat_table->ant_ave_rssi[i] > max_rssi_path_a) {
				max_rssi_path_a = p_dm_fat_table->ant_ave_rssi[i];
				pckcnt_path_a = p_dm_fat_table->ant_rssi_cnt[i];
				target_ant_path_a =  i ;
			} else if (p_dm_fat_table->ant_ave_rssi[i] == max_rssi_path_a) {
				if ((p_dm_fat_table->ant_rssi_cnt[i])   >   pckcnt_path_a) {
					max_rssi_path_a = p_dm_fat_table->ant_ave_rssi[i];
					pckcnt_path_a = p_dm_fat_table->ant_rssi_cnt[i];
					target_ant_path_a = i ;
				}
			}

			PHYDM_DBG("*** ant-index : [ %d ],      counter = (( %d )),     Avg RSSI = (( %d ))\n", i, p_dm_fat_table->ant_rssi_cnt[i],  p_dm_fat_table->ant_ave_rssi[i]);
		}

		/* 1 DECISION STATE */

		/* 2 Select TRX Antenna */

		phydm_fast_training_enable(p_dm, FAT_OFF);

		/* 3 [path-A]--------------------------- */
		if (is_pkt_filter_macth_path_a  == false) {
			/* PHYDM_DBG(p_dm,DBG_ANT_DIV, ("{path-A}: None Packet is matched\n")); */
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("{path-A}: None Packet is matched\n"));
			odm_ant_div_on_off(p_dm, ANTDIV_OFF);
		} else {
			PHYDM_DBG("target_ant_path_a = (( %d )) , max_rssi_path_a = (( %d ))\n", target_ant_path_a, max_rssi_path_a);

			/* 3 [ update RX-optional ant ]        Default RX is Omni, Optional RX is the best decision by FAT */
			if (p_dm->support_ic_type == ODM_RTL8188E)
				odm_set_bb_reg(p_dm, 0x864, BIT(8) | BIT(7) | BIT(6), target_ant_path_a);
			else if (p_dm->support_ic_type == ODM_RTL8192E)
				odm_set_bb_reg(p_dm, 0xB38, BIT(8) | BIT(7) | BIT(6), target_ant_path_a); /* Optional RX [pth-A] */

			/* 3 [ update TX ant ] */
			odm_update_tx_ant(p_dm, target_ant_path_a, (p_dm_fat_table->train_idx));

			if (target_ant_path_a == 0)
				odm_ant_div_on_off(p_dm, ANTDIV_OFF);
		}

		/* 2 Reset counter */
		for (i = 0; i < (p_dm->fat_comb_a); i++) {
			p_dm_fat_table->ant_sum_rssi[i] = 0;
			p_dm_fat_table->ant_rssi_cnt[i] = 0;
		}

		p_dm_fat_table->fat_state = FAT_PREPARE_STATE;
		return;
	}

	/* 1 NORMAL STATE */
	if (p_dm_fat_table->fat_state == FAT_PREPARE_STATE) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Start Prepare state ]\n"));

		odm_set_next_mac_addr_target(p_dm);

		/* 2 Prepare Training */
		p_dm_fat_table->fat_state = FAT_TRAINING_STATE;
		phydm_fast_training_enable(p_dm, FAT_ON);
		odm_ant_div_on_off(p_dm, ANTDIV_ON);		/* enable HW AntDiv */
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Start Training state]\n"));

		odm_set_timer(p_dm, &p_dm->fast_ant_training_timer, p_dm->antdiv_intvl); /* ms */
	}
}

void
odm_fast_ant_training_callback(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct _ADAPTER	*padapter = p_dm->adapter;
	if (padapter->net_closed == true)
		return;
	/* if(*p_dm->p_is_net_closed == true) */
	/* return; */
#endif

#if USE_WORKITEM
	odm_schedule_work_item(&p_dm->fast_ant_training_workitem);
#else
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("******odm_fast_ant_training_callback******\n"));
	odm_fast_ant_training(p_dm);
#endif
}

void
odm_fast_ant_training_work_item_callback(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("******odm_fast_ant_training_work_item_callback******\n"));
	odm_fast_ant_training(p_dm);
}

#endif

void
odm_ant_div_init(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct			*p_dm_fat_table = &p_dm->dm_fat_table;
	struct _sw_antenna_switch_			*p_dm_swat_table = &p_dm->dm_swat_table;


	if (!(p_dm->support_ability & ODM_BB_ANT_DIV)) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Return!!!]   Not Support Antenna Diversity Function\n"));
		return;
	}

	/* 2 [--General---] */
	p_dm->antdiv_period = 0;

	p_dm_fat_table->is_become_linked = false;
	p_dm_fat_table->ant_div_on_off = 0xff;

	/* 3       -   AP   - */

	/* 2 [---Set MAIN_ANT as default antenna if Auto-ant enable---] */
	odm_ant_div_on_off(p_dm, ANTDIV_OFF);

	p_dm->ant_type = ODM_AUTO_ANT;

	p_dm_fat_table->rx_idle_ant = 0xff; /*to make RX-idle-antenna will be updated absolutly*/
	odm_update_rx_idle_ant(p_dm, MAIN_ANT);
	phydm_keep_rx_ack_ant_by_tx_ant_time(p_dm, 0);  /* Timming issue: keep Rx ant after tx for ACK ( 5 x 3.2 mu = 16mu sec)*/

	/* 2 [---Set TX Antenna---] */
	if (p_dm_fat_table->p_force_tx_ant_by_desc == NULL) {
	p_dm_fat_table->force_tx_ant_by_desc = 0;
	p_dm_fat_table->p_force_tx_ant_by_desc = &(p_dm_fat_table->force_tx_ant_by_desc);
	}
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("p_force_tx_ant_by_desc = %d\n", *p_dm_fat_table->p_force_tx_ant_by_desc));

	if (*(p_dm_fat_table->p_force_tx_ant_by_desc) == true)
		odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_DESC);
	else
	odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);

	/* 2 [--8723B---] */
#if (RTL8723B_SUPPORT == 1)
	if (p_dm->support_ic_type == ODM_RTL8723B) {
		p_dm->ant_div_type = S0S1_SW_ANTDIV;
		/* p_dm->ant_div_type = CG_TRX_HW_ANTDIV; */

		if (p_dm->ant_div_type != S0S1_SW_ANTDIV && p_dm->ant_div_type != CG_TRX_HW_ANTDIV) {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Return!!!] 8723B  Not Supprrt This AntDiv type\n"));
			p_dm->support_ability &= ~(ODM_BB_ANT_DIV);
			return;
		}

		if (p_dm->ant_div_type == S0S1_SW_ANTDIV)
			odm_s0s1_sw_ant_div_init_8723b(p_dm);
		else if (p_dm->ant_div_type == CG_TRX_HW_ANTDIV)
			odm_trx_hw_ant_div_init_8723b(p_dm);
	}
#endif

	/*
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** support_ic_type=[%lu]\n",p_dm->support_ic_type));
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** AntDiv support_ability=[%lu]\n",(p_dm->support_ability & ODM_BB_ANT_DIV)>>6));
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("*** AntDiv type=[%d]\n",p_dm->ant_div_type));
	*/
}

void
odm_ant_div(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ADAPTER		*p_adapter	= p_dm->adapter;
	struct phydm_fat_struct			*p_dm_fat_table = &p_dm->dm_fat_table;
#if (defined(CONFIG_HL_SMART_ANTENNA))
	struct smt_ant_honbo			*pdm_sat_table = &(p_dm->dm_sat_table);
#endif

#ifdef ODM_EVM_ENHANCE_ANTDIV

	if (p_dm->is_linked) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("tp_active_occur=((%d)), EVM_method_enable=((%d))\n",
		p_dm->tp_active_occur, p_dm_fat_table->EVM_method_enable));

		if ((p_dm->tp_active_occur == 1) && (p_dm_fat_table->EVM_method_enable == 1)) {

			p_dm_fat_table->idx_ant_div_counter_5g = p_dm->antdiv_period;
			p_dm_fat_table->idx_ant_div_counter_2g = p_dm->antdiv_period;
		}
	}
#endif

	if (*p_dm->p_band_type == ODM_BAND_5G) {
		if (p_dm_fat_table->idx_ant_div_counter_5g <  p_dm->antdiv_period) {
			p_dm_fat_table->idx_ant_div_counter_5g++;
			return;
		} else
			p_dm_fat_table->idx_ant_div_counter_5g = 0;
	} else	if (*p_dm->p_band_type == ODM_BAND_2_4G) {
		if (p_dm_fat_table->idx_ant_div_counter_2g <  p_dm->antdiv_period) {
			p_dm_fat_table->idx_ant_div_counter_2g++;
			return;
		} else
			p_dm_fat_table->idx_ant_div_counter_2g = 0;
	}

	/* ---------- */

	/* ---------- */
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN || DM_ODM_SUPPORT_TYPE == ODM_CE)

	if (p_dm_fat_table->enable_ctrl_frame_antdiv) {

		if ((p_dm->data_frame_num <= 10) && (p_dm->is_linked))
			p_dm_fat_table->use_ctrl_frame_antdiv = 1;
		else
			p_dm_fat_table->use_ctrl_frame_antdiv = 0;

		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("use_ctrl_frame_antdiv = (( %d )), data_frame_num = (( %d ))\n", p_dm_fat_table->use_ctrl_frame_antdiv, p_dm->data_frame_num));
		p_dm->data_frame_num = 0;
	}

	if (p_adapter->MgntInfo.AntennaTest)
		return;

#endif

	/* ---------- */

	if (p_dm->antdiv_select == 1)
		p_dm->ant_type = ODM_FIX_MAIN_ANT;
	else if (p_dm->antdiv_select == 2)
		p_dm->ant_type = ODM_FIX_AUX_ANT;
	else { /* if (p_dm->antdiv_select==0) */
		p_dm->ant_type = ODM_AUTO_ANT;
	}

	/* PHYDM_DBG(p_dm, DBG_ANT_DIV,("ant_type= (( %d )) , pre_ant_type= (( %d ))\n",p_dm->ant_type,p_dm->pre_ant_type)); */

	if (p_dm->ant_type != ODM_AUTO_ANT) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Fix Antenna at (( %s ))\n", (p_dm->ant_type == ODM_FIX_MAIN_ANT) ? "MAIN" : "AUX"));

		if (p_dm->ant_type != p_dm->pre_ant_type) {
			odm_ant_div_on_off(p_dm, ANTDIV_OFF);
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_REG);

			if (p_dm->ant_type == ODM_FIX_MAIN_ANT)
				odm_update_rx_idle_ant(p_dm, MAIN_ANT);
			else if (p_dm->ant_type == ODM_FIX_AUX_ANT)
				odm_update_rx_idle_ant(p_dm, AUX_ANT);
		}
		p_dm->pre_ant_type = p_dm->ant_type;
		return;
	} else {
		if (p_dm->ant_type != p_dm->pre_ant_type) {
			odm_ant_div_on_off(p_dm, ANTDIV_ON);
			odm_tx_by_tx_desc_or_reg(p_dm, TX_BY_DESC);
		}
		p_dm->pre_ant_type = p_dm->ant_type;
	}


	/* 3 ----------------------------------------------------------------------------------------------------------- */

#if (RTL8723B_SUPPORT == 1)
	/* 2 [--8723B---] */
	if (p_dm->support_ic_type == ODM_RTL8723B) {
		if (phydm_is_bt_enable_8723b(p_dm)) {
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[BT is enable!!!]\n"));
			if (p_dm_fat_table->is_become_linked == true) {
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Set REG 948[9:6]=0x0\n"));
				if (p_dm->support_ic_type == ODM_RTL8723B)
					odm_set_bb_reg(p_dm, 0x948, BIT(9) | BIT(8) | BIT(7) | BIT(6), 0x0);

				p_dm_fat_table->is_become_linked = false;
			}
		} else {
			if (p_dm->ant_div_type == S0S1_SW_ANTDIV) {

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
				odm_s0s1_sw_ant_div(p_dm, SWAW_STEP_PEEK);
#endif
			} else if (p_dm->ant_div_type == CG_TRX_HW_ANTDIV)
				odm_hw_ant_div(p_dm);
		}
	}
#endif
}

void odm_antsel_statistics(
	void			*p_dm_void,
	void			*p_phy_info_void,
	u8			antsel_tr_mux,
	u32			mac_id,
	u32			utility,
	u8			method,
	u8			is_cck_rate
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;
	struct phydm_phyinfo_struct	*p_phy_info = (struct phydm_phyinfo_struct *)p_phy_info_void;

	if (method == RSSI_METHOD) {

		if (is_cck_rate) {
			if (antsel_tr_mux == ANT1_2G) {
				if (p_dm_fat_table->main_ant_sum_cck[mac_id] > 65435) /*to prevent u16 overflow, max(RSSI)=100, 65435+100 = 65535 (u16)*/
					return;

				p_dm_fat_table->main_ant_sum_cck[mac_id] += (u16)utility;
				p_dm_fat_table->main_ant_cnt_cck[mac_id]++;
			} else {
				if (p_dm_fat_table->aux_ant_sum_cck[mac_id] > 65435)
					return;

				p_dm_fat_table->aux_ant_sum_cck[mac_id] += (u16)utility;
				p_dm_fat_table->aux_ant_cnt_cck[mac_id]++;
			}

		} else { /*ofdm rate*/

			if (antsel_tr_mux == ANT1_2G) {
				if (p_dm_fat_table->main_ant_sum[mac_id] > 65435)
					return;

				p_dm_fat_table->main_ant_sum[mac_id] += (u16)utility;
				p_dm_fat_table->main_ant_cnt[mac_id]++;
			} else {
				if (p_dm_fat_table->aux_ant_sum[mac_id] > 65435)
					return;

				p_dm_fat_table->aux_ant_sum[mac_id] += (u16)utility;
				p_dm_fat_table->aux_ant_cnt[mac_id]++;
			}
		}
	}
#ifdef ODM_EVM_ENHANCE_ANTDIV
	else if (method == EVM_METHOD) {
		if (p_dm->rate_ss == 1) {

			if (antsel_tr_mux == ANT1_2G) {
				p_dm_fat_table->main_ant_evm_sum[mac_id] += ((p_phy_info->rx_mimo_evm_dbm[0])<<5);
				p_dm_fat_table->main_ant_evm_cnt[mac_id]++;
			} else {
				p_dm_fat_table->aux_ant_evm_sum[mac_id] += ((p_phy_info->rx_mimo_evm_dbm[0])<<5);
				p_dm_fat_table->aux_ant_evm_cnt[mac_id]++;
			}

		} else {/*>= 2SS*/

			if (antsel_tr_mux == ANT1_2G) {

				p_dm_fat_table->main_ant_evm_2ss_sum[mac_id][0] += (p_phy_info->rx_mimo_evm_dbm[0]<<5);
				p_dm_fat_table->main_ant_evm_2ss_sum[mac_id][1] += (p_phy_info->rx_mimo_evm_dbm[1]<<5);
				p_dm_fat_table->main_ant_evm_2ss_cnt[mac_id]++;

			} else {

				p_dm_fat_table->aux_ant_evm_2ss_sum[mac_id][0] += (p_phy_info->rx_mimo_evm_dbm[0]<<5);
				p_dm_fat_table->aux_ant_evm_2ss_sum[mac_id][1] += (p_phy_info->rx_mimo_evm_dbm[1]<<5);
				p_dm_fat_table->aux_ant_evm_2ss_cnt[mac_id]++;
			}
		}

	} else if (method == CRC32_METHOD) {

		if (antsel_tr_mux == ANT1_2G) {
			p_dm_fat_table->main_crc32_ok_cnt += utility;
			p_dm_fat_table->main_crc32_fail_cnt++;
		} else {
			p_dm_fat_table->aux_crc32_ok_cnt += utility;
			p_dm_fat_table->aux_crc32_fail_cnt++;
		}

	} else if (method == TP_METHOD) {
		if (((utility <= ODM_RATEMCS15) && (utility >= ODM_RATEMCS0)) &&
			(p_dm_fat_table->fat_state_cnt <= p_dm->antdiv_tp_period)
		) {

			if (antsel_tr_mux == ANT1_2G) {
				p_dm_fat_table->antdiv_tp_main += (phy_rate_table[utility])<<5;
				p_dm_fat_table->antdiv_tp_main_cnt++;
			} else {
				p_dm_fat_table->antdiv_tp_aux += (phy_rate_table[utility])<<5;
				p_dm_fat_table->antdiv_tp_aux_cnt++;
			}
		}
	}
#endif
}

void
odm_process_rssi_for_ant_div(
	void			*p_dm_void,
	void			*p_phy_info_void,
	void			*p_pkt_info_void
)
{
	struct PHY_DM_STRUCT				*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_phyinfo_struct			*p_phy_info = (struct phydm_phyinfo_struct *)p_phy_info_void;
	struct phydm_perpkt_info_struct				*p_pktinfo = (struct phydm_perpkt_info_struct *)p_pkt_info_void;
	struct phydm_fat_struct		*p_dm_fat_table = &p_dm->dm_fat_table;
#if (defined(CONFIG_HL_SMART_ANTENNA))
	struct smt_ant_honbo	*pdm_sat_table = &(p_dm->dm_sat_table);
	u32			beam_tmp;
	u8			next_ant;
	u8			train_pkt_number;
#endif
	u8			is_cck_rate = FALSE;
	u8			rx_power_ant0 = p_phy_info->rx_mimo_signal_strength[0];
	u8			rx_power_ant1 = p_phy_info->rx_mimo_signal_strength[1];
	u8			rx_evm_ant0 = p_phy_info->rx_mimo_signal_quality[0];
	u8			rx_evm_ant1 = p_phy_info->rx_mimo_signal_quality[1];
	u8			rssi_avg;

	is_cck_rate = (p_pktinfo->data_rate <= ODM_RATE11M) ? true : false;

	if ((p_dm->support_ic_type & ODM_IC_2SS) && (!is_cck_rate)) {

		if (rx_power_ant1 < 100)
			rssi_avg = (u8)odm_convert_to_db((odm_convert_to_linear(rx_power_ant0) + odm_convert_to_linear(rx_power_ant1))>>1); /*averaged PWDB*/
		
	} else {
		rx_power_ant0 = (u8)p_phy_info->rx_pwdb_all;
		rssi_avg = rx_power_ant0;
	}
	
#ifdef CONFIG_HL_SMART_ANTENNA_TYPE2
	if ((p_dm->ant_div_type == HL_SW_SMART_ANT_TYPE2) && (p_dm_fat_table->fat_state == FAT_TRAINING_STATE))
		phydm_process_rssi_for_hb_smtant_type2(p_dm, p_phy_info, p_pktinfo, rssi_avg);	/*for 8822B*/
	else
#endif

#ifdef CONFIG_HL_SMART_ANTENNA_TYPE1
#ifdef CONFIG_FAT_PATCH
	if ((p_dm->ant_div_type == HL_SW_SMART_ANT_TYPE1) && (p_dm_fat_table->fat_state == FAT_TRAINING_STATE)) {

		/*[Beacon]*/
		if (p_pktinfo->is_packet_beacon) {

			pdm_sat_table->beacon_counter++;
			PHYDM_DBG(p_dm, DBG_ANT_DIV, ("MatchBSSID_beacon_counter = ((%d))\n", pdm_sat_table->beacon_counter));

			if (pdm_sat_table->beacon_counter >= pdm_sat_table->pre_beacon_counter + 2) {

				if (pdm_sat_table->ant_num > 1) {
					next_ant = (p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
					odm_update_rx_idle_ant(p_dm, next_ant);
				}

				pdm_sat_table->update_beam_idx++;

				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("pre_beacon_counter = ((%d)), pkt_counter = ((%d)), update_beam_idx = ((%d))\n",
					pdm_sat_table->pre_beacon_counter, pdm_sat_table->pkt_counter, pdm_sat_table->update_beam_idx));

				pdm_sat_table->pre_beacon_counter = pdm_sat_table->beacon_counter;
				pdm_sat_table->pkt_counter = 0;
			}
		}
		/*[data]*/
		else if (p_pktinfo->is_packet_to_self) {

			if (pdm_sat_table->pkt_skip_statistic_en == 0) {
				/*
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("StaID[%d]:  antsel_pathA = ((%d)), hw_antsw_occur = ((%d)), Beam_num = ((%d)), RSSI = ((%d))\n",
					p_pktinfo->station_id, p_dm_fat_table->antsel_rx_keep_0, p_dm_fat_table->hw_antsw_occur, pdm_sat_table->fast_training_beam_num, rx_power_ant0));
				*/
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("ID[%d][pkt_cnt = %d]: {ANT, Beam} = {%d, %d}, RSSI = ((%d))\n",
					p_pktinfo->station_id, pdm_sat_table->pkt_counter, p_dm_fat_table->antsel_rx_keep_0, pdm_sat_table->fast_training_beam_num, rx_power_ant0));

				pdm_sat_table->pkt_rssi_sum[p_dm_fat_table->antsel_rx_keep_0][pdm_sat_table->fast_training_beam_num] += rx_power_ant0;
				pdm_sat_table->pkt_rssi_cnt[p_dm_fat_table->antsel_rx_keep_0][pdm_sat_table->fast_training_beam_num]++;
				pdm_sat_table->pkt_counter++;

				#if 1
				train_pkt_number = pdm_sat_table->beam_train_cnt[p_dm_fat_table->rx_idle_ant - 1][pdm_sat_table->fast_training_beam_num];
				#else
				train_pkt_number =  pdm_sat_table->per_beam_training_pkt_num;
				#endif

				/*Swich Antenna erery N pkts*/
				if (pdm_sat_table->pkt_counter == train_pkt_number) {

					if (pdm_sat_table->ant_num > 1) {

						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("packet enugh ((%d ))pkts ---> Switch antenna\n", train_pkt_number));
						next_ant = (p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
						odm_update_rx_idle_ant(p_dm, next_ant);
					}

					pdm_sat_table->update_beam_idx++;
					PHYDM_DBG(p_dm, DBG_ANT_DIV, ("pre_beacon_counter = ((%d)), update_beam_idx_counter = ((%d))\n",
						pdm_sat_table->pre_beacon_counter, pdm_sat_table->update_beam_idx));

					pdm_sat_table->pre_beacon_counter = pdm_sat_table->beacon_counter;
					pdm_sat_table->pkt_counter = 0;
				}
			}
		}

		/*Swich Beam after switch "pdm_sat_table->ant_num" antennas*/
		if (pdm_sat_table->update_beam_idx == pdm_sat_table->ant_num) {

			pdm_sat_table->update_beam_idx = 0;
			pdm_sat_table->pkt_counter = 0;
			beam_tmp = pdm_sat_table->fast_training_beam_num;

			if (pdm_sat_table->fast_training_beam_num >= (pdm_sat_table->beam_patten_num_each_ant - 1)) {

				p_dm_fat_table->fat_state = FAT_DECISION_STATE;

				#if DEV_BUS_TYPE == RT_PCI_INTERFACE
				odm_fast_ant_training_hl_smart_antenna_type1(p_dm);
				#else
				odm_schedule_work_item(&pdm_sat_table->hl_smart_antenna_decision_workitem);
				#endif


			} else {
				pdm_sat_table->fast_training_beam_num++;
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Update Beam_num (( %d )) -> (( %d ))\n", beam_tmp, pdm_sat_table->fast_training_beam_num));
				phydm_set_all_ant_same_beam_num(p_dm);

				p_dm_fat_table->fat_state = FAT_TRAINING_STATE;
			}
		}

	}
#else

	if (p_dm->ant_div_type == HL_SW_SMART_ANT_TYPE1) {
		if ((p_dm->support_ic_type & ODM_HL_SMART_ANT_TYPE1_SUPPORT) &&
		    (p_pktinfo->is_packet_to_self)   &&
		    (p_dm_fat_table->fat_state == FAT_TRAINING_STATE)
		   ) {

			if (pdm_sat_table->pkt_skip_statistic_en == 0) {
				/*
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("StaID[%d]:  antsel_pathA = ((%d)), hw_antsw_occur = ((%d)), Beam_num = ((%d)), RSSI = ((%d))\n",
					p_pktinfo->station_id, p_dm_fat_table->antsel_rx_keep_0, p_dm_fat_table->hw_antsw_occur, pdm_sat_table->fast_training_beam_num, rx_power_ant0));
				*/
				PHYDM_DBG(p_dm, DBG_ANT_DIV, ("StaID[%d]:  antsel_pathA = ((%d)), is_packet_to_self = ((%d)), Beam_num = ((%d)), RSSI = ((%d))\n",
					p_pktinfo->station_id, p_dm_fat_table->antsel_rx_keep_0, p_pktinfo->is_packet_to_self, pdm_sat_table->fast_training_beam_num, rx_power_ant0));


				pdm_sat_table->pkt_rssi_sum[p_dm_fat_table->antsel_rx_keep_0][pdm_sat_table->fast_training_beam_num] += rx_power_ant0;
				pdm_sat_table->pkt_rssi_cnt[p_dm_fat_table->antsel_rx_keep_0][pdm_sat_table->fast_training_beam_num]++;
				pdm_sat_table->pkt_counter++;

				/*swich beam every N pkt*/
				if ((pdm_sat_table->pkt_counter) >= (pdm_sat_table->per_beam_training_pkt_num)) {

					pdm_sat_table->pkt_counter = 0;
					beam_tmp = pdm_sat_table->fast_training_beam_num;

					if (pdm_sat_table->fast_training_beam_num >= (pdm_sat_table->beam_patten_num_each_ant - 1)) {

						p_dm_fat_table->fat_state = FAT_DECISION_STATE;

						#if DEV_BUS_TYPE == RT_PCI_INTERFACE
						odm_fast_ant_training_hl_smart_antenna_type1(p_dm);
						#else
						odm_schedule_work_item(&pdm_sat_table->hl_smart_antenna_decision_workitem);
						#endif


					} else {
						pdm_sat_table->fast_training_beam_num++;
						phydm_set_all_ant_same_beam_num(p_dm);

						p_dm_fat_table->fat_state = FAT_TRAINING_STATE;
						PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Update  Beam_num (( %d )) -> (( %d ))\n", beam_tmp, pdm_sat_table->fast_training_beam_num));
					}
				}
			}
		}
	}
#endif
	else
#endif
		if (p_dm->ant_div_type == CG_TRX_SMART_ANTDIV) {
			if ((p_dm->support_ic_type & ODM_SMART_ANT_SUPPORT) && (p_pktinfo->is_packet_to_self)   && (p_dm_fat_table->fat_state == FAT_TRAINING_STATE)) { /* (p_pktinfo->is_packet_match_bssid && (!p_pktinfo->is_packet_beacon)) */
				u8	antsel_tr_mux;
				antsel_tr_mux = (p_dm_fat_table->antsel_rx_keep_2 << 2) | (p_dm_fat_table->antsel_rx_keep_1 << 1) | p_dm_fat_table->antsel_rx_keep_0;
				p_dm_fat_table->ant_sum_rssi[antsel_tr_mux] += rx_power_ant0;
				p_dm_fat_table->ant_rssi_cnt[antsel_tr_mux]++;
			}
		} else { /* ant_div_type != CG_TRX_SMART_ANTDIV */
			if ((p_dm->support_ic_type & ODM_ANTDIV_SUPPORT) && (p_pktinfo->is_packet_to_self || p_dm_fat_table->use_ctrl_frame_antdiv)) {

				if (p_dm->ant_div_type == S0S1_SW_ANTDIV) {

					if (is_cck_rate || (p_dm->support_ic_type == ODM_RTL8188F))
						p_dm_fat_table->antsel_rx_keep_0 = (p_dm_fat_table->rx_idle_ant == MAIN_ANT) ? ANT1_2G : ANT2_2G;

						odm_antsel_statistics(p_dm, p_phy_info, p_dm_fat_table->antsel_rx_keep_0, p_pktinfo->station_id, rx_power_ant0, RSSI_METHOD, is_cck_rate);

				} else {
					
					odm_antsel_statistics(p_dm, p_phy_info, p_dm_fat_table->antsel_rx_keep_0, p_pktinfo->station_id, rx_power_ant0, RSSI_METHOD, is_cck_rate);

					#ifdef ODM_EVM_ENHANCE_ANTDIV
					if (p_dm->support_ic_type == ODM_RTL8192E) {
						if (!is_cck_rate) {
							odm_antsel_statistics(p_dm, p_phy_info, p_dm_fat_table->antsel_rx_keep_0, p_pktinfo->station_id, rx_evm_ant0, EVM_METHOD, is_cck_rate);
							odm_antsel_statistics(p_dm, p_phy_info, p_dm_fat_table->antsel_rx_keep_0, p_pktinfo->station_id, rx_evm_ant0, TP_METHOD, is_cck_rate);
						}

					}
					#endif
				}
			}
		}
	/* PHYDM_DBG(p_dm,DBG_ANT_DIV,("is_cck_rate=%d, PWDB_ALL=%d\n",is_cck_rate, p_phy_info->rx_pwdb_all)); */
	/* PHYDM_DBG(p_dm,DBG_ANT_DIV,("antsel_tr_mux=3'b%d%d%d\n",p_dm_fat_table->antsel_rx_keep_2, p_dm_fat_table->antsel_rx_keep_1, p_dm_fat_table->antsel_rx_keep_0)); */
}

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
void
odm_set_tx_ant_by_tx_info(
	void			*p_dm_void,
	u8			*p_desc,
	u8			mac_id

)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct	*p_dm_fat_table = &p_dm->dm_fat_table;

	if (!(p_dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (p_dm->ant_div_type == CGCS_RX_HW_ANTDIV)
		return;

	if (p_dm->support_ic_type == ODM_RTL8723B) {
#if (RTL8723B_SUPPORT == 1)
		SET_TX_DESC_ANTSEL_A_8723B(p_desc, p_dm_fat_table->antsel_a[mac_id]);
		/*PHYDM_DBG(p_dm,DBG_ANT_DIV, ("[8723B] SetTxAntByTxInfo_WIN: mac_id=%d, antsel_tr_mux=3'b%d%d%d\n",
			mac_id, p_dm_fat_table->antsel_c[mac_id], p_dm_fat_table->antsel_b[mac_id], p_dm_fat_table->antsel_a[mac_id]));*/
#endif
	}
}
#elif (DM_ODM_SUPPORT_TYPE == ODM_AP)

void odm_set_tx_ant_by_tx_info(
	struct	rtl8192cd_priv		*priv,
	struct	tx_desc	*pdesc,
	unsigned short			aid
)
{
	struct PHY_DM_STRUCT	*p_dm = GET_PDM_ODM(priv);/*&(priv->pshare->_dmODM);*/
	struct phydm_fat_struct		*p_dm_fat_table = &(p_dm->dm_fat_table);

	if (!(p_dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (p_dm->ant_div_type == CGCS_RX_HW_ANTDIV)
		return;

	if (p_dm->support_ic_type == ODM_RTL8881A) {
		/*panic_printk("[%s] [%d]   ******ODM_SetTxAntByTxInfo_8881E******\n",__FUNCTION__,__LINE__);	*/
		pdesc->Dword6 &= set_desc(~(BIT(18) | BIT(17) | BIT(16)));
		pdesc->Dword6 |= set_desc(p_dm_fat_table->antsel_a[aid] << 16);
	} else if (p_dm->support_ic_type == ODM_RTL8192E) {
		/*panic_printk("[%s] [%d]   ******ODM_SetTxAntByTxInfo_8192E******\n",__FUNCTION__,__LINE__);	*/
		pdesc->Dword6 &= set_desc(~(BIT(18) | BIT(17) | BIT(16)));
		pdesc->Dword6 |= set_desc(p_dm_fat_table->antsel_a[aid] << 16);
	} else if (p_dm->support_ic_type == ODM_RTL8188E) {
		/*panic_printk("[%s] [%d]   ******ODM_SetTxAntByTxInfo_8188E******\n",__FUNCTION__,__LINE__);*/
		pdesc->Dword2 &= set_desc(~BIT(24));
		pdesc->Dword2 &= set_desc(~BIT(25));
		pdesc->Dword7 &= set_desc(~BIT(29));

		pdesc->Dword2 |= set_desc(p_dm_fat_table->antsel_a[aid] << 24);
		pdesc->Dword2 |= set_desc(p_dm_fat_table->antsel_b[aid] << 25);
		pdesc->Dword7 |= set_desc(p_dm_fat_table->antsel_c[aid] << 29);


	} else if (p_dm->support_ic_type == ODM_RTL8812) {
		/*[path-A]*/
		/*panic_printk("[%s] [%d]   ******ODM_SetTxAntByTxInfo_8881E******\n",__FUNCTION__,__LINE__);*/

		pdesc->Dword6 &= set_desc(~BIT(16));
		pdesc->Dword6 &= set_desc(~BIT(17));
		pdesc->Dword6 &= set_desc(~BIT(18));

		pdesc->Dword6 |= set_desc(p_dm_fat_table->antsel_a[aid] << 16);
		pdesc->Dword6 |= set_desc(p_dm_fat_table->antsel_b[aid] << 17);
		pdesc->Dword6 |= set_desc(p_dm_fat_table->antsel_c[aid] << 18);

	}
}

#if 1 /*def CONFIG_WLAN_HAL*/
void
odm_set_tx_ant_by_tx_info_hal(
	struct	rtl8192cd_priv		*priv,
	void	*pdesc_data,
	u16					aid
)
{
	struct PHY_DM_STRUCT	*p_dm = GET_PDM_ODM(priv);/*&(priv->pshare->_dmODM);*/
	struct phydm_fat_struct		*p_dm_fat_table = &(p_dm->dm_fat_table);
	PTX_DESC_DATA_88XX	pdescdata = (PTX_DESC_DATA_88XX)pdesc_data;

	if (!(p_dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (p_dm->ant_div_type == CGCS_RX_HW_ANTDIV)
		return;

	if (p_dm->support_ic_type & (ODM_RTL8881A | ODM_RTL8192E | ODM_RTL8814A)) {
		/*panic_printk("[%s] [%d] ******odm_set_tx_ant_by_tx_info_hal******\n",__FUNCTION__,__LINE__);*/
		pdescdata->ant_sel = 1;
		pdescdata->ant_sel_a = p_dm_fat_table->antsel_a[aid];
	}
}
#endif	/*#ifdef CONFIG_WLAN_HAL*/

#endif

void
odm_ant_div_config(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_fat_struct			*p_dm_fat_table = &p_dm->dm_fat_table;
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("WIN Config Antenna Diversity\n"));
	/*
	if(p_dm->support_ic_type==ODM_RTL8723B)
	{
		if((!p_dm->dm_swat_table.ANTA_ON || !p_dm->dm_swat_table.ANTB_ON))
			p_dm->support_ability &= ~(ODM_BB_ANT_DIV);
	}
	*/
	if (p_dm->support_ic_type == ODM_RTL8723D) {

		p_dm->ant_div_type = S0S1_TRX_HW_ANTDIV;
		/**/
	}
#elif (DM_ODM_SUPPORT_TYPE & (ODM_CE))

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("CE Config Antenna Diversity\n"));

	if (p_dm->support_ic_type == ODM_RTL8723B)
		p_dm->ant_div_type = S0S1_SW_ANTDIV;



#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP))

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("AP Config Antenna Diversity\n"));

	/* 2 [ NOT_SUPPORT_ANTDIV ] */
#if (defined(CONFIG_NOT_SUPPORT_ANTDIV))
	p_dm->support_ability &= ~(ODM_BB_ANT_DIV);
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Disable AntDiv function] : Not Support 2.4G & 5G Antenna Diversity\n"));

	/* 2 [ 2G&5G_SUPPORT_ANTDIV ] */
#elif (defined(CONFIG_2G5G_SUPPORT_ANTDIV))
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Enable AntDiv function] : 2.4G & 5G Support Antenna Diversity Simultaneously\n"));
	p_dm_fat_table->ant_div_2g_5g = (ODM_ANTDIV_2G | ODM_ANTDIV_5G);

	if (p_dm->support_ic_type & ODM_ANTDIV_SUPPORT)
		p_dm->support_ability |= ODM_BB_ANT_DIV;
	if (*p_dm->p_band_type == ODM_BAND_5G) {
#if (defined(CONFIG_5G_CGCS_RX_DIVERSITY))
		p_dm->ant_div_type = CGCS_RX_HW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = CGCS_RX_HW_ANTDIV\n"));
		panic_printk("[ 5G] : AntDiv type = CGCS_RX_HW_ANTDIV\n");
#elif (defined(CONFIG_5G_CG_TRX_DIVERSITY) || defined(CONFIG_2G5G_CG_TRX_DIVERSITY_8881A))
		p_dm->ant_div_type = CG_TRX_HW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = CG_TRX_HW_ANTDIV\n"));
		panic_printk("[ 5G] : AntDiv type = CG_TRX_HW_ANTDIV\n");
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY))
		p_dm->ant_div_type = CG_TRX_SMART_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = CG_SMART_ANTDIV\n"));
#elif (defined(CONFIG_5G_S0S1_SW_ANT_DIVERSITY))
		p_dm->ant_div_type = S0S1_SW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = S0S1_SW_ANTDIV\n"));
#endif
	} else if (*p_dm->p_band_type == ODM_BAND_2_4G) {
#if (defined(CONFIG_2G_CGCS_RX_DIVERSITY))
		p_dm->ant_div_type = CGCS_RX_HW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = CGCS_RX_HW_ANTDIV\n"));
#elif (defined(CONFIG_2G_CG_TRX_DIVERSITY) || defined(CONFIG_2G5G_CG_TRX_DIVERSITY_8881A))
		p_dm->ant_div_type = CG_TRX_HW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = CG_TRX_HW_ANTDIV\n"));
#elif (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		p_dm->ant_div_type = CG_TRX_SMART_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = CG_SMART_ANTDIV\n"));
#elif (defined(CONFIG_2G_S0S1_SW_ANT_DIVERSITY))
		p_dm->ant_div_type = S0S1_SW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = S0S1_SW_ANTDIV\n"));
#endif
	}

	/* 2 [ 5G_SUPPORT_ANTDIV ] */
#elif (defined(CONFIG_5G_SUPPORT_ANTDIV))
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Enable AntDiv function] : Only 5G Support Antenna Diversity\n"));
	panic_printk("[ Enable AntDiv function] : Only 5G Support Antenna Diversity\n");
	p_dm_fat_table->ant_div_2g_5g = (ODM_ANTDIV_5G);
	if (*p_dm->p_band_type == ODM_BAND_5G) {
		if (p_dm->support_ic_type & ODM_ANTDIV_5G_SUPPORT_IC)
			p_dm->support_ability |= ODM_BB_ANT_DIV;
#if (defined(CONFIG_5G_CGCS_RX_DIVERSITY))
		p_dm->ant_div_type = CGCS_RX_HW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = CGCS_RX_HW_ANTDIV\n"));
		panic_printk("[ 5G] : AntDiv type = CGCS_RX_HW_ANTDIV\n");
#elif (defined(CONFIG_5G_CG_TRX_DIVERSITY))
		p_dm->ant_div_type = CG_TRX_HW_ANTDIV;
		panic_printk("[ 5G] : AntDiv type = CG_TRX_HW_ANTDIV\n");
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = CG_TRX_HW_ANTDIV\n"));
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY))
		p_dm->ant_div_type = CG_TRX_SMART_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = CG_SMART_ANTDIV\n"));
#elif (defined(CONFIG_5G_S0S1_SW_ANT_DIVERSITY))
		p_dm->ant_div_type = S0S1_SW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 5G] : AntDiv type = S0S1_SW_ANTDIV\n"));
#endif
	} else if (*p_dm->p_band_type == ODM_BAND_2_4G) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Not Support 2G ant_div_type\n"));
		p_dm->support_ability &= ~(ODM_BB_ANT_DIV);
	}

	/* 2 [ 2G_SUPPORT_ANTDIV ] */
#elif (defined(CONFIG_2G_SUPPORT_ANTDIV))
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ Enable AntDiv function] : Only 2.4G Support Antenna Diversity\n"));
	p_dm_fat_table->ant_div_2g_5g = (ODM_ANTDIV_2G);
	if (*p_dm->p_band_type == ODM_BAND_2_4G) {
		if (p_dm->support_ic_type & ODM_ANTDIV_2G_SUPPORT_IC)
			p_dm->support_ability |= ODM_BB_ANT_DIV;
#if (defined(CONFIG_2G_CGCS_RX_DIVERSITY))
		p_dm->ant_div_type = CGCS_RX_HW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = CGCS_RX_HW_ANTDIV\n"));
#elif (defined(CONFIG_2G_CG_TRX_DIVERSITY))
		p_dm->ant_div_type = CG_TRX_HW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = CG_TRX_HW_ANTDIV\n"));
#elif (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		p_dm->ant_div_type = CG_TRX_SMART_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = CG_SMART_ANTDIV\n"));
#elif (defined(CONFIG_2G_S0S1_SW_ANT_DIVERSITY))
		p_dm->ant_div_type = S0S1_SW_ANTDIV;
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[ 2.4G] : AntDiv type = S0S1_SW_ANTDIV\n"));
#endif
	} else if (*p_dm->p_band_type == ODM_BAND_5G) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Not Support 5G ant_div_type\n"));
		p_dm->support_ability &= ~(ODM_BB_ANT_DIV);
	}
#endif
#endif

	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[AntDiv Config Info] AntDiv_SupportAbility = (( %x ))\n", ((p_dm->support_ability & ODM_BB_ANT_DIV) ? 1 : 0)));
	PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[AntDiv Config Info] be_fix_tx_ant = ((%d))\n", p_dm->dm_fat_table.b_fix_tx_ant));

}


void
odm_ant_div_timers(
	void		*p_dm_void,
	u8		state
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	if (state == INIT_ANTDIV_TIMMER) {
#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
		odm_initialize_timer(p_dm, &(p_dm->dm_swat_table.phydm_sw_antenna_switch_timer),
			(void *)odm_sw_antdiv_callback, NULL, "phydm_sw_antenna_switch_timer");
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		odm_initialize_timer(p_dm, &p_dm->fast_ant_training_timer,
			(void *)odm_fast_ant_training_callback, NULL, "fast_ant_training_timer");
#endif

#ifdef ODM_EVM_ENHANCE_ANTDIV
		odm_initialize_timer(p_dm, &p_dm->evm_fast_ant_training_timer,
			(void *)odm_evm_fast_ant_training_callback, NULL, "evm_fast_ant_training_timer");
#endif
	} else if (state == CANCEL_ANTDIV_TIMMER) {
#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
		odm_cancel_timer(p_dm, &(p_dm->dm_swat_table.phydm_sw_antenna_switch_timer));
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		odm_cancel_timer(p_dm, &p_dm->fast_ant_training_timer);
#endif

#ifdef ODM_EVM_ENHANCE_ANTDIV
		odm_cancel_timer(p_dm, &p_dm->evm_fast_ant_training_timer);
#endif
	} else if (state == RELEASE_ANTDIV_TIMMER) {
#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
		odm_release_timer(p_dm, &(p_dm->dm_swat_table.phydm_sw_antenna_switch_timer));
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		odm_release_timer(p_dm, &p_dm->fast_ant_training_timer);
#endif

#ifdef ODM_EVM_ENHANCE_ANTDIV
		odm_release_timer(p_dm, &p_dm->evm_fast_ant_training_timer);
#endif
	}

}

void
phydm_antdiv_debug(
	void		*p_dm_void,
	u32		*const dm_value,
	u32		*_used,
	char			*output,
	u32		*_out_len
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	/*struct phydm_fat_struct*			p_dm_fat_table = &p_dm->dm_fat_table;*/
	u32 used = *_used;
	u32 out_len = *_out_len;

	if (dm_value[0] == 1) { /*fixed or auto antenna*/

		if (dm_value[1] == 0) {
			p_dm->ant_type = ODM_AUTO_ANT;
			PHYDM_SNPRINTF((output + used, out_len - used, "AntDiv: Auto\n"));
		} else if (dm_value[1] == 1) {
			p_dm->ant_type = ODM_FIX_MAIN_ANT;
			PHYDM_SNPRINTF((output + used, out_len - used, "AntDiv: Fix Main\n"));
		} else if (dm_value[1] == 2) {
			p_dm->ant_type = ODM_FIX_AUX_ANT;
			PHYDM_SNPRINTF((output + used, out_len - used, "AntDiv: Fix Aux\n"));
		}

		if (p_dm->ant_type != ODM_AUTO_ANT) {

			odm_stop_antenna_switch_dm(p_dm);
			if (p_dm->ant_type == ODM_FIX_MAIN_ANT)
				odm_update_rx_idle_ant(p_dm, MAIN_ANT);
			else if (p_dm->ant_type == ODM_FIX_AUX_ANT)
				odm_update_rx_idle_ant(p_dm, AUX_ANT);
		} else {
			phydm_enable_antenna_diversity(p_dm);
		}
		p_dm->pre_ant_type = p_dm->ant_type;
	} else if (dm_value[0] == 2) { /*dynamic period for AntDiv*/

		p_dm->antdiv_period = (u8)dm_value[1];
		PHYDM_SNPRINTF((output + used, out_len - used, "AntDiv_period = ((%d))\n", p_dm->antdiv_period));
	}
	*_used = used;
	*_out_len = out_len;
}

#endif /*#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))*/

void odm_ant_div_reset(void *p_dm_void)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	if (p_dm->ant_div_type == S0S1_SW_ANTDIV) {
#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
		odm_s0s1_sw_ant_div_reset(p_dm);
#endif
	}
}

void odm_antenna_diversity_init(void *p_dm_void)
{
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

#if 0
	if (*(p_dm->p_mp_mode) == true)
		return;
#endif

	odm_ant_div_config(p_dm);
	odm_ant_div_init(p_dm);
#endif
}

void odm_antenna_diversity(void *p_dm_void)
{
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))

	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	if (*(p_dm->p_mp_mode) == true)
		return;

	if (!(p_dm->support_ability & ODM_BB_ANT_DIV)) {
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("[Return!!!]   Not Support Antenna Diversity Function\n"));
		return;
	}

	if (p_dm->pause_ability & ODM_BB_ANT_DIV) {
		
		PHYDM_DBG(p_dm, DBG_ANT_DIV, ("Return: Pause AntDIv in LV=%d\n", p_dm->pause_lv_table.lv_antdiv));
		return;
	}

	odm_ant_div(p_dm);
#endif
}
