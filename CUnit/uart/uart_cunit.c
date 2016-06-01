/*---------------------------------------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (c) 2007 by Winbond Electronics Corporation                                                   */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                          */
/* Project:   DA8205                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Nano103.h"
#include "CUnit.h"
#include "Console.h"
#include "uart_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

extern unsigned int STATE0; 
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void) { return 0; }
int suite_success_clean(void) { return 0; }



/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

CU_SuiteInfo suites[] =
{
    {"UART API", suite_success_init, suite_success_clean,NULL, NULL, UART_ApiTests},
    {"UART MACRO", suite_success_init, suite_success_clean,NULL, NULL, UART_MacroTests},
    CU_SUITE_INFO_NULL
};



#define UartChNum 2
UART_T *UartCh[UartChNum] = {
    UART0, UART1  
};



void UART1_IRQHandler()
{
    
}

void UART0_IRQHandler()
{
    
}

void TestFunc_UART_EnableFlowCtrl() 
{   
    uint8_t u8UartChIdx;    //UART channel index    
    
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {
        UART_EnableFlowCtrl( UartCh[u8UartChIdx] );  
        CU_ASSERT( (UartCh[u8UartChIdx]->CTRL & (UART_CTRL_ATORTSEN_Msk|UART_CTRL_ATOCTSEN_Msk))  
                                            == (UART_CTRL_ATORTSEN_Msk|UART_CTRL_ATOCTSEN_Msk) ); 
        CU_ASSERT( (UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSACTLV_Msk) == UART_MODEM_RTSACTLV_Msk );     
        CU_ASSERT( (UartCh[u8UartChIdx]->MODEM & UART_MODEM_CTSACTLV_Msk) == UART_MODEM_CTSACTLV_Msk );          
        UART_DisableFlowCtrl( UartCh[u8UartChIdx] ); 
        CU_ASSERT( (UartCh[u8UartChIdx]->CTRL & (UART_CTRL_ATORTSEN_Msk|UART_CTRL_ATOCTSEN_Msk)) == 0 );    
    }       
}


#define UartINTEnSelNum 10
const uint32_t au32UartINTEnSel[UartINTEnSelNum] = {
    UART_INTEN_RDAIEN_Msk,       //0 : Receive data available interrupt
    UART_INTEN_THREIEN_Msk,      //1 : Transmit holding register empty interrupt
    UART_INTEN_RLSIEN_Msk,       //2 : Receive line status interrupt  
    UART_INTEN_MODEMIEN_Msk,     //3 : Modem status interrupt
    UART_INTEN_RXTOIEN_Msk,      //4 : Rx time-out interrupt    
    UART_INTEN_BUFERRIEN_Msk,   //5 : Buffer error interrupt
    UART_INTEN_WKUPIEN_Msk,       //6 : Wake-up interrupt
	UART_INTEN_ABRIEN_Msk,
    UART_INTEN_LINIEN_Msk,       //8 : LIN bus interrupt 
	UART_INTEN_TXENDIEN_Msk,
};

void TestFunc_UART_EnableInt() 
{   
    uint8_t u8UartChIdx;    //UART channel index   
    uint8_t u8UartINTEnIdx; //bit
    
    /* test loop */
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {    
        //enable interrupt
        for( u8UartINTEnIdx=0; u8UartINTEnIdx<UartINTEnSelNum; u8UartINTEnIdx++)
        {
            //skip THRE interrupt
            if( u8UartINTEnIdx==1 ) continue;
        
            UART_EnableInt( UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx] );     
            CU_ASSERT( (UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) 
                                                == au32UartINTEnSel[u8UartINTEnIdx] ); 
        }
    
        //disable interrupt
        for( u8UartINTEnIdx=0; u8UartINTEnIdx<UartINTEnSelNum; u8UartINTEnIdx++)
        {       
            UART_DisableInt( UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx] );    
            CU_ASSERT( (UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) == 0 ); 
        } 
    }        
}

uint32_t GetUartBaudrate(UART_T* uart)
{
    uint8_t u8UartClkSrcSel, u8UartClkDivNum;
    //uint32_t u32ClkTbl[4] = {__HXT, __LXT, 0, __HIRC12M};
    uint32_t u32Baud_Div;
	uint32_t clk = 0;

	if(uart == UART0)
	{
		/* Get UART clock source selection */
		u8UartClkSrcSel = (CLK->CLKSEL1 & CLK_CLKSEL1_UART0SEL_Msk) >> CLK_CLKSEL1_UART0SEL_Pos;

		/* Get UART clock divider number */
		u8UartClkDivNum = (CLK->CLKDIV0 & CLK_CLKDIV0_UART0DIV_Msk) >> CLK_CLKDIV0_UART0DIV_Pos;
	}
	else
	{
		/* Get UART clock source selection */
		u8UartClkSrcSel = (CLK->CLKSEL2 & CLK_CLKSEL2_UART1SEL_Msk) >> CLK_CLKSEL2_UART1SEL_Pos;

		/* Get UART clock divider number */
		u8UartClkDivNum = (CLK->CLKDIV0 & CLK_CLKDIV0_UART1DIV_Msk) >> CLK_CLKDIV0_UART1DIV_Pos;
	}
		
	switch (u8UartClkSrcSel) {
    case 0:
        clk = __HXT; /* HXT */
        break;
    case 1:
        clk = __LXT;  /* LXT */
        break;
    case 2:
        clk = SysGet_PLLClockFreq(); /* PLL */
        break;
    case 3:
		if(CLK->CLKSEL0 & CLK_CLKSEL0_HIRCSEL_Msk)
			clk = __HIRC36M; /* HIRC */
		else
		{
			if(CLK->PWRCTL & CLK_PWRCTL_HIRC0FSEL_Msk)
				clk = __HIRC16M;
			else
				clk = __HIRC12M;
		}
        break;
		
	default:
		clk = __MIRC;
		break;		
		
    }

    /* Get UART baud rate divider */
    u32Baud_Div = (uart->BAUD & UART_BAUD_BRD_Msk) >> UART_BAUD_BRD_Pos;

    /* Calculate UART baud rate if baud rate is set in MODE 0 */
    if((uart->BAUD & UART_BAUD_DIV16EN_Msk) == UART_BAUD_MODE0)
        return (clk / (u8UartClkDivNum + 1) / (u32Baud_Div + 1));

    /* Calculate UART baud rate if baud rate is set in MODE 1 */
    else if((uart->BAUD & UART_BAUD_DIV16EN_Msk) == UART_BAUD_MODE1)
        return (clk / (u8UartClkDivNum + 1) / (u32Baud_Div + 1)) >> 4;

    /* Unsupported baud rate setting */
    else
        return 0;
}

const uint32_t au32UartBRSel[11]={ 
    2400, 
    4800, 
    9600, 
    14400, 
    19200, 
    38400,
    57600, 
    115200, 
    230400, 
    460800, 
    921600
};

const uint32_t au32UartClkSel[5]={ 
    CLK_CLKSEL1_UART0SEL_HXT, 
    CLK_CLKSEL1_UART0SEL_LXT, 
    CLK_CLKSEL1_UART0SEL_PLL,
    CLK_CLKSEL1_UART0SEL_HIRC,
	CLK_CLKSEL1_UART0SEL_MIRC,
};
    

void TestFunc_UART_Open() 
{
    uint8_t u8UartChIdx;    //UART channel index           
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index    
    uint8_t u8UartDivIdx;   //clock divider index     
    uint32_t u32ClkTbl[5] = {__HXT, __LXT, 0, __HIRC12M, __MIRC}; 
                                 
    /* Get PLL frequency */
    u32ClkTbl[2] = CLK_GetPLLClockFreq();    
    
	if(CLK->CLKSEL0 & CLK_CLKSEL0_HIRCSEL_Msk)
		u32ClkTbl[3] = __HIRC36M; /* HIRC 36M Hz*/
	else
	{
		if(CLK->PWRCTL & CLK_PWRCTL_HIRC0FSEL_Msk)
			u32ClkTbl[3] = __HIRC16M; /* HIRC 16M Hz*/
		else
			u32ClkTbl[3] = __HIRC12M; /* HIRC 12M Hz*/
	}

    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0);    
       
    /* Test loop */
    //select UART cahnnel
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        //select UART baud rate
        for( u8UartBRIdx=0; u8UartBRIdx<8; u8UartBRIdx++)
        {
            //select UART clock source
            for( u8UartClkIdx=0; u8UartClkIdx<5; u8UartClkIdx++)
            {
                //skip reserved clock source case
                //if(u8UartClkIdx==2) continue;
                
                //select UART clock divider
                for( u8UartDivIdx=0; u8UartDivIdx<16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if( u32ClkTbl[u8UartClkIdx]<8*(au32UartBRSel[u8UartBRIdx])*(u8UartDivIdx+1) ) continue;

                    //select UART clock source
					if(UartCh[u8UartChIdx] == UART0)
					{
                    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART0DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART0DIV_Pos); //select UART clock divider
					}
					else if (UartCh[u8UartChIdx] == UART1)
					{
						CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_UART1SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART1DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART1DIV_Pos); //select UART clock divider
					}
            
                    //test function      
                    UART_Open( UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx] );
             
                    //check 
                    CU_ASSERT( UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_UART );    
                    CU_ASSERT( (UartCh[u8UartChIdx]->LINE & (UART_WORD_LEN_8|UART_PARITY_NONE|UART_STOP_BIT_1)) == (UART_WORD_LEN_8|UART_PARITY_NONE|UART_STOP_BIT_1) );
                    CU_ASSERT( (UartCh[u8UartChIdx]->LINE & (UART_LINE_RFITL_Msk|UART_LINE_RTSTRGLV_Msk)) == 0 );                  
                    CU_ASSERT( GetUartBaudrate(UartCh[u8UartChIdx])>(((au32UartBRSel[u8UartBRIdx])*90)/100) &&
                               GetUartBaudrate(UartCh[u8UartChIdx])<(((au32UartBRSel[u8UartBRIdx])*110)/100) ); 
                }  
            }
        }        
    }

    //restore UART debug port setting for printf 
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_HIRC;
    CLK->CLKDIV0 &= (~CLK_CLKDIV0_UART0DIV_Msk);
	CLK->CLKDIV1 &= (~CLK_CLKDIV0_UART1DIV_Msk);
}

const uint32_t au32UartWordSel[4]={ 
    UART_WORD_LEN_5, 
    UART_WORD_LEN_6, 
    UART_WORD_LEN_7,
    UART_WORD_LEN_8,
};

const uint32_t au32UartParitySel[5]={ 
    UART_PARITY_NONE, 
    UART_PARITY_ODD, 
    UART_PARITY_EVEN,
    UART_PARITY_MARK,
    UART_PARITY_SPACE,    
};

const uint32_t au32UartStopSel[2]={ 
    UART_STOP_BIT_1, 
    UART_STOP_BIT_2, 
};

void TestFunc_UART_SetLine_Config() 
{
    //variable for baud rate 
    uint8_t u8UartChIdx;    //channel index           
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index    
    uint8_t u8UartDivIdx;   //clock divider index     
    uint32_t u32ClkTbl[5] = {__HXT, __LXT, 0, __HIRC12M, __MIRC};      
 
    //variable for data format 
    uint8_t u8UartWordIdx;      //word length index  
    uint8_t u8UartParityIdx;    //parity index  
    uint8_t u8UartStopIdx;      //stop bit length index 

    /* Get PLL frequency */
    u32ClkTbl[2] = CLK_GetPLLClockFreq();    
    
	if(CLK->CLKSEL0 & CLK_CLKSEL0_HIRCSEL_Msk)
		u32ClkTbl[3] = __HIRC36M; /* HIRC 36M Hz*/
	else
	{
		if(CLK->PWRCTL & CLK_PWRCTL_HIRC0FSEL_Msk)
			u32ClkTbl[3] = __HIRC16M; /* HIRC 16M Hz*/
		else
			u32ClkTbl[3] = __HIRC12M; /* HIRC 12M Hz*/
	}    
    
    //wait UART send message finish before change clock
    //UART_WAIT_TX_EMPTY(UART0);  
       
    /* Test loop */
    //select UART cahnnel
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        //select UART baud rate
        for( u8UartBRIdx=0; u8UartBRIdx<8; u8UartBRIdx++)
        {
            //select UART clock source
            for( u8UartClkIdx=0; u8UartClkIdx<5; u8UartClkIdx++)
            {
                //skip reserved clock souce case
                //if(u8UartClkIdx==2) continue;
                
                //select UART clock divider
                for( u8UartDivIdx=0; u8UartDivIdx<16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if( u32ClkTbl[u8UartClkIdx]<8*(au32UartBRSel[u8UartBRIdx])*(u8UartDivIdx+1) ) continue;

                    //select UART word length
                    for( u8UartWordIdx=0; u8UartWordIdx<4; u8UartWordIdx++)
                    {
                        //select UART parity
                        for( u8UartParityIdx=0; u8UartParityIdx<5; u8UartParityIdx++)   
                        {   
                            //select UART stop bit length
                            for( u8UartStopIdx=0; u8UartStopIdx<2; u8UartStopIdx++)   
                            {                          
                                //select UART clock source
								if(UartCh[u8UartChIdx] == UART0)
								{
                                CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
									CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART0DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART0DIV_Pos); //select UART clock divider
								}
								else if (UartCh[u8UartChIdx] == UART1)
								{
									CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_UART1SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
									CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART1DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART1DIV_Pos); //select UART clock divider
								}

                                //test function      
                                UART_SetLine_Config( UartCh[u8UartChIdx],                  //select UART cahnnel 
                                                     au32UartBRSel[u8UartBRIdx],           //select UART baud rate 
                                                     au32UartWordSel[u8UartWordIdx],       //select UART word length 
                                                     au32UartParitySel[u8UartParityIdx],   //select UART parity 
                                                     au32UartStopSel[u8UartStopIdx] );     //select UART stop bit length                
                                //check     
                                CU_ASSERT( (UartCh[u8UartChIdx]->LINE &(au32UartWordSel[u8UartWordIdx] | au32UartParitySel[u8UartParityIdx] | au32UartStopSel[u8UartStopIdx])) == 
								           (au32UartWordSel[u8UartWordIdx] | au32UartParitySel[u8UartParityIdx] | au32UartStopSel[u8UartStopIdx]) );          
                                CU_ASSERT( GetUartBaudrate(UartCh[u8UartChIdx])>(((au32UartBRSel[u8UartBRIdx])*90)/100) &&
                                           GetUartBaudrate(UartCh[u8UartChIdx])<(((au32UartBRSel[u8UartBRIdx])*110)/100) ); 
                                
                            } //u8UartStopIdx  
                        } //u8UartParityIdx    
                    } //u8UartWordIdx                    
                } //u8UartDivIdx  
            } //u8UartClkIdx
        } //u8UartBRIdx        
    } //u8UartChIdx
    
    //restore UART debug port setting for printf 
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_HIRC;
    CLK->CLKDIV0 &= (~CLK_CLKDIV0_UART0DIV_Msk);
    
    //restore UART setting for other test
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8|UART_PARITY_NONE|UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER( 12000000, 115200);    
    }
}

void TestFunc_UART_SetTimeoutCnt()
{
    uint8_t u8UartChIdx;        //channel index    
    uint16_t u16UartToutCnt;    //time out counter    
      
    /* Test loop */
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {      
        for( u16UartToutCnt=0; u16UartToutCnt<512; u16UartToutCnt++)
        {
            UART_SetTimeoutCnt( UartCh[u8UartChIdx], u16UartToutCnt);
            CU_ASSERT( (UartCh[u8UartChIdx]->INTEN & UART_INTEN_RXTOIEN_Msk) == UART_INTEN_RXTOIEN_Msk );    
            CU_ASSERT( (UartCh[u8UartChIdx]->TOUT & UART_TOUT_TOIC_Msk) == (u16UartToutCnt<<UART_TOUT_TOIC_Pos) );    
        } 
    }        
}

void TestFunc_UART_SelectIrDAMode()
{
    uint8_t u8UartChIdx;    //UART channel index           
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index    
    uint8_t u8UartDivIdx;   //clock divider index     
    uint32_t u32ClkTbl[5] = {__HXT, __LXT, 0, __HIRC12M, __MIRC}; 
    
    /* Get PLL frequency */
    u32ClkTbl[2] = CLK_GetPLLClockFreq();
                                 
	if(CLK->CLKSEL0 & CLK_CLKSEL0_HIRCSEL_Msk)
		u32ClkTbl[3] = __HIRC36M; /* HIRC 36M Hz*/
	else
	{
		if(CLK->PWRCTL & CLK_PWRCTL_HIRC0FSEL_Msk)
			u32ClkTbl[3] = __HIRC16M; /* HIRC 16M Hz*/
		else
			u32ClkTbl[3] = __HIRC12M; /* HIRC 12M Hz*/
	} 
                                 
    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0);    
       
    /* Test loop */
    //select UART cahnnel
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        //select UART baud rate
        for( u8UartBRIdx=2; u8UartBRIdx<3; u8UartBRIdx++)
        {
            //select UART clock source
            for( u8UartClkIdx=0; u8UartClkIdx<1; u8UartClkIdx++)
            {
                //skip reserved clock souce case
                //if(u8UartClkIdx==2) continue;
                
                //select UART clock divider
                for( u8UartDivIdx=0; u8UartDivIdx<16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 0 case in mode 0
                    if( u32ClkTbl[u8UartClkIdx]<(1*(au32UartBRSel[u8UartBRIdx])*(u8UartDivIdx+1))<<4 ) continue;

                    //select UART clock source
					if(UartCh[u8UartChIdx] == UART0)
					{
                    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART0DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART0DIV_Pos); //select UART clock divider
					}
					else if (UartCh[u8UartChIdx] == UART1)
					{
						CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_UART1SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART1DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART1DIV_Pos); //select UART clock divider
					}
            
                    //test function
                    //UART_SelectIrDAMode( UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx], UART_IRCR_TX_SELECT );
					UART_SelectIrDAMode( UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx], 1 ); // Tx Select
                    CU_ASSERT( UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_IrDA );  
                    CU_ASSERT( (UartCh[u8UartChIdx]->IRDA & UART_IRDA_TXEN_Msk) == UART_IRDA_TXEN_Msk );
                    CU_ASSERT( (UartCh[u8UartChIdx]->IRDA & UART_IRDA_TXINV_Msk) == 0 );      
                    CU_ASSERT( GetUartBaudrate(UartCh[u8UartChIdx])>(((au32UartBRSel[u8UartBRIdx])*90)/100) &&
                               GetUartBaudrate(UartCh[u8UartChIdx])<(((au32UartBRSel[u8UartBRIdx])*110)/100) ); 
                
                    //UART_SelectIrDAMode( UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx], UART_IRCR_RX_SELECT );
					UART_SelectIrDAMode( UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx], 0 ); // RX Select
                    CU_ASSERT( UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_IrDA );  
                    CU_ASSERT( (UartCh[u8UartChIdx]->IRDA & UART_IRDA_TXEN_Msk) == 0 );
                    CU_ASSERT( (UartCh[u8UartChIdx]->IRDA & UART_IRDA_RXINV_Msk) == UART_IRDA_RXINV_Msk );                            
                    CU_ASSERT( GetUartBaudrate(UartCh[u8UartChIdx])>(((au32UartBRSel[u8UartBRIdx])*90)/100) &&
                               GetUartBaudrate(UartCh[u8UartChIdx])<(((au32UartBRSel[u8UartBRIdx])*110)/100) ); 
                               
                 }  
            }
        }        
    }
    
    //restore UART debug port setting for printf 
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_HIRC;
    CLK->CLKDIV0 &= (~CLK_CLKDIV0_UART0DIV_Msk);

    //restore UART setting for other test
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8|UART_PARITY_NONE|UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER( 12000000, 115200);    
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART; 
    }
}

void TestFunc_UART_SelectRS485Mode()
{
    uint8_t u8UartChIdx;        //UART channel index   
    uint8_t u8RS485ModeIdx;     //RS485 mode   
    uint16_t u16RS485AddrIdx;   //RS485 address  
    
    /* Test loop */
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {
        for( u8RS485ModeIdx=0; u8RS485ModeIdx<8; u8RS485ModeIdx++)
        {
            //skip AAD and NMM set at the same time case
            if( u8RS485ModeIdx==3 || u8RS485ModeIdx==7 ) continue; 

            for( u16RS485AddrIdx=0; u16RS485AddrIdx<256; u16RS485AddrIdx++)
            {
                UART_SelectRS485Mode( UartCh[u8UartChIdx], (u8RS485ModeIdx<<UART_ALTCTL_RS485NMM_Pos), u16RS485AddrIdx );
                CU_ASSERT( UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_RS485 );  
                CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & (UART_ALTCTL_RS485NMM_Msk|UART_ALTCTL_RS485AAD_Msk|UART_ALTCTL_RS485AUD_Msk)) 
                                          == (u8RS485ModeIdx<<UART_ALTCTL_RS485NMM_Pos) ); 
                CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_ADRMPID_Msk) == (u16RS485AddrIdx<<UART_ALTCTL_ADRMPID_Pos) );                    
            }            
        }
    }        
    
    //restore UART setting for other test
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8|UART_PARITY_NONE|UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER( 12000000, 115200);    
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART; 
        UartCh[u8UartChIdx]->ALTCTL &= ~(UART_ALTCTL_RS485NMM_Msk|UART_ALTCTL_RS485AAD_Msk|UART_ALTCTL_RS485AUD_Msk);   
    } 
}

void TestFunc_UART_TestMacroBR()
{
    uint8_t u8UartChIdx;    //UART channel index           
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index    
    uint8_t u8UartDivIdx;   //clock divider index     
	uint32_t u32Baud_Div = 0; 
    uint32_t u32ClkTbl[5] = {__HXT, __LXT, 0, __HIRC12M, __MIRC};  
    
    /* Get PLL frequency */
    u32ClkTbl[2] = CLK_GetPLLClockFreq();
                                 
	if(CLK->CLKSEL0 & CLK_CLKSEL0_HIRCSEL_Msk)
		u32ClkTbl[3] = __HIRC36M; /* HIRC 36M Hz*/
	else
	{
		if(CLK->PWRCTL & CLK_PWRCTL_HIRC0FSEL_Msk)
			u32ClkTbl[3] = __HIRC16M; /* HIRC 16M Hz*/
		else
			u32ClkTbl[3] = __HIRC12M; /* HIRC 12M Hz*/
	} 
                                 
    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0);    
       
    /* Test macro: UART_BAUD_MODE0_DIVIDER */ 
    //select UART cahnnel
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)  
    {
        //select UART baud rate
        for( u8UartBRIdx=7; u8UartBRIdx<8; u8UartBRIdx++)
        {
            //select UART clock source
            for( u8UartClkIdx=0; u8UartClkIdx<5; u8UartClkIdx++)
            {
                //skip reserved clock souce case
                //if(u8UartClkIdx==1) continue;
                
                //select UART clock divider
                for( u8UartDivIdx=0; u8UartDivIdx<16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 0 case in mode 0
                    if( u32ClkTbl[u8UartClkIdx]<(8*(au32UartBRSel[u8UartBRIdx])*(u8UartDivIdx+1)) ) continue;
                   
                    //select UART clock source
					if(UartCh[u8UartChIdx] == UART0)
					{
                    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART0DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART0DIV_Pos); //select UART clock divider
					}
					else if (UartCh[u8UartChIdx] == UART1)
					{
						CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_UART1SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART1DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART1DIV_Pos); //select UART clock divider
					}
        
					u32Baud_Div = UART_BAUD_MODE0_DIVIDER( ((u32ClkTbl[u8UartClkIdx])/(u8UartDivIdx+1)), au32UartBRSel[u8UartBRIdx] );
					if( u32Baud_Div > 0xFFFF ) continue;
					if( u32Baud_Div < 8 ) continue;
            
                    /* Test macro: UART_BAUD_MODE0_DIVIDER */    
                    UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE0 | u32Baud_Div;   
                    CU_ASSERT( GetUartBaudrate(UartCh[u8UartChIdx])>(((au32UartBRSel[u8UartBRIdx])*90)/100) &&
                               GetUartBaudrate(UartCh[u8UartChIdx])<(((au32UartBRSel[u8UartBRIdx])*110)/100) ); 

                } //u8UartDivIdx
            } //u8UartClkIdx
        } //u8UartBRIdx
    } //u8UartChIdx
    
    /* Test macro: UART_BAUD_MODE1_DIVIDER */ 
    //select UART cahnnel
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)  
    {
        //select UART baud rate
        for( u8UartBRIdx=2; u8UartBRIdx<3; u8UartBRIdx++)
        {
            //select UART clock source
            for( u8UartClkIdx=0; u8UartClkIdx<5; u8UartClkIdx++)
            {
                //skip reserved clock souce case
                if(u8UartClkIdx==1) continue;
                
                //select UART clock divider
                for( u8UartDivIdx=0; u8UartDivIdx<16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if( u32ClkTbl[u8UartClkIdx]<1*(au32UartBRSel[u8UartBRIdx])*(u8UartDivIdx+1) << 4 ) continue;
                   
                    //select UART clock source
					if(UartCh[u8UartChIdx] == UART0)
					{
                    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART0DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART0DIV_Pos); //select UART clock divider
					}
					else if (UartCh[u8UartChIdx] == UART1)
					{
						CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_UART1SEL_Msk)) | au32UartClkSel[u8UartClkIdx];
						CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_UART1DIV_Msk)) | (u8UartDivIdx<<CLK_CLKDIV0_UART1DIV_Pos); //select UART clock divider
					}
        
					u32Baud_Div = UART_BAUD_MODE1_DIVIDER( ((u32ClkTbl[u8UartClkIdx])/(u8UartDivIdx+1)), au32UartBRSel[u8UartBRIdx] );
					if( u32Baud_Div > 0xFFFF ) continue;

                    //test function     
                    UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE1 | UART_BAUD_MODE1_DIVIDER( ((u32ClkTbl[u8UartClkIdx])/(u8UartDivIdx+1)), au32UartBRSel[u8UartBRIdx] );   
                    CU_ASSERT( GetUartBaudrate(UartCh[u8UartChIdx])>(((au32UartBRSel[u8UartBRIdx])*80)/100) &&
                               GetUartBaudrate(UartCh[u8UartChIdx])<(((au32UartBRSel[u8UartBRIdx])*120)/100) );                     
                    
                } //u8UartDivIdx
            } //u8UartClkIdx
        } //u8UartBRIdx
    } //u8UartChIdx    

    //restore UART debug port setting for printf 
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKDIV0 &= (~CLK_CLKDIV0_UART0DIV_Msk);

    //restore UART setting for other test
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8|UART_PARITY_NONE|UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER( 12000000, 115200);    
    } 
}

void TestFunc_UART_TestMacroFIFO()
{
    uint8_t u8UartChIdx;    //UART channel index  
    uint8_t i;  //number control    
    
    //wait UART send message finish 
    UART_WAIT_TX_EMPTY(UART0);       
    
    /* Set UART1 internal loopback mode */
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        UartCh[u8UartChIdx]->MODEM |= (0x80000000); // loop back mode
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8|UART_PARITY_NONE|UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER( 12000000, 115200);

        /* Reset Tx/Rx FIFO */
        UartCh[u8UartChIdx]->CTRL |= 0x3; 
        while( UartCh[u8UartChIdx]->CTRL & 0x3 );         
    }     

    /* Test macro : UART_WRITE
                    UART_READ
                    UART_GET_TX_EMPTY
                    UART_GET_RX_EMPTY
                    UART_IS_TX_EMPTY
                    UART_WAIT_TX_EMPTY
                    UART_IS_RX_READY
                    UART_IS_TX_FULL
                    UART_IS_RX_FULL
                    UART_GET_TX_FULL
                    UART_GET_RX_FULL */
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {  
        //check initaial state 
        CU_ASSERT( UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXEMPTY_Msk);   
        CU_ASSERT( UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXEMPTY_Msk);      
        CU_ASSERT( UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 1);    
        UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]);   
        CU_ASSERT( UART_IS_RX_READY(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_GET_TX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_GET_RX_FULL(UartCh[u8UartChIdx]) == 0);         
    
        //check Tx full state 
        //set CTS high level active
        UartCh[u8UartChIdx]->MODEM |= UART_MODEM_CTSACTLV_Msk; 
		//set RTS low level active
		UartCh[u8UartChIdx]->MODEM &= ~UART_MODEM_RTSACTLV_Msk;
        //enable CTS/RTS auto flow control 
        UartCh[u8UartChIdx]->CTRL |= UART_CTRL_ATOCTSEN_Msk;
		UartCh[u8UartChIdx]->CTRL |= UART_CTRL_ATORTSEN_Msk;
        //send data to let Tx full
        for( i=0; i<17; i++ ) // for nano // Tx FIFO need equal 16, then Tx full flag will set 1
        {
            UART_WRITE( UartCh[u8UartChIdx], (0x55+i) );
        }   
		
		while(!(UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_TXFULL_Msk));
		
        //check macro
        CU_ASSERT( UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == 0);   
        CU_ASSERT( UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXEMPTY_Msk);      
        CU_ASSERT( UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 0);      
        CU_ASSERT( UART_IS_RX_READY(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 1); 
        CU_ASSERT( UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 0);  
        CU_ASSERT( UART_GET_TX_FULL(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXFULL_Msk); 
        CU_ASSERT( UART_GET_RX_FULL(UartCh[u8UartChIdx]) == 0);       

        //check Rx full state 
        //set RTS high level active
		UartCh[u8UartChIdx]->MODEM |= UART_MODEM_RTSACTLV_Msk; 
        //wait Tx send finished
        UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]);       
        //check macro
        CU_ASSERT( UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXEMPTY_Msk);   
        CU_ASSERT( UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == 0);      
        CU_ASSERT( UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 1);      
        CU_ASSERT( UART_IS_RX_READY(UartCh[u8UartChIdx]) == 1); 
        CU_ASSERT( UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 1);   
        CU_ASSERT( UART_GET_TX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_GET_RX_FULL(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXFULL_Msk);          
        
        //read Rx empty      
        for( i=0; i<17; i++ )
        {
            UART_READ(UartCh[u8UartChIdx]);
        }      
        //check macro
        CU_ASSERT( UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXEMPTY_Msk);   
        CU_ASSERT( UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXEMPTY_Msk);      
        CU_ASSERT( UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 1);   
        UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]);
		while(UART_IS_RX_READY(UartCh[u8UartChIdx]));
        CU_ASSERT( UART_IS_RX_READY(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_GET_TX_FULL(UartCh[u8UartChIdx]) == 0); 
        CU_ASSERT( UART_GET_RX_FULL(UartCh[u8UartChIdx]) == 0);                
    }
     
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {          
        //disable CTS auto flow control after test
        UartCh[u8UartChIdx]->CTRL &= ~UART_CTRL_ATOCTSEN_Msk;   
        //disable UART1 internal loopback mode after test */
        UartCh[u8UartChIdx]->MODEM &= ~(0x80000000);
    }        
}

void TestFunc_UART_TestMacroRTS()
{
    uint8_t u8UartChIdx;    //UART channel index   
    
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    { 
        /* Test macro : UART_CLEAR_RTS
                        UART_SET_RTS */ 
        //disable RTS auto flow control
        UartCh[u8UartChIdx]->CTRL &= ~UART_CTRL_ATORTSEN_Msk;      
        UART_SET_RTS(UartCh[u8UartChIdx]);
        CU_ASSERT( (UartCh[u8UartChIdx]->MODEM & (UART_MODEM_RTSSTS_Msk|UART_MODEM_RTSACTLV_Msk)) == (UART_MODEM_RTSSTS_Msk) );     
        UART_CLEAR_RTS(UartCh[u8UartChIdx]);
        CU_ASSERT( (UartCh[u8UartChIdx]->MODEM & (UART_MODEM_RTSSTS_Msk|UART_MODEM_RTSACTLV_Msk)) == UART_MODEM_RTSACTLV_Msk );         
    }  
}

void TestFunc_UART_TestMacroINT()
{
    uint8_t u8UartChIdx;    //UART channel index  
    uint8_t u8UartINTEnIdx; //bit 
    
    //wait UART send message finish 
    //UART_WAIT_TX_EMPTY(UART0);      
    
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    { 
        for( u8UartINTEnIdx=0; u8UartINTEnIdx<UartINTEnSelNum; u8UartINTEnIdx++)
        { 
            /* Test macro : UART_ENABLE_INT
                            UART_DISABLE_INT */  
            //Enable bit in IER register
            //0 : Receive data available interrupt
            //1 : Transmit holding register empty interrupt
            //2 : Receive line status interrupt   
            //3 : Modem status interrupt
            //4 : Rx time-out interrupt    
            //5 : Buffer error interrupt
            //6 : Wake-up interrupt         
            //7 : LIN bus interrupt                 
            UART_ENABLE_INT( UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx] );     
            CU_ASSERT( (UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) == au32UartINTEnSel[u8UartINTEnIdx] ); 
            UART_DISABLE_INT( UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx] );     
            CU_ASSERT( (UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) == 0 );  
        }            
    }        
    
    for( u8UartChIdx=0; u8UartChIdx<2; u8UartChIdx++)
    { 
        /* Test macro : UART_GET_INT_FLAG */ 
        
		UartCh[u8UartChIdx]->CTRL |= 0x3;
		while(UartCh[u8UartChIdx]->CTRL & 0x3);
		
        //enable UART1 internal loopback 
        UartCh[u8UartChIdx]->MODEM |= 0x80000000;    
       
        //0 : Receive data available interrupt      
        //1 : Transmit holding register empty interrupt  
        // set CTS high level active
        UartCh[u8UartChIdx]->MODEM |= UART_MODEM_CTSACTLV_Msk;   
		// set RTS low level active
        UartCh[u8UartChIdx]->MODEM &= ~UART_MODEM_RTSACTLV_Msk;  
        // enable CTS/RTS auto flow control 
        UartCh[u8UartChIdx]->CTRL |= UART_CTRL_ATOCTSEN_Msk; 
		UartCh[u8UartChIdx]->CTRL |= UART_CTRL_ATORTSEN_Msk;
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_THREIF_Msk) == 1 );
        // fill Tx 
        UART_WRITE( UartCh[u8UartChIdx], 0x55 );
        // check macro    
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_THREIF_Msk) == 0 ); 
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RDAIF_Msk) == 0 );     
        // set RTS high level active to release data in Tx FIFO
        UartCh[u8UartChIdx]->MODEM |= UART_MODEM_RTSACTLV_Msk; 
        // wait data send out
        while( (UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_TXENDF_Msk) == 0 );
        // check macro    
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_THREIF_Msk) == 1 );
        // wait data receive
        while( (UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == UART_FIFOSTS_RXEMPTY_Msk );    
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RDAIF_Msk) == 1 );        
        // read Rx 
        UART_READ( UartCh[u8UartChIdx] );    
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RDAIF_Msk) == 0 );
        // disable CTS auto flow control after test
        UartCh[u8UartChIdx]->CTRL &= ~UART_CTRL_ATOCTSEN_Msk;  

        //2 : Receive line status interrupt 
        // check macro    
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 0 );   
        // send break 
        UartCh[u8UartChIdx]->LINE |= UART_LINE_BCB_Msk;
        // wait break error happen
        while( (UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_BIF_Msk) == 0 );      
        // check macro      
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 1 );
        // stop send break 
        UartCh[u8UartChIdx]->LINE &= ~UART_LINE_BCB_Msk;
        // clear receive line status flag
        UART_ClearIntFlag( UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk );
        // check macro      
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 0 ); 
        // clear Rx FIFO // Rx Rest
        UartCh[u8UartChIdx]->CTRL |= UART_CTRL_RXRST_Msk;
        while( (UartCh[u8UartChIdx]->CTRL & UART_CTRL_RXRST_Msk) == UART_CTRL_RXRST_Msk );
      
        //3 : Modem status interrupt
		UartCh[u8UartChIdx]->MODEM = 0; // Let modem status change
		
        // check macro    
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_MODEMIF_Msk) == 1 );   
        // clear CTS flag     
        //UartCh[u8UartChIdx]->MSR |= UART_MSR_DCTSF_Msk;
        UART_ClearIntFlag( UartCh[u8UartChIdx], UART_INTSTS_MODEMIF_Msk );        
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_MODEMIF_Msk) == 0 );   
		UartCh[u8UartChIdx]->MODEM |= UART_MODEM_RTSACTLV_Msk;
		UartCh[u8UartChIdx]->MODEM |= UART_MODEM_CTSACTLV_Msk;
		UartCh[u8UartChIdx]->MODEM |= 0x80000000;
     
        //4 : Rx time-out interrupt   
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RXTOIF_Msk) == 0 );      
        // enable time out counter    
        UartCh[u8UartChIdx]->TOUT |= (1 << UART_TOUT_TOIC_Pos);
        // send Tx 
        UART_WRITE( UartCh[u8UartChIdx], 0x55 );
        // wait Rx time out happen
        while( (UartCh[u8UartChIdx]->INTSTS & UART_INTSTS_RXTOIF_Msk) == 0 );    
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RXTOIF_Msk) == 1 );  
        // disable time out counter after finish test
        UartCh[u8UartChIdx]->TOUT &= ~UART_TOUT_TOIC_Msk;
        // read Rx 
        UART_READ( UartCh[u8UartChIdx] );      
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RXTOIF_Msk) == 0 );    
        
        //5 : Buffer error interrupt    
        // check macro
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_BUFERRIF_Msk) == 0 ); 
        // send data and wait Rx buffer error flag happen
        while( (UartCh[u8UartChIdx]->INTSTS & UART_INTSTS_BUFERRIF_Msk ) == 0 )
        {
            // send Tx 
            UART_WRITE( UartCh[u8UartChIdx], 0x55 );
        }      
        // check macro    
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_BUFERRIF_Msk) == 1 ); 
        // clear Rx buffer error flag
        //UartCh[u8UartChIdx]->FSR |= UART_FSR_RX_OVER_IF_Msk;
        UART_ClearIntFlag( UartCh[u8UartChIdx], UART_INTSTS_BUFERRIF_Msk );         
        while( (UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_RXOVIF_Msk) == UART_FIFOSTS_RXOVIF_Msk );
        // check macro    
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_BUFERRIF_Msk) == 0 );    
        // clear Rx FIFO // RX Reset
        UartCh[u8UartChIdx]->CTRL |= UART_CTRL_RXRST_Msk;
        while( (UartCh[u8UartChIdx]->CTRL & UART_CTRL_RXRST_Msk) == UART_CTRL_RXRST_Msk );   
        
        /* Test macro : UART_RS485_CLEAR_ADDR_FLAG
                        UART_RS485_GET_ADDR_FLAG */
        // check macro    
        CU_ASSERT( UART_RS485_GET_ADDR_FLAG( UartCh[u8UartChIdx] ) == 0 ); 
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 0 );     
        // set RS485 mode
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_RS485;
        // set RS485 address detection enable 
        UartCh[u8UartChIdx]->ALTCTL |= (UART_ALTCTL_ADDRDEN_Msk | UART_ALTCTL_RS485NMM_Msk);
        // set parity 1
        UartCh[u8UartChIdx]->LINE = UART_WORD_LEN_8 | UART_PARITY_ODD | UART_STOP_BIT_1;    
        // send Tx 
        UART_WRITE( UartCh[u8UartChIdx], 0x55 );
        // wait RS485 address flag happen
        while( (UartCh[u8UartChIdx]->TRSR & UART_TRSR_ADDRDETF_Msk) == 0 );  
        // check macro    
        CU_ASSERT( UART_RS485_GET_ADDR_FLAG( UartCh[u8UartChIdx] ) == 1 );
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 1 );     
        // clear RS485 address flag
        UART_RS485_CLEAR_ADDR_FLAG( UartCh[u8UartChIdx] );  
        // check macro    
        CU_ASSERT( UART_RS485_GET_ADDR_FLAG( UartCh[u8UartChIdx] ) == 0 );
        CU_ASSERT( UART_GET_INT_FLAG( UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 0 );                   
        
        //disable UART1 internal loopback mode after test */
        UartCh[u8UartChIdx]->MODEM &= ~(0x80000000);      
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART;  
        UartCh[u8UartChIdx]->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
        UartCh[u8UartChIdx]->ALTCTL &= ~(UART_ALTCTL_ADDRDEN_Msk | UART_ALTCTL_RS485NMM_Msk);          
    }

}


void TestFunc_UART_TestMacro()
{
    /* Test macro: UART_BAUD_MODE0_DIVIDER
                   UART_BAUD_MODE2_DIVIDER */    
    TestFunc_UART_TestMacroBR();    
    
    /* Test macro : UART_WRITE
                    UART_READ
                    UART_GET_TX_EMPTY
                    UART_GET_RX_EMPTY
                    UART_IS_TX_EMPTY
                    UART_WAIT_TX_EMPTY
                    UART_IS_RX_READY
                    UART_IS_TX_FULL
                    UART_IS_RX_FULL
                    UART_GET_TX_FULL
                    UART_GET_RX_FULL */
    TestFunc_UART_TestMacroFIFO();   
    
    /* Test macro : UART_CLEAR_RTS
                    UART_SET_RTS */ 
    TestFunc_UART_TestMacroRTS();
    
    /* Test macro : UART_ENABLE_INT
                    UART_DISABLE_INT      
                    UART_GET_INT_FLAG
                    UART_RS485_CLEAR_ADDR_FLAG
                    UART_RS485_GET_ADDR_FLAG */ 
    TestFunc_UART_TestMacroINT();                      
}

const uint32_t au32UartRxTrgSel[4]={ 
    UART_LINE_RFITL_1BYTE, 
    UART_LINE_RFITL_4BYTES, 
    UART_LINE_RFITL_8BYTES,
    UART_LINE_RFITL_14BYTES,     
};

const uint32_t au32UartRTSTrgSel[4]={ 
    UART_LINE_RTS_TRI_LEV_1BYTE, 
    UART_LINE_RTS_TRI_LEV_4BYTES, 
    UART_LINE_RTS_TRI_LEV_8BYTES,
    UART_LINE_RTS_TRI_LEV_14BYTES,     
};

void TestFunc_UART_TestConstant()
{
    uint8_t u8UartChIdx;        //UART channel index 
    uint8_t u8UartRxTrgIdx;     //Rx FIFO trigger level index    
    uint8_t u8UartRTSTrgIdx;    //RTS trigger level index     
    
    //wait UART send message finish 
    //UART_WAIT_TX_EMPTY(UART0);      

    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    { 
        //select Rx FIFO trigger level
        for( u8UartRxTrgIdx=0; u8UartRxTrgIdx<4; u8UartRxTrgIdx++)
        {
            UartCh[u8UartChIdx]->LINE = (UartCh[u8UartChIdx]->LINE & (~UART_LINE_RFITL_Msk)) | au32UartRxTrgSel[u8UartRxTrgIdx];
            CU_ASSERT( (UartCh[u8UartChIdx]->LINE & UART_LINE_RFITL_Msk) == au32UartRxTrgSel[u8UartRxTrgIdx] );  
        }
        
        //select RTS trigger level index        
        for( u8UartRTSTrgIdx=0; u8UartRTSTrgIdx<4; u8UartRTSTrgIdx++)
        {
             UartCh[u8UartChIdx]->LINE = (UartCh[u8UartChIdx]->LINE & (~UART_LINE_RTSTRGLV_Msk)) | au32UartRTSTrgSel[u8UartRTSTrgIdx];
             CU_ASSERT( (UartCh[u8UartChIdx]->LINE & UART_LINE_RTSTRGLV_Msk) == au32UartRTSTrgSel[u8UartRTSTrgIdx] );  
        }
        
        //select RTS active level 
        UartCh[u8UartChIdx]->MODEM = (UartCh[u8UartChIdx]->MODEM & (~UART_MODEM_RTSACTLV_Msk)) | UART_RTS_IS_LOW_LEV_TRG;
        CU_ASSERT( (UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSACTLV_Msk) == UART_RTS_IS_LOW_LEV_TRG );  
        
        UartCh[u8UartChIdx]->MODEM = (UartCh[u8UartChIdx]->MODEM & (~UART_MODEM_RTSACTLV_Msk)) | UART_RTS_IS_HIGH_LEV_TRG;
        CU_ASSERT( (UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSACTLV_Msk) == UART_RTS_IS_HIGH_LEV_TRG );     
    }     

    //CU_ASSERT( UART0_FIFO_SIZE == 64 );  
    //CU_ASSERT( UART1_FIFO_SIZE == 16 );  
    //CU_ASSERT( UART1_FIFO_SIZE == 16 );      
    
}

void TestFunc_UART_SelectLINMode()
{
    uint8_t u8UartChIdx;        //UART channel index   
    uint8_t u8LinModeIdx;       //LIN mode   
    uint32_t u32LinBreakLen;    //LIN Break Length  
       
    /* Test loop */      
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {
        for( u8LinModeIdx=0; u8LinModeIdx<2; u8LinModeIdx++)
        {                 
            for( u32LinBreakLen=1; u32LinBreakLen<8; u32LinBreakLen++)
            {
                UART_SelectLINMode( UartCh[u8UartChIdx], UART_ALTCTL_LINTXEN_Msk, u32LinBreakLen );
                UartCh[u8UartChIdx]->DAT = 0x55;
                UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]);
                CU_ASSERT( UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_LIN );  
                CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_BRKFL_Msk) == u32LinBreakLen );  

                UART_SelectLINMode( UartCh[u8UartChIdx], UART_ALTCTL_LINRXEN_Msk, 0 );
                CU_ASSERT( UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_LIN );  
                CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_LINRXEN_Msk) == UART_ALTCTL_LINRXEN_Msk );                 
            }            
        }
    }        
 
    //restore UART setting for other test
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {   
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART; 
        UartCh[u8UartChIdx]->ALTCTL &= ~(UART_ALTCTL_LINTXEN_Msk|UART_ALTCTL_LINRXEN_Msk);   
    } 
}

void TestFunc_UART_ReadWrite()
{
    uint8_t u8UartChIdx, i;        //UART channel index   
    uint8_t u8TxData[8]={1, 2, 3, 4, 5, 6, 7, 8}, 
            u8RxData[8]={0};     

    /* Test loop */
    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->MODEM |= (0x80000000); 
        
        /* Reset Tx/Rx FIFO */
        UartCh[u8UartChIdx]->CTRL |= (0x3); 
        while( UartCh[u8UartChIdx]->CTRL & 0x3 );                
        
        /* Write data */
        UART_Write(UartCh[u8UartChIdx], u8TxData, 8);
        
        /* Read data */
        UART_Read(UartCh[u8UartChIdx], u8RxData, 8);   

        for( i=0; i<8; i++)
        {
            CU_ASSERT( u8TxData[i] == u8RxData[i] );             
        }            
    
    }        

}

void TestFunc_UART_TestConstantLIN()
{
    uint8_t u8UartChIdx;    //UART channel index 
    uint8_t u32LinBreakLen;  //LIN break field length
    //uint8_t u8LinBSLen;     //LIN Break/Sync Delimiter Length
    uint16_t u16LinPID;       //LIN PID
    
    //wait UART send message finish 
    //UART_WAIT_TX_EMPTY(UART0);      

    for( u8UartChIdx=0; u8UartChIdx<UartChNum; u8UartChIdx++)
    { 
        // UART_ALT_CTL_LIN_RX_EN_Msk
        //UartCh[u8UartChIdx]->LIN_CTL |= UART_LIN_CTL_LINS_EN;
		UartCh[u8UartChIdx]->ALTCTL |= UART_ALTCTL_LINRXEN_Msk;
        CU_ASSERT( UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_LINRXEN_Msk );  

        // UART_ALT_CTL_LIN_TX_EN_Msk
        //UartCh[u8UartChIdx]->LIN_CTL |= UART_LIN_CTL_LINS_HDET_EN;
		UartCh[u8UartChIdx]->ALTCTL |= UART_ALTCTL_LINTXEN_Msk;
        CU_ASSERT( UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_LINTXEN_Msk );          
        
        // UART_ALT_CTL_Bit_ERR_EN_Msk
        //UartCh[u8UartChIdx]->LIN_CTL |= UART_LIN_CTL_LINS_ARS_EN;
		UartCh[u8UartChIdx]->ALTCTL |= UART_ALTCTL_BITERREN_Msk;
        CU_ASSERT( UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_BITERREN_Msk );                 
                                             
        //select LIN break field length
        for( u32LinBreakLen=1; u32LinBreakLen<=7; u32LinBreakLen++)
        {
            UartCh[u8UartChIdx]->ALTCTL = (UartCh[u8UartChIdx]->ALTCTL & (~UART_ALTCTL_BRKFL_Msk)) | u32LinBreakLen;
            CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_BRKFL_Msk) == u32LinBreakLen );  
        }
 
        // UART_ALT_CTL_LIN_HEAD_SEL_Msk  //“bbreak field
        UartCh[u8UartChIdx]->ALTCTL = (UartCh[u8UartChIdx]->ALTCTL & (~UART_ALTCTL_LINHSEL_Msk)) | (0 << UART_ALTCTL_LINHSEL_Pos);
        CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_LINHSEL_Msk) == (0<<UART_ALTCTL_LINHSEL_Pos) ); 
        
        // UART_ALT_CTL_LIN_HEAD_SEL_Msk  //“bbreak field + sync
        UartCh[u8UartChIdx]->ALTCTL = (UartCh[u8UartChIdx]->ALTCTL & (~UART_ALTCTL_LINHSEL_Msk)) | (1<<UART_ALTCTL_LINHSEL_Pos);
        CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_LINHSEL_Msk) == (1<<UART_ALTCTL_LINHSEL_Pos) ); 
       
        // UART_ALT_CTL_LIN_HEAD_SEL_Msk  //“bbreak field + sync + PID
        UartCh[u8UartChIdx]->ALTCTL = (UartCh[u8UartChIdx]->ALTCTL & (~UART_ALTCTL_LINHSEL_Msk)) | (2<<UART_ALTCTL_LINHSEL_Pos);
        CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_LINHSEL_Msk) == (2<<UART_ALTCTL_LINHSEL_Pos) );         

        //select LIN PID
        for( u16LinPID=0; u16LinPID<256; u16LinPID++)
        {
            UartCh[u8UartChIdx]->ALTCTL = (UartCh[u8UartChIdx]->ALTCTL & (~UART_ALTCTL_ADRMPID_Msk)) | (u16LinPID<<UART_ALTCTL_ADRMPID_Pos);
            CU_ASSERT( (UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_ADRMPID_Msk) == (u16LinPID<<UART_ALTCTL_ADRMPID_Pos) );  
        }  
         
    }             
}



CU_TestInfo UART_ApiTests[] =
{
    //{"Testing UART_EnableFlowCtrl Function.", TestFunc_UART_EnableFlowCtrl},  
    //{"Testing UART_EnableInt Function.", TestFunc_UART_EnableInt}, 
    //{"Testing UART_Open Function.", TestFunc_UART_Open},
    //{"Testing UART_SetLine_Config Function.", TestFunc_UART_SetLine_Config},
    //{"Testing UART_SetTimeoutCnt Function.", TestFunc_UART_SetTimeoutCnt},
    //{"Testing UART_SelectIrDAMode Function.", TestFunc_UART_SelectIrDAMode},
    //{"Testing UART_SelectRS485Mode Function.", TestFunc_UART_SelectRS485Mode},
    //{"Testing UART_ReadWrite Function.", TestFunc_UART_ReadWrite},
    //{"Testing UART_SelectLINMode Function.", TestFunc_UART_SelectLINMode},


    CU_TEST_INFO_NULL
};

CU_TestInfo UART_MacroTests[] =
{
    //{"Testing Macro about baud rate.", TestFunc_UART_TestMacroBR},
    //{"Testing Macro about FIFO.", TestFunc_UART_TestMacroFIFO},
    //{"Testing Macro about RTS.", TestFunc_UART_TestMacroRTS},
    //{"Testing Macro about interrupt.", TestFunc_UART_TestMacroINT},
    //{"Testing Constant.", TestFunc_UART_TestConstant},
    {"Testing Constant about LIN.", TestFunc_UART_TestConstantLIN},


    CU_TEST_INFO_NULL
};

