/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/12/29 3:05p $
 * @brief    Demonstrate how to receive data in UART RS485 mode.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"
#include "uart.h"
#include "sys.h"
#include "clk.h"



#define RXBUFSIZE 512

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
extern char GetChar(void);

/**
  * @brief  UART RS485 interrupt Handle function.
  * @param  None.
  * @return None.
  */
void RS485_INT_HANDLE(void)
{
    volatile char addr;
    volatile char regRX;

    if((UART1->INTSTS & UART_INTSTS_RLSIF_Msk) && (UART1->INTSTS & UART_INTSTS_RDAIF_Msk))    /* RLS INT & RDA INT */
    {
        if((UART1->TRSR & UART_TRSR_ADDRDETF_Msk) && (UART1->FUNCSEL & 0x3))   /* ADD_IF, RS485 mode */
        {
            addr = UART1->DAT;
            UART1->TRSR |= UART_TRSR_ADDRDETF_Msk;             /* clear ADD_IF flag */
        }
    }
    else if((UART1->INTSTS & UART_INTSTS_RDAIF_Msk))     /* Rx Ready */
    {
        /* RDA INT */
        regRX = UART1->DAT;

        if(IsRS485ISR_TX_PORT)
            UART1->DAT  = regRX;  // transfer data
        else
            u8RecData[r_pointer++] = regRX;  // receive data
    }
    else if((UART1->INTSTS & UART_INTSTS_RXTOIF_Msk))     /* Rx Ready */
    {
        /* Time-out INT */
        regRX = UART1->DAT;

        if(IsRS485ISR_TX_PORT)
            UART1->DAT  = regRX;  // transfer data
        else
            u8RecData[r_pointer++] = regRX;  // receive data
    }
    else if(UART1->INTSTS & UART_INTSTS_BUFERRIF_Msk)           /* Buff Error INT */
    {
        printf("\nBuffer Error...\n");
        while(1);
    }
}

/**
 *  @brief  ISR to handle UART Channel 1 interrupt event.
 *  @param  None
 *  @return None
 */
void UART1_IRQHandler(void)
{
    if((UART1->FUNCSEL & 0x3) == 0x3)   // RS485 function
    {
        RS485_INT_HANDLE();
    }
}

/**
 *  @brief  RS485 Receive Test Function.
 *  @param  None
 *  @return None
 *  @detail Wait Data and store in u8RecData buffer
 */
void RS485_ReceiveTest()
{
    int32_t i;

    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|               RS485 Function Test                         |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| 1). Wait %4d data bytes.                                  |\n",RXBUFSIZE);
    printf("| 2). Press any key to start.                               |\n");
    printf("+-----------------------------------------------------------+\n\n");
    GetChar();
    /* variables set default value */
    IsRS485ISR_RX_PORT = TRUE;
    IsRS485ISR_TX_PORT = FALSE;
    r_pointer = 0;

    /* Set UART Configuration */
    UART_Open(UART1, 115200);
    /* Set Data Format*/ /* Only need parity enable whenever parity ODD/EVEN */
    UART_SetLine_Config(UART1, 0, UART_WORD_LEN_8, UART_PARITY_EVEN, UART_STOP_BIT_1);

    /* Set RTS pin output is high level active */
    UART1->MODEM &= ~UART_MODEM_RTSACTLV_Msk;
    /* Set RS485 Configuration */
    UART_SelectRS485Mode(UART1, UART_ALTCTL_RS485AAD_Msk|UART_ALTCTL_RS485AUD_Msk|UART_ALTCTL_ADDRDEN_Msk, 0xC0); // Set AAD, AUD mode and Address

    r_pointer = 0;

    /* Check Rx empty, otherwise read Rx */
    printf("Starting to receive %d bytes data...\n", RXBUFSIZE);

    UART_EnableInt(UART1, UART_INTEN_RLSIEN_Msk|UART_INTEN_RDAIEN_Msk|UART_INTEN_RXTOIEN_Msk|UART_INTEN_BUFERRIEN_Msk);
    NVIC_EnableIRQ(UART1_IRQn);

    while(r_pointer<(RXBUFSIZE-1));

    /* Compare Data */
    for(i=0; i<(RXBUFSIZE-1); i++)
    {
        if(u8RecData[i] != ((i+1)&0xFF) )
        {
            printf("Compare Data Failed\n");
        }
    }
    printf("\n Receive OK & Check OK\n");
    printf(" Press Any key to end this test \n");
    GetChar();

    IsRS485ISR_RX_PORT = FALSE;

    /* Disable UART */
    UART_Close(UART1);

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

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 &~ CLK_CLKSEL0_HCLKSEL_Msk) | (CLK_CLKSEL0_HCLKSEL_HXT);

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
    /* set uart baudrate is 115200 */
    UART_Open(UART0, 115200);
}

/**
 *  @brief  Config UART1.
 *  @param  None
 *  @return None
 */
void UART1_Init()
{
    /* set uart baudrate is 57600 */
    UART_Open(UART1, 57600);
}

/**
 *  @brief  main function.
 *  @param  None
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
    printf("| RS485 Receive function test                                |\n");
    printf("+-----------------------------------------------------------+\n");

    /* RS485 Receive Data demo code */
    RS485_ReceiveTest();    /* RS485 Receive Test (Need two module board to test) */

    while(1);
}


#ifdef USE_ASSERT
/**
  * @brief  The function prints the source file name and line number where the assert_param() error
  *         occurs, and then stops in an infinite loop. User can add his own codes here if necessary.
  * @param[in] file Source file name
  * @param[in] line Line number
  * @return None
  */
void assert_error(uint8_t * file, uint32_t line)
{
    GCR->PB_L_MFP = (GCR->PB_L_MFP & ~0x77) | (PB1_MFP_UART0_TX | PB0_MFP_UART0_RX);  /* Select multi-function pin for UART0 */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN;    /* Enable UART0 clock */
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_UART_MASK;
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART_MASK) | CLK_CLKSEL1_UART_HXT;  /* Select 12 MHz XTAL */


    /* Set UART to 115200,n,8,1,none */
    UART0->BAUD = 0x67;             /* Baud Rate:115200 for 12MHz */
    UART0->TLCTL = 0x03;            /* Word len is 8 bits */

    printf("[%s] line %d : wrong parameters.\r\n", file, line);

    /* Infinite loop */
    while(1) ;

}
#endif


/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/



