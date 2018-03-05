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
#include <common.h>
#include <command.h>

#define MV_SW_RST_CONTROL_REG_CORE0     0x0020800
#define MV_FABRIC_RST_CONTROL_REG       0x0020840
#define MV_FABRIC_CONTROL_REG           0x0020200
#define MV_FABRIC_CONFIG_REG            0x0020204
#define MV_CLOCK_GATING_CONTROL         0x0018220
#define MV_PWR_MANAGEMENT_PWR_DOWN_REG  0x001820C

#define MV_INTERRUPT_SOURCE_I_CONTROL_REG     0x0020B00
#define MV_CORE_TIMER_REG                     0x0021850
#define MV_IRQ_ACK_REG                        0x00218b4


/*Gbe0, Gbe1, Gbe2, Gbe3 Port RX Queue Configuration*/
unsigned int MV_GBEx_PORT_RXQ_CONFIG_REG[4] = {0x0071400, 0x0075400, 0x0031400, 0x0035400 };

/* #define MV_MEMIO32_WRITE(addr, data) printf("mw 0x%X 0x%X\n", (addr), (data)) */
#define MV_MEMIO32_WRITE(addr, data) ((*((volatile unsigned int*)(addr))) = ((unsigned int)(data)))
#define MV_MEMIO32_READ(addr)        ((*((volatile unsigned int*)(addr))))

int do_warmRst(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    int i, gbe, gatingReg, core;
	unsigned int base = simple_strtoul( argv[1], NULL, 16 );
    /*Set all 4 cores in reset*/
    for ( i = 0; i < 4; i++ ){
        MV_MEMIO32_WRITE((base | (MV_SW_RST_CONTROL_REG_CORE0 + 8*i)), (0xF01));
    }

    /*Set all 4 core Clock gating control */
    for ( i = 0; i < 4; i++ ){
        MV_MEMIO32_WRITE((base | (MV_SW_RST_CONTROL_REG_CORE0 + 4 + 8*i)), (0x01010101));
    }

    /*Fix only used GBE default value in RX queue control registers*/
    gatingReg = MV_MEMIO32_READ(MV_CLOCK_GATING_CONTROL);
    for( gbe = 0; gbe < 3; gbe++ ){
        /*If Gbe powered down(bits 1,2,3,4) - skip.*/
        /*Note: this skip may be false in some systems,
            so if warm reset hangs - try cancel it and fix only used GBEs*/
        if( !(gatingReg>>(gbe+1) & 0x1) ){
            continue;
        }
        for ( i=0; i < 8; i++){
            MV_MEMIO32_WRITE((base | ( MV_GBEx_PORT_RXQ_CONFIG_REG[gbe] + 4*i)), (0x40));
        }
    }
    /*Reset all units in Fabric*/
    MV_MEMIO32_WRITE((base | (MV_FABRIC_RST_CONTROL_REG)), (0x11F0101));

    /*Restore default Clock Gating values*/
    for ( i = 0; i < 5; i++ ){
        MV_MEMIO32_WRITE((base | (MV_PWR_MANAGEMENT_PWR_DOWN_REG + 4*i)), (0x0));
    }
    MV_MEMIO32_WRITE((base | (MV_CLOCK_GATING_CONTROL)), (0xDBFFA239));

    /*Reset all interrupt control registers*/
    for ( i=0; i < 115; i++ ){
        MV_MEMIO32_WRITE((base | (MV_INTERRUPT_SOURCE_I_CONTROL_REG + 4*i)), (0x0));
    }

    /*Reset Timers control registers. per core*/
    for( core = 0; core < 4; core++)
        for( i = 0; i < 8; i++)
            MV_MEMIO32_WRITE((base | ( MV_CORE_TIMER_REG + 0x100*core + 4*i)), (0x0));

    /*Reset per core IRQ ack register*/
    for( core = 0; core < 4; core++)
            MV_MEMIO32_WRITE((base | ( MV_IRQ_ACK_REG + 0x100*core)), (0x3FF));

    /*Set Fabric control and config to defaults*/
    MV_MEMIO32_WRITE((base | (MV_FABRIC_CONTROL_REG)), (0x2));
    MV_MEMIO32_WRITE((base | (MV_FABRIC_CONFIG_REG)), (0x3));

    /*Kick in Fabric units*/
    MV_MEMIO32_WRITE((base | (MV_FABRIC_RST_CONTROL_REG)), (0x0));

    /*Kick in Core0 to start boot process*/
    MV_MEMIO32_WRITE((base | (MV_SW_RST_CONTROL_REG_CORE0)), (0xF00));

    return 1;
}

U_BOOT_CMD(
		   warmRst,      2,     1,      do_warmRst,
	 "warmRst	- Warn Reset AXP card over PEX.\n",
  " PEX base address of AXP internal registers \n"
		  "\tWarm reset of AXP card connected via PEX, requeres previously configured PEX base address.\n"
		  );

