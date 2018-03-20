/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 9 $
 * $Date: 16/02/19 9:37a $
 * @brief    Configure Timer0 to ADC and move converted data to SRAM using PDMA.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

#define PDMA_CH 1  /* PDMA channel */
#define ADC_TEST_COUNT 32  /* PDMA transfer count */

uint32_t g_au32RxPDMADestination[ADC_TEST_COUNT];  /* Buffer for PDMA */
uint32_t au32AdcData[ADC_TEST_COUNT]; /* Data from ADC data register */

volatile uint32_t g_u32PdmaTDoneInt; /* PDMA transfer done interrupt flag */
volatile uint32_t g_u32PdmaTAbortInt; /* PDMA transfer abort interrupt flag */

void PDMA_IRQHandler(void) /* PDMA interrupt service routine */
{
    uint32_t status = PDMA_GET_INT_STATUS();  /* Get PDMA interrupt status */

    // Check PDMA interrupt status
    if (status & 0x2)   /* CH1 */
    {
        // Check if PDMA transfer done
        if (PDMA_GET_CH_INT_STS(1) & 0x2)
            g_u32PdmaTDoneInt = 1;
        // Clear PDMA transfer done interrupt flag
        PDMA_CLR_CH_INT_FLAG(1, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x4)     /* CH2 */
    {
        // Check if PDMA transfer done
        if (PDMA_GET_CH_INT_STS(2) & 0x2)
            g_u32PdmaTDoneInt = 2;
        // Clear PDMA transfer done interrupt flag
        PDMA_CLR_CH_INT_FLAG(2, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x8)     /* CH3 */
    {
        // Check if PDMA transfer done
        if (PDMA_GET_CH_INT_STS(3) & 0x2)
            g_u32PdmaTDoneInt = 3;
        // Clear PDMA transfer done interrupt flag
        PDMA_CLR_CH_INT_FLAG(3, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x10)     /* CH4 */
    {
        // Check if PDMA transfer done
        if (PDMA_GET_CH_INT_STS(4) & 0x2)
            g_u32PdmaTDoneInt = 4;
        // Clear PDMA transfer done interrupt flag
        PDMA_CLR_CH_INT_FLAG(4, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x20)     /* CH5 */
    {
        // Check if PDMA transfer done
        if (PDMA_GET_CH_INT_STS(5) & 0x2)
            g_u32PdmaTDoneInt = 5;
        // Clear PDMA transfer done interrupt flag
        PDMA_CLR_CH_INT_FLAG(5, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x40)     /* CH6 */
    {
        // Check if PDMA transfer done
        if (PDMA_GET_CH_INT_STS(6) & 0x2)
            g_u32PdmaTDoneInt = 6;
        // Clear PDMA transfer done interrupt flag
        PDMA_CLR_CH_INT_FLAG(6, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else
        printf("unknown interrupt !!\n");
}

void TMR0_IRQHandler(void)  // Timer0 interrupt service routine
{
    TIMER_ClearIntFlag(TIMER0);  // Clear timer0 interrupt
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
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(PDMA_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HXT,CLK_UART0_CLK_DIVIDER(1));
    CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_ADC_CLK_DIVIDER(5));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);

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
    uint32_t u32SrcAddr, u32DstAddr;
    uint32_t u32PDMACnt;

    /* Configure PDMA channel 1 to trigger ADC */
    PDMA_Open(1 << PDMA_CH);
    u32PDMACnt = 9; //Transmit 2 cycles => ((4*2)+1)
    PDMA_SetTransferCnt(PDMA_CH, PDMA_WIDTH_32, u32PDMACnt);
    u32SrcAddr = (uint32_t)&ADC->PDMA;
    u32DstAddr = (uint32_t)g_au32RxPDMADestination;
    /* Set PDMA source and destination address */
    PDMA_SetTransferAddr(PDMA_CH, u32SrcAddr, PDMA_SAR_FIX, u32DstAddr, PDMA_DAR_INC);
    PDMA_SetTimeOut(PDMA_CH, 0, 0x5555);
    /* Enable PDMA interrupt */
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
    uint32_t u32TimerSrc;
    uint32_t u32PDMACnt;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    UART0_Init(); /* Init UART0 for printf */

    printf("\nThis sample code demonstrate Timer trigger ADC function\n");

    // Enable channel 0,1,2,3
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_CONTINUOUS, ADC_CH_0_MASK|ADC_CH_1_MASK|ADC_CH_2_MASK|ADC_CH_3_MASK);

    // Set reference voltage to AVDD
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);

    u32TimerSrc = 0; // Timer0
    u32PDMACnt = 3; // Transfer (3+1) ADC result whenever timer event occurred
    ADC_EnableTimerTrigger(ADC, u32TimerSrc, u32PDMACnt); // Enable timer 0 trigger ADC

    /* Enable ADC PDMA */
    ADC_ENABLE_PDMA(ADC);

    /* Configure PDMA channel 1 */
    PDMA_INIT();

    // Give a dummy target frequency here. Will over write capture resolution with macro
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000000);

    // Update prescale to set proper resolution.
    // Timer 0 clock source is 12MHz, to set resolution to 1us, we need to
    // set clock divider to 12. e.g. set prescale to 12 - 1 = 11
    TIMER_SET_PRESCALE_VALUE(TIMER0, 11);

    // Set compare value
    TIMER_SET_CMP_VALUE(TIMER0, 0xFFFFF);

    // Set Timer 0 time out to trigger ADC
    TIMER_SetTriggerSource(TIMER0, TIMER_TIMEOUT_TRIGGER);
    TIMER_SetTriggerTarget(TIMER0, TIMER_CTL_TRGADC_Msk);

    // Enable timer0 interrupt
    TIMER_EnableInt(TIMER0);
    // Enable NVIC timer0 interrupt
    NVIC_EnableIRQ(TMR0_IRQn);

    // Start Timer 0
    TIMER_Start(TIMER0);

    // Wait for PDMA transfer down
    while(g_u32PdmaTDoneInt == 0);

    // Check PDMA data
    if((g_au32RxPDMADestination[8] & 0xFFF) != ADC_GET_CONVERSION_DATA(ADC, 0))
        printf("PDMA data error\n");
    else
        printf("PDMA data ok\n");

    while (1);

}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


