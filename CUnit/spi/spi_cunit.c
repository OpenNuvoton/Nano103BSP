/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   spi_cunit.c                                                                                           */
/*            The test function of SPI for CUnit.                                                          */
/* Project:   M051                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Nano103.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "spi_cunit.h"

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int SPI_Tests_Init(void)
{
    return 0;
}
int SPI_Tests_Clean(void)
{
    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

#ifdef MACRO_TEST
CU_SuiteInfo suites[] = {
    {"SPI MACRO", SPI_Tests_Init, SPI_Tests_Clean, NULL, NULL, SPI_MacroTests},
    //{"SPI API", SPI_Tests_Init, SPI_Tests_Clean, NULL, NULL, SPI_ApiTests},
    CU_SUITE_INFO_NULL
};
#endif

#ifdef API_TEST
CU_SuiteInfo suites[] = {
    //{"SPI MACRO", SPI_Tests_Init, SPI_Tests_Clean, NULL, NULL, SPI_MacroTests},
    {"SPI API", SPI_Tests_Init, SPI_Tests_Clean, NULL, NULL, SPI_ApiTests},
    CU_SUITE_INFO_NULL
};
#endif


void MACRO_SPI_ABORT_3WIRE_TRANSFER()
{
    SPI_ABORT_3WIRE_TRANSFER(SPI0);
    CU_ASSERT_TRUE((SPI0->SSCTL & SPI_SSCTL_SLVABORT_Msk) == 0);	//auto clear
}

void MACRO_SPI_CLR_3WIRE_START_INT_FLAG()
{
    /* Set PC.0 as GPIO */
    SYS->GPB_MFPL &= ~SYS_GPC_MFPL_PC0MFP_Msk;
	/* Set to output mode */
	PC->MODE = 0x1;
	
    PC0 = 0;
	
	SPI0->CLKDIV = 0;
    /* Slave mode, FIFO mode disabled, SPI mode 0. */
    SPI0->CTL = 0x00050004;
    /* Enable Slave 3-wire mode. */
    SPI0->SSCTL |= SPI_SSCTL_SLV3WIRE_Msk;
    SPI0->CTL |= 1;
    CU_ASSERT((SPI0->STATUS & 0x00000800)==0);
    __NOP();
    __NOP();
    PC0 = 1;
    __NOP();
    __NOP();
		
    CU_ASSERT((SPI0->STATUS & 0x00000800)==0x00000800);
    SPI_CLR_3WIRE_START_INT_FLAG(SPI0);
    CU_ASSERT((SPI0->STATUS & 0x00000800)==0);

	/* Set to input mode */
	PC->MODE = 0x0;
	    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_CLR_UNIT_TRANS_INT_FLAG()
{
    SPI0->CLKDIV = 4;
    SPI0->CTL = 5;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);
    CU_ASSERT((SPI0->STATUS & 0x00000080)==0);
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;

}

void MACRO_SPI_ENABLE_DISABLE_2BIT_MODE()
{
    /* Check TWOB bit */
    CU_ASSERT((SPI0->CTL & 0x00400000)==0);
    SPI_ENABLE_2BIT_MODE(SPI0);
    /* Check TWOB bit */
    CU_ASSERT((SPI0->CTL & 0x00400000)==0x00400000);
    SPI_DISABLE_2BIT_MODE(SPI0);
    /* Check TWOB bit */
    CU_ASSERT((SPI0->CTL & 0x00400000)==0);    
}

void MACRO_SPI_ENABLE_DISABLE_3WIRE_MODE()
{
    /* Check SLV3WIRE bit */
    CU_ASSERT((SPI0->SSCTL & SPI_SSCTL_SLV3WIRE_Msk)==0);
    SPI_ENABLE_3WIRE_MODE(SPI0);
    /* Check SLV3WIRE bit */
    CU_ASSERT((SPI0->SSCTL & SPI_SSCTL_SLV3WIRE_Msk)==SPI_SSCTL_SLV3WIRE_Msk);
    SPI_DISABLE_3WIRE_MODE(SPI0);
    /* Check SLV3WIRE bit */
    CU_ASSERT((SPI0->SSCTL & SPI_SSCTL_SLV3WIRE_Msk)==0);
}

void MACRO_SPI_DISABLE_DUAL_MODE()
{
    /* Set DUAL_IO_EN bit */
    SPI_ENABLE_DUAL_MODE(SPI0);
    /* Check DUAL_IO_EN bit */
    CU_ASSERT((SPI0->CTL & 0x20000000)==0x20000000);
    SPI_DISABLE_DUAL_MODE(SPI0);
    /* Check DUAL_IO_EN bit */
    CU_ASSERT((SPI0->CTL & 0x20000000)==0);
    
    /* Set DUAL_IO_EN bit */
    SPI_ENABLE_DUAL_MODE(SPI1);
    /* Check DUAL_IO_EN bit */
    CU_ASSERT((SPI1->CTL & 0x20000000)==0x20000000);
    SPI_DISABLE_DUAL_MODE(SPI1);
    /* Check DUAL_IO_EN bit */
    CU_ASSERT((SPI1->CTL & 0x20000000)==0);
}

void MACRO_SPI_ENABLE_DUAL_INPUT_MODE()
{
    SPI_ENABLE_DUAL_INPUT_MODE(SPI0);
    /* Check DUAL_IO_EN and DUAL_IO_DIR bit */
    CU_ASSERT((SPI0->CTL & 0x30000000)==0x20000000);
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
    
    SPI_ENABLE_DUAL_INPUT_MODE(SPI1);
    /* Check DUAL_IO_EN and DUAL_IO_DIR bit */
    CU_ASSERT((SPI1->CTL & 0x30000000)==0x20000000);
   /* Reset SPI1 */
    SYS->IPRST2 |= SYS_IPRST2_SPI1RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI1RST_Msk;
}

void MACRO_SPI_ENABLE_DUAL_OUTPUT_MODE()
{
    SPI_ENABLE_DUAL_OUTPUT_MODE(SPI0);
    /* Check DUAL_IO_EN and DUAL_IO_DIR bit */
    CU_ASSERT((SPI0->CTL & 0x30000000)==0x30000000);
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
    
    SPI_ENABLE_DUAL_OUTPUT_MODE(SPI1);
    /* Check DUAL_IO_EN and DUAL_IO_DIR bit */
    CU_ASSERT((SPI1->CTL & 0x30000000)==0x30000000);
    /* Reset SPI1 */
    SYS->IPRST2 |= SYS_IPRST2_SPI1RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI1RST_Msk;
}

void MACRO_SPI_TRIGGER_RX_PDMA()
{
    /* Check RX_DMA_GO bit */
    CU_ASSERT((SPI0->PDMACTL & 0x00000002)==0);
    SPI_TRIGGER_RX_PDMA(SPI0);
    /* Check RX_DMA_GO bit */
    CU_ASSERT((SPI0->PDMACTL & 0x00000002)==0x00000002);
     /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_TRIGGER_TX_PDMA()
{
    /* Check TX_DMA_GO bit */
    CU_ASSERT((SPI0->PDMACTL & 0x00000001)==0);
    SPI_TRIGGER_TX_PDMA(SPI0);
    /* Check TX_DMA_GO bit */
    CU_ASSERT((SPI0->PDMACTL & 0x00000001)==0x00000001);
     /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;    
}

void MACRO_SPI_GET_RX_FIFO_COUNT_EMPTY_FLAG()
{
    SPI0->CLKDIV = 4;
    /* Master mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00210004;
    /* Check RX_EMPTY flag */
    CU_ASSERT(SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0)==1);
    SPI0->TX0 = 1;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);
    /* Check RX_EMPTY flag */
    CU_ASSERT(SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0)==0);
    SPI0->TX0 = 2;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);
	SPI0->TX0 = 3;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);
	SPI0->TX0 = 4;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);
	SPI0->TX0 = 5;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);
	SPI0->TX0 = 6;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);
	SPI0->TX0 = 7;
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    SPI_CLR_UNIT_TRANS_INT_FLAG(SPI0);

    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_GET_TX_FIFO_EMPTY_FULL_FLAG()
{
    SPI0->CLKDIV = 0xFF;
    /* Slave mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00250004;
    /* Check TX_EMPTY flag */
    CU_ASSERT(SPI_GET_TX_FIFO_EMPTY_FLAG(SPI0)==1);
    /* Check TX_FULL flag */
    CU_ASSERT(SPI_GET_TX_FIFO_FULL_FLAG(SPI0)==0);
    SPI0->TX0 = 1;
    /* Check TX_EMPTY flag */
    CU_ASSERT(SPI_GET_TX_FIFO_EMPTY_FLAG(SPI0)==0);
    /* Check TX_FULL flag */
    CU_ASSERT(SPI_GET_TX_FIFO_FULL_FLAG(SPI0)==0);
    SPI0->TX0 = 2;
	SPI0->TX0 = 3;
	SPI0->TX0 = 4;
	SPI0->TX0 = 5;
	SPI0->TX0 = 6;
	SPI0->TX0 = 7;
	SPI0->TX0 = 8;
    /* Check TX_FULL flag */
    CU_ASSERT(SPI_GET_TX_FIFO_FULL_FLAG(SPI0)==1);

    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_READ_RX0()
{
    SPI0->CLKDIV = 4;
    /* Master mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00200004;
    /* Write 3 data to TX FIFO */
    SPI0->TX0 = 1;
    SPI0->TX0 = 2;
	SPI0->TX0 = 2;
	/* Trigger SPI */
    SPI_TRIGGER(SPI0);
    /* Wait data transfer */
    while((SPI0->CTL & 0x00000001)!=0x00000000) __NOP();
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPI0)==3);
    SPI_READ_RX0(SPI0);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPI0)==2);
    SPI_READ_RX0(SPI0);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPI0)==1);
    SPI_READ_RX0(SPI0);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPI0)==0);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_READ_RX1()
{
    uint32_t u32RegValue;
    
    SPI0->CLKDIV = 4;
    /* Master mode, 2-Bit Transfer mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00410004;
    /* Trigger SPI */
    SPI_TRIGGER(SPI0);
    /* Wait data transfer */
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    u32RegValue = SPI_READ_RX1(SPI0);
    CU_ASSERT(u32RegValue==SPI0->RX1);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_WRITE_TX0()
{
    SPI0->CLKDIV = 0xFF;
    /* Slave mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00250004;
    CU_ASSERT((SPI0->STATUS & 0x00f00000)==0);
    SPI_WRITE_TX0(SPI0, 1);
    CU_ASSERT((SPI0->STATUS & 0x00f00000)==0x00100000);
    SPI_WRITE_TX0(SPI0, 2);
    CU_ASSERT((SPI0->STATUS & 0x00F00000)==0x00200000);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_WRITE_TX1()
{
    SPI0->CLKDIV = 0xFF;
    /* Slave mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00240204;
    CU_ASSERT((SPI0->STATUS & 0x00f00000)==0);
	SPI_WRITE_TX1(SPI0, 1);
    CU_ASSERT((SPI0->STATUS & 0x00f00000)==0x00100000);
    SPI_WRITE_TX1(SPI0, 2);
    CU_ASSERT((SPI0->STATUS & 0x00f00000)==0x00200000);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_SET_SS_LEVEL()
{
    SPI_SET_SS_LEVEL(SPI0, 0, 0);
    CU_ASSERT(SPI0->SSCTL==0x24000003);
    
    SPI_SET_SS_LEVEL(SPI0, 0, 1);
    CU_ASSERT(SPI0->SSCTL==0x24000001);
    
    SPI_SET_SS_LEVEL(SPI0, 1, 0);
    CU_ASSERT(SPI0->SSCTL==0x24000002);
    
    SPI_SET_SS_LEVEL(SPI0, 1, 1);
    CU_ASSERT(SPI0->SSCTL==0x24000000);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_SET_SS0_HIGH()
{
    SPI_SET_SS0_HIGH(SPI0);
    CU_ASSERT(SPI0->SSCTL==0x24000000);
    
     /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_SET_SS1_HIGH()
{
    SPI_SET_SS1_HIGH(SPI0);
    CU_ASSERT(SPI0->SSCTL==0x24000000);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_SET_SS0_LOW()
{
    SPI_SET_SS0_LOW(SPI0);
    CU_ASSERT(SPI0->SSCTL==0x24000001);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_SET_SS1_LOW()
{
    SPI_SET_SS1_LOW(SPI0);
    CU_ASSERT(SPI0->SSCTL==0x24000002);
    
    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_ENABLE_DISABLE_BYTE_REORDER()
{
    SPI_ENABLE_BYTE_REORDER(SPI0);
    CU_ASSERT_TRUE(SPI0->CTL & 0x00080000);
    SPI_DISABLE_BYTE_REORDER(SPI0);
    CU_ASSERT_FALSE(SPI0->CTL & 0x00080000);
}

void MACRO_SPI_SET_SUSPEND_CYCLE()
{
    SPI_SET_SUSPEND_CYCLE(SPI0, 0);
    CU_ASSERT((SPI0->CTL & 0x0000F000)==0);
    SPI_SET_SUSPEND_CYCLE(SPI0, 8);
    CU_ASSERT((SPI0->CTL & 0x0000F000)==0x00008000);
    SPI_SET_SUSPEND_CYCLE(SPI0, 15);
    CU_ASSERT((SPI0->CTL & 0x0000F000)==0x0000F000);

    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_SET_LSB_MSB_FIRST()
{
    SPI_SET_LSB_FIRST(SPI0);
    CU_ASSERT_TRUE(SPI0->CTL & 0x00000400);
    SPI_SET_MSB_FIRST(SPI0);
    CU_ASSERT_FALSE(SPI0->CTL & 0x00000400);
}

void MACRO_SPI_SET_DATA_WIDTH()
{
    SPI_SET_DATA_WIDTH(SPI0, 32);
    CU_ASSERT((SPI0->CTL & 0x000000F8)==0);
    SPI_SET_DATA_WIDTH(SPI0, 8);
    CU_ASSERT((SPI0->CTL & 0x000000F8)==0x00000040);
    SPI_SET_DATA_WIDTH(SPI0, 16);
    CU_ASSERT((SPI0->CTL & 0x000000F8)==0x00000080);

    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_IS_BUSY()
{
    SPI0->CLKDIV = 0xFF;
    SPI0->CTL |= 1;
    CU_ASSERT_TRUE(SPI_IS_BUSY(SPI0));
    /* Wait data transfer */
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    CU_ASSERT_FALSE(SPI_IS_BUSY(SPI0));

    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void MACRO_SPI_TRIGGER()
{
    SPI0->CLKDIV = 0xFF;
    CU_ASSERT_FALSE(SPI0->CTL & 1);
    SPI_TRIGGER(SPI0);
    CU_ASSERT_TRUE(SPI0->CTL & 1);
    /* Wait data transfer */
    while( (SPI0->STATUS & 0x00000080)==0 ) __NOP();
    CU_ASSERT_FALSE(SPI0->CTL & 1);

    /* Reset SPI0 */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_Open()
{
    uint32_t u32ReturnValue;

    u32ReturnValue = SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 1000000);
    CU_ASSERT( SPI0->CTL == 0x00000044 );
    CU_ASSERT( SPI0->CLKDIV == 0x000000b );
    CU_ASSERT( SPI0->SSCTL == 0x24000000 );
    CU_ASSERT( u32ReturnValue == 1000000 );
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;

    u32ReturnValue = SPI_Open(SPI0, SPI_SLAVE, SPI_MODE_1, 16, 2000000);
    CU_ASSERT( SPI0->CTL == 0x00040082 );
    CU_ASSERT( SPI0->CLKDIV == 0x00000005 );
    CU_ASSERT( u32ReturnValue == 2000000 );
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;

    u32ReturnValue = SPI_Open(SPI0, SPI_MASTER, SPI_MODE_2, 24, 3000000);
    CU_ASSERT( SPI0->CTL == 0x000008C2 );
    CU_ASSERT( SPI0->CLKDIV == 0x00000003 );
    CU_ASSERT( SPI0->SSCTL == 0x24000000 );
    CU_ASSERT( u32ReturnValue == 3000000 );
    
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;

    u32ReturnValue = SPI_Open(SPI0, SPI_MASTER, SPI_MODE_3, 32, 12000000);
    CU_ASSERT( SPI0->CTL == 0x00000804 );
    CU_ASSERT( SPI0->CLKDIV == 0x00000000 );
    CU_ASSERT( SPI0->SSCTL == 0x24000000 );
    CU_ASSERT( u32ReturnValue == 12000000 );
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_Close()
{
    SPI0->CTL = 0x0000FFF4;
    SPI_Close(SPI0);
    CU_ASSERT(SPI0->CTL == 0x10003004);

    SPI0->CTL = 0x0000FFF4;
    SPI_Close(SPI0);
    CU_ASSERT(SPI0->CTL == 0x10003004);

    SPI0->CTL = 0x0000FFF4;
    SPI_Close(SPI0);
    CU_ASSERT(SPI0->CTL == 0x10003004);
}

void API_SPI_ClearRxFIFO()
{
    SPI0->CLKDIV = 4;
    /* Master mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00000004;
    /* Write 2 data to TX FIFO */
    SPI0->TX0 = 1;
    SPI0->CTL |= 0x00200000;	
    SPI0->TX0 = 2;
    SPI0->CTL |= 0x1;
    /* Wait data transfer */
    while((SPI0->CTL & 0x1)!=0x0) __NOP();
    CU_ASSERT(SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0)==0);
    SPI_ClearRxFIFO(SPI0);
    CU_ASSERT(SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0)==1);

    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_ClearTxFIFO()
{
    SPI0->CLKDIV = 0xFF;
    /* Slave mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00250004;
    /* Write 2 data to TX FIFO */
    SPI0->TX0 = 1;
    SPI0->TX0 = 2;
    SPI0->TX0 = 2;
    SPI0->TX0 = 2;
    SPI0->TX0 = 2;
    SPI0->TX0 = 2;
    SPI0->TX0 = 2;
    SPI0->TX0 = 2;
    CU_ASSERT(SPI_GET_TX_FIFO_FULL_FLAG(SPI0)==1);
	CU_ASSERT(SPI_GET_TX_FIFO_EMPTY_FLAG(SPI0)==0);
    SPI_ClearTxFIFO(SPI0);
    CU_ASSERT(SPI_GET_TX_FIFO_EMPTY_FLAG(SPI0)==1);

    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_EnableAutoSS_DisableAutoSS()
{
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS0_ACTIVE_HIGH);
    CU_ASSERT(SPI0->SSCTL==0x2400000D);
    SPI_DisableAutoSS(SPI0);
    CU_ASSERT((SPI0->SSCTL & SPI_SSCTL_AUTOSS_Msk)==0x0);
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS0_ACTIVE_LOW);
    CU_ASSERT(SPI0->SSCTL==0x24000009);
    SPI_DisableAutoSS(SPI0);
    CU_ASSERT((SPI0->SSCTL & SPI_SSCTL_AUTOSS_Msk)==0x0);
	
	SPI_EnableAutoSS(SPI0, SPI_SS1, SPI_SS1_ACTIVE_HIGH);
    CU_ASSERT(SPI0->SSCTL==0x2400000E);
    SPI_DisableAutoSS(SPI0);
    CU_ASSERT((SPI0->SSCTL & SPI_SSCTL_AUTOSS_Msk)==0x0);
    SPI_EnableAutoSS(SPI0, SPI_SS1, SPI_SS1_ACTIVE_LOW);
    CU_ASSERT(SPI0->SSCTL==0x2400000A);
    SPI_DisableAutoSS(SPI0);
    CU_ASSERT((SPI0->SSCTL & SPI_SSCTL_AUTOSS_Msk)==0x0);

    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_SetBusClock_GetBusClock()
{
    uint32_t u32ReturnValue;

    u32ReturnValue = SPI_SetBusClock(SPI0, 200*1000);
    CU_ASSERT( SPI0->CLKDIV == 0x0000003B );
    CU_ASSERT( u32ReturnValue == 200*1000 );
    u32ReturnValue = SPI_GetBusClock(SPI0);
    CU_ASSERT( u32ReturnValue == 200*1000 );
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;

    u32ReturnValue = SPI_SetBusClock(SPI0, 100*1000);
    CU_ASSERT( SPI0->CLKDIV == 0x00000077 );
    CU_ASSERT( SPI0->SSCTL == 0x24000000 );
    CU_ASSERT( u32ReturnValue == 100*1000 );
    u32ReturnValue = SPI_GetBusClock(SPI0);
    CU_ASSERT( u32ReturnValue == 100*1000 );
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;

    u32ReturnValue = SPI_SetBusClock(SPI0, 100*1000*1000);
    CU_ASSERT( SPI0->CLKDIV == 0x00000000 );
    CU_ASSERT( u32ReturnValue == 12*1000*1000 );
    u32ReturnValue = SPI_GetBusClock(SPI0);
    CU_ASSERT( u32ReturnValue == 12*1000*1000 );
    
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_EnableFIFO_DisableFIFO()
{
    SPI_EnableFIFO(SPI0, 4, 4);
    CU_ASSERT( SPI0->CTL & 0x00200000 );
    SPI_DisableFIFO(SPI0);
    CU_ASSERT_FALSE( SPI0->CTL & 0x00200000 );
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_EnableInt_DisableInt()
{
    SPI_EnableInt(SPI0, SPI_IE_MASK);
    CU_ASSERT_TRUE(SPI0->CTL & 0x00020000);
    SPI_DisableInt(SPI0, SPI_IE_MASK);
    CU_ASSERT_FALSE(SPI0->CTL & 0x00020000);

    SPI_EnableInt(SPI0, SPI_SSTAIEN_MASK);
    CU_ASSERT_TRUE(SPI0->SSCTL & 0x00000200);
    SPI_DisableInt(SPI0, SPI_SSTAIEN_MASK);
    CU_ASSERT_FALSE(SPI0->SSCTL & 0x00000200);
    
    SPI_EnableInt(SPI0, SPI_FIFO_TXTHIEN_MASK);
    CU_ASSERT_TRUE(SPI0->FIFOCTL & 0x00000008);
    SPI_DisableInt(SPI0, SPI_FIFO_TXTHIEN_MASK);
    CU_ASSERT_FALSE(SPI0->FIFOCTL & 0x00000008);
    
    SPI_EnableInt(SPI0, SPI_FIFO_RXTHIEN_MASK);
    CU_ASSERT_TRUE(SPI0->FIFOCTL & 0x00000004);
    SPI_DisableInt(SPI0, SPI_FIFO_RXTHIEN_MASK);
    CU_ASSERT_FALSE(SPI0->FIFOCTL & 0x00000004);
    
    SPI_EnableInt(SPI0, SPI_FIFO_RXOVIEN_MASK);
    CU_ASSERT_TRUE(SPI0->FIFOCTL & 0x00000010);
    SPI_DisableInt(SPI0, SPI_FIFO_RXOVIEN_MASK);
    CU_ASSERT_FALSE(SPI0->FIFOCTL & 0x00000010);
    
    SPI_EnableInt(SPI0, SPI_FIFO_TIMEOUIEN_MASK);
    CU_ASSERT_TRUE(SPI0->FIFOCTL & 0x00000080);
    SPI_DisableInt(SPI0, SPI_FIFO_TIMEOUIEN_MASK);
    CU_ASSERT_FALSE(SPI0->FIFOCTL & 0x00000080);

    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_GetStatus()
{
    /* SPI busy status test */
    SPI0->CLKDIV = 0xFF;
   
    /* Slave mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00250004;
       
    /* SPI TX empty flag test */
    /* Check TX empty flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x4) == 0x4);
    /* Write to TX FIFO */
    SPI0->TX0 = 1;
    /* Check TX empty flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x4) == 0x0);
    
    /* SPI TX full flag test */
    /* Check TX full flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x8) == 0x0);
    /* Write to TX FIFO */
    SPI0->TX0 = 2;
    SPI0->TX0 = 3;
    SPI0->TX0 = 4;
    SPI0->TX0 = 5;
    SPI0->TX0 = 6;
    SPI0->TX0 = 7;
    SPI0->TX0 = 8;
    /* Check TX full flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x8) == 0x8);
    /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
    
    /* SPI RX empty flag test */
    /* Master mode, FIFO mode enabled, SPI mode 0. */
    SPI0->CTL = 0x00210004;
    /* Check RX empty flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x1) == 0x1);
    /* Write to TX FIFO */
    SPI0->TX0 = 1;
    /* Wait data transfer */
//    while( (SPI0->CNTRL & (SPI_CNTRL_TX_EMPTY_Msk | SPI_CNTRL_GO_BUSY_Msk))!=SPI_CNTRL_TX_EMPTY_Msk ) __NOP();
	while( SPI0->CTL & SPI_CTL_GOBUSY_Msk);
	while( (SPI0->STATUS & SPI_STATUS_TXEMPTY_Msk)!=SPI_STATUS_TXEMPTY_Msk ) __NOP();
    /* Check RX empty flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x1) == 0x0);
    
    /* SPI RX full flag test */
    /* Check RX full flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x2) == 0x0);
    /* Write to TX FIFO */
    SPI0->TX0 = 2;
    SPI0->TX0 = 3;
    SPI0->TX0 = 4;
    SPI0->TX0 = 5;
    SPI0->TX0 = 6;
    SPI0->TX0 = 7;
    SPI0->TX0 = 8;
    /* Wait data transfer */
//    while( (SPI0->CNTRL & (SPI_CNTRL_TX_EMPTY_Msk | SPI_CNTRL_GO_BUSY_Msk))!=SPI_CNTRL_TX_EMPTY_Msk ) __NOP();
	while( SPI0->CTL & SPI_CTL_GOBUSY_Msk);
	while( (SPI0->STATUS & SPI_STATUS_TXEMPTY_Msk)!=SPI_STATUS_TXEMPTY_Msk ) __NOP();
    /* Check RX full flag */
    CU_ASSERT((SPI_GET_STATUS(SPI0) & 0x2) == 0x2);
     /* Reset SPI */
    SYS->IPRST2 |= SYS_IPRST2_SPI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_SPI0RST_Msk;
}

void API_SPI_WakeUp(void)
{
     /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable I2C wake-up */
    SPI_EnableWakeup(SPI0);
    CU_ASSERT((SPI0->CTL & SPI_CTL_WKCLKEN_Msk) == SPI_CTL_WKCLKEN_Msk);
	
	CLK->PWRCTL |= CLK_PWRCTL_WAKEINT_EN;
	NVIC_EnableIRQ(PDWU_IRQn);
	
    /* Processor use deep sleep */
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;

    /* System power down enable */
    CLK->PWRCTL |= (CLK_PWRCTL_PDEN_Msk | CLK_PWRCTL_PDWKDLY_Msk );
	
    printf("\n");
    printf("CHIP enter power down status.\n");

    /* Waiting for UART printf finish*/
    while(((UART0->FIFOSTS) & UART_FIFOSTS_TXEMPTY_Msk) == 0);

    /*  Use WFI instruction to idle the CPU. NOTE:
        If ICE is attached, system will wakeup immediately because ICE is a wakeup event. */
    __WFI();
    __NOP();
    __NOP();
    __NOP();
	
	printf("Wakeup!!\n");
	
    /* Disable power wake-up interrupt */
    CLK->PWRCTL &= ~CLK_PWRCTL_PDWKIEN_Msk;
    NVIC_EnableIRQ(PDWU_IRQn);
    
	SPI_DisableWakeup(SPI0);
    CU_ASSERT((SPI0->CTL & SPI_CTL_WKCLKEN_Msk) != SPI_CTL_WKCLKEN_Msk);
	
    /* Lock protected registers */
    SYS_LockReg();
}

CU_TestInfo SPI_MacroTests[] = {

    {"Abort SPI 3-wire transfer", MACRO_SPI_ABORT_3WIRE_TRANSFER},
//    {"Clear SPI Slave 3-wire start interrupt flag", MACRO_SPI_CLR_3WIRE_START_INT_FLAG},
    {"Clear SPI unit transfer interrupt flag", MACRO_SPI_CLR_UNIT_TRANS_INT_FLAG},
    {"Enable/Disable SPI 2-Bit transfer mode", MACRO_SPI_ENABLE_DISABLE_2BIT_MODE},
    {"Enable/Disable SPI Slave 3-Wire mode", MACRO_SPI_ENABLE_DISABLE_3WIRE_MODE},
    {"Trigger SPI RX DMA transfer", MACRO_SPI_TRIGGER_RX_PDMA},
    {"Trigger SPI TX DMA transfer", MACRO_SPI_TRIGGER_TX_PDMA},
	{"Get SPI RX FIFO count and empty flag", MACRO_SPI_GET_RX_FIFO_COUNT_EMPTY_FLAG},
    {"Get SPI TX FIFO empty and full flag", MACRO_SPI_GET_TX_FIFO_EMPTY_FULL_FLAG},
    {"Read SPI RX0", MACRO_SPI_READ_RX0},
    {"Read SPI RX1", MACRO_SPI_READ_RX1},
    {"Write SPI TX0", MACRO_SPI_WRITE_TX0},
    {"Write SPI TX1", MACRO_SPI_WRITE_TX1},
    {"Set the states of SPI SS0 pin and SS1 pin", MACRO_SPI_SET_SS_LEVEL},
    {"Set SPI SS0 pin to high state", MACRO_SPI_SET_SS0_HIGH},
    {"Set SPI SS1 pin to high state", MACRO_SPI_SET_SS1_HIGH},
    {"Set SPI SS0 pin to low state", MACRO_SPI_SET_SS0_LOW},
    {"Set SPI SS1 pin to low state", MACRO_SPI_SET_SS1_LOW},
    {"Enable/Disable SPI Byte Reorder function", MACRO_SPI_ENABLE_DISABLE_BYTE_REORDER},
    {"Set SPI suspend cycle", MACRO_SPI_SET_SUSPEND_CYCLE},
    {"Set SPI transfer sequence with LSB/MSB first", MACRO_SPI_SET_LSB_MSB_FIRST},
    {"Set SPI data width", MACRO_SPI_SET_DATA_WIDTH},
    {"Get SPI busy flag", MACRO_SPI_IS_BUSY},
    {"Trigger SPI data transfer", MACRO_SPI_TRIGGER},
  
    CU_TEST_INFO_NULL
};

CU_TestInfo SPI_ApiTests[] = {

//     {"SPI Open", API_SPI_Open},
//     {"SPI Close", API_SPI_Close},
//     {"Clear SPI RX FIFO", API_SPI_ClearRxFIFO},
//     {"Clear SPI TX FIFO", API_SPI_ClearTxFIFO},
  
    {"Enable/Disable SPI automatic slave selection function", API_SPI_EnableAutoSS_DisableAutoSS},
    {"Set/Get SPI bus clock rate", API_SPI_SetBusClock_GetBusClock},
    {"Enable/Disable SPI FIFO mode", API_SPI_EnableFIFO_DisableFIFO},
    {"Enable/Disable SPI interrupt function", API_SPI_EnableInt_DisableInt},
//   {"Get/Clear SPI interrupt flag", API_SPI_GetIntFlag_ClearIntFlag},
    {"Get SPI status", API_SPI_GetStatus},
//    {"SPI wakeup", API_SPI_WakeUp},
    CU_TEST_INFO_NULL
};
