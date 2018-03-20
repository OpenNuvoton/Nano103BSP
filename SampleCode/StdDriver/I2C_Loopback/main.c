/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 5 $
 * $Date: 15/12/25 4:04p $
 * @brief    An I2C master/slave demo by connecting I2C0 and I2C1 interface.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

/* Slave address */
#define SLAVE_ADDRESS 0x16

/* I2C function handler */
typedef void (*I2C_FUNC)(uint32_t u32Status);

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8DeviceAddr;             // slave address
uint32_t g_u32SlaveBuffAddr;        // slave receive buffer for address part
uint8_t g_au8MasterTxData[3];       // master transmit buffer
uint8_t g_u8SlvData[256];           // slave receive buffer for data part
uint8_t g_u8MasterRxData;           // master receive buffer
uint8_t g_au8SlaveRxData[3];        // slave receive buffer
volatile uint8_t g_u8MasterDataLen; // count for master transmit
uint8_t g_u8SlaveDataLen;           // count for slave transmit
volatile uint8_t g_u8EndFlag = 0;   // function end flag

/* Function handler pointer */
volatile I2C_FUNC s_I2C0HandlerFn = NULL;
volatile I2C_FUNC s_I2C1HandlerFn = NULL;
/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32Status;

    /* clear interrupt flag */
    I2C0->INTSTS |= I2C_INTSTS_INTSTS_Msk;

    /* get status */
    u32Status = I2C_GET_STATUS(I2C0);

    /* check if time-out occur */
    if (I2C_GET_TIMEOUT_FLAG(I2C0))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    }
    else
    {
        /* jump to handler */
        if (s_I2C0HandlerFn != NULL)
            s_I2C0HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    /* clear interrupt flag */
    I2C1->INTSTS |= I2C_INTSTS_INTSTS_Msk;

    /* get status */
    u32Status = I2C_GET_STATUS(I2C1);

    /* check if time-out occur */
    if (I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C1 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
        /* jump to handler */
        if (s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08)                          /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1));  /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    }
    else if (u32Status == 0x18)                     /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    }
    else if (u32Status == 0x20)                     /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_STO | I2C_SI);
    }
    else if (u32Status == 0x28)                     /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8MasterDataLen != 2)
        {
            /* if length<2, just trigger I2C and go on */
            I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);     /* Write data to I2C buffer */
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);      /* Trigger I2C */
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_SI);    /* Generate I2C repeat START */
        }
    }
    else if (u32Status == 0x10)                     /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1) | 0x01);  /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    }
    else if (u32Status == 0x40)                     /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    }
    else if (u32Status == 0x58)                     /* DATA has been received and NACK has been returned */
    {
        g_u8MasterRxData = I2C_GET_DATA(I2C0);      /* Get data */
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);    /* I2C STOP */
        g_u8EndFlag = 1;                            /* Exit wait loop in main function */
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08)                          /* START has been transmitted */
    {
        I2C_SET_DATA(I2C0, g_u8DeviceAddr << 1);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    }
    else if (u32Status == 0x18)                     /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    }
    else if (u32Status == 0x20)                     /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_STO | I2C_SI);
    }
    else if (u32Status == 0x28)                     /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8MasterDataLen != 3)
        {
            /* if length<3, just trigger I2C and go on */
            I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);     /* Write data to I2C buffer */
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);                              /* Trigger I2C */
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);                    /* I2C STOP */
            g_u8EndFlag = 1;                                                /* Exit wait loop in main function */
        }
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 Tx and Rx Callback Function                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_SlaveTRx(uint32_t u32Status)
{
    if (u32Status == 0x60)                               /* Own SLA+W has been receive; ACK has been return */
    {
        g_u8SlaveDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);      /* Set ACK as slave side */
    }
    else if (u32Status == 0x80)                        /* Previously address with own SLA address
                                                               Data has been received; ACK has been returned*/
    {
        g_au8SlaveRxData[g_u8SlaveDataLen] = I2C_GET_DATA(I2C1);    /* Read Rx data */
        g_u8SlaveDataLen++;

        /* Pack receive address and data into receive array */
        /* Address part: byte-0 and byte-1 */
        if (g_u8SlaveDataLen == 2)
        {
            g_u32SlaveBuffAddr = (g_au8SlaveRxData[0] << 8) + g_au8SlaveRxData[1];
        }

        /* Data part: byte-3 */
        if (g_u8SlaveDataLen == 3)
        {
            g_u8SlvData[g_u32SlaveBuffAddr] = g_au8SlaveRxData[2];
            g_u8SlaveDataLen = 0;
        }
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);     /* Set ACK as slave side */
    }
    else if(u32Status == 0xA8)                  /* Own SLA+R has been receive; ACK has been return */
    {

        I2C_SET_DATA(I2C1, g_u8SlvData[g_u32SlaveBuffAddr]);    /* Write data into I2C buffer */
        g_u32SlaveBuffAddr++;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);     /* Set ACK as slave side */
    }
    else if (u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
                                                   Not ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);     /* Set ACK as slave side */
    }
    else if (u32Status == 0x88)                 /* Previously addressed with own SLA address; NOT ACK has
                                                   been returned */
    {
        g_u8SlaveDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);     /* Set ACK as slave side */
    }
    else if (u32Status == 0xA0)                 /* A STOP or repeated START has been received while still
                                                   addressed as Slave/Receiver*/
    {
        g_u8SlaveDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);      /* Set ACK as slave side */
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
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

    /* Enable UART0/I2C0/I2C1 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(I2C0_MODULE);
    CLK_EnableModuleClock(I2C1_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    /* Set PA multi-function pins for I2C0/I2C1 */
    /* I2C0: GPA8 - SDA, GPA9 - SCL */
    /* I2C1: GPA10 - SDA, GPA11 - SCL */
    SYS->GPA_MFPH = (SYS_GPA_MFPH_PA8MFP_I2C0_SDA | SYS_GPA_MFPH_PA9MFP_I2C0_SCL | SYS_GPA_MFPH_PA10MFP_I2C1_SDA | SYS_GPA_MFPH_PA11MFP_I2C1_SCL);

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Configure I2C0 to 100KHz speed and enable interrupt                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_Init(void)
{
    /* Open I2C0 and set clock to 100k */
    I2C_Open(I2C0, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C0 clock %d Hz\n", I2C_GetBusClockFreq(I2C0));

    /* Enable I2C0 interrupt */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Configure I2C1 to 100KHz speed and enable interrupt                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_Init(void)
{
    /* Open I2C1 and set clock to 100k */
    I2C_Open(I2C1, 400000);

    /* Get I2C1 Bus Clock */
    printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));

    /* Set I2C1 2 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, SLAVE_ADDRESS, I2C_GCMODE_DISABLE);   /* Set Slave Address */

    /* Enable I2C1 interrupt */
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);

    /* I2C enter no address SLV mode (slave) */
    I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main (void)
{
    uint32_t i;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("+-------------------------------------------------------+\n");
    printf("|      Nano103 Series I2C Cross Test Sample Code        |\n");
    printf("+-------------------------------------------------------+\n");
    printf("  SCK: GPA8 <--> GPA10\n");
    printf("  SDA: GPA9 <--> GPA11\n");

    /*
        Master(I2C0) writes data to Salve (I2C1) and check Rx data of slave.
        And please make sure wire connection between I2C0 and I2C1 is correct like above printf message.
    */

    /* Configure I2C0 as master and I2C1 as slave */
    I2C0_Init();
    I2C1_Init();

    /* Clear slave data buffer */
    for (i = 0; i < 0x100; i++)
        g_u8SlvData[i] = 0;

    /* I2C function to Slave receive/transmit data */
    s_I2C1HandlerFn = (I2C_FUNC)I2C_SlaveTRx;

    /* Specific salve address */
    g_u8DeviceAddr = SLAVE_ADDRESS;

    /* loop for 0x100 times  */
    for (i = 0; i < 0x100; i++)
    {
        /*
            Master Tx stage:
            Write some data to salve side
        */


        /*     Write address and data into tx buffer:
               Data[15:8] --> g_au8MasterTxData[0]
               Data[7:0] --> g_au8MasterTxData[1] */

        g_au8MasterTxData[0] = (uint8_t)((i & 0xFF00) >> 8);
        g_au8MasterTxData[1] = (uint8_t)(i & 0x00FF);
        g_au8MasterTxData[2] = (uint8_t)(g_au8MasterTxData[1] + 3);

        /* Reset some variables */
        g_u8MasterDataLen = 0;
        g_u8EndFlag = 0;

        /* I2C function to write data to slave */
        s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterTx;

        /* I2C as master sends START signal */
        I2C_SET_CONTROL_REG(I2C0, I2C_STA);

        /* Wait I2C Tx Finish */
        while (g_u8EndFlag == 0);

        /* Make sure I2C0 STOP already */
        while(I2C0->CTL & I2C_CTL_STO_Msk);

        /* Reset variable */
        g_u8EndFlag = 0;

        /*
            Master Rx stage:
            Receive data which just sent to slave before
        */

        /* I2C function to read data from slave */
        s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterRx;

        /* Reset some variables */
        g_u8MasterDataLen = 0;
        g_u8DeviceAddr = SLAVE_ADDRESS;

        /* I2C as master sends START signal */
        I2C_SET_CONTROL_REG(I2C0, I2C_STA);

        /* Wait I2C Rx Finish */
        while (g_u8EndFlag == 0);

        /* Make sure I2C0 STOP already */
        while(I2C0->CTL & I2C_CTL_STO_Msk);

        /* Compare Tx and Rx data */
        if (g_u8MasterRxData != g_au8MasterTxData[2])
        {
            printf("I2C Byte Write/Read Failed, Data 0x%x\n", g_u8MasterRxData);
            return -1;
        }
    }

    printf("\nTest Completely !!\n");
    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
