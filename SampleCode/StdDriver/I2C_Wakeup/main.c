/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 15/12/16 4:00p $
 * @brief    Demonstrate how to wake up system form Power-down mode by I2C interrupt.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void PDWU_IRQHandler(void)
{
    /* Clear I2C wake up flag */
    I2C_CLEAR_WAKEUP_FLAG(I2C0);

    /* Clear wake up flag */
    CLK->WKINTSTS = 1;
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
    CLK_EnableModuleClock(I2C0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    /* Set PA multi-function pins for I2C0 */
    /* I2C0: GPA8 - SDA, GPA9 - SCL */
    SYS->GPA_MFPH = (SYS_GPA_MFPH_PA8MFP_I2C0_SDA | SYS_GPA_MFPH_PA9MFP_I2C0_SCL);

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Init Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_Init(void)
{
    /* Open I2C0 and set clock to 100k */
    I2C_Open(I2C0, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C0));

    /* Set I2C0 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x25, I2C_GCMODE_DISABLE);   /* Set Slave Address */

    NVIC_EnableIRQ(PDWU_IRQn);

    /* Enable I2C0 wakeup function */
    I2C_EnableWakeup(I2C0);

    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main (void)
{
    volatile uint8_t u8RxData[3];       // Receive buffer
    volatile uint8_t u8ReadWrite;       // indicate master wants to read/write before waking up

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("+-------------------------------------------------------+\n");
    printf("|        Nano103 Series I2C Wake Up Sample Code         |\n");
    printf("+-------------------------------------------------------+\n");
    printf("Note: Master need to send the I2C signal likes the following :\n");
    printf("    S | SLA+W | DATA | DATA | DATA | P \n");
    printf(" or S | SLA+R | DATA | DATA | DATA | P \n\n");

    /* Init I2C0 as Slave */
    I2C0_Init();

    printf("Enter power down mode !!!\n");
    // Wait 'til UART FIFO empty to get a cleaner console out
    while(!UART_GET_TX_EMPTY(UART0));

    /* Un-lock protected register */
    SYS_UnlockReg();

    /* Enable wake up interrupt */
    CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;

    /* System enters power down */
    CLK_PowerDown();

    /* System wakes up, SCK pin will be held down until software clears wake up done flag */
    /* Need to make sure I2C sla is done(after the 9th bit) */
    while(!I2C_GET_WAKEUP_ACK_DONE_FLAG(I2C0));

    /* Remember master wants to read or write data */
    u8ReadWrite = I2C_GET_WAKEUP_RW_FLAG(I2C0);

    /* If master wants to read, we need to prepare Tx output data here before release SCK pin */
    if(u8ReadWrite)
        I2C_SET_DATA(I2C0, 0x11);

    /* I2C can release SCK pin and enter I2C normal operation */
    I2C_CLEAR_WAKEUP_ACK_DONE_FLAG(I2C0);

    if(u8ReadWrite)     // master wants to read
    {
        /* 1st data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it

        /* 2nd data */
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        I2C_SET_DATA(I2C0, 0x55);                   // prepare tx data
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it

        /* 3rd data */
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        I2C_SET_DATA(I2C0, 0xAA);                   // prepare tx data
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it

        /* stop */
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_STO);// trigger it
        while(I2C0->CTL & I2C_CTL_STO_Msk);        // wait until STOP happens
    }
    else            // master wants to write
    {
        /* 1st data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        u8RxData[0] = I2C_GET_DATA(I2C0);           // receive data

        /* 2nd data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        u8RxData[1] = I2C_GET_DATA(I2C0);           // receive data

        /* 3rd data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        u8RxData[2] = I2C_GET_DATA(I2C0);           // receive data

        /* stop */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_STO);// trigger it
        while(I2C0->CTL & I2C_CTL_STO_Msk);        // wait until STOP happens
    }

    printf("Wake up !!\n");

    while(1);
}
