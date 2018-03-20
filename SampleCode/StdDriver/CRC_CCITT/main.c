/**************************************************************************//**
 * @file     main.c
 * @version  V2.10
 * $Date: 15/12/17 5:18p $
 * @brief    Calculate the CRC-CCITT checksum value by CRC DMA mode.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "Nano103.h"

uint8_t volatile g_u8IsTargetAbortINTFlag = 0;  /* Global variable of Target abort interrupt */
uint8_t volatile g_u8IsBlockTransferDoneINTFlag = 0;    /* Global variable of Data transfer done interrupt */

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
    uint32_t status = CRC_GET_INT_FLAG();   /* Get interrupt status */

    /* check which interrupt occurred */
    if (status & DMA_CRC_DMAISTS_TDIF_Msk)      /* Is Data transfer done interrupt? */
    {

        CRC_CLR_INT_FLAG(DMA_CRC_DMAISTS_TDIF_Msk); /* Clear Block Transfer Done Interrupt Flag */

        g_u8IsBlockTransferDoneINTFlag++;
    }
    else if (status & DMA_CRC_DMAISTS_TABTIF_Msk)       /* Is Target abort interrupt? */
    {

        CRC_CLR_INT_FLAG(DMA_CRC_DMAISTS_TABTIF_Msk);   /* Clear Target Abort Interrupt Flag */

        g_u8IsTargetAbortINTFlag++;
    }
    else
    {
        printf("Un-expected interrupts. \n");   /* unexpected interrupt occurred */
    }
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
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

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
/*  Test DMA mode                                                                                          */
/*  CRC-CCITT Polynomial Mode Test                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void CRC_CCITTPolyModeTest(uint32_t u32SrcAddr, uint32_t u32TransByteCount)
{
    uint32_t u32TargetChecksum = 0x29B1, u32CalChecksum = 0;

    /*
        CRC-CCITT polynomial mode test
        Calculate string "123456789" CCITT checksum by DMA mode
        Seed value is 0xFFFF
        Checksum complement disable
        Checksum Reverse disable
        Write Data Complement disable
        Write Data Reverse disable
        Checksum should be 0x29B1
    */
    printf("# Calculate string \"123456789\" CRC-CCITT checksum value by CRC DMA mode. \n");
    printf("    - Seed value is 0xFFFF          \n");
    printf("    - Checksum Complement disable   \n");
    printf("    - Checksum Reverse disable      \n");
    printf("    - Write Data Complement disable \n");
    printf("    - Write Data Reverse disable    \n");
    printf("    - Checksum should be 0x%X       \n", u32TargetChecksum);
    printf("... \n\n");

    /* initial global variable */
    g_u8IsTargetAbortINTFlag = g_u8IsBlockTransferDoneINTFlag = 0;

    /* Enable CRC channel clock */
    /* Configure CRC Operation Settings for CRC DMA mode */
    CRC_Open(CRC_CCITT, 0, 0xFFFF, 0);

    /* Enable DMA Target Abort and Block Transfer Done Interrupt */
    CRC_ENABLE_INT(DMA_CRC_DMAINTEN_TABTIEN_Msk|DMA_CRC_DMAINTEN_TDIEN_Msk);

    /* Enable PDMA and CRC NVIC */
    NVIC_EnableIRQ(PDMA_IRQn);

    /* Trigger CRC DMA transfer */
    CRC_StartDMATransfer(u32SrcAddr, u32TransByteCount);

    /* Wait CRC Interrupt Flag occurred */
    while (1)
    {
        if (g_u8IsTargetAbortINTFlag == 1)      /* target abort occurred */
        {
            printf("DMA Target Abort Interrupt occurred. \n");
            break;
        }
        if (g_u8IsBlockTransferDoneINTFlag == 1)    /* data transfer done */
        {
            break;
        }
    }

    /* Disable PDMA and CRC NVIC */
    NVIC_DisableIRQ(PDMA_IRQn);

    /* Get CRC Checksum value */
    u32CalChecksum = CRC_GetChecksum();
    if (g_u8IsBlockTransferDoneINTFlag == 1)
    {
        printf("CRC checksum is 0x%X ... %s.\n\n", u32CalChecksum, (u32CalChecksum==u32TargetChecksum)?"PASS":"FAIL");
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Test CPU mode                                                                                          */
/*  CRC-8 Polynomial Mode Test                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void CRC_CRC8PolyModeTest(uint32_t u32SrcAddr, uint32_t u32TransByteCount)
{
    uint32_t i = 0;
    uint32_t u32TargetChecksum = 0x58, u32CalChecksum = 0;
    uint8_t *p8SrcAddr;

    /*
        CRC-8 polynomial mode test
        Calculate string "123456789" CRC8 checksum by CPU mode
        Seed value is 0x5A
        Data length is 8-bit
        Checksum complement disable
        Checksum Reverse disable
        Write Data Complement disable
        Write Data Reverse disable
        Checksum should be 0x58
    */
    printf("# Calculate string \"123456789\" CRC-8 checksum value by CRC CPU mode. \n");
    printf("    - Seed value is 0x5A            \n");
    printf("    - CPU Write Length is 8-bit     \n");
    printf("    - Checksum Complement disable   \n");
    printf("    - Checksum Reverse disable      \n");
    printf("    - Write Data Complement disable \n");
    printf("    - Write Data Reverse disable    \n");
    printf("    - Checksum should be 0x%X       \n", u32TargetChecksum);
    printf("... \n\n");

    p8SrcAddr = (uint8_t *)u32SrcAddr;

    /* Enable CRC channel clock */
    /* Configure CRC Operation Settings for CRC DMA mode */
    CRC_Open(CRC_8, 0, 0x5A, CRC_CPU_WDATA_8);

    /* In CPU mode, user need to write data into data port */
    for (i=0; i<u32TransByteCount; i++)
    {
        CRC_WRITE_DATA((p8SrcAddr[i]&0xFF));    /* write data into data port */
    }

    /* Get CRC Checksum value */
    u32CalChecksum = CRC_GetChecksum();
    printf("CRC checksum is 0x%X ... %s.\n\n", u32CalChecksum, (u32CalChecksum==u32TargetChecksum)?"PASS":"FAIL");

}


/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main (void)
{
    const uint8_t acCRCSrcPattern[] = "123456789";  /* Test pattern */

    /* Init System, IP clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.

    /* Init UART for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz \n", SystemCoreClock);   /* show CPU clock */

    /* This sample is for CRC function */
    /* Specified CCITT-DMA mode and CRC8-CPU mode */
    printf("+--------------------------------------+ \n");
    printf("|    Nano103 CRC Driver Sample Code    | \n");
    printf("+--------------------------------------+ \n");

    /* CRC-CCITT Polynomial mode with DMA mode */
    printf(" CRC-CCITT Polynomial mode test \n");
    CRC_CCITTPolyModeTest((uint32_t )acCRCSrcPattern, strlen((char *)acCRCSrcPattern));

    /* CRC-8 Polynomial mode with CPU mode */
    printf(" CRC-8 Polynomial mode test \n");
    CRC_CRC8PolyModeTest((uint32_t )acCRCSrcPattern, strlen((char *)acCRCSrcPattern));

    printf("\nExit CRC Sample Code. \n");
    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
