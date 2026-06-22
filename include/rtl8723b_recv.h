// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTL8723B_RECV_H__
#define __RTL8723B_RECV_H__

#define RECV_BLK_SZ 512
#define RECV_BLK_CNT 16
#define RECV_BLK_TH RECV_BLK_CNT

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
	#define MAX_RECVBUF_SZ  (RX_DMA_SIZE_8723B - RX_DMA_RESERVED_SIZE_8723B)
#endif

/* Rx smooth factor */
#define	Rx_Smooth_Factor (20)

#ifdef CONFIG_SDIO_HCI
	#ifndef CONFIG_SDIO_RX_COPY
		#undef MAX_RECVBUF_SZ
		#define MAX_RECVBUF_SZ	(RX_DMA_SIZE_8723B - RX_DMA_RESERVED_SIZE_8723B)
	#endif /* !CONFIG_SDIO_RX_COPY */
#endif /* CONFIG_SDIO_HCI */

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
	s32 rtl8723bs_init_recv_priv(PADAPTER padapter);
	void rtl8723bs_free_recv_priv(PADAPTER padapter);
#endif

void rtl8723b_query_rx_desc_status(union recv_frame *precvframe, u8 *pdesc);

#endif /* __RTL8723B_RECV_H__ */
