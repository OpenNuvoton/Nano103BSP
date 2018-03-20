/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 16/02/19 9:37a $
 * @brief    Convert ADC channel 12(Band-gap) in Single mode and print conversion results.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

volatile uint8_t u8ADF;  // ADC conversion finish flag

void ADC_IRQHandler(void);  //ADC interrupt service routine

void ADC_IRQHandler(void)  //ADC interrupt service routine
{
    uint32_t u32Flag;  //ADC interrupt status flag

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    // Check ADC conversion finish interrupt flag
    if(u32Flag & ADC_ADF_INT)
        u8ADF = 1;

    // Clear ADC conversion finish interrupt flag
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set HCLK source form HXT and HCLK source divide 1  */
    //CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);

    /*  Set HCLK frequency 32MHz */
    //CLK_SetCoreClock(32000000);

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(ADC_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));
    CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_ADC_CLK_DIVIDER(5));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    /* Set PA.0 multi-function pin for ADC channel 0 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA0MFP_Msk) | SYS_GPA_MFPL_PA0MFP_ADC_CH0;

    /* Disable PA.0 digital input path */
    PA->DINOFF |= ((1 << 0) << GPIO_DINOFF_DINOFF0_Pos);

    /* Enable band-gap */
    SYS->IVREFCTL |= 7;

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

int32_t main (void)
{
    uint32_t u32Result;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    UART0_Init(); /* Init UART0 for printf */

    /*
       This sample code demonstrate ADC single mode conversion.
       Configure to ADC single-ended, single mode and enable channel 12(Band-gap)
       Set reference voltage to AVDD.
       It convert channel 12(Band-gap) and print conversion result
    */

    // Enable channel 12 (Band-gap)
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE, ADC_CH_12_MASK);

    // Set reference voltage to AVDD
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);

    // Power on ADC
    ADC_POWER_ON(ADC);

    // Enable ADC ADC_IF interrupt
    ADC_EnableInt(ADC, ADC_ADF_INT);
    // Enable NVIC ADC interrupt
    NVIC_EnableIRQ(ADC_IRQn);

    u8ADF = 0;

    while (1)
    {
        if (ADC_IS_BUSY())
            continue;

        // Start conversion
        ADC_START_CONV(ADC);

        // Wait for ADC conversion finish
        while (u8ADF == 0);

        // Get conversion data
        u32Result = ADC_GET_CONVERSION_DATA(ADC, 12);
        printf("Band-gap = 0x%x\n",u32Result);
    }

}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


