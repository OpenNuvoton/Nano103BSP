/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 16/02/19 9:36a $
 * @brief    Demonstrate PWM0 channel 0 trigger ADC.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

void ADC_IRQHandler(void);  //ADC interrupt service routine

void ADC_IRQHandler(void)  //ADC interrupt service routine
{
    uint32_t u32Flag;  //ADC interrupt status flag

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    // Check ADC conversion finish interrupt flag
    if(u32Flag & ADC_ADF_INT)
        printf("Channel 0 conversion result is 0x%x\n", (unsigned int)ADC_GET_CONVERSION_DATA(ADC, 0));

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
    CLK_EnableModuleClock(PWM0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HXT,CLK_UART0_CLK_DIVIDER(1));
    CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_ADC_CLK_DIVIDER(5));
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL1_PWM0SEL_PCLK0, 0);

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

// Init PWM0 channel 0 to trigger ADC
static void PWM0_Init(void)
{
    // PWM0 frequency is 100Hz, duty 30%
    PWM_ConfigOutputChannel(PWM0, 0, 100, 30);

    // Enable PWM0 channel 0 even zero point to trigger ADC
    PWM_EnableADCTrigger (PWM0, 0, PWM_TRIGGER_ADC_EVEN_ZERO_POINT);

    // Start
    PWM_Start(PWM0, 0x1);
}

int32_t main (void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    UART0_Init(); /* Init UART0 for printf */

    printf("\nThis sample code demonstrate PWM0 channel 0 trigger ADC function\n");

    /*
       This sample code demonstrate PWM0 channel 0 trigger ADC function.
       Configure to ADC single-ended, single mode and enable channel 0.
       Set reference voltage to AVDD.
       Besides, we have to configure PWM to trigger ADC.
       Set PWM0 frequency to 100Hz, duty 30%
       Enable PWM0 channel 0 even zero point to trigger ADC
       Then start PWM0 channel 0. 
       Whenever the counter of PWM0 channel 0 is zero, 
       PWM sends a request to ADC and ADC will do a conversion.
    */
    
    // Enable channel 0
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE, ADC_CH_0_MASK);

    // Set reference voltage to AVDD
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);

    // Power on ADC
    ADC_POWER_ON(ADC);

    // Enable PWM trigger
    ADC_EnableHWTrigger(ADC, ADC_TRIGGER_BY_PWM, 0);
    
    // Enable ADC ADC_IF interrupt
    ADC_EnableInt(ADC, ADC_ADF_INT);
    // Enable NVIC ADC interrupt
    NVIC_EnableIRQ(ADC_IRQn);

    // Init PWM0
    PWM0_Init();

    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


