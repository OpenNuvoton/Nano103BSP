/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 5 $
 * $Date: 15/12/25 4:01p $
 * @brief    Read/write EEPROM via an I2C interface.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8DeviceAddr;     // slave address
uint8_t g_au8TxData[3];     // master transmit buffer
uint8_t g_u8RxData;         // master receive buffer
volatile uint8_t g_u8DataLen;   // count for master transmit
volatile uint8_t g_u8EndFlag = 0;   // function end flag

/* I2C handler */
typedef void (*I2C_FUNC)(uint32_t u32Status);
static I2C_FUNC s_I2C0HandlerFn = NULL;

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
    if (I2C_GET_TIMEOUT_FLAG(I2C0)) {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    } else {
        /* jump to handler */
        if (s_I2C0HandlerFn != NULL)
            s_I2C0HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08) {                        /* START has been transmitted and prepare SLA+W */
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1));  /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    } else if (u32Status == 0x18) {                 /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen++]);     /* Write data to I2C buffer */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    } else if (u32Status == 0x20) {                 /* SLA+W has been transmitted and NACK has been received */
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);  /* I2C STOP*/
    } else if (u32Status == 0x28) {                 /* DATA has been transmitted and ACK has been received */
        if (g_u8DataLen != 2) {
            /* if length<2, just trigger I2C and go on */
            I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen++]);  /* Write data to I2C buffer */
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);               /* Trigger I2C */
        } else {
            /* generate Repeat START */
            I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_SI);
        }
    } else if (u32Status == 0x10) {                         /* Repeat START has been transmitted and prepare SLA+R */
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1) | 0x01);   /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);                  /* Trigger I2C */
    } else if (u32Status == 0x40) {                         /* SLA+R has been transmitted and ACK has been received */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);                  /* Trigger I2C */
    } else if (u32Status == 0x58) {                         /* DATA has been received and NACK has been returned */
        g_u8RxData = I2C_GET_DATA(I2C0);                    /* Get I2C data */
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);        /* I2C STOP */
        g_u8EndFlag = 1;                                    /* Exit wait loop in main function */
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08) {                        /* START has been transmitted */
        I2C_SET_DATA(I2C0, g_u8DeviceAddr << 1);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    } else if (u32Status == 0x18) {                 /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen++]);     /* Write data to I2C buffer */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);          /* Trigger I2C */
    } else if (u32Status == 0x20) {                 /* SLA+W has been transmitted and NACK has been received */
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);  /* I2C STOP */
    } else if (u32Status == 0x28) {                 /* DATA has been transmitted and ACK has been received */
        if (g_u8DataLen != 3) {
             /* if length<3, just trigger I2C and go on */
            I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);              /* Trigger I2C */
        } else {
            I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);    /* I2C STOP */
            g_u8EndFlag = 1;                                /* Exit wait loop in main function */
        }
    } else {
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
/* Configure I2C0 to 100KHz speed and enable interrupt                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_Init(void)
{
    /* Open I2C0 and set clock to 100k */
    I2C_Open(I2C0, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C0));

    /* Set I2C0 2 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, I2C_GCMODE_DISABLE);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, I2C_GCMODE_DISABLE);   /* Slave Address : 0x35 */

    /* Enable I2C0 interrupt */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
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

    /*
        This sample code sets I2C bus clock to 100kHz. Then, accesses EEPROM 24LC64 with Byte Write
        and Byte Read operations, and check if the read data is equal to the programmed data.
    */

    printf("+-------------------------------------------------------+\n");
    printf("|    Nano103 Series I2C Sample Code with EEPROM 24LC64  |\n");
    printf("+-------------------------------------------------------+\n");

    /* Init I2C0 to access EEPROM */
    I2C0_Init();
    
    /* Set EEPROM address to 0x50 */
    g_u8DeviceAddr = 0x50;

    for (i = 0; i < 2; i++) {
        /* Write address and data into tx buffer */
        g_au8TxData[0] = (uint8_t)((i & 0xFF00) >> 8);
        g_au8TxData[1] = (uint8_t)(i & 0x00FF);
        g_au8TxData[2] = (uint8_t)(g_au8TxData[1] + 3);
        
        /* Reset some variables */
        g_u8DataLen = 0;
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
		
		/* Wait write operation complete */
		CLK_SysTickDelay(10000);

        /* I2C function to read data from slave */
        s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterRx;
        
        /* Reset some variables */
        g_u8DataLen = 0;
        g_u8DeviceAddr = 0x50;
        
        /* I2C as master sends START signal */
        I2C_SET_CONTROL_REG(I2C0, I2C_STA);

        /* Wait I2C Rx Finish */
        while (g_u8EndFlag == 0);
        
        /* Make sure I2C0 STOP already */
        while(I2C0->CTL & I2C_CTL_STO_Msk);
                
        /* Compare data */
        if (g_u8RxData != g_au8TxData[2]) {
            printf("I2C Byte Write/Read Failed, Data 0x%x\n", g_u8RxData);
            return -1;
        }
    }
    
    printf("I2C Access EEPROM Test OK\n");
	while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/

