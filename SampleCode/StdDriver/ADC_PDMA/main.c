/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 10 $
 * $Date: 16/02/19 9:36a $
 * @brief    Use PDMA channel 1 to move ADC channel 0, 1, 2 converted data to SRAM.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

#define PDMA_CH 1  /* PDMA channel */
#define ADC_TEST_COUNT 32  /* PDMA transfer count */

uint32_t g_au32RxPDMADestination[ADC_TEST_COUNT]; /* For PDMA */
uint32_t au32AdcData[ADC_TEST_COUNT]; /* Data from ADC data register */

volatile uint32_t g_u32PdmaTDoneInt; /* PDMA transfer done flag */
volatile uint32_t g_u32PdmaTAbortInt; /* PDMA transfer abort flag */

void PDMA_IRQHandler(void) /* PDMA interrupt service routine */
{
    uint32_t status = PDMA_GET_INT_STATUS();  /* Get PDMA interrupt status */

    //Check PDMA interrupt status
    if (status & 0x2)   /* CH1 */
    {
        //Check if transfer done
        if (PDMA_GET_CH_INT_STS(1) & 0x2)
            g_u32PdmaTDoneInt = 1;
        //Clear transfer done flag
        PDMA_CLR_CH_INT_FLAG(1, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x4)     /* CH2 */
    {
        //Check if transfer done
        if (PDMA_GET_CH_INT_STS(2) & 0x2)
            g_u32PdmaTDoneInt = 2;
        //Clear transfer done flag
        PDMA_CLR_CH_INT_FLAG(2, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x8)     /* CH3 */
    {
        //Check if transfer done
        if (PDMA_GET_CH_INT_STS(3) & 0x2)
            g_u32PdmaTDoneInt = 3;
        //Clear transfer done flag
        PDMA_CLR_CH_INT_FLAG(3, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x10)     /* CH4 */
    {
        //Check if transfer done
        if (PDMA_GET_CH_INT_STS(4) & 0x2)
            g_u32PdmaTDoneInt = 4;
        //Clear transfer done flag
        PDMA_CLR_CH_INT_FLAG(4, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x20)     /* CH5 */
    {
        //Check if transfer done
        if (PDMA_GET_CH_INT_STS(5) & 0x2)
            g_u32PdmaTDoneInt = 5;
        //Clear transfer done flag
        PDMA_CLR_CH_INT_FLAG(5, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x40)     /* CH6 */
    {
        //Check if transfer done
        if (PDMA_GET_CH_INT_STS(6) & 0x2)
            g_u32PdmaTDoneInt = 6;
        //Clear transfer done flag
        PDMA_CLR_CH_INT_FLAG(6, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else
        printf("unknown interrupt !!\n");
}


/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);

    /*  Set HCLK frequency 32MHz */
    CLK_SetCoreClock(32000000);

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(PDMA_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HXT,CLK_UART0_CLK_DIVIDER(1));
    CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_ADC_CLK_DIVIDER(5));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    /* Set PA multi-function pins for ADC */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk | SYS_GPA_MFPL_PA2MFP_Msk);
    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA0MFP_ADC_CH0 | SYS_GPA_MFPL_PA1MFP_ADC_CH1 | SYS_GPA_MFPL_PA2MFP_ADC_CH2;

    /* Disable PA.0 PA.1 PA.2 PA.3 digital input path */
    PA->DINOFF = PA->DINOFF | (((1<<0) | (1<<1) | (1<<2) | (1<<3)) << GPIO_DINOFF_DINOFF0_Pos);

    /* Lock protected registers */
    SYS_LockReg();
}


void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_ResetModule(UART0_RST);  /* Reset UART0 */
    UART_Open(UART0, 115200);  /* Configure UART0 and set UART0 Baudrate */
}

void PDMA_INIT(void)  /* Initialize PDMA */
{
    uint32_t u32SrcAddr, u32DstAddr; /* Source and destination address for PDMA */

    /* Configure PDMA channel 1 to trigger ADC */
    PDMA_Open(1 << PDMA_CH);
    /* Set PDMA transfer count */
    PDMA_SetTransferCnt(PDMA_CH, PDMA_WIDTH_32, ADC_TEST_COUNT);
    /* PDMA source address is ADC register, PDMA */
    u32SrcAddr = (uint32_t)&ADC->PDMA;
    /* PDMA destination address is a buffer in SRAM */
    u32DstAddr = (uint32_t)g_au32RxPDMADestination;
    /* Set source/destination address */
    PDMA_SetTransferAddr(PDMA_CH, u32SrcAddr, PDMA_SAR_FIX, u32DstAddr, PDMA_DAR_INC);
    /* Set timeout */
    PDMA_SetTimeOut(PDMA_CH, 0, 0x5555);
    /* Enable PDMA transfer done interrupt */
    PDMA_EnableInt(PDMA_CH, PDMA_CH_INTENn_TDIEN_Msk);
    /* Enable NVIC PDMA interrupt */
    NVIC_EnableIRQ(PDMA_IRQn);

    /* Set PDMA Channel 1 for ADC, and start timeout counting */
    PDMA_SetTransferMode(PDMA_CH, PDMA_ADC, 0, 0);

    /* Trigger PDMA */
    PDMA_Trigger(PDMA_CH);
}

int32_t main (void)
{
    uint32_t u32DataCount;
    uint32_t u32ErrorCount;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    UART0_Init(); /* Init UART0 for printf */

    printf("+------------------------------------------------------------------------+\n");
    printf("|                 ADC PDMA Sample Code                                   |\n");
    printf("+------------------------------------------------------------------------+\n");

    /*
       This sample code demonstrates ADC PDMA function.
       Set ADC operation mode to single cycle scan mode, and enable channel 0,1,2,3.
       Enable ADC PDMA function, and trigger ADC conversion.
       Compare the log of ADC conversion data register with the content of PDMA target buffer.
       Finally, print the test result.
     */

    // Enable channel 0,1,2,3
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE_CYCLE, ADC_CH_0_MASK | ADC_CH_1_MASK | ADC_CH_2_MASK | ADC_CH_3_MASK);

    // Set reference voltage to AVDD
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);

    // Power on ADC
    ADC_POWER_ON(ADC);

    /* Enable ADC PDMA */
    ADC_ENABLE_PDMA(ADC);

    /* Configure PDMA channel 1 */
    PDMA_INIT();

    /* Enable PDMA IRQ */
    NVIC_EnableIRQ(PDMA_IRQn);

    /* Clear destination buffer */
    for(u32DataCount = 0; u32DataCount < ADC_TEST_COUNT; u32DataCount++)
        g_au32RxPDMADestination[u32DataCount] = 0;

    /* Reset u32DataCount and u32ErrorCount */
    u32DataCount = 0;
    u32ErrorCount = 0;

    /* Start conversion */
    ADC_START_CONV(ADC);

    /* Continuously trigger ADC until u32DataCount >= PDMA transfer count (ADC_TEST_COUNT) */
    while(1)
    {
        uint32_t u32Ch;
        /* Check ADC conversion finish flag */
        if(ADC_GET_INT_FLAG(ADC,ADC_ADF_INT) == 1)
        {
            /* Clear ADC conversion finish flag */
            ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

            /* Get conversion data from ADC data register */
            for (u32Ch = 0; u32Ch < 4; u32Ch++)
            {
                au32AdcData[u32DataCount++] = ADC_GET_CONVERSION_DATA(ADC, u32Ch);
                if(u32DataCount >= ADC_TEST_COUNT)
                    break;
            }
            /* Check if reach PDMA transfer count */
            if (u32DataCount < ADC_TEST_COUNT)
                ADC_START_CONV(ADC);
            else
                break;
        }
    }

    /* Wait for PDMA transfer done */
    while(g_u32PdmaTDoneInt == 0);

    /* Compare the log of ADC conversion data register with the content of PDMA target buffer */
    for(u32DataCount = 0; u32DataCount < ADC_TEST_COUNT; u32DataCount++)
    {
        if( au32AdcData[u32DataCount] != (g_au32RxPDMADestination[u32DataCount] & 0xFFF) )
        {
            printf("*** Count %d, conversion result: 0x%X, PDMA result: 0x%X.\n",
                   u32DataCount, au32AdcData[u32DataCount], g_au32RxPDMADestination[u32DataCount]);
            u32ErrorCount++;
        }
    }

    /* Print test result */
    if (u32ErrorCount == 0)
        printf("PASS!\n");
    else
        printf("FAIL!\n");

    while (1);

}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


