/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 15/12/18 1:59p $
 * @brief    Demonstrate how to transmit LIN header and response.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"
#include "uart.h"
#include "sys.h"
#include "clk.h"



#define RXBUFSIZE 3072

/*---LIN Check sum mode-------------------*/
#define MODE_CLASSIC    2
#define MODE_ENHANCED   1

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---Using in UART Test -------------------*/
volatile uint8_t comRbuf[1024];
volatile uint16_t comRbytes = 0;        /* Available receiving bytes */
volatile uint16_t comRhead  = 0;
volatile uint16_t comRtail  = 0;
volatile int32_t g_bWait    = TRUE;
uint8_t u8SendData[12] = {0};

/*---Using in RS485 Test -------------------*/
uint8_t u8RecData[RXBUFSIZE]  = {0};
int32_t w_pointer =0;
volatile int32_t r_pointer = 0;
int32_t IsRS485ISR_TX_PORT = FALSE;
int32_t IsRS485ISR_RX_PORT = FALSE;

/*---Using in LIN Test -------------------*/
uint8_t testPattern[] = {0x00,0x55,0xAA,0xFF,0x00,0x55,0xFF,0xAA};

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void UART_INT_HANDLE(uint32_t u32IntStatus);
int32_t DataCompare(uint8_t InBuffer[],uint8_t OutBuffer[],int32_t len);
extern char GetChar(void);

/**
  * @brief  UART LIN interrupt Handle function.
  * @param  None.
  * @return None.
  */
void LIN_HANDLE(void)
{
    int32_t i = 0;
    volatile uint32_t REG = 0;

    if(UART1->INTSTS & UART_INTSTS_LINIF_Msk)  /* LIN INT */
    {
        UART1->TRSR |= UART_TRSR_LINRXIF_Msk; // clear LIN RX interrupt flag
        g_bWait = FALSE;
    }

    if(!g_bWait)
    {
        if(UART1->INTSTS & UART_INTSTS_RDAIF_Msk)  /* RDA INT */
        {
            u8RecData[r_pointer++] = UART1->DAT; // receive data to buffer
        }

        if(r_pointer==11)
        {
            printf("  %02x \t",u8RecData[1]);           /* ID */
            for(i =2; i<10; i++)
            {
                printf("%02x,",u8RecData[i] );          /* Data Bytes */
            }
            printf("  %02x \t",u8RecData[10] );         /* CheckSum */

            /* Compare data */
            if(DataCompare(u8SendData,u8RecData,10))
            {
                printf("\tOK\n");
                r_pointer=0;
            }
            else
            {
                printf("...Failed\n");
            }
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Sub-Function for LIN                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
/**
  * @brief  he checksum byte.
  * @param  DataBuffer[]  data buffer
  * @param  Offset        1:  Compute not include ID  (LIN1.1) \n
  *                       2:  Compute n include ID  (LIN2.0)
  * @return (255-CheckSum) checksum.
  */
uint32_t cCheckSum(uint8_t DataBuffer[], uint32_t Offset)
{
    uint32_t i,CheckSum =0;

    for(i=Offset,CheckSum=0; i<=9; i++)
    {
        CheckSum+=DataBuffer[i];
        if (CheckSum>=256)
            CheckSum-=255;
    }
    return (255-CheckSum);
}

/**
  * @brief  Compute the Parity Bit.
  * @param  i      LIN ID.
  * @return number LIN ID + Parity bit.
  */
int8_t Parity(int i)
{
    int8_t number = 0 ;
    int8_t ID[6];
    int8_t p_Bit[2];
    int8_t mask =0;

    if(i>=64)
        printf("The ID is not match protocol\n");
    for(mask=0; mask<7; mask++)
        ID[mask] = (i & (1<<mask))>>mask;

    p_Bit[0] = (ID[0] + ID[1] + ID[2] + ID[4])%2;
    p_Bit[1] = (!((ID[1] + ID[3] + ID[4] + ID[5])%2));

    number = i + (p_Bit[0] <<6) + (p_Bit[1]<<7);
    return number;

}

/**
  * @brief  compare data.
  * @param  InBuffer[]  data buffer.
  * @param  OutBuffer[] data buffer.
  * @param  len         compare data length
  * @return TRUE : compare data ok. \n
  *         FALSE: compare data fail
  */
int32_t DataCompare(uint8_t InBuffer[],uint8_t OutBuffer[],int32_t len)
{
    int i=0;
    for(i=0; i<len; i++)
    {
        if(InBuffer[i]!=OutBuffer[i])
        {
            printf("In[%d] = %x , Out[%d] = %d\n",i,InBuffer[i],i,OutBuffer[i]);
            return FALSE;
        }
    }
    return TRUE;
}

/**
 *  @brief  ISR to handle UART Channel 1 interrupt event.
 *  @param  None
 *  @return None
 */
void UART1_IRQHandler(void)
{
    if((UART1->FUNCSEL & 0x3) == 0x1)   // LIN function
    {
        LIN_HANDLE();
    }
}

/**
 *  @brief  LIN send header. Only Send the LIN protocol header (Sync.+ID + parity field)
 *  @param  tUART The pointer of the specified UART module
 *  @param  id LIN ID
 *  @return None
 */
void LIN_HeaderSend(UART_T *tUART,int32_t id)
{
    w_pointer =0 ;

    /* Set LIN operation mode */
    tUART->ALTCTL &= ~(UART_ALTCTL_BRKFL_Msk | UART_ALTCTL_LINRXEN_Msk | UART_ALTCTL_LINTXEN_Msk);
    tUART->ALTCTL |= (13 & UART_ALTCTL_BRKFL_Msk);
    tUART->ALTCTL |= (UART_ALTCTL_LINTXEN_Msk | UART_ALTCTL_LINRXEN_Msk);

    u8SendData[w_pointer++] = 0x55 ;                /* SYNC */

    u8SendData[w_pointer++] = Parity(id);           /* ID */

    UART_Write(tUART,u8SendData,2); /* Send Data */
}

/**
 *  @brief  LIN send response. Only Send the LIN protocol response (Data + CheckSum field)
 *  @param  tUART            The pointer of the specified UART module
 *  @param  checkSumOption   1:  Compute not include ID  (LIN1.1) \n
 *                           2:  Compute n include ID  (LIN2.0)
 *  @param  Pattern[]        response data
 *  @return None
 */
void LIN_ResponseSend(UART_T *tUART,int32_t checkSumOption,uint8_t Pattern[])
{
    int32_t i;

    for(i=0; i<8; i++)
        u8SendData[w_pointer++] = Pattern[i] ;

    u8SendData[w_pointer++] = cCheckSum(u8SendData,checkSumOption) ;

    UART_Write(tUART,u8SendData+2,9);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Sub-Function for LIN END                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

/**
 *  @brief  The function is UART LIN mode demo code.
 *  @param  None
 *  @return None
 */
void LIN_FunctionTest()
{
    int32_t i=0;

    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|               LIN Function Test                           |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| The program is used to test LIN BUS.                      |\n");
    printf("| It will send ID 0~10 by a fixed pattern                   |\n");
    printf("| Enter any key to start                                    |\n");
    printf("+-----------------------------------------------------------+\n\n");
    GetChar();

    printf("\nLIN Sample Demo. \n");

    /* Set UART Configuration */
    UART_Open(UART1, 9600); /* set UART baudrate is 9600 */
    UART_SetLine_Config(UART1, 9600, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1); /* Set UART Line config */
    UART_SelectLINMode(UART1,  (UART_ALTCTL_LINTXEN_Msk | UART_ALTCTL_LINRXEN_Msk), 5); /* Select LIN operation mode */

    UART_EnableInt(UART1, UART_INTEN_RLSIEN_Msk|UART_INTEN_RDAIEN_Msk|UART_INTEN_LINIEN_Msk); /* Enable interrupt */
    NVIC_EnableIRQ(UART1_IRQn); /* Enable NVIC IRQ */

    printf("+-----------------------------------------------------------+\n");
    printf("|[ID]   [DATA]                   [CheckSum] [Result]        |\n");
    printf("+-----------------------------------------------------------+\n");

    for(i=0x00; i<10; i++)
    {
        g_bWait =TRUE;
        LIN_HeaderSend(UART1,i); /* Send LIN header */
        while(g_bWait);
        LIN_ResponseSend(UART1,MODE_ENHANCED,testPattern); /* Send LIN response */
        CLK_SysTickDelay(5000); /* Wait transfer done */
    }

    printf("\nLIN Sample Demo End.\n");

    UART_DisableInt(UART1, UART_INTEN_RDAIEN_Msk|UART_INTEN_LINIEN_Msk); /* Disable interrupt */

    UART_Close(UART1); /* Disable UART */
}

/**
 *  @brief  Init system clock and I/O multi function .
 *  @param  None
 *  @return None
 */
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    //CLK->PWRCTL &= ~CLK_PWRCTL_HXT_EN_Msk;
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk; // HXT Enabled

    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk); /* Waiting for 12MHz clock ready */

    CLK->CLKSEL0 = (CLK->CLKSEL0 &~ CLK_CLKSEL0_HCLKSEL_Msk) | (CLK_CLKSEL0_HCLKSEL_HXT); /* Switch HCLK clock source to XTAL */

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; // UART0 Clock Enable
    CLK->APBCLK |= CLK_APBCLK_UART1_EN; // UART1 Clock Enable

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART0SEL_Pos);// Clock source from external 12 MHz crystal clock
    CLK->CLKSEL2 &= ~CLK_CLKSEL2_UART1SEL_Msk;
    CLK->CLKSEL2 |= (0x0 << CLK_CLKSEL2_UART1SEL_Pos);// Clock source from external 12 MHz crystal clock

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk|SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |=  (SYS_GPB_MFPL_PB0MFP_UART0_RXD|SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Set PB multi-function pins for UART1 RXD, TXD, RTS, CTS  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk |
                       SYS_GPB_MFPL_PB6MFP_Msk | SYS_GPB_MFPL_PB7MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB4MFP_UART1_RXD | SYS_GPB_MFPL_PB5MFP_UART1_TXD |
                      SYS_GPB_MFPL_PB6MFP_UART1_nRTS  | SYS_GPB_MFPL_PB7MFP_UART1_nCTS);

    /* Lock protected registers */
    SYS_LockReg();

}

/**
 *  @brief  Config UART0.
 *  @param  None
 *  @return None
 */
void UART0_Init()
{
    UART_Open(UART0, 115200); /* set UART baudrate is 115200 */
}

/**
 *  @brief  Config UART1.
 *  @param  None
 *  @return None
 */
void UART1_Init()
{
    UART_Open(UART1, 57600); /* set UART baudrate is 57600 */
}

/**
 *  @brief main function.
 *  @param[in] None
 *  @return None
 */
int32_t main()
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART0 for printf */
    UART0_Init();
    /* Init UART1 for printf */
    UART1_Init();

    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|               UART Sample Program                         |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| LIN function test                                         |\n");
    printf("+-----------------------------------------------------------+\n");

    LIN_FunctionTest();     /* LIN Function Test */

    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/



