/**************************************************************************//**
 * @file     main.c
 * @version  V2.10
 * $Date: 15/12/17 5:19p $
 * @brief    Use PDMA channel 2 to demonstrate memory to memory transfer.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

#define TEST_CH     4                   /* NANO103 PDMA support 4 channels */

uint32_t PDMA_TEST_LENGTH = 64;         /* Test 64 words */
uint8_t SrcArray[256];                  /* Test source array */
uint8_t DestArray[256];                 /* Test destination array */
uint32_t volatile u32IsTestOver = 0;    /* Check which channel interrupt occurred */

/**
 * @brief       DMA IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The DMA default IRQ, declared in startup_nano103.s.
 */
void PDMA_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS();   /* Get interrupt status */

    if (status & 0x2) {         /* CH1 */
        if (PDMA_GET_CH_INT_STS(1) & 0x2)   /* Transfer down interrupt */
            u32IsTestOver = 1;              /* channel 1 interrupt occurred */
        PDMA_CLR_CH_INT_FLAG(1, PDMA_CH_INTSTSn_TDIF_Msk);  /* Clear transfer down interrupt */
    } else if (status & 0x4) {  /* CH2 */
        if (PDMA_GET_CH_INT_STS(2) & 0x2)   /* Transfer down interrupt */
            u32IsTestOver = 2;              /* channel 2 interrupt occurred */
        PDMA_CLR_CH_INT_FLAG(2, PDMA_CH_INTSTSn_TDIF_Msk);  /* Clear transfer down interrupt */
    } else if (status & 0x8) {  /* CH3 */
        if (PDMA_GET_CH_INT_STS(3) & 0x2)   /* Transfer down interrupt */
            u32IsTestOver = 3;              /* channel 3 interrupt occurred */
        PDMA_CLR_CH_INT_FLAG(3, PDMA_CH_INTSTSn_TDIF_Msk);  /* Clear transfer down interrupt */
    } else if (status & 0x10) { /* CH4 */
        if (PDMA_GET_CH_INT_STS(4) & 0x2)   /* Transfer down interrupt */
            u32IsTestOver = 4;              /* channel 4 interrupt occurred */
        PDMA_CLR_CH_INT_FLAG(4, PDMA_CH_INTSTSn_TDIF_Msk);  /* Clear transfer down interrupt */
    } else  /* unknown interrupt occurred */
        printf("unknown interrupt !!\n");
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL = (CLK->PWRCTL & ~CLK_PWRCTL_HXTEN_Msk) | (0x1 << CLK_PWRCTL_HXTEN_Pos); // HXT Enabled

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk)|CLK_CLKSEL0_HCLKSEL_HXT;

    /* Enable IP clock */
    CLK->AHBCLK |= CLK_AHBCLK_DMA_EN;   /* Enable DMA clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; /* Enable UART0 Clock */

    /* Select IP clock source */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk)|(0x0 << CLK_CLKSEL1_UART0SEL_Pos);  /* Clock source from external 12 MHz */

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);
}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main (void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.

    /* Init UART for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz <%d>\n", SystemCoreClock, TEST_CH);  /* show CPU clock */

    /* This sample is for PDMA function */
    /* Test memory to memory transfer */
    printf("+--------------------------------------+ \n");
    printf("|    Nano103 PDMA Driver Sample Code   | \n");
    printf("+--------------------------------------+ \n");

    PDMA_Open(1 << TEST_CH);    /* Enable Channel 4 */
    /* Configure Channel 4 */
    PDMA_SetTransferCnt(TEST_CH, PDMA_WIDTH_32, PDMA_TEST_LENGTH); /* Set data width is 32-bit, transfer length is 64 bytes */
    /*
        Set source address is from SrcArray, and address incremented
        Set destination address is to DestArray, and address incremented
    */
    PDMA_SetTransferAddr(TEST_CH, (uint32_t)SrcArray, PDMA_SAR_INC, (uint32_t)DestArray, PDMA_DAR_INC);
    PDMA_SetTransferMode(TEST_CH, PDMA_MEM, 0, 0);      /* memory to memory test */
    PDMA_SetTimeOut(TEST_CH, 0, 0x5555);                /* Disable timeout function, timeout count is 0x55555 */
    PDMA_EnableInt(TEST_CH, PDMA_CH_INTENn_TDIEN_Msk);  /* Enable transfer down interrupt */
    NVIC_EnableIRQ(PDMA_IRQn);                          /* Enable PDMA NVIC */
    u32IsTestOver = 0;
    PDMA_Trigger(TEST_CH);                              /* PDMA active */
    while(u32IsTestOver == 0);                          /* wait data transfer finish */

    if (u32IsTestOver == TEST_CH)                       /* Check test OK or not */
        printf("test done...\n");

    PDMA_Close();   /* Disable PDMA */
    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
