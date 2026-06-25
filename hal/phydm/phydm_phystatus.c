// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "phydm_precomp.h"

void phydm_rx_statistic_cal(
	struct PHY_DM_STRUCT				*p_phydm,
	u8									*p_phy_status,
	struct phydm_perpkt_info_struct				*p_pktinfo
)
{
	u8									date_rate = (p_pktinfo->data_rate & 0x7f);

	if (date_rate <= ODM_RATE54M)
	{
		p_phydm->phy_dbg_info.num_qry_legacy_pkt[date_rate]++;
	} else if (date_rate <= ODM_RATEMCS31)
	{
		p_phydm->phy_dbg_info.num_qry_ht_pkt[date_rate - ODM_RATEMCS0]++;
		p_phydm->phy_dbg_info.ht_pkt_not_zero = true;
	}
}

void phydm_reset_phystatus_avg(struct PHY_DM_STRUCT *p_dm)
{
	struct phydm_phystatus_avg		*p_dbg_avg = &(p_dm->phy_dbg_info.phystatus_statistic_avg);
	odm_memory_set(p_dm, &(p_dbg_avg->rssi_cck_avg), 0, sizeof(struct phydm_phystatus_avg));
}

void phydm_reset_phystatus_statistic(struct PHY_DM_STRUCT *p_dm)
{
	struct phydm_phystatus_statistic		*p_dbg_statistic = &(p_dm->phy_dbg_info.phystatus_statistic_info);
	odm_memory_set(p_dm, &(p_dbg_statistic->rssi_cck_sum), 0, sizeof(struct phydm_phystatus_statistic));
}

void phydm_avg_phystatus_index(
	struct PHY_DM_STRUCT			*p_dm,
	struct phydm_phyinfo_struct		*p_phy_info,
	struct phydm_perpkt_info_struct			*p_pktinfo
)
{
	u8	rate_ss = phydm_rate_to_num_ss(p_dm, p_pktinfo->data_rate);
	struct phydm_phystatus_statistic		*p_dbg_statistic = &(p_dm->phy_dbg_info.phystatus_statistic_info);

	if (p_pktinfo->data_rate <= ODM_RATE11M) {

		/*RSSI*/
		p_dbg_statistic->rssi_cck_sum += p_phy_info->rx_mimo_signal_strength[0];
		p_dbg_statistic->rssi_cck_cnt++;
	} else if (p_pktinfo->data_rate <= ODM_RATE54M) {

		/*evm*/
		p_dbg_statistic->evm_ofdm_sum += p_phy_info->rx_mimo_evm_dbm[0];

		/*SNR*/
		p_dbg_statistic->snr_ofdm_sum += p_phy_info->rx_snr[0];

		/*RSSI*/
		p_dbg_statistic->rssi_ofdm_sum += p_phy_info->rx_mimo_signal_strength[0];
		p_dbg_statistic->rssi_ofdm_cnt++;
	} else if (rate_ss == 1) {

		/*evm*/
		p_dbg_statistic->evm_1ss_sum += p_phy_info->rx_mimo_evm_dbm[0];

		/*SNR*/
		p_dbg_statistic->snr_1ss_sum += p_phy_info->rx_snr[0];

		p_dbg_statistic->rssi_1ss_sum += p_phy_info->rx_mimo_signal_strength[0];
		p_dbg_statistic->rssi_1ss_cnt++;
	} else if (rate_ss == 2) {
		#if (defined(PHYDM_COMPILE_ABOVE_2SS))
		/*evm*/
		p_dbg_statistic->evm_2ss_sum[0] += p_phy_info->rx_mimo_evm_dbm[0];
		p_dbg_statistic->evm_2ss_sum[1] += p_phy_info->rx_mimo_evm_dbm[1];

		/*SNR*/
		p_dbg_statistic->snr_2ss_sum[0] += p_phy_info->rx_snr[0];
		p_dbg_statistic->snr_2ss_sum[1] += p_phy_info->rx_snr[1];

		/*RSSI*/
		p_dbg_statistic->rssi_2ss_sum[0] += p_phy_info->rx_mimo_signal_strength[0];
		p_dbg_statistic->rssi_2ss_sum[1] += p_phy_info->rx_mimo_signal_strength[1];
		p_dbg_statistic->rssi_2ss_cnt++;
		#endif
	} else if (rate_ss == 3) {
		#if (defined(PHYDM_COMPILE_ABOVE_3SS))
		/*evm*/
		p_dbg_statistic->evm_3ss_sum[0] += p_phy_info->rx_mimo_evm_dbm[0];
		p_dbg_statistic->evm_3ss_sum[1] += p_phy_info->rx_mimo_evm_dbm[1];
		p_dbg_statistic->evm_3ss_sum[2] += p_phy_info->rx_mimo_evm_dbm[2];

		/*SNR*/
		p_dbg_statistic->snr_3ss_sum[0] += p_phy_info->rx_snr[0];
		p_dbg_statistic->snr_3ss_sum[1] += p_phy_info->rx_snr[1];
		p_dbg_statistic->snr_3ss_sum[2] += p_phy_info->rx_snr[2];

		/*RSSI*/
		p_dbg_statistic->rssi_3ss_sum[0] += p_phy_info->rx_mimo_signal_strength[0];
		p_dbg_statistic->rssi_3ss_sum[1] += p_phy_info->rx_mimo_signal_strength[1];
		p_dbg_statistic->rssi_3ss_sum[2] += p_phy_info->rx_mimo_signal_strength[2];
		p_dbg_statistic->rssi_3ss_cnt++;
		#endif
	} else if (rate_ss == 4) {
		#if (defined(PHYDM_COMPILE_ABOVE_4SS))
		/*evm*/
		p_dbg_statistic->evm_4ss_sum[0] += p_phy_info->rx_mimo_evm_dbm[0];
		p_dbg_statistic->evm_4ss_sum[1] += p_phy_info->rx_mimo_evm_dbm[1];
		p_dbg_statistic->evm_4ss_sum[2] += p_phy_info->rx_mimo_evm_dbm[2];
		p_dbg_statistic->evm_4ss_sum[3] += p_phy_info->rx_mimo_evm_dbm[3];

		/*SNR*/
		p_dbg_statistic->snr_4ss_sum[0] += p_phy_info->rx_snr[0];
		p_dbg_statistic->snr_4ss_sum[1] += p_phy_info->rx_snr[1];
		p_dbg_statistic->snr_4ss_sum[2] += p_phy_info->rx_snr[2];
		p_dbg_statistic->snr_4ss_sum[3] += p_phy_info->rx_snr[3];

		/*RSSI*/
		p_dbg_statistic->rssi_4ss_sum[0] += p_phy_info->rx_mimo_signal_strength[0];
		p_dbg_statistic->rssi_4ss_sum[1] += p_phy_info->rx_mimo_signal_strength[1];
		p_dbg_statistic->rssi_4ss_sum[2] += p_phy_info->rx_mimo_signal_strength[2];
		p_dbg_statistic->rssi_4ss_sum[3] += p_phy_info->rx_mimo_signal_strength[3];
		p_dbg_statistic->rssi_4ss_cnt++;
		#endif
	}
}

u8 phydm_get_signal_quality(
	struct phydm_phyinfo_struct *p_phy_info,
	struct PHY_DM_STRUCT *p_dm,
	struct _phy_status_rpt_8192cd *p_phy_sta_rpt
	)
{
	u8 SQ_rpt;
	u8 result = 0;

	if (p_phy_info->rx_pwdb_all > 40 && !p_dm->is_in_hct_test)
		result = 100;
	else {
		SQ_rpt = p_phy_sta_rpt->cck_sig_qual_ofdm_pwdb_all;

		if (SQ_rpt > 64)
			result = 0;
		else if (SQ_rpt < 20)
			result = 100;
		else
			result = ((64 - SQ_rpt) * 100) / 44;

	}

	return result;
}

u8
phydm_query_rx_pwr_percentage(
	s8		ant_power
)
{
	if ((ant_power <= -100) || (ant_power >= 20))
		return	0;
	else if (ant_power >= 0)
		return	100;
	else
		return 100 + ant_power;
}


s32
phydm_signal_scale_mapping_92c_series(
	struct PHY_DM_STRUCT *p_dm,
	s32 curr_sig
)
{
	s32 ret_sig = 0;
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
	if (p_dm->support_interface  == ODM_ITRF_PCIE) {
		/* step 1. Scale mapping. */
		if (curr_sig >= 61 && curr_sig <= 100)
			ret_sig = 90 + ((curr_sig - 60) / 4);
		else if (curr_sig >= 41 && curr_sig <= 60)
			ret_sig = 78 + ((curr_sig - 40) / 2);
		else if (curr_sig >= 31 && curr_sig <= 40)
			ret_sig = 66 + (curr_sig - 30);
		else if (curr_sig >= 21 && curr_sig <= 30)
			ret_sig = 54 + (curr_sig - 20);
		else if (curr_sig >= 5 && curr_sig <= 20)
			ret_sig = 42 + (((curr_sig - 5) * 2) / 3);
		else if (curr_sig == 4)
			ret_sig = 36;
		else if (curr_sig == 3)
			ret_sig = 27;
		else if (curr_sig == 2)
			ret_sig = 18;
		else if (curr_sig == 1)
			ret_sig = 9;
		else
			ret_sig = curr_sig;
	}
#endif

#if ((DEV_BUS_TYPE == RT_USB_INTERFACE) || (DEV_BUS_TYPE == RT_SDIO_INTERFACE))
	if ((p_dm->support_interface  == ODM_ITRF_USB) || (p_dm->support_interface  == ODM_ITRF_SDIO)) {
		if (curr_sig >= 51 && curr_sig <= 100)
			ret_sig = 100;
		else if (curr_sig >= 41 && curr_sig <= 50)
			ret_sig = 80 + ((curr_sig - 40) * 2);
		else if (curr_sig >= 31 && curr_sig <= 40)
			ret_sig = 66 + (curr_sig - 30);
		else if (curr_sig >= 21 && curr_sig <= 30)
			ret_sig = 54 + (curr_sig - 20);
		else if (curr_sig >= 10 && curr_sig <= 20)
			ret_sig = 42 + (((curr_sig - 10) * 2) / 3);
		else if (curr_sig >= 5 && curr_sig <= 9)
			ret_sig = 22 + (((curr_sig - 5) * 3) / 2);
		else if (curr_sig >= 1 && curr_sig <= 4)
			ret_sig = 6 + (((curr_sig - 1) * 3) / 2);
		else
			ret_sig = curr_sig;
	}

#endif
	return ret_sig;
}

s32
phydm_signal_scale_mapping(
	struct PHY_DM_STRUCT *p_dm,
	s32 curr_sig
)
{
#ifdef CONFIG_SIGNAL_SCALE_MAPPING
		return phydm_signal_scale_mapping_92c_series(p_dm, curr_sig);
#else
		return curr_sig;
#endif

}

static u8 phydm_evm_db_to_percentage(s8 value)
{
	/*  */
	/* -33dB~0dB to 0%~99% */
	/*  */
	s8 ret_val;

	ret_val = value;
	ret_val /= 2;

	/*dbg_print("value=%d\n", value);*/
	/*ODM_RT_DISP(FRX, RX_PHY_SQ, ("EVMdbToPercentage92C value=%d / %x\n", ret_val, ret_val));*/
#ifdef ODM_EVM_ENHANCE_ANTDIV
	if (ret_val >= 0)
		ret_val = 0;

	if (ret_val <= -40)
		ret_val = -40;

	ret_val = 0 - ret_val;
	ret_val *= 3;
#else
	if (ret_val >= 0)
		ret_val = 0;

	if (ret_val <= -33)
		ret_val = -33;

	ret_val = 0 - ret_val;
	ret_val *= 3;

	if (ret_val == 99)
		ret_val = 100;
#endif

	return (u8)ret_val;
}

static u8
phydm_evm_dbm_jaguar_series(
	s8 value
)
{
	s8 ret_val = value;

	/* -33dB~0dB to 33dB ~ 0dB */
	if (ret_val == -128)
		ret_val = 127;
	else if (ret_val < 0)
		ret_val = 0 - ret_val;

	ret_val  = ret_val >> 1;
	return (u8)ret_val;
}

static s16
phydm_cfo(
	s8 value
)
{
	s16  ret_val;

	if (value < 0) {
		ret_val = 0 - value;
		ret_val = (ret_val << 1) + (ret_val >> 1) ;  /* *2.5~=312.5/2^7 */
		ret_val = ret_val | BIT(12);  /* set bit12 as 1 for negative cfo */
	} else {
		ret_val = value;
		ret_val = (ret_val << 1) + (ret_val >> 1) ; /* *2.5~=312.5/2^7 */
	}
	return ret_val;
}

s8
phydm_cck_rssi_convert(
	struct PHY_DM_STRUCT	*p_dm,
	u16		lna_idx,
	u8		vga_idx
)
{
	return (p_dm->cck_lna_gain_table[lna_idx] - (vga_idx << 1));
}

void
phydm_get_cck_rssi_table_from_reg(
	struct PHY_DM_STRUCT	*p_dm
)
{
	u8	used_lna_idx_tmp;
	u32	reg_0xa80 = 0x7431, reg_0xabc = 0xcbe5edfd; /*example: {-53, -43, -33, -27, -19, -13, -3, 1}*/ /*{0xCB, 0xD5, 0xDF, 0xE5, 0xED, 0xF3, 0xFD, 0x2}*/
	u8	i;

	PHYDM_DBG(p_dm, ODM_COMP_INIT, ("CCK LNA Gain table init\n"));

	if (!(p_dm->support_ic_type & (ODM_RTL8197F)))
		return;

	reg_0xa80 = odm_get_bb_reg(p_dm, 0xa80, 0xFFFF);
	reg_0xabc = odm_get_bb_reg(p_dm, 0xabc, MASKDWORD);

	PHYDM_DBG(p_dm, ODM_COMP_INIT, ("reg_0xa80 = 0x%x\n", reg_0xa80));
	PHYDM_DBG(p_dm, ODM_COMP_INIT, ("reg_0xabc = 0x%x\n", reg_0xabc));

	for (i = 0; i <= 3; i++) {
		used_lna_idx_tmp = (u8)((reg_0xa80 >> (4*i)) & 0x7);
		p_dm->cck_lna_gain_table[used_lna_idx_tmp] = (s8)((reg_0xabc >> (8*i)) & 0xff);
	}

	PHYDM_DBG(p_dm, ODM_COMP_INIT, ("cck_lna_gain_table = {%d,%d,%d,%d,%d,%d,%d,%d}\n",
		p_dm->cck_lna_gain_table[0],
		p_dm->cck_lna_gain_table[1],
		p_dm->cck_lna_gain_table[2],
		p_dm->cck_lna_gain_table[3],
		p_dm->cck_lna_gain_table[4],
		p_dm->cck_lna_gain_table[5],
		p_dm->cck_lna_gain_table[6],
		p_dm->cck_lna_gain_table[7]));

}

u8
phydm_rate_to_num_ss(
	struct PHY_DM_STRUCT		*p_dm,
	u8			data_rate
)
{
	u8	num_ss = 1;

	if (data_rate  <= ODM_RATE54M)
		num_ss = 1;
	else if (data_rate  <= ODM_RATEMCS31)
		num_ss = ((data_rate  - ODM_RATEMCS0) >> 3) + 1;
	else if (data_rate  <= ODM_RATEVHTSS1MCS9)
		num_ss = 1;
	else if (data_rate  <= ODM_RATEVHTSS2MCS9)
		num_ss = 2;
	else if (data_rate  <= ODM_RATEVHTSS3MCS9)
		num_ss = 3;
	else if (data_rate  <= ODM_RATEVHTSS4MCS9)
		num_ss = 4;

	return num_ss;
}

#if (ODM_IC_11N_SERIES_SUPPORT == 1)
void phydm_rx_phy_status92c_series_parsing(
	struct PHY_DM_STRUCT					*p_dm,
	struct phydm_phyinfo_struct			*p_phy_info,
	u8						*p_phy_status,
	struct phydm_perpkt_info_struct			*p_pktinfo
)
{
	u8				i, max_spatial_stream;
	s8				rx_pwr[4], rx_pwr_all = 0;
	u8				EVM, PWDB_ALL = 0, PWDB_ALL_BT;
	u8				RSSI, total_rssi = 0;
	boolean				is_cck_rate = false;
	u8				rf_rx_num = 0;
	u8				LNA_idx = 0;
	u8				VGA_idx = 0;
	u8				cck_agc_rpt;
	u8				num_ss;
	u8				stream_rxevm_tmp = 0;
	struct _phy_status_rpt_8192cd *p_phy_sta_rpt = (struct _phy_status_rpt_8192cd *)p_phy_status;

	is_cck_rate = (p_pktinfo->data_rate <= ODM_RATE11M) ? true : false;
	p_dm->rate_ss = phydm_rate_to_num_ss(p_dm, p_pktinfo->data_rate);

	if (p_pktinfo->is_to_self)
		p_dm->curr_station_id = p_pktinfo->station_id;

	p_phy_info->rx_mimo_signal_quality[RF_PATH_A] = -1;
	p_phy_info->rx_mimo_signal_quality[RF_PATH_B] = -1;


	if (is_cck_rate) {
		p_dm->phy_dbg_info.num_qry_phy_status_cck++;
		cck_agc_rpt = p_phy_sta_rpt->cck_agc_rpt_ofdm_cfosho_a;

		if (p_dm->support_ic_type & (ODM_RTL8703B)) {

		} else { /*3 bit LNA*/

			LNA_idx = ((cck_agc_rpt & 0xE0) >> 5);
			VGA_idx = (cck_agc_rpt & 0x1F);

#if (RTL8723B_SUPPORT == 1)
			if (p_dm->support_ic_type & (ODM_RTL8723B)) {

				rx_pwr_all = odm_CCKRSSI_8723B(LNA_idx, VGA_idx);
				/**/
			}
#endif
		}

		PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("ext_lna_gain (( %d )), LNA_idx: (( 0x%x )), VGA_idx: (( 0x%x )), rx_pwr_all: (( %d ))\n",
			p_dm->ext_lna_gain, LNA_idx, VGA_idx, rx_pwr_all));

		if (p_dm->board_type & ODM_BOARD_EXT_LNA)
			rx_pwr_all -= p_dm->ext_lna_gain;

		PWDB_ALL = phydm_query_rx_pwr_percentage(rx_pwr_all);

		if (p_pktinfo->is_to_self) {
			p_dm->cck_lna_idx = LNA_idx;
			p_dm->cck_vga_idx = VGA_idx;
		}
		p_phy_info->rx_pwdb_all = PWDB_ALL;

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
		p_phy_info->bt_rx_rssi_percentage = PWDB_ALL;
		p_phy_info->recv_signal_power = rx_pwr_all;
#endif

		/* (3) Get Signal Quality (EVM) */
		{
			u8	SQ;
				SQ = phydm_get_signal_quality(p_phy_info, p_dm, p_phy_sta_rpt);

			/* dbg_print("cck SQ = %d\n", SQ); */
			p_phy_info->signal_quality = SQ;
			p_phy_info->rx_mimo_signal_quality[RF_PATH_A] = SQ;
			p_phy_info->rx_mimo_signal_quality[RF_PATH_B] = -1;
		}

		for (i = RF_PATH_A; i < ODM_RF_PATH_MAX; i++) {
			if (i == 0)
				p_phy_info->rx_mimo_signal_strength[0] = PWDB_ALL;
			else
				p_phy_info->rx_mimo_signal_strength[1] = 0;
		}
	} else { /* 2 is OFDM rate */
		p_dm->phy_dbg_info.num_qry_phy_status_ofdm++;

		/* (1)Get RSSI for HT rate */

		for (i = RF_PATH_A; i < ODM_RF_PATH_MAX; i++) {
			/* 2008/01/30 MH we will judge RF RX path now. */
			if (p_dm->rf_path_rx_enable & BIT(i))
				rf_rx_num++;
			/* else */
			/* continue; */

			rx_pwr[i] = ((p_phy_sta_rpt->path_agc[i].gain & 0x3F) * 2) - 110;

			if (p_pktinfo->is_to_self) {
				p_dm->ofdm_agc_idx[i] = (p_phy_sta_rpt->path_agc[i].gain & 0x3F);
				/**/
			}

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
			p_phy_info->rx_pwr[i] = rx_pwr[i];
#endif

			/* Translate DBM to percentage. */
			RSSI = phydm_query_rx_pwr_percentage(rx_pwr[i]);
			total_rssi += RSSI;
			/* RT_DISP(FRX, RX_PHY_SS, ("RF-%d RXPWR=%x RSSI=%d\n", i, rx_pwr[i], RSSI)); */

			p_phy_info->rx_mimo_signal_strength[i] = (u8) RSSI;

			/* Get Rx snr value in DB */
			p_phy_info->rx_snr[i] = p_dm->phy_dbg_info.rx_snr_db[i] = (s8)(p_phy_sta_rpt->path_rxsnr[i] / 2);

			/* Record Signal Strength for next packet */
		}

		/* (2)PWDB, Average PWDB calculated by hardware (for rate adaptive) */

		rx_pwr_all = (((p_phy_sta_rpt->cck_sig_qual_ofdm_pwdb_all) >> 1) & 0x7f) - 110;

		PWDB_ALL_BT = PWDB_ALL = phydm_query_rx_pwr_percentage(rx_pwr_all);

		p_phy_info->rx_pwdb_all = PWDB_ALL;
		/* PHYDM_DBG(p_dm,DBG_RSSI_MNTR, ("ODM OFDM RSSI=%d\n",p_phy_info->rx_pwdb_all)); */
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
		p_phy_info->bt_rx_rssi_percentage = PWDB_ALL_BT;
		p_phy_info->rx_power = rx_pwr_all;
		p_phy_info->recv_signal_power = rx_pwr_all;
#endif
		{
			/* (3)EVM of HT rate */

			if (p_pktinfo->data_rate >= ODM_RATEMCS8 && p_pktinfo->data_rate <= ODM_RATEMCS15)
				max_spatial_stream = 2; /* both spatial stream make sense */
			else
				max_spatial_stream = 1; /* only spatial stream 1 makes sense */

			for (i = 0; i < max_spatial_stream; i++) {
				/* Do not use shift operation like "rx_evmX >>= 1" because the compilor of free build environment */
				/* fill most significant bit to "zero" when doing shifting operation which may change a negative */
				/* value to positive one, then the dbm value (which is supposed to be negative)  is not correct anymore. */
				EVM = phydm_evm_db_to_percentage((p_phy_sta_rpt->stream_rxevm[i]));	/* dbm */

				if (i == RF_PATH_A) /* Fill value in RFD, Get the first spatial stream only */
					p_phy_info->signal_quality = (u8)(EVM & 0xff);
				
				p_phy_info->rx_mimo_signal_quality[i] = (u8)(EVM & 0xff);

				if (p_phy_sta_rpt->stream_rxevm[i] < 0)
					stream_rxevm_tmp = (u8)(0 - (p_phy_sta_rpt->stream_rxevm[i]));

				if (stream_rxevm_tmp == 64)
					stream_rxevm_tmp = 0;

				p_phy_info->rx_mimo_evm_dbm[i] = stream_rxevm_tmp;

			}
		}

		num_ss = phydm_rate_to_num_ss(p_dm, p_pktinfo->data_rate);
		odm_parsing_cfo(p_dm, p_pktinfo, p_phy_sta_rpt->path_cfotail, num_ss);

	}
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	/* UI BSS List signal strength(in percentage), make it good looking, from 0~100. */
	/* It is assigned to the BSS List in GetValueFromBeaconOrProbeRsp(). */
	if (is_cck_rate) {
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		/* 2012/01/12 MH Use customeris signal strength from HalComRxdDesc.c/ */
		p_phy_info->signal_strength = SignalScaleProc(p_dm->adapter, PWDB_ALL, true, true);
#else
		p_phy_info->signal_strength = (u8)(phydm_signal_scale_mapping(p_dm, PWDB_ALL));/*PWDB_ALL;*/
#endif /*#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)*/
	} else {
		if (rf_rx_num != 0) {
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
			/* 2012/01/12 MH Use customeris signal strength from HalComRxdDesc.c/ */
			p_phy_info->signal_strength = SignalScaleProc(p_dm->adapter, (total_rssi /= rf_rx_num), true, false);
#else
			p_phy_info->signal_strength = (u8)(phydm_signal_scale_mapping(p_dm, total_rssi /= rf_rx_num));
#endif
		}
	}
#endif /*#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))*/

	/* dbg_print("is_cck_rate = %d, p_phy_info->rx_pwdb_all = %d, p_phy_sta_rpt->cck_agc_rpt_ofdm_cfosho_a = 0x%x\n", */
	/* is_cck_rate, p_phy_info->rx_pwdb_all, p_phy_sta_rpt->cck_agc_rpt_ofdm_cfosho_a); */

	/* For 92C/92D HW (Hybrid) Antenna Diversity */
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
	/* For 88E HW Antenna Diversity */
	p_dm->dm_fat_table.antsel_rx_keep_0 = p_phy_sta_rpt->ant_sel;
	p_dm->dm_fat_table.antsel_rx_keep_1 = p_phy_sta_rpt->ant_sel_b;
	p_dm->dm_fat_table.antsel_rx_keep_2 = p_phy_sta_rpt->antsel_rx_keep_2;
#endif

	if (p_pktinfo->is_packet_match_bssid) {
		phydm_avg_phystatus_index(p_dm, p_phy_info, p_pktinfo);
		phydm_rx_statistic_cal(p_dm, p_phy_status, p_pktinfo);
	}

}
#endif

void phydm_reset_rssi_for_dm(struct PHY_DM_STRUCT *p_dm, u8 station_id)
{
	struct cmn_sta_info		*p_sta;
	p_sta = p_dm->p_phydm_sta_info[station_id];

	if (!is_sta_active(p_sta)) {
		/**/
		return;
	}
	PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("Reset RSSI for macid = (( %d ))\n", station_id));

	p_sta->rssi_stat.rssi_cck = -1;
	p_sta->rssi_stat.rssi_ofdm = -1;
	p_sta->rssi_stat.rssi = -1;
	p_sta->rssi_stat.ofdm_pkt_cnt = 0;
	p_sta->rssi_stat.cck_pkt_cnt = 0;
	p_sta->rssi_stat.cck_sum_power = 0;
	p_sta->rssi_stat.is_send_rssi = RA_RSSI_STATE_INIT;
	p_sta->rssi_stat.packet_map = 0;
	p_sta->rssi_stat.valid_bit = 0;
}

void phydm_process_rssi_for_dm(
	struct PHY_DM_STRUCT					*p_dm,
	struct phydm_phyinfo_struct			*p_phy_info,
	struct phydm_perpkt_info_struct			*p_pktinfo
)
{
	s32			rssi_ave;
	s8			undecorated_smoothed_pwdb, undecorated_smoothed_cck, undecorated_smoothed_ofdm;
	u8			i, is_cck_rate = 0;
	u8			RSSI_max, RSSI_min;
	u32			weighting = 0;
	u8			send_rssi_2_fw = 0;
	struct cmn_sta_info		*p_sta;

	if (p_pktinfo->station_id >= ODM_ASSOCIATE_ENTRY_NUM)
		return;

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
	odm_s0s1_sw_ant_div_by_ctrl_frame_process_rssi(p_dm, p_phy_info, p_pktinfo);
#endif

	p_sta = p_dm->p_phydm_sta_info[p_pktinfo->station_id];

	if (!is_sta_active(p_sta)) {
		return;
		/**/
	}

	if ((!p_pktinfo->is_packet_match_bssid))/*data frame only*/
		return;

	if (p_pktinfo->is_packet_beacon)
		p_dm->phy_dbg_info.num_qry_beacon_pkt++;

	is_cck_rate = (p_pktinfo->data_rate <= ODM_RATE11M) ? true : false;
	p_dm->rx_rate = p_pktinfo->data_rate;

	/* --------------Statistic for antenna/path diversity------------------ */
	if (p_dm->support_ability & ODM_BB_ANT_DIV) {
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
		odm_process_rssi_for_ant_div(p_dm, p_phy_info, p_pktinfo);
#endif
	}
	/* -----------------Smart Antenna Debug Message------------------ */

	undecorated_smoothed_cck =  p_sta->rssi_stat.rssi_cck;
	undecorated_smoothed_ofdm = p_sta->rssi_stat.rssi_ofdm;
	undecorated_smoothed_pwdb = p_sta->rssi_stat.rssi;

	if (p_pktinfo->is_packet_to_self || p_pktinfo->is_packet_beacon) {

		if (!is_cck_rate) { /* ofdm rate */
			{
				if (p_phy_info->rx_mimo_signal_strength[RF_PATH_B] == 0) {
					rssi_ave = p_phy_info->rx_mimo_signal_strength[RF_PATH_A];
					p_dm->RSSI_A = p_phy_info->rx_mimo_signal_strength[RF_PATH_A];
					p_dm->RSSI_B = 0;
				} else {
					/*dbg_print("p_rfd->status.rx_mimo_signal_strength[0] = %d, p_rfd->status.rx_mimo_signal_strength[1] = %d\n",*/
					/*p_rfd->status.rx_mimo_signal_strength[0], p_rfd->status.rx_mimo_signal_strength[1]);*/
					p_dm->RSSI_A =  p_phy_info->rx_mimo_signal_strength[RF_PATH_A];
					p_dm->RSSI_B = p_phy_info->rx_mimo_signal_strength[RF_PATH_B];

					if (p_phy_info->rx_mimo_signal_strength[RF_PATH_A] > p_phy_info->rx_mimo_signal_strength[RF_PATH_B]) {
						RSSI_max = p_phy_info->rx_mimo_signal_strength[RF_PATH_A];
						RSSI_min = p_phy_info->rx_mimo_signal_strength[RF_PATH_B];
					} else {
						RSSI_max = p_phy_info->rx_mimo_signal_strength[RF_PATH_B];
						RSSI_min = p_phy_info->rx_mimo_signal_strength[RF_PATH_A];
					}
					if ((RSSI_max - RSSI_min) < 3)
						rssi_ave = RSSI_max;
					else if ((RSSI_max - RSSI_min) < 6)
						rssi_ave = RSSI_max - 1;
					else if ((RSSI_max - RSSI_min) < 10)
						rssi_ave = RSSI_max - 2;
					else
						rssi_ave = RSSI_max - 3;
				}
			}

			/* 1 Process OFDM RSSI */
			if (undecorated_smoothed_ofdm <= 0) {	/* initialize */
				undecorated_smoothed_ofdm = (s8)p_phy_info->rx_pwdb_all;
				PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("OFDM_INIT: (( %d ))\n", undecorated_smoothed_ofdm));
			} else {
				if (p_phy_info->rx_pwdb_all > (u32)undecorated_smoothed_ofdm) {
					undecorated_smoothed_ofdm =
						(s8)((((undecorated_smoothed_ofdm)*(RX_SMOOTH_FACTOR - 1)) +
						(rssi_ave)) / (RX_SMOOTH_FACTOR));
					undecorated_smoothed_ofdm = undecorated_smoothed_ofdm + 1;
					PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("OFDM_1: (( %d ))\n", undecorated_smoothed_ofdm));
				} else {
					undecorated_smoothed_ofdm =
						(s8)((((undecorated_smoothed_ofdm)*(RX_SMOOTH_FACTOR - 1)) +
						(rssi_ave)) / (RX_SMOOTH_FACTOR));
					PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("OFDM_2: (( %d ))\n", undecorated_smoothed_ofdm));
				}
			}
			if (p_sta->rssi_stat.ofdm_pkt_cnt != 64) {
				i = 63;
				p_sta->rssi_stat.ofdm_pkt_cnt -= (u8)(((p_sta->rssi_stat.packet_map >> i) & BIT(0)) - 1);
			}
			p_sta->rssi_stat.packet_map = (p_sta->rssi_stat.packet_map << 1) | BIT(0);

		} else {
			rssi_ave = p_phy_info->rx_pwdb_all;
			p_dm->RSSI_A = (u8) p_phy_info->rx_pwdb_all;
			p_dm->RSSI_B = 0xFF;
			p_dm->RSSI_C = 0xFF;
			p_dm->RSSI_D = 0xFF;

			if (p_sta->rssi_stat.cck_pkt_cnt <= 63)
				p_sta->rssi_stat.cck_pkt_cnt++;

			/* 1 Process CCK RSSI */
			if (undecorated_smoothed_cck <= 0) {	/* initialize */
				undecorated_smoothed_cck = (s8)p_phy_info->rx_pwdb_all;
				p_sta->rssi_stat.cck_sum_power = (u16)p_phy_info->rx_pwdb_all ; /*reset*/
				p_sta->rssi_stat.cck_pkt_cnt = 1; /*reset*/
				PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("CCK_INIT: (( %d ))\n", undecorated_smoothed_cck));
			} else if (p_sta->rssi_stat.cck_pkt_cnt <= CCK_RSSI_INIT_COUNT) {

				p_sta->rssi_stat.cck_sum_power = p_sta->rssi_stat.cck_sum_power + (u16)p_phy_info->rx_pwdb_all;
				undecorated_smoothed_cck = p_sta->rssi_stat.cck_sum_power / p_sta->rssi_stat.cck_pkt_cnt;

				PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("CCK_0: (( %d )), SumPow = (( %d )), cck_pkt = (( %d ))\n",
					undecorated_smoothed_cck, p_sta->rssi_stat.cck_sum_power, p_sta->rssi_stat.cck_pkt_cnt));
			} else {
				if (p_phy_info->rx_pwdb_all > (u32)undecorated_smoothed_cck) {
					undecorated_smoothed_cck =
						(s8)((((undecorated_smoothed_cck)*(RX_SMOOTH_FACTOR - 1)) +
						(p_phy_info->rx_pwdb_all)) / (RX_SMOOTH_FACTOR));
					undecorated_smoothed_cck = undecorated_smoothed_cck + 1;
					PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("CCK_1: (( %d ))\n", undecorated_smoothed_cck));
				} else {
					undecorated_smoothed_cck =
						(s8)((((undecorated_smoothed_cck)*(RX_SMOOTH_FACTOR - 1)) +
						(p_phy_info->rx_pwdb_all)) / (RX_SMOOTH_FACTOR));
					PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("CCK_2: (( %d ))\n", undecorated_smoothed_cck));
				}
			}
			i = 63;
			p_sta->rssi_stat.ofdm_pkt_cnt -= (u8)((p_sta->rssi_stat.packet_map >> i) & BIT(0));
			p_sta->rssi_stat.packet_map = p_sta->rssi_stat.packet_map << 1;
		}

		/* if(p_entry) */
		{
			/* 2011.07.28 LukeLee: modified to prevent unstable CCK RSSI */
			if (p_sta->rssi_stat.ofdm_pkt_cnt == 64) { /* speed up when all packets are OFDM*/
				undecorated_smoothed_pwdb = undecorated_smoothed_ofdm;
				PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("PWDB_0[%d] = (( %d ))\n", p_pktinfo->station_id, undecorated_smoothed_cck));
			} else {
				if (p_sta->rssi_stat.valid_bit < 64)
					p_sta->rssi_stat.valid_bit++;

				if (p_sta->rssi_stat.valid_bit == 64) {
					weighting = ((p_sta->rssi_stat.ofdm_pkt_cnt) > 4) ? 64 : (p_sta->rssi_stat.ofdm_pkt_cnt << 4);
					undecorated_smoothed_pwdb = (s8)((weighting * undecorated_smoothed_ofdm + (64 - weighting) * undecorated_smoothed_cck) >> 6);
					PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("PWDB_1[%d] = (( %d )), W = (( %d ))\n", p_pktinfo->station_id, undecorated_smoothed_cck, weighting));
				} else {
					if (p_sta->rssi_stat.valid_bit != 0)
						undecorated_smoothed_pwdb =
						(p_sta->rssi_stat.ofdm_pkt_cnt * undecorated_smoothed_ofdm + (p_sta->rssi_stat.valid_bit - p_sta->rssi_stat.ofdm_pkt_cnt) * undecorated_smoothed_cck) / p_sta->rssi_stat.valid_bit;
					else
						undecorated_smoothed_pwdb = 0;

					PHYDM_DBG(p_dm, DBG_RSSI_MNTR, ("PWDB_2[%d] = (( %d )), ofdm_pkt = (( %d )), Valid_Bit = (( %d ))\n",
						p_pktinfo->station_id, undecorated_smoothed_cck, p_sta->rssi_stat.ofdm_pkt_cnt, p_sta->rssi_stat.valid_bit));
				}
			}

			if ((p_sta->rssi_stat.ofdm_pkt_cnt >= 1 || p_sta->rssi_stat.cck_pkt_cnt >= 5) && (p_sta->rssi_stat.is_send_rssi == RA_RSSI_STATE_INIT)) {

				send_rssi_2_fw = 1;
				p_sta->rssi_stat.is_send_rssi = RA_RSSI_STATE_SEND;
			}

			p_sta->rssi_stat.rssi_cck = undecorated_smoothed_cck;
			p_sta->rssi_stat.rssi_ofdm = undecorated_smoothed_ofdm;
			p_sta->rssi_stat.rssi = undecorated_smoothed_pwdb;

			if (send_rssi_2_fw) { /* Trigger init rate by RSSI */

				if (p_sta->rssi_stat.ofdm_pkt_cnt != 0)
					p_sta->rssi_stat.rssi = undecorated_smoothed_ofdm;

				PHYDM_DBG(p_dm, DBG_RSSI_MNTR,
					("[Send to FW] PWDB = (( %d )), ofdm_pkt = (( %d )), cck_pkt = (( %d ))\n",
					undecorated_smoothed_pwdb, p_sta->rssi_stat.ofdm_pkt_cnt, p_sta->rssi_stat.cck_pkt_cnt));

			}
		}
	}
}

void odm_phy_status_query(
	struct PHY_DM_STRUCT		*p_dm,
	struct phydm_phyinfo_struct	*p_phy_info,
	u8							*p_phy_status,
	struct phydm_perpkt_info_struct		*p_pktinfo
)
{
	if (p_dm->support_ic_type & ODM_IC_PHY_STATUE_NEW_TYPE) {
		/* deadcode */
	} else if (p_dm->support_ic_type & ODM_IC_11AC_SERIES) {
		/* deadcode */
	} else if (p_dm->support_ic_type & ODM_IC_11N_SERIES) {
		#if	ODM_IC_11N_SERIES_SUPPORT
		phydm_rx_phy_status92c_series_parsing(p_dm, p_phy_info, p_phy_status, p_pktinfo);
		phydm_process_rssi_for_dm(p_dm, p_phy_info, p_pktinfo);
		#endif
	}
}

void phydm_rx_phy_status_init(void *p_dm_void)
{
#ifdef PHYDM_PHYSTAUS_SMP_MODE
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct pkt_process_info			*p_pkt_process = &(p_dm->pkt_proc_struct);

	if (p_dm->support_ic_type == ODM_RTL8822B) {

		p_pkt_process->phystatus_smp_mode_en = 1;
		p_pkt_process->pre_ppdu_cnt = 0xff;

		odm_set_mac_reg(p_dm, 0x60f, BIT(7), 1); /*phystatus sampling mode enable*/

		odm_set_bb_reg(p_dm, 0x9e4, 0x3ff, 0x0); /*First update timming*/
		odm_set_bb_reg(p_dm, 0x9e4, 0xfc00, 0x0); /*Update Sampling time*/
	}
#endif
}
