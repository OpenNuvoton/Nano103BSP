
/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 8 $
 * $Date: 15/12/27 11:42a $
 * @brief    Demonstrate PWM Capture function by using PWM0 channel 0 to capture the output of PWM0 channel 2.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

void PWM0_IRQHandler(void);  // PWM0 interrupt service routine

void PWM0_IRQHandler(void)  // PWM0 interrupt service routine
{
    if(PWM_GetCaptureIntFlag(PWM0, 0) > 1) { // Get capture interrupt flag
        PWM_ClearCaptureIntFlag(PWM0, 0, PWM_CAPTURE_INT_FALLING_LATCH); // Clear capture interrupt flag
    }
}

/*--------------------------------------------------------------------------------------*/
/* Capture function to calculate the input waveform information                         */
/* u32Count[4] : Keep the internal counter value when input signal rising / falling     */
/*               happens                                                                */
/*                                                                                      */
/* time    A    B     C     D                                                           */
/*           ___   ___   ___   ___   ___   ___   ___   ___                              */
/*      ____|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_____                        */
/* index              0 1   2 3                                                         */
/*                                                                                      */
/* The capture internal counter down count from 0x10000, and reload to 0x10000 after    */
/* input signal falling happens (Time B/C/D)                                            */
/*--------------------------------------------------------------------------------------*/
void CalPeriodTime(PWM_T *PWM, uint32_t u32Ch)
{
    uint16_t u32Count[4];
    uint32_t u32i;
    uint16_t u16RisingTime, u16FallingTime, u16HighPeroid, u16LowPeroid, u16TotalPeroid;

    /* Clear Capture Falling Indicator (Time A) */
    PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_FALLING_LATCH | PWM_CAPTURE_INT_RISING_LATCH);

    /* Wait for Capture Falling Indicator  */
    while(PWM_GetCaptureIntFlag(PWM, u32Ch) < 2);

    /* Clear Capture Falling Indicator (Time B)*/
    PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_FALLING_LATCH);

    u32i = 0;

    while(u32i < 4) {
        /* Wait for Capture Falling Indicator */
        while(PWM_GetCaptureIntFlag(PWM, u32Ch) < 2);

        /* Clear Capture Falling and Rising Indicator */
        PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_FALLING_LATCH | PWM_CAPTURE_INT_RISING_LATCH);

        /* Get Capture Falling Latch Counter Data */
        u32Count[u32i++] = PWM_GET_CAPTURE_FALLING_DATA(PWM, u32Ch);

        /* Wait for Capture Rising Indicator */
        while(PWM_GetCaptureIntFlag(PWM, u32Ch) < 1);

        /* Clear Capture Rising Indicator */
        PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_RISING_LATCH);

        /* Get Capture Rising Latch Counter Data */
        u32Count[u32i++] = PWM_GET_CAPTURE_RISING_DATA(PWM, u32Ch);
    }

    u16RisingTime = u32Count[1];

    u16FallingTime = u32Count[0];

    u16HighPeroid = u32Count[1] - u32Count[2];

    u16LowPeroid = 0x10000 - u32Count[1];

    u16TotalPeroid = 0x10000 - u32Count[2];

    printf("\nPWM generate: \nHigh Period=7199 ~ 7201, Low Period=16799 ~ 16801, Total Period=23999 ~ 24001\n");
    printf("\nCapture Result: Rising Time = %d, Falling Time = %d \nHigh Period = %d, Low Period = %d, Total Period = %d.\n\n",
           u16RisingTime, u16FallingTime, u16HighPeroid, u16LowPeroid, u16TotalPeroid);
    if((u16HighPeroid < 7199) || (u16HighPeroid > 7201) || (u16LowPeroid < 16799) || (u16LowPeroid > 16801) || (u16TotalPeroid < 23999) || (u16TotalPeroid > 24001))
        printf("Capture Test Fail!!\n");
    else
        printf("Capture Test Pass!!\n");
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

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(PWM0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HXT,CLK_UART0_CLK_DIVIDER(1));
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL1_PWM0SEL_PCLK0, 0);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB.0, PB.1 multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    /* Set PA.12 and PA.14 multi-function pins for PWM0 channel 0 and 2 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA12MFP_Msk)) | SYS_GPA_MFPH_PA12MFP_PWM0_CH0;
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_PWM0_CH2;

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


int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("+------------------------------------------------------------------------+\n");
    printf("|                          PWM Driver Sample Code                        |\n");
    printf("|                                                                        |\n");
    printf("+------------------------------------------------------------------------+\n");
    printf("  This sample code will use PWM0 channel 0 to capture\n  the signal from PWM0 channel 2.\n");
    printf("  I/O configuration:\n");
    printf("    PWM0_CH0(PA.12 PWM0 channel 0) <--> PWM0_CH2(PA.14 PWM0 channel 2)\n\n");
    printf("Use PWM0 Channel 0(PA.12) to capture the PWM0 Channel 2(PA.14) Waveform\n");

    while(1) {
        printf("Press any key to start PWM Capture Test\n");
        getchar();

        /*--------------------------------------------------------------------------------------*/
        /* Set the PWM1 Channel 0 as PWM output function.                                       */
        /*--------------------------------------------------------------------------------------*/

        /* Assume PWM output frequency is 250Hz and duty ratio is 30%, user can calculate PWM settings by follows.
           duty ratio = (CMR+1)/(CNR+1)
           cycle time = CNR+1
           High level = CMR+1
           PWM clock source frequency = __HXT = 12000000
           (CNR+1) = PWM clock source frequency/prescaler/clock source divider/PWM output frequency
                   = 12000000/2/1/250 = 24000
           (Note: CNR is 16 bits, so if calculated value is larger than 65536, user should increase prescale value.)
           CNR = 23999
           duty ratio = 30% ==> (CMR+1)/(CNR+1) = 30%
           CMR = 7199
           Prescale value is 1 : prescaler= 2
           Clock divider is PWM_CSR_DIV1 : clock divider =1
        */

        /* set PWM0 channel 2 output configuration */
        PWM_ConfigOutputChannel(PWM0, 2, 250, 30);

        /* Enable PWM Output path for PWM0 channel 2 */
        PWM_EnableOutput(PWM0, PWM_CH_2_MASK);

        /* Enable Timer for PWM0 channel 2 */
        PWM_Start(PWM0, PWM_CH_2_MASK);

        /*--------------------------------------------------------------------------------------*/
        /* Set the PWM0 channel 0 for capture function                                          */
        /*--------------------------------------------------------------------------------------*/

        /* If input minimum frequency is 250Hz, user can calculate capture settings by follows.
           Capture clock source frequency = __HXT = 12000000 in the sample code.
           (CNR+1) = Capture clock source frequency/prescaler/clock source divider/minimum input frequency
                   = 12000000/2/1/250 = 24000
           (Note: CNR is 16 bits, so if calculated value is larger than 65536, user should increase prescale value.)
           CNR = 0xFFFF
           (Note: In capture mode, user should set CNR to 0xFFFF to increase capture frequency range.)
        */

        /* set PWM0 channel 0 capture configuration */
        PWM_ConfigCaptureChannel(PWM0, 0, 166, 0);

        /* Enable capture falling edge interrupt for PWM0 channel 0 */
        //PWM_EnableCaptureInt(PWM0, 0, PWM_CAPTURE_INT_FALLING_LATCH);

        /* Enable PWM0 NVIC interrupt */
        //NVIC_EnableIRQ(PWM0_IRQn);

        /* Enable Timer for PWM0 channel 0 */
        PWM_Start(PWM0, PWM_CH_0_MASK);

        /* Enable Capture Function for PWM0 channel 0 */
        PWM_EnableCapture(PWM0, PWM_CH_0_MASK);

        /* Enable falling capture reload */
        PWM0->CAPCTL |= PWM_CAPCTL_FCRLDENn_Msk;

        /* Wait until PWM0 channel 0 Timer start to count */
        while((PWM0->CNT[0]) == 0);

        /* Capture the Input Waveform Data */
        CalPeriodTime(PWM0, 0);
        /*---------------------------------------------------------------------------------------------------------*/
        /* Stop PWM1 channel 0 (Recommended procedure method 1)                                                    */
        /* Set PWM Timer loaded value(Period) as 0. When PWM internal counter(CNT) reaches to 0, disable PWM Timer */
        /*---------------------------------------------------------------------------------------------------------*/

        /* Set PWM0 channel 2 loaded value as 0 */
        PWM_Stop(PWM0, PWM_CH_2_MASK);

        /* Wait until PWM1 channel 0 Timer Stop */
        while((PWM0->CNT[2] & PWM_CNTEN_CNTEN2_Msk) != 0);

        /* Disable Timer for PWM0 channel 2 */
        PWM_ForceStop(PWM0, PWM_CH_2_MASK);

        /* Disable PWM Output path for PWM0 channel 2 */
        PWM_DisableOutput(PWM0, PWM_CH_2_MASK);

        /*---------------------------------------------------------------------------------------------------------*/
        /* Stop PWM0 channel 0 (Recommended procedure method 1)                                                    */
        /* Set PWM Timer loaded value(Period) as 0. When PWM internal counter(CNT) reaches to 0, disable PWM Timer */
        /*---------------------------------------------------------------------------------------------------------*/

        /* Disable PWM0 NVIC */
        //NVIC_DisableIRQ(PWM0_IRQn);

        /* Set loaded value as 0 for PWM0 channel 0 */
        PWM_Stop(PWM0, PWM_CH_0_MASK);

        /* Wait until PWM0 channel 0 current counter reach to 0 */
        while((PWM0->CNT[0] & PWM_CNTEN_CNTEN0_Msk) != 0);

        /* Disable Timer for PWM0 channel 0 */
        PWM_ForceStop(PWM0, PWM_CH_0_MASK);

        /* Disable Capture Function and Capture Input path for  PWM0 channel 0 */
        PWM_DisableCapture(PWM0, PWM_CH_0_MASK);

        /* Clear Capture Interrupt flag for PWM0 channel 0 */
        PWM_ClearCaptureIntFlag(PWM0, 0, PWM_CAPTURE_INT_FALLING_LATCH);
    }
}


/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


