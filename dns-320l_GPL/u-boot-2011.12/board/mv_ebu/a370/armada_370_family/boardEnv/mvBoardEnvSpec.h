/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef __INCmvBoardEnvSpech
#define __INCmvBoardEnvSpech

#include "mvSysHwConfig.h"

/* For future use */
#define BD_ID_DATA_START_OFFS			0x0
#define BD_DETECT_SEQ_OFFS				0x0
#define BD_SYS_NUM_OFFS					0x4
#define BD_NAME_OFFS					0x8

#define MV_BOARD_DIMM0_I2C_ADDR			0x56
#define MV_BOARD_DIMM0_I2C_ADDR_TYPE 	ADDR7_BIT
#define MV_BOARD_DIMM_I2C_CHANNEL		0x0


/* Board specific configuration */
/* ============================ */

/* boards ID numbers */
#define BOARD_ID_BASE			0x0

/* New board ID numbers */
#define DB_88F6710_BP_ID		(BOARD_ID_BASE)
#define DB_88F6710_PCAC_ID		(DB_88F6710_BP_ID + 1)
#define RD_88F6710_ID                   (DB_88F6710_PCAC_ID + 1)
#define MV_MAX_BOARD_ID			(RD_88F6710_ID + 1)
#define INVALID_BAORD_ID			0xFFFFFFFF

/******************/
/* DB-88F6710-BP */
/******************/
#define DB_88F6710_MPP0_7		0x11111111
#define DB_88F6710_MPP8_15		0x11111111
#define DB_88F6710_MPP16_23		0x22222111
#define DB_88F6710_MPP24_31		0x02222222
#define DB_88F6710_MPP32_39		0x11111111
#define DB_88F6710_MPP40_47		0x11111111

#ifdef MV_INCLUDE_NOR
#define DB_88F6710_MPP48_55		0x41111111
#define DB_88F6710_MPP56_63		0x11111140
#else
#define DB_88F6710_MPP48_55		0x41111110
#define DB_88F6710_MPP56_63		0x11000140
#endif

#define DB_88F6710_MPP64_67		0x00000011

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
31	Giga_inN		IN
48	USB_Dev_Detect	IN
59	7seg bit0		OUT
60	7seg bit1		OUT
61	7seg bit2		OUT
*/
#define DB_88F6710_GPP_OUT_ENA_LOW		(~(0x0))
#define DB_88F6710_GPP_OUT_ENA_MID		(~(BIT27 | BIT28 | BIT29))
#define DB_88F6710_GPP_OUT_ENA_HIGH		(~(0x0))

#define DB_88F6710_GPP_OUT_VAL_LOW		0x0
#define DB_88F6710_GPP_OUT_VAL_MID		0x0
#define DB_88F6710_GPP_OUT_VAL_HIGH		0x0

#define DB_88F6710_GPP_POL_LOW			0x0
#define DB_88F6710_GPP_POL_MID			0x0
#define DB_88F6710_GPP_POL_HIGH			0x0

/*******************/
/* DB-88F6710-PCAC */
/*******************/

#define DB_88F6710_PCAC_MPP0_7			0x00001111
#define DB_88F6710_PCAC_MPP8_15			0x00000000
#define DB_88F6710_PCAC_MPP16_23		0x00000110
#define DB_88F6710_PCAC_MPP24_31		0x10000000
#define DB_88F6710_PCAC_MPP32_39		0x11111111
#define DB_88F6710_PCAC_MPP40_47		0x01111111
#define DB_88F6710_PCAC_MPP48_55		0x00000000
#define DB_88F6710_PCAC_MPP56_63		0x14040000
#define DB_88F6710_PCAC_MPP64_67		0x00000011

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
58	7seg bit0		OUT
59	7seg bit1		OUT
61	7seg bit2		OUT
*/
#define DB_88F6710_PCAC_GPP_OUT_ENA_LOW		(~(0x0))
#define DB_88F6710_PCAC_GPP_OUT_ENA_MID		(~(BIT26 | BIT27 | BIT29))
#define DB_88F6710_PCAC_GPP_OUT_ENA_HIGH	(~(0x0))

#define DB_88F6710_PCAC_GPP_OUT_VAL_LOW		0x0
#define DB_88F6710_PCAC_GPP_OUT_VAL_MID		0x0
#define DB_88F6710_PCAC_GPP_OUT_VAL_HIGH	0x0

#define DB_88F6710_PCAC_GPP_POL_LOW			0x0
#define DB_88F6710_PCAC_GPP_POL_MID			0x0
#define DB_88F6710_PCAC_GPP_POL_HIGH		0x0

/*******************/
/* RD_88F6710 */
/*******************/
#if 1 //ALPHA_CUSTOMIZE, for DNS-340L
#define RD_88F6710_MPP0_7         0x11101111 //MPP control REG0 0x18000, BLUE_ADD
#define RD_88F6710_MPP8_15        0x11111111 //MPP control REG1 0x18004, BLUE_ADD
#define RD_88F6710_MPP16_23       0x22222111 //MPP control REG2 0x18008, BLUE_ADD
#define RD_88F6710_MPP24_31       0x02222222 //MPP control REG3 0x1800c, BLUE_ADD
#define RD_88F6710_MPP32_39       0x11111110 //MPP control REG4 0x18010, BLUE_ADD
#define RD_88F6710_MPP40_47       0x01111111 //MPP control REG5 0x18014, BLUE_ADD
#define RD_88F6710_MPP48_55       0x00000000 //MPP control REG6 0x18018, BLUE_ADD
#define RD_88F6710_MPP56_63       0x00220000 //MPP control REG7 0x1801c, BLUE_ADD
#define RD_88F6710_MPP64_67       0x00000000 //MPP control REG8 0x18020, BLUE_ADD
#else
#define RD_88F6710_MPP0_7         0x00001111
#define RD_88F6710_MPP8_15        0x33333030
#define RD_88F6710_MPP16_23       0x22222110
#define RD_88F6710_MPP24_31       0x02222222
#define RD_88F6710_MPP32_39       0x11111110
#define RD_88F6710_MPP40_47       0x01111111
#define RD_88F6710_MPP48_55       0x33344444
#define RD_88F6710_MPP56_63       0x03555556
#define RD_88F6710_MPP64_67       0x00000000
#endif
/* GPPs ARMADA370
MPP#	NAME			IN/OUT
----------------------------------------------
5       PEX RST#                OUT (1)
6       GPP_PB                  IN
8       Fan power control       OUT (1)
10      SDIO Status             IN
16      SDIO WP                 IN
31      Switch Interrupt        IN
32      User LED                OUT(?)
47      USB Power On            OUT(0)
63      HDD Select              OUT(0)
64      Int HDD Power           OUT(1)
65      Ext HDD Power           OUT(0)

*/
#if 1  //ALPHA_CUSTOMIZE, for DNS-340L
#define RD_88F6710_GPP_OUT_ENA_LOW	0xFFFFFFFF 		//GPIO dataout enable REG0 0x18104
#define RD_88F6710_GPP_OUT_ENA_MID	0xF0007FFF 		//GPIO dataout enable REG0 0x18144
#define RD_88F6710_GPP_OUT_ENA_HIGH	0xFFFFFFFF 		//GPIO dataout enable REG0 0x18184 

#define RD_88F6710_GPP_OUT_VAL_LOW	0x00000010		//GPIO dataout REG0 0x18100
#define RD_88F6710_GPP_OUT_VAL_MID	0x00070000 		//GPIO dataout REG1 0x18140
#define RD_88F6710_GPP_OUT_VAL_HIGH	0x00000002		//GPIO dataout REG2 0x18180


#define RD_88F6710_GPP_POL_LOW		0x0
#define RD_88F6710_GPP_POL_MID		0x0
#define RD_88F6710_GPP_POL_HIGH		0x0
#else
#define RD_88F6710_GPP_OUT_ENA_LOW	(~(BIT5 | BIT8))
#define RD_88F6710_GPP_OUT_ENA_MID	(~(BIT0 | BIT15 | BIT31))
#define RD_88F6710_GPP_OUT_ENA_HIGH	(~(BIT0 | BIT1))

#define RD_88F6710_GPP_OUT_VAL_LOW	(BIT5 | BIT8)
#define RD_88F6710_GPP_OUT_VAL_MID	0x0
#define RD_88F6710_GPP_OUT_VAL_HIGH	0x0


#define RD_88F6710_GPP_POL_LOW		BIT31
#define RD_88F6710_GPP_POL_MID		0x0
#define RD_88F6710_GPP_POL_HIGH		0x0
#endif



#endif /* __INCmvBoardEnvSpech */
