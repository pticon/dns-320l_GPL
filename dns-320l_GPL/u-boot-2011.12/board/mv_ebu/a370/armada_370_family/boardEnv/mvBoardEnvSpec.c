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
#include "mvCommon.h"
#include "mvBoardEnvLib.h"
#include "mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"
#include "pex/mvPexRegs.h"

#define ARRSZ(x)	(sizeof(x)/sizeof(x[0]))

/***********************/
/* ARMADA-370 DB BOARD */
/***********************/

#define DB_88F6710_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F6710_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_U8	db88f6710InfoBoardDebugLedIf[] = {59, 60, 61};

MV_BOARD_TWSI_INFO	db88f6710InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_TWSI_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
#if 0  //ALPHA_CUSTOMIZE
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
#endif
};

MV_BOARD_MAC_INFO db88f6710InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0,0,0},
	{BOARD_MAC_SPEED_AUTO, 0x1,0,0},
};

MV_BOARD_SWITCH_INFO db88f6710InfoBoardSwitchValue[] = {
	{
		.switchIrq = (31 + 128),	/* set to -1 for timer operation. 128 is the base IRQ number for GPP interrupts */
		.switchPort = {0, 1, 2, 3, 4},
		.cpuPort = 6,
		.connectedPort = {-1, 6},
		.smiScanMode = 2,
		.quadPhyAddr = 0,
		.forceLinkMask = 0x0
	}
};

MV_BOARD_MODULE_TYPE_INFO db88f6710InfoBoardModTypeInfo[] = {
	{
#if 1 //ALPHA_CUSTOMIZE
		.boardMppGrp1Mod	= MV_BOARD_RGMII0 | MV_BOARD_RGMII1,
		.boardMppGrp2Mod	= NULL
#else
		.boardMppGrp1Mod	= MV_BOARD_AUTO,
		.boardMppGrp2Mod	= MV_BOARD_AUTO
#endif
	}
};

MV_BOARD_GPP_INFO db88f6710InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
#if  0 //ALPHA_CUSTOMIZE	
	{BOARD_GPP_USB_VBUS, 48} /* from MPP map */
#endif
};

MV_DEV_CS_INFO db88f6710InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO db88f6710InfoBoardMppConfigValue[] = {
	{ {
		DB_88F6710_MPP0_7,
		DB_88F6710_MPP8_15,
		DB_88F6710_MPP16_23,
		DB_88F6710_MPP24_31,
		DB_88F6710_MPP32_39,
		DB_88F6710_MPP40_47,
		DB_88F6710_MPP48_55,
		DB_88F6710_MPP56_63,
		DB_88F6710_MPP64_67,
	} }
};


MV_BOARD_TDM_INFO	db88f6710Tdm880[]	= { {0} }; /* SPI Cs */

MV_BOARD_TDM_SPI_INFO db88f6710TdmSpiInfo[] = { {1} }; /* SPI controller ID */

MV_BOARD_INFO db88f6710Info = {
#if 1 //ALPHA_CUSTOMIZE
	.boardName				= "DB-88F6710-BP_ALPHA_DNS340L",
#else
	.boardName				= "DB-88F6710-BP",
#endif	
	.enableModuleScan 			= MV_TRUE,
	.numBoardMppTypeValue		= ARRSZ(db88f6710InfoBoardModTypeInfo),
	.pBoardModTypeValue			= db88f6710InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db88f6710InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db88f6710InfoBoardMppConfigValue,
#if 1 //ALPHA_CUSTOMIZE
	.intsGppMaskLow				= NULL,	/* for Switch link interrupt */
#else
	.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
#endif	
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(db88f6710InfoBoardDeCsInfo),
	.pDevCsInfo					= db88f6710InfoBoardDeCsInfo,
	.numBoardTwsiDev			= ARRSZ(db88f6710InfoBoardTwsiDev),
	.pBoardTwsiDev				= db88f6710InfoBoardTwsiDev,
	.numBoardMacInfo			= ARRSZ(db88f6710InfoBoardMacInfo),
	.pBoardMacInfo				= db88f6710InfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(db88f6710InfoBoardGppInfo),
	.pBoardGppInfo				= db88f6710InfoBoardGppInfo,
	.activeLedsNumber			= ARRSZ(db88f6710InfoBoardDebugLedIf),
	.pLedGppPin					= db88f6710InfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 16000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6710_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6710_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6710_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6710_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6710_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6710_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6710_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6710_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6710_GPP_POL_HIGH,

	/* External Switch Configuration */
#if 1 //ALPHA_CUSTOMIZE
	.pSwitchInfo = NULL,
	.switchInfoNum = NULL,
#else
	.pSwitchInfo = db88f6710InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(db88f6710InfoBoardSwitchValue),
#endif

	/* TDM configuration */
#if  1 //ALPHA_CUSTOMIZE
	.numBoardTdmInfo		= NULL,
	.pBoardTdmInt2CsInfo	= NULL,
	.boardTdmInfoIndex		= NULL,
	.pBoardTdmSpiInfo		= NULL,
#else
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {db88f6710Tdm880},
	.boardTdmInfoIndex		= 0,
	.pBoardTdmSpiInfo 		= db88f6710TdmSpiInfo,
#endif

	/* NOR init params */
	.norFlashReadParams		= DB_88F6710_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= DB_88F6710_BOARD_NOR_WRITE_PARAMS
};

/*************************/
/* ARMADA-370 PCAC BOARD */
/*************************/

MV_U8	db88f6710pcacInfoBoardDebugLedIf[] = {58, 59, 61};

MV_BOARD_MAC_INFO db88f6710pcacInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0, 0, 0},
	{BOARD_MAC_SPEED_AUTO, 0x1, 0, 0},
};

MV_BOARD_MODULE_TYPE_INFO db88f6710pcacInfoBoardModTypeInfo[] = {
	{
		.boardMppGrp1Mod	= MV_BOARD_AUTO,
		.boardMppGrp2Mod	= MV_BOARD_AUTO
	}
};

MV_BOARD_GPP_INFO db88f6710pcacInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS, 24} /* from MPP map */
};

MV_DEV_CS_INFO db88f6710pcacInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO db88f6710pcacInfoBoardMppConfigValue[] = {
	{ {
		DB_88F6710_PCAC_MPP0_7,
		DB_88F6710_PCAC_MPP8_15,
		DB_88F6710_PCAC_MPP16_23,
		DB_88F6710_PCAC_MPP24_31,
		DB_88F6710_PCAC_MPP32_39,
		DB_88F6710_PCAC_MPP40_47,
		DB_88F6710_PCAC_MPP48_55,
		DB_88F6710_PCAC_MPP56_63,
		DB_88F6710_PCAC_MPP64_67,
	} }
};


MV_BOARD_INFO db88f6710pcacInfo = {
	.boardName					= "DB-88F6710-PCAC",
	.enableModuleScan 			= MV_FALSE,
	.numBoardMppTypeValue		= ARRSZ(db88f6710pcacInfoBoardModTypeInfo),
	.pBoardModTypeValue			= db88f6710pcacInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db88f6710pcacInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db88f6710pcacInfoBoardMppConfigValue,
	.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(db88f6710pcacInfoBoardDeCsInfo),
	.pDevCsInfo					= db88f6710pcacInfoBoardDeCsInfo,
	.numBoardTwsiDev			= 0,
	.pBoardTwsiDev				= NULL,
	.numBoardMacInfo			= ARRSZ(db88f6710pcacInfoBoardMacInfo),
	.pBoardMacInfo				= db88f6710pcacInfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(db88f6710pcacInfoBoardGppInfo),
	.pBoardGppInfo				= db88f6710pcacInfoBoardGppInfo,
	.activeLedsNumber			= ARRSZ(db88f6710pcacInfoBoardDebugLedIf),
	.pLedGppPin					= db88f6710pcacInfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6710_PCAC_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6710_PCAC_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6710_PCAC_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6710_PCAC_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6710_PCAC_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6710_PCAC_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6710_PCAC_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6710_PCAC_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6710_PCAC_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = NULL,
	.switchInfoNum = 0,

	.numBoardTdmInfo	= { 0 },
	.pBoardTdmInt2CsInfo	= { NULL },
	.boardTdmInfoIndex	= -1,

	/* NOR init params */
	.norFlashReadParams	= 0,
	.norFlashWriteParams	= 0,
};

/*************************/
/* ARMADA-370 RD BOARD */
/*************************/
#define RD_88F6710_BOARD_NOR_READ_PARAMS	0x403E07CF
#define RD_88F6710_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_U8	rd88F6710InfoBoardDebugLedIf[] = {32};

MV_BOARD_TWSI_INFO	rd88F6710InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_TWSI_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
#if 1  //ALPHA_CUSTOMIZE
	//{BOARD_DEV_TWSI_SATR, 0x50, ADDR7_BIT},
#else
	{BOARD_DEV_TWSI_SATR, 0x50, ADDR7_BIT},
#endif
};

MV_BOARD_MAC_INFO rd88F6710InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0,  0, 0},
#if 1 //ALPHA_CUSTOMIZE
	{BOARD_MAC_SPEED_AUTO, 0x1,  0, 0},
#else
	{BOARD_MAC_SPEED_1000M, 0x10, 0, 0},
#endif
};

MV_BOARD_SWITCH_INFO rd88F6710InfoBoardSwitchValue[] = {
	{
		.switchIrq = (31 + 128),	/* set to -1 for timer operation. 128 is the base IRQ number for GPP interrupts */
		.switchPort = {0, 1, 2, 3, -1},
		.cpuPort = 5,
		.connectedPort = {-1, 5},
		.smiScanMode = 2,
		.quadPhyAddr = 0,
		.forceLinkMask = 0x0
	}
};

MV_BOARD_MODULE_TYPE_INFO rd88F6710InfoBoardModTypeInfo[] = {
	{
#if 1 //ALPHA_CUSTOMIZE
		.boardMppGrp1Mod	= MV_BOARD_RGMII0 | MV_BOARD_RGMII1,
		.boardMppGrp2Mod	= NULL
#else
		.boardMppGrp1Mod	= MV_BOARD_SDIO | MV_BOARD_RGMII1,
		.boardMppGrp2Mod	= MV_BOARD_TDM
#endif
	}
};

MV_BOARD_GPP_INFO rd88F6710InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS, 24} /* from MPP map */
};

MV_DEV_CS_INFO rd88F6710InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO rd88F6710InfoBoardMppConfigValue[] = {
	{ {
		RD_88F6710_MPP0_7,
		RD_88F6710_MPP8_15,
		RD_88F6710_MPP16_23,
		RD_88F6710_MPP24_31,
		RD_88F6710_MPP32_39,
		RD_88F6710_MPP40_47,
		RD_88F6710_MPP48_55,
		RD_88F6710_MPP56_63,
		RD_88F6710_MPP64_67,
	} }
};

MV_BOARD_TDM_INFO	rd88F6710Tdm880[]	= { {1}, {2} };
MV_BOARD_TDM_INFO	rd88F6710Tdm792[]	= { {1}, {2}, {3}, {4}, {6}, {7} };
MV_BOARD_TDM_INFO	rd88F6710Tdm3215[]	= { {1} };

MV_BOARD_INFO rd88F6710Info = {
	.boardName				= "RD-88F6710_ALpha_DNS340L",
	.enableModuleScan 			= MV_FALSE,
	.numBoardMppTypeValue			= ARRSZ(rd88F6710InfoBoardModTypeInfo),
	.pBoardModTypeValue			= rd88F6710InfoBoardModTypeInfo,
	.numBoardMppConfigValue			= ARRSZ(rd88F6710InfoBoardMppConfigValue),
	.pBoardMppConfigValue			= rd88F6710InfoBoardMppConfigValue,
#if 1 //ALPHA_CUSTOMIZE
	.intsGppMaskLow				= 0,
#else
	.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
#endif
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(rd88F6710InfoBoardDeCsInfo),
	.pDevCsInfo				= rd88F6710InfoBoardDeCsInfo,
	.numBoardTwsiDev			= ARRSZ(rd88F6710InfoBoardTwsiDev),
	.pBoardTwsiDev				= rd88F6710InfoBoardTwsiDev,
	.numBoardMacInfo			= ARRSZ(rd88F6710InfoBoardMacInfo),
	.pBoardMacInfo				= rd88F6710InfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(rd88F6710InfoBoardGppInfo),
	.pBoardGppInfo				= rd88F6710InfoBoardGppInfo,
	.activeLedsNumber			= ARRSZ(rd88F6710InfoBoardDebugLedIf),
	.pLedGppPin				= rd88F6710InfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 80000,

	/* GPP values */
	.gppOutEnValLow			= RD_88F6710_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_88F6710_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= RD_88F6710_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= RD_88F6710_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_88F6710_GPP_OUT_VAL_MID,
	.gppOutValHigh			= RD_88F6710_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= RD_88F6710_GPP_POL_LOW,
	.gppPolarityValMid		= RD_88F6710_GPP_POL_MID,
	.gppPolarityValHigh		= RD_88F6710_GPP_POL_HIGH,

	/* External Switch Configuration */
#if 1 //ALPHA_CUSTOMIZE
	.pSwitchInfo = NULL,
	.switchInfoNum = NULL,
#else
	.pSwitchInfo = rd88F6710InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(rd88F6710InfoBoardSwitchValue),
#endif

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
#if 1  //ALPHA_CUSTOMIZE
	.numBoardTdmInfo		= NULL,
	.pBoardTdmInt2CsInfo		= NULL,
	.boardTdmInfoIndex		= NULL,
#else
	.numBoardTdmInfo		= {2, 6, 1},
	.pBoardTdmInt2CsInfo		= {rd88F6710Tdm880, rd88F6710Tdm792, rd88F6710Tdm3215},
	.boardTdmInfoIndex		= -1,
#endif
	/* NOR init params */
	.norFlashReadParams		= RD_88F6710_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= RD_88F6710_BOARD_NOR_WRITE_PARAMS
};

MV_BOARD_INFO *boardInfoTbl[] = {
	&db88f6710Info,
	&db88f6710pcacInfo,
	&rd88F6710Info
};
