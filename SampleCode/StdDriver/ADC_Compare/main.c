/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 9 $
 * $Date: 16/02/19 9:36a $
 * @brief    Demonstrate ADC conversion and comparison function by monitoring the conversion result of channel 0.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

void ADC_IRQHandler(void);  //ADC interrupt service routine

void ADC_IRQHandler(void)  //ADC interrupt service routine
{
    uint32_t u32Flag; //ADC interrupt status flag

    // Get ADC comparator interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_CMP0_INT | ADC_CMP1_INT);

    // Show the comparison result
    if(u32Flag & ADC_CMP0_INT)
        printf("Channel 0 input < 0x200\n");
    if(u32Flag & ADC_CMP1_INT)
        printf("Channel 0 input >= 0x200\n");

    // Clear ADC comparator interrupt flag
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

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HXT,CLK_UART0_CLK_DIVIDER(1));
    CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_ADC_CLK_DIVIDER(5));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
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

int32_t main (void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    UART0_Init(); /* Init UART0 for printf */

    printf("\nThis sample code demonstrate ADC conversion and comparison function\n");
    printf("by monitoring the conversion result of channel 0\n");

    /*
       This sample code demonstrate ADC conversion and comparison function
       by monitoring the conversion result of channel 0.
       The comparison data and condition are set as below
       Comparator 0 : monitor channel 0 input less than 0x200
       Comparator 1 : monitor channel 0 input greater or equal to 0x200
       When input voltage of channel 0 meet the condition of comparator 0,
       output message shows "Channel 0 input < 0x200".
       When input voltage of channel 0 meet the condition of comparator 1,
       output message shows "Channel 0 input >= 0x200".
       You can change the input voltage of channel 0 to see the different output message
     */

    // Enable channel 0
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE, ADC_CH_0_MASK);

    // Set reference voltage to AVDD
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);

    // Power on ADC
    ADC_POWER_ON(ADC);

    // Configure and enable Comparator 0 to monitor channel 0 input less than 0x200
    ADC_ENABLE_CMP0(ADC, 0, ADC_CMP_LESS_THAN, 0x200, 16);
    // Configure and enable Comparator 1 to monitor channel 0 input greater or equal to 0x200
    ADC_ENABLE_CMP1(ADC, 0, ADC_CMP_GREATER_OR_EQUAL_TO, 0x200, 16);

    // Enable ADC comparator 0 interrupt
    ADC_EnableInt(ADC, ADC_CMP0_INT);
    // Enable ADC comparator 1 interrupt
    ADC_EnableInt(ADC, ADC_CMP1_INT);
    // Enable NVIC ADC interrupt
    NVIC_EnableIRQ(ADC_IRQn);

    // Continuously trigger ADC
    while(1)
    {
        // Trigger ADC conversion if it is idle
        if(!ADC_IS_BUSY(ADC))
        {
            ADC_START_CONV(ADC);
        }
    }

}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


