/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 5 $
 * $Date: 16/01/06 11:55a $
 * @brief   Demonstrate how to use DSRC control transfer/recieve data to RF IC
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"
#include "dsrc.h"
#include "rf_module.h"


/*  Initialize global variables */
volatile uint8_t dsrc_start_frame=0;  // Start of frame 0x7E flag
volatile uint8_t dsrc_stop_frame=0;   // End of frame 0x7E flag
volatile uint8_t dsrc_crc_corr=0;     // CRC15 results are correct flag
volatile uint8_t dsrc_wakeup=0;       // Wakeup flag from RF IC wakeup pins
volatile uint8_t dsrc_tmr[5]={0};     // T2,T3 and T4 flags


void EnableTestClk(void){  //for test
#if 1
    SYS_UnlockReg();
  printf("Measur PC0(CON30.2) , it should output dsrc_clk\n");
  //outpw(CLK_BASE+0x60,(1<<7)|0x35);  //pclk_spi1
  //outpw(CLK_BASE+0x60,(1<<7)|0x32);  //pclk_tmr2
  //outpw(CLK_BASE+0x60,(1<<7)|0x2b);  //pclk_dsrc
  //outpw(CLK_BASE+0x60,(1<<7)|0x38);  //tmr2_clk
  outpw(CLK_BASE+0x60,(1<<7)|0x3b);    //dsrc_clk
  //outpw(CLK_BASE+0x60,(1<<7)|0x3c);  //spi1_clk
  //outpw(CLK_BASE+0x60,(1<<7)|0x05);  //hclk
  
  //outpw(CLK_BASE+0x60,0x81);    //HIRC0
  //outpw(CLK_BASE+0x60,0x8D);    //HIRC1
  //outpw(CLK_BASE+0x60,0x8F);    //MIRC
#endif
}

/**
 * @brief       DSRC interrupt handler
 * @param[in]   None
 * @return      None
 * @details     This function services DSRC interrupts. 
 */
void DSRC_IRQHandler(void)
{
  
  //Check whether start of frame 0x7E detection Flag is detected
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_STRFRM_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_STRFRMIE_Msk))
  {
    /* Clear start of frame 0x7E flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_STRFRM_Msk);
    dsrc_start_frame=1;  //set 
  }
  
  /* Check whether end of frame 0x7E detection Flag is detected */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_STPFRM_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_STPFRMIE_Msk))
  {
    /* Clear end of frame 0x7E flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_STPFRM_Msk);
    dsrc_stop_frame=1;
  }
  
  /* Check whether TA4 time out is detected */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_T4TO_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_T4TOIE_Msk))
  {
    /* Clear TA4 time out flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_T4TO_Msk); 
    dsrc_tmr[4]=1;
  }
  
  /* Check whether TA3 time out is detected */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_T3TO_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_T3TOIE_Msk))
  {
    /* Clear TA3 time out flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_T3TO_Msk);
    dsrc_tmr[3]=1;
  }
  
  /* Check whether TA2 time out is detected */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_T2TO_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_T2TOIE_Msk))
  {
    /* Clear TA2 time out flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_T2TO_Msk);
    dsrc_tmr[2]=1;
  }
  
  /* Check whether bit rate error is detected */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_BRATERR_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_BRATERRIE_Msk))
  {
    /* Clear bit rate error flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_BRATERR_Msk);
  }
  
  /* Check whether received data error is detected */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_RXDATERR_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_RXDATERRIE_Msk))
  {
    /* Clear received data error flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_RXDATERR_Msk);
  }
  
  /* Check whether CRC check correct */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_CRCCOR_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_CRCCORIE_Msk))
  {
    //Check CRC0 compare OK
    if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_CRC0_OK_Msk))
      DSRC_SET_SEED(DSRC,0); //CRC0 compare OK
    else 
      DSRC_SET_SEED(DSRC,1); //CRC1 compare OK
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_CRCCOR_Msk);  //Clear CRC correct flag
    dsrc_crc_corr=1;  //Set CRC correct flag to '1'
  }
  
  /* Check whether CRC error */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk) && DSRC_GET_INT(DSRC,DSRC_INTEN_CRCERRIE_Msk))
  {
    /* Clear CRC error flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk);
  }
  
  /* Check whether external pin wake-up event is detected */
  if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_EPWKF_Msk) && DSRC_GET_INT(DSRC,DSRC_STATUS_EPWKF_Msk))
  {
    /* Clear external pin wake-up event flag */
    DSRC_CLR_FLAG(DSRC,DSRC_STATUS_EPWKF_Msk);
    dsrc_wakeup=1;
  }
}


/**
 * @brief       Initialize system
 * @param[in]   None
 * @return      None
 * @details     This function set clock and I/O Multi-function 
 */
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk);
  
    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* First, Clear PB0 and PB1 multi-function pins to '0', and then    */
    /* Set PB0 and PB1 multi-function pins to UART0 RXD and TXD         */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );
    
    /* First, Clear PC8, PC9, PC10 and PC11 multi function pin to '0', and then      */
    /* Set PC8, PC9, PC10 and PC11 multi function pin to SS0, CLK, MISO0, M0SI       */
    SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC8MFP_Msk | SYS_GPC_MFPH_PC9MFP_Msk | SYS_GPC_MFPH_PC10MFP_Msk | SYS_GPC_MFPH_PC11MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC8MFP_SPI1_SS0 | SYS_GPC_MFPH_PC9MFP_SPI1_CLK | SYS_GPC_MFPH_PC10MFP_SPI1_MISO0 | SYS_GPC_MFPH_PC11MFP_SPI1_MOSI0);

    /* First Clear PD6 and PD7 multi function pin to '0', and then */
    /* Set PD6 and PD7 multi function pin to SPI1 MOSI1, MISO1     */
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD6MFP_Msk | SYS_GPD_MFPL_PD6MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD6MFP_SPI1_MOSI1 | SYS_GPD_MFPL_PD7MFP_SPI1_MISO1);
    
    /* Set PB multi-function pins for Clock Output */
    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO;

    /* Lock protected registers */
    SYS_LockReg();
}


/**
 * @brief       Initialize UART0
 * @param[in]   None
 * @return      None
 * @details     This function set UART0 Baudrate and reset UART0
 */
void UART0_Init(void)
{
  /* Reset IP */
  SYS_ResetModule(UART0_RST);

  /* Configure UART0 and set UART0 Baudrate */
  UART_Open(UART0, 115200);
}


/**
 * @brief       Initialize SPI1
 * @param[in]   None
 * @return      None
 * @details     This function set clock and SPI1 parameters
 */
void SPI1_Init(void){
  
  /* Enable SPI1 module clock */
  CLK_EnableModuleClock(SPI1_MODULE);
  
  /* Select SPI1 clock source */
  CLK_SetModuleClock(SPI1_MODULE,CLK_CLKSEL2_SPI1SEL_HIRC,NULL);
  
  /* Set the SPI bus clock to 12Mhz */
  SPI_SetBusClock(SPI1, 12000000);
  
  /* Enable FIFO mode with  Tx/Rx FIFO threshold */
  SPI_EnableFIFO(SPI1,7,7);
  
  /* Set the SPI transfer sequence with LSB first */
  SPI_SET_LSB_FIRST(SPI1);
  
  /* Enable slave 3-wire mode */
  SPI_ENABLE_3WIRE_MODE(SPI1);
}


/**
 * @brief       Initialize TMR2
 * @param[in]   None
 * @return      None
 * @details     This function set clock and TMR2 parameters
 */
void TMR2_Init(void ){
  /* Enable TMR2 module clock */
  CLK_EnableModuleClock(TMR2_MODULE);
  
  /* Select TMR2 clock source */
  CLK_SetModuleClock(TMR2_MODULE,CLK_CLKSEL2_TMR2SEL_HIRC,CLK_TMR2_CLK_DIVIDER(1));
}


#pragma pack(32)
/* Test pattern 1 */
volatile static uint8_t TestSend1[] = {0x03, 0xFF, 0xFF, 0xFF, 0xC0, 0x03, 0x91, 0xD0, 
              0x00, 0x01, 0xC1, 0x01, 0x00, 0x27, 0x45, 0x54, 
              0x43, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x02,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x20, 0x07, 0x01, 0x01, 0x20, 0x10, 0x12, 0x31,
              0x03, 0xFF, 0xFF, 0xFF, 0x12, 0x82, 0x10 };//0xF1C9

/* Test pattern 2 */
volatile static uint8_t TestSend2[] = {0x03, 0xFF, 0xFF, 0xFF, 0xE0, 0xF7, 0x00, 0xD0, 
              0x18, 0x01, 0x19, 0x02, 0x02, 0x19, 0x6F, 0x15, 
              0x84, 0x0E, 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53,
              0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31,
              0xA5, 0x03, 0x88, 0x01, 0x01, 0x90, 0x00, 0x1C,
              0x45, 0x54, 0x43, 0x01, 0x00, 0x01, 0x00, 0x01, 
              0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x20, 0x07, 0x01, 0x01, 0x20, 0x10,
              0x12, 0x31, 0x90, 0x00, 0x00 };//0xEE8C

/* Test pattern 3 */
volatile static uint8_t TestSend3[] = {0x03, 0xFF, 0xFF, 0xFF, 0xE0, 0x77, 0x00, 0xD0, 
              0x18, 0x01, 0x1B, 0x00};//0x0770

/* Recieve data buffer from TPB */
volatile static uint8_t EncodeBuf0[256*2];
#pragma pack()

/**
 * @brief       Initialize PDMA
 * @param[in]   None
 * @return      None
 * @details     This function set clock and PDMA ch1&ch2 parameters
 */
void PDMA_Init(void){
  /* PDMA Enable */
  CLK_EnableModuleClock(PDMA_MODULE);
  
  /* Configure channel 2 */
  //PDMA function
  PDMAGCR->GCTL=0x00000600; // enable channel clock [ch1 & ch2]
  PDMAGCR->REQSEL0=0x00011100;
  
  //ch1 for DSRC RX
  PDMA1->CTLn=0x00080021;  //SAD_SEL[5:4], P2M[3:2], DMAEN[0], Transfer Width[20:19]
  PDMA1->SAn=0x401F0024;
  PDMA1->DAn=(uint32_t)EncodeBuf0;
  PDMA1->CNTn=0xFFFFFFFF;
  PDMA1->CTLn=PDMA1->CTLn| (1<<23); //TRIGEN[23];
}

/**
 * @brief       Enable DSRC Rx PDMA
 * @param[in]   None
 * @return      None
 * @details     This function enable Rx PDMA
 */
void DSRC_RX_PDMA_ON(){
  PDMA1->CTLn=PDMA1->CTLn | (1<<0);
  PDMA1->CTLn=PDMA1->CTLn | (1<<23); //TRIGEN[23];
}

/**
 * @brief       Disable DSRC Rx PDMA
 * @param[in]   None
 * @return      None
 * @details     This function disable Rx PDMA
 */
void DSRC_RX_PDMA_OFF(){
  PDMA1->CTLn=PDMA1->CTLn& ~(1<<0); //Disable TRIGEN[23];
}


/**
 * @brief       Set buffer to DSRC Tx PDMA
 * @param[in]   buf  base addres of transfer buffer
 * @param[in]   len  length of transfer buffer
 * @return      None
 * @details     This function set buffer to DSRC Tx PDMA
 */
void DSRC_TX(volatile uint8_t *buf,uint32_t len)
{
  /*---------------------------------------------------------------------------------------------------------*/
  /* Configure DSRC TX buffer                                                                                */
  /*---------------------------------------------------------------------------------------------------------*/
  PDMA2->CTLn=0x00080081;  //SAD_SEL[5:4], P2M[3:2], DMAEN[0], Transfer Width[20:19]
  PDMA2->SAn=(uint32_t)buf;
  PDMA2->DAn=0x401F0020;
  PDMA2->CNTn=len;
  PDMA2->INTENn|=0x2;
  PDMA2->CTLn=PDMA2->CTLn| (1<<23); //TRIGEN[23];
}


static uint8_t volatile g_au8WAKEUP_INT = 0;  //Wakeup interrupt flag


/**
 * @brief       PDWU interrupt handler
 * @param[in]   None
 * @return      None
 * @details     This function services PDWU interrupts. 
 */
void PDWU_IRQHandler(void)
{	
  CLK->WKINTSTS = 1;
  g_au8WAKEUP_INT = 1;
}


void Enter_PowerDown(){
  g_au8WAKEUP_INT=0;
  SYS_UnlockReg();
  CLK->PWRCTL |= (1<<6) | (1<<5) ; //Enable PDEN and PDWKIEN
  outpw(0xE000ED10, inpw(0xE000ED10)|0x4); // Sleep Deep
  SYS_LockReg();
  __WFI();
}



/**
 * @brief       Initialize DSRC
  * @param[in]  u32Mode decides the DSRC module is operating. Valid values are:
  *              - \ref DSRC_MODE_FM0
  *              - \ref DSRC_MODE_SPI
 * @return      u32WakePin Enable wakeup pin
 * @details     This function initial DSRC
 */
void DSRC_Init(uint32_t u32mode,uint32_t u32EnWakeup){
  /* Enable DSRC module clock */
  CLK_EnableModuleClock(DSRC_MODULE);
  
  /* Select DSRC clock source */
  CLK_SetModuleClock(DSRC_MODULE,CLK_CLKSEL2_DSRCSEL_HIRC,CLK_DSRC_CLK_DIVIDER(1));
  
  TMR2_Init(); // Initialize TMR2
  SPI1_Init(); // Initialize SPI1
  PDMA_Init(); // Initialize PDMA
  
  /* First Clear PB9 and PB10 multi function pin to '0', and then   */
  /* Set PB9 and PB10 multi function pin for RF RX_ON and RF TX_ON  */
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  SYS->GPB_MFPH |= SYS_GPB_MFPH_PB10MFP_DSRC_TX_EN | SYS_GPB_MFPH_PB9MFP_DSRC_RX_EN;
    
  if(u32mode==DSRC_MODE_FM0)
  {
    /* First Clear PC11 multi function pin to '0', and then */
    /* Set PC11 multi function pin for FM0                  */
    SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC11MFP_Msk | SYS_GPC_MFPH_PC10MFP_Msk);
    SYS->GPC_MFPH |= SYS_GPC_MFPH_PC11MFP_DSRC_FM0_RX | SYS_GPC_MFPH_PC10MFP_DSRC_FM0_TX;
    
    /* Set Bit Rate Accuracy Control Bits for FM0 */
    DSRC_SET_BATE_CTL(DSRC,4);
    
    /* Select SPI1 clock source to HXT */
    CLK_SetModuleClock(SPI1_MODULE,CLK_CLKSEL2_SPI1SEL_HXT,NULL);
    
    /* Set the SPI1 bus clock */
    SPI_SetBusClock(SPI1, 512000);
  }
  
  if(u32EnWakeup==1)
  {
    #if 0
    SYS->GPB_MFPL =(SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk)|SYS_GPB_MFPL_PB2MFP_DSRC_WAKEUP;  //DSRC_RF_wakeup(PB2,con34.6)
    #else
    SYS->GPE_MFPL =(SYS->GPE_MFPL & ~SYS_GPE_MFPL_PE5MFP_Msk)|SYS_GPE_MFPL_PE5MFP_DSRC_WAKEUP; //DSRC_RF_wakeup(PE5,con30.15)
    #endif
    DSRC_WKPOL_TRIGGER(DSRC,DSRC_CTL_WKPOL_FALLING); //set external wakeup pin trigger type to falling trigger.
  }
}

/**
 * @brief       DSRC FM0 mode demo function
 * @param[in]   None
 * @return      None
 * @details     This function is FM0 mode demo
 */
void DSRC_FM0_test()
{
  /*  Initialize variables */
  uint32_t rbcnt,i;
  volatile uint32_t number=0;

  /* Initialize DSRC */
  DSRC_Init(DSRC_MODE_FM0,0); 

  /* Enable DSRC,CRC,PDMA */
  DSRC_Open(DSRC,DSRC_MODE_FM0,1,1);
  
  /* 0: Disable CRC Generation on Data MSB for CRC */
  /* 1: Enable Bit order reverse for CRC checksum */
  /* 1: Enable 1'complement for CRC checksum for CRC */
  /* 1: Enable Swap high byte and low bytes */
  DSRC_SET_CRC_FMT(DSRC,0,1,1,1);
  
  /* Set preamble length is one word and pareamble = 0xFFFF0000 */
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_32,0xFFFF0000);
  
  /* Set DSRC RX on to high(RXON=1) */
  DSRC_RXON(DSRC);
  
  /* Set TA2 time out */
  DSRC_SET_TIMEROUT(DSRC,2,0x7F,0x1FFFFF);
  
  /* Set TA3 time out */
  DSRC_SET_TIMEROUT(DSRC,3,0x01,0xF50000);
  
  /* Set TA4 time out */
  DSRC_SET_TIMEROUT(DSRC,4,0x00,0x000330+600);//816
  
  /* Enable TA4 interrupt */
  DSRC_EnableInt(DSRC,DSRC_INTEN_T4TOIE_Msk);
  /* Enable CRC corrent interrupt*/
  DSRC_EnableInt(DSRC,DSRC_INTEN_CRCCORIE_Msk);
  NVIC_EnableIRQ(DSRC_IRQn);

  while(1)
  {
    if(dsrc_crc_corr==1)
    {
      number++;
      /* Disable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_OFF();
      
      /* Receive bytes = DSRC->RBCNT + CRC16(2 bytes) */
      rbcnt=DSRC->RBCNT+2;
      
      if(number==1){ 
        /* Transfer pattern3 data */
        DSRC_TX(TestSend3,sizeof(TestSend3)/sizeof(uint8_t));
      }else if(number==2){ 
        /* Transfer pattern2 data */
        DSRC_TX(TestSend2,sizeof(TestSend2)/sizeof(uint8_t));
      }else{  
        /* Transfer pattern1 data */
        DSRC_TX(TestSend1,sizeof(TestSend1)/sizeof(uint8_t));
        number=0;
      }

      #if 1
      /* Use printf(*) have a trouble for TA4 time out */
      printf("STATU=0x%08x,CHECKSUM=0x%08x\n",DSRC->STATUS,DSRC->CHKSUM);
      printf("RXDATA=");
      for(i=0;i<rbcnt;i++)
        printf("%2X, ",EncodeBuf0[i]);
      printf("\n");
      #endif
      
      /* Waiting for current setat emachine form WAIT_TX state to WAIT_RX state */
      while(DSRC_GET_CurStateMachine(DSRC)!=0x1);
      
      /* Enable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_ON();
      dsrc_crc_corr=0;
    }else{
      //Check CRC error
      if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk))
        DSRC_CLR_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk); //Clear CRC error flag
    }
  }
}

/**
 * @brief       DSRC FM0 mode with power-down demo function
 * @param[in]   None
 * @return      None
 * @details     This function is FM0 mode with power-down demo
 */
void DSRC_FM0_PD_test(){
  uint32_t rbcnt,i;
  volatile uint32_t number=0;

  /* Initialize DSRC and enable wakeup pin */
  DSRC_Init(DSRC_MODE_FM0,1);

  /* Enable DSRC,CRC,PDMA */
  DSRC_Open(DSRC,DSRC_MODE_FM0,1,1);
  
  /* 0: Disable CRC Generation on Data MSB for CRC */
  /* 1: Enable Bit order reverse for CRC checksum */
  /* 1: Enable 1'complement for CRC checksum for CRC */
  /* 1: Enable Swap high byte and low bytes */
  DSRC_SET_CRC_FMT(DSRC,0,1,1,1);
  
  /* Set preamble length is one word and pareamble = 0xFFFF0000 */
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_32,0xFFFF0000);
    
  /* Set TA2,TA3 and TA4 time out */
  DSRC_SET_TIMEROUT(DSRC,2,0x7F,0x1FFFFF);
  DSRC_SET_TIMEROUT(DSRC,3,0x01,0xF50000);
  DSRC_SET_TIMEROUT(DSRC,4,0x00,0x000330+600);//816
  
  /* Enable TA4 interrupt */
  DSRC_EnableInt(DSRC,DSRC_INTEN_T2TOIE_Msk|DSRC_INTEN_T3TOIE_Msk|DSRC_INTEN_T4TOIE_Msk);
  /* Enable CRC corrent interrupt*/
  DSRC_EnableInt(DSRC,DSRC_INTEN_CRCCORIE_Msk);
  NVIC_EnableIRQ(DSRC_IRQn);


   EnableTestClk();  //for test
   
  /* Enter PowerDown */
  Enter_PowerDown();
  while(1)
  {
      if(dsrc_tmr[2]==1 || dsrc_tmr[3]==1)
      {
        dsrc_tmr[2]=0;
        dsrc_tmr[3]=0;
        Enter_PowerDown(); /* Enter PowerDown */
      }
    if(dsrc_crc_corr==1)
    {
      number++;
      /* Disable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_OFF();
      
      /* Set SPI1 clock source to HXT */
      CLK_SetModuleClock(SPI1_MODULE,CLK_CLKSEL2_SPI1SEL_HXT,0);
      
      /* Receive bytes = DSRC->RBCNT + CRC16(2 bytes) */
      rbcnt=DSRC->RBCNT+2;
      
      if(number==1){ 
        /* Transfer pattern3 data */
        DSRC_TX(TestSend3,sizeof(TestSend3)/sizeof(uint8_t));
      }else if(number==2){ 
        /* Transfer pattern2 data */
        DSRC_TX(TestSend2,sizeof(TestSend2)/sizeof(uint8_t));
      }else{  
        /* Transfer pattern1 data */
        DSRC_TX(TestSend1,sizeof(TestSend1)/sizeof(uint8_t));
        number=0;
      }

      #if 1
      /* Use printf(*) have a trouble for TA4 time out */
      printf("STATU=0x%08x,CHECKSUM=0x%08x\n",DSRC->STATUS,DSRC->CHKSUM);
      printf("RXDATA=");
      for(i=0;i<rbcnt;i++)
        printf("%2X, ",EncodeBuf0[i]);
      printf("\n");
      #endif
      
      /* Waiting for current setat emachine form WAIT_TX state to WAIT_RX state */
      while(DSRC_GET_CurStateMachine(DSRC)!=0x1);
      
      /* Enable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_ON();
      
      /* Set SPI1 clock source to HIRC */
      CLK_SetModuleClock(SPI1_MODULE,CLK_CLKSEL2_SPI1SEL_HIRC,0);
      
      dsrc_crc_corr=0;
      Enter_PowerDown(); /* Enter PowerDown */
    }else{
      //Check CRC error
      if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk))
        DSRC_CLR_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk); //Clear CRC error flag
    }
  }
}

/**
 * @brief       DSRC SPI mode demo function
 * @param[in]   None
 * @return      None
 * @details     This function is SPI mode demo
 */
void DSRC_SPI_test(){
  uint32_t rbcnt,i;
  volatile uint32_t number=0;

  /* Initialize DSRC */
  DSRC_Init(DSRC_MODE_SPI,0);

  /* Enable DSRC,CRC,PDMA */
  DSRC_Open(DSRC,DSRC_MODE_SPI,1,1);
  
  /* 0: Disable CRC Generation on Data MSB for CRC */
  /* 1: Enable Bit order reverse for CRC checksum */
  /* 1: Enable 1'complement for CRC checksum for CRC */
  /* 1: Enable Swap high byte and low bytes */
  DSRC_SET_CRC_FMT(DSRC,0,1,1,1);
  
  /* Set preamble length is one word and pareamble = 0xFFFF0000 */
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_32,0xFFFF0000);
    
  /* Set TA2,TA3 and TA4 time out */
  DSRC_SET_TIMEROUT(DSRC,2,0x7F,0x1FFFFF);
  DSRC_SET_TIMEROUT(DSRC,3,0x01,0xF50000);
  DSRC_SET_TIMEROUT(DSRC,4,0x00,0x000330+600);//816
  
  /* Enable TA4 interrupt */
  DSRC_EnableInt(DSRC,DSRC_INTEN_T4TOIE_Msk);
  /* Enable CRC corrent interrupt*/
  DSRC_EnableInt(DSRC,DSRC_INTEN_CRCCORIE_Msk); //Clear CRC error flag
  NVIC_EnableIRQ(DSRC_IRQn);

  while(1)
  {
    if(dsrc_crc_corr==1)
    {
      number++;
      /* Disable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_OFF();
      
      /* Receive bytes = DSRC->RBCNT + CRC16(2 bytes) */
      rbcnt=DSRC->RBCNT+2;
      
      if(number==1){ 
        /* Transfer pattern3 data */
        DSRC_TX(TestSend3,sizeof(TestSend3)/sizeof(uint8_t));
      }else if(number==2){ 
        /* Transfer pattern2 data */
        DSRC_TX(TestSend2,sizeof(TestSend2)/sizeof(uint8_t));
      }else{  
        /* Transfer pattern1 data */
        DSRC_TX(TestSend1,sizeof(TestSend1)/sizeof(uint8_t));
        number=0;
      }

      #if 1
      /* Use printf(*) have a trouble for TA4 time out */
      printf("STATU=0x%08x,CHECKSUM=0x%08x\n",DSRC->STATUS,DSRC->CHKSUM);
      printf("RXDATA=");
      for(i=0;i<rbcnt;i++)
        printf("%2X, ",EncodeBuf0[i]);
      printf("\n");
      #endif
      
      /* Waiting for current setat emachine form WAIT_TX state to WAIT_RX state */
      while(DSRC_GET_CurStateMachine(DSRC)!=0x1);
      
      /* Enable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_ON();
      dsrc_crc_corr=0;
    }else{
      //Check CRC error
      if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk))
        DSRC_CLR_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk); //Clear CRC error flag
    }
  }
}

/**
 * @brief       DSRC SPI mode with power-down demo function
 * @param[in]   None
 * @return      None
 * @details     This function is SPI mode with power-down demo
 */
void DSRC_SPI_PD_test(){
  
  volatile uint32_t number=0,rbcnt,i;

  /* Initialize DSRC and enable wakeup pin */
  DSRC_Init(DSRC_MODE_SPI,1);

  /* Enable DSRC,CRC,PDMA */
  DSRC_Open(DSRC,DSRC_MODE_SPI,1,1);
  
  /* 0: Disable CRC Generation on Data MSB for CRC */
  /* 1: Enable Bit order reverse for CRC checksum */
  /* 1: Enable 1'complement for CRC checksum for CRC */
  /* 1: Enable Swap high byte and low bytes */
  DSRC_SET_CRC_FMT(DSRC,0,1,1,1);
  
  /* Set preamble length is one word and pareamble = 0xFFFF0000 */
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_32,0xFFFF0000);
  
  /* Set TA2,TA3 and TA4 time out */
  DSRC_SET_TIMEROUT(DSRC,2,0x7F,0x1FFFFF);
  DSRC_SET_TIMEROUT(DSRC,3,0x01,0xF50000);
  DSRC_SET_TIMEROUT(DSRC,4,0x00,0x000330+600);//816
  
  /* Enable TA4 interrupt */
  DSRC_EnableInt(DSRC,DSRC_INTEN_T2TOIE_Msk|DSRC_INTEN_T3TOIE_Msk|DSRC_INTEN_T4TOIE_Msk);
  /* Enable CRC corrent interrupt*/
  DSRC_EnableInt(DSRC,DSRC_INTEN_CRCCORIE_Msk);
  NVIC_EnableIRQ(DSRC_IRQn);
  
  /* Enter PowerDown */
  //Enter_PowerDown(); 
  while(1)
  {
    if(dsrc_tmr[2]==1 || dsrc_tmr[3]==1)
    {
      dsrc_tmr[2]=0;
      dsrc_tmr[3]=0;
      Enter_PowerDown(); /* Enter PowerDown */
    }
    if(dsrc_crc_corr==1)
    {
      number++;
      /* Disable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_OFF();
      
      /* Receive bytes = DSRC->RBCNT + CRC16(2 bytes) */
      rbcnt=DSRC->RBCNT+2;
      
      /* Set SPI1 clock source to HXT */
      CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(0x3<<24) ) | (2<<24);  
      
      if(number==1){ 
        /* Transfer pattern3 data */
        DSRC_TX(TestSend3,sizeof(TestSend3)/sizeof(uint8_t));
      }else if(number==2){ 
        /* Transfer pattern2 data */
        DSRC_TX(TestSend2,sizeof(TestSend2)/sizeof(uint8_t));
      }else{  
        /* Transfer pattern1 data */
        DSRC_TX(TestSend1,sizeof(TestSend1)/sizeof(uint8_t));
        number=0;
      }

      #if 1
      /* Use printf(*) have a trouble for TA4 time out */
      printf("STATU=0x%08x,CHECKSUM=0x%08x\n",DSRC->STATUS,DSRC->CHKSUM);
      printf("RXDATA=");
      for(i=0;i<rbcnt;i++)
        printf("%2X, ",EncodeBuf0[i]);
      printf("\n");
      #endif
      
      /* Waiting for current setat emachine form WAIT_TX state to WAIT_RX state */
      while(DSRC_GET_CurStateMachine(DSRC)!=0x1);
      
      /* Enable PDMA for DSRC rx channel */
      DSRC_RX_PDMA_ON();
      
      /* Set SPI1 clock source to HIRC */
      CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(0x3<<24) ) | (3<<24);
      
      dsrc_crc_corr=0;
      Enter_PowerDown(); /* Enter PowerDown */
    }else{
      //Check CRC error
      if(DSRC_GET_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk))
        DSRC_CLR_FLAG(DSRC,DSRC_STATUS_CRCERR_Msk); //Clear CRC error flag
    }
  }
}

/**
 * @brief       DSRC FM0 mode with force Rx demo function
 * @param[in]   None
 * @return      None
 * @details     This function is FM0 mode with force Rx demo
 */
void DSRC_FM0_ForceRX_test(){
  
  volatile uint32_t number=0,i,rbcnt;
  
  /* Initialize DSRC */
  DSRC_Init(DSRC_MODE_FM0,0);
  
  /* Enable DSRC,CRC,PDMA */
  DSRC_Open(DSRC,DSRC_MODE_FM0,1,1);
  
  /* 0: Disable CRC Generation on Data MSB for CRC */
  /* 1: Enable Bit order reverse for CRC checksum */
  /* 1: Enable 1'complement for CRC checksum for CRC */
  /* 1: Enable Swap high byte and low bytes */
  DSRC_SET_CRC_FMT(DSRC,0,1,1,1);
  
  /* Set preamble length is one word and pareamble = 0xFFFF0000 */
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_32,0xFFFF0000);

  /* Enable CRC corrent interrupt*/
  DSRC_EnableInt(DSRC,DSRC_INTEN_CRCCORIE_Msk);
  NVIC_EnableIRQ(DSRC_IRQn);
  
  while(1)
  {
    if(dsrc_crc_corr==1)
    {
      /* Disable DSRC Rx PDMA */
      DSRC_RX_PDMA_OFF(); 
      
      /* rbcnt = DSRC->RBCNT+ 2(CRC16) */
      rbcnt=DSRC->RBCNT+2;
#if 1
      /* Show RXDATA value to UART */
      printf("RXDATA=");
      for(i=0;i<rbcnt;i++)
        printf("%2X, ",EncodeBuf0[i]);
      printf("\n");
#endif
      
      /* Enable DSRC Rx PDMA */
      DSRC_RX_PDMA_ON();
      dsrc_crc_corr=0;
      
      /* DSRC state machine jump to Rx state */
      DSRC_FORCE_RX();
    }
  }
}

/**
 * @brief       DSRC FM0 mode with force Tx demo function
 * @param[in]   None
 * @return      None
 * @details     This function is FM0 mode with force Tx demo
 */
void DSRC_FM0_ForceTX_test(){
  
  volatile uint32_t number=0,num;
  
  /* Initialize DSRC */
  DSRC_Init(DSRC_MODE_FM0,0);
  
  /* Enable DSRC,CRC,PDMA */
  DSRC_Open(DSRC,DSRC_MODE_FM0,1,1);
  
  /* 0: Disable CRC Generation on Data MSB for CRC */
  /* 1: Enable Bit order reverse for CRC checksum */
  /* 1: Enable 1'complement for CRC checksum for CRC */
  /* 1: Enable Swap high byte and low bytes */
  DSRC_SET_CRC_FMT(DSRC,0,1,1,1);
  
  /* Set preamble length is one word and pareamble = 0xFFFF0000 */
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_32,0xFFFF0000);
  
  while(1)
  {
    printf("Send pattern(0~2):");
    
    /*  Input '0' : DSRC tranfer test pattern 1 to RF IC
     *  Input '1' : DSRC tranfer test pattern 2 to RF IC
     *  Input '2' : DSRC tranfer test pattern 3 to RF IC
     */
    num=getchar();
    printf("%c\n",num);
    switch(num)
    {
      case '0':
        DSRC_TX(TestSend1,sizeof(TestSend1)/sizeof(uint8_t)); //Set TestSend1 pattern to DSRC Tx PDMA
      break;
      case '1':
        DSRC_TX(TestSend2,sizeof(TestSend2)/sizeof(uint8_t)); //Set TestSend2 pattern to DSRC Tx PDMA
      break;
      case '2':
        DSRC_TX(TestSend3,sizeof(TestSend3)/sizeof(uint8_t)); //Set TestSend3 pattern to DSRC Tx PDMA
      break;
    }
    
    /* Go to TX state and transfer test paatern                                            */
    /* Automatic transfer data from DSRC Tx PDMA, when DSRC state machine is Tx state      */
    /* DSRC Tx PDMA transer is finished, DSRC state machine automatically jump to Rx state */ 
    DSRC_FORCE_TX(DSRC); 
    
    /* Check DSRC state machine is Rx state */
    while(DSRC_GET_CurStateMachine(DSRC)!=0x1);  
  }
}

/**
 * @brief       Show main menu
 * @param[in]   None
 * @return      None
 * @details     This function is show main menu with demo
 */
char Show_MainMenu(void)
{
    char item;
    printf("\n\n");
    printf("+------------------------------------------------+\n");
    printf("%-31s @ %-14s |\n",  "|Nano103 DSRC Demo Code v1.0", __DATE__);
    printf("+------------------------------------------------+\n");
    printf("%-48s |\n",  "| [1] DSRC SPI test");
    printf("%-48s |\n",  "| [2] DSRC FM0 test");
    printf("%-48s |\n",  "| [3] DSRC SPI test with powerdown");
    printf("%-48s |\n",  "| [4] DSRC FM0 test with powerdown");
    printf("%-48s |\n",  "| [5] DSRC FM0 FoceRX test");
    printf("%-48s |\n",  "| [6] DSRC FM0 FoceTX test");
    printf("+------------------------------------------------+\n");
    printf("Start test item : ");
    item=getchar();
    printf("%c\n",item);
    return item;
}


int32_t main (void)
{
    char num;

   	SYS_UnlockReg(); /* Unlock protected registers */
    outpw(SYS_BASE+0x60,0x5aa5); ////Disable POR
    outpw(SYS_BASE+0x64,inpw(SYS_BASE+0x64)&~(1<<7)); //Disable LVR
    SYS_LockReg(); /* Lock protected registers */
  
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();
  
  
    /* Nano103 connect RF IC : 
     *  PB10 <-> TX_ON
     *  PB9  <-> RX_ON
     *  PC0  <-> SPI_CS#
     *  PC1  <-> SPI_CLK
     *  PC3  <-> SPI_MOSI
     *  PE5  <-> WAKEUP
     */

   /* [1] DSRC SPI test :                                                                         */
   /*       Show DSRC SPI mode how to communicate(tranfer/receive) RF IC using PDMA               */
   /* [2] DSRC FM0 test :                                                                         */ 
   /*       Show DSRC FM0 mode how to communicate(tranfer/receive) RF IC using PDMA               */
   /* [3] DSRC SPI test with powerdown :                                                          */
   /*       Show DSRC FM0 mode how to communicate(tranfer/receive) RF IC using PDMA and pwoerdown */
   /* [4] DSRC FM0 test with powerdown :                                                          */
   /*       Show DSRC FM0 mode how to communicate(tranfer/receive) RF IC using PDMA and pwoerdown */
   /* [5] DSRC FM0 FoceRX test :                                                                  */
   /*       Show DSRC FM0 mode how to only transfer data to RF IC using PDMA                      */
   /* [6] DSRC FM0 FoceTX test :                                                                  */
   /*       Show DSRC FM0 mode how to only receive data to RF IC using PDMA                       */
   while(1){
    num=Show_MainMenu(); //show menu with demo
    switch(num){
      case '1':
        ET6603_Init(10); //Initialize ET6603 RF IC SPI mode
        DSRC_SPI_test(); //DSRC SPI mode demo
        break;
      case '2':
        ET6603FM0_Init(10); //Initialize ET6603 RF IC FM0 mode
        DSRC_FM0_test(); //DSRC FM0 mode demo
        break;
      case '3':
        ET6603_Init(10); //Initialize ET6603 RF IC SPI mode
        DSRC_SPI_PD_test(); //DSRC SPI mode demo with power-down
        break;
      case '4':
        ET6603FM0_Init(10); //Initialize ET6603 RF IC FM0 mode
        DSRC_FM0_PD_test(); //DSRC FM0 mode demo with power-down
        break;
      case '5':
        ET6603FM0_Init(10); //Initialize ET6603 RF IC FM0 mode
        DSRC_FM0_ForceRX_test();  //DSRC FM0 Force TX demo
        break;
      case '6':
        ET6603FM0_Init(10); //Initialize ET6603 RF IC FM0 mode
        DSRC_FM0_ForceTX_test(); //DSRC FM0 Force TX demo
        break;
    }
  }
}


/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
