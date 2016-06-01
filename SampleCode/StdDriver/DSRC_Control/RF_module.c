#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano103.h"

//#include "sys_init.h"
//#include "DrvPDMA.h"
//#include "DrvGPIO.h"
//#include "DrvSPI.h"
#include "RF_module.h"
#include "SKY6605.h"

#define MAX_LEN		2048
#define DSRC_WAKEUP_ISR	10
#define ET_6603_VERSON_05//clyu

//uint8_t dsrc_rbuf[MAX_LEN+2];

#define CS_LOW			PC0=0
#define CS_HIGH			PC0=1
#define SCK_LOW			PC1=0
#define SCK_HIGH		PC1=1
#define DATA_LOW		PC3=0
#define DATA_HIGH		PC3=1

uint8_t pdma1_TC=0;

void delayus(uint32_t us)
{
	volatile uint32_t nloop = us;
	while(nloop--);
}

void WriteET6603(unsigned short value)
{  
	char i;
	DATA_LOW;
	CS_LOW;
	for(i=0; i<16; i++)
	{
		SCK_LOW;
		if (value & 0x8000)
			DATA_HIGH;
		else
			DATA_LOW;
		SCK_HIGH;
		value <<= 1;
	}
	SCK_LOW;
	CS_HIGH;
}
void WriteET6603_Sequence(unsigned char sequence_length,unsigned char *reg_value)
{
	unsigned char i,j;
	unsigned char value;
	
	DATA_LOW;
	CS_LOW;
	for(j=0;j<sequence_length;j++)
	{
		value = *(reg_value+j);
		for(i=0; i<8; i++)
		{
			SCK_LOW;
			if (value & 0x80)
				DATA_HIGH;
			else
				DATA_LOW;
			SCK_HIGH;
			value <<= 1;
		}
	}
	SCK_LOW;
	CS_HIGH;
	return ;
}

void ET6602_Init_Register(unsigned char send_power)
{
}


void SKY6605FM0_Init(unsigned char send_power){
  uint32_t i,tmp;
  //PB10:DSRC_RF TX_EN,PB9:DSRC_RF RX_EN
  tmp= SYS->GPB_MFPH & (SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  GPIO_SetMode(PB, (1<<10)|(1<<9), GPIO_PMD_OUTPUT);
  
  //PB2,WAKEUP_EN#
  PB2=0;
  SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk);
  GPIO_SetMode(PB, (1<<2), GPIO_PMD_OUTPUT);
  
  TXON_DISABLE;
  WAKEUP_ENABLE;
  for(i=0;i<0x5000;i++);

  GPIO_SetMode(PC, (1<<0), GPIO_PMD_OUTPUT); 	//PC0	-	CS
  GPIO_SetMode(PC, (1<<1), GPIO_PMD_OUTPUT);	//PC1	-	SCK
  GPIO_SetMode(PC, (1<<3), GPIO_PMD_OUTPUT);	//PC3	-	MOSI

  PC0=1;				//CS High
  PC1=0;				//SCK Low
  PC3=0;				//MOSI Low

/****************************************************************/
	//WriteET6603(SKY6605_SYS_CTRL<<offset | 0x43);delayus(1);
  sky6605_init();
  
  sky6605_set_MFOUT_rxdata();
/****************************************************************/
  TXON_DISABLE;
  PC9=0;//SCK_LOW;
  PC11=0;//DATA_LOW;
  PC8=1;//CS_HIGH;
  
  RXON_ENABLE;
  
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk)|tmp;
}
void ET6603FM0_Init(unsigned char send_power){
  unsigned char ET6603_buf[16];

  uint32_t i,tmp;
  //PB10:DSRC_RF TX_EN,PB9:DSRC_RF RX_EN
  tmp= SYS->GPB_MFPH & (SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  GPIO_SetMode(PB, (1<<10)|(1<<9), GPIO_PMD_OUTPUT);
  
  //PB2,WAKEUP_EN#
  PB2=0;
  SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk);
  GPIO_SetMode(PB, (1<<2), GPIO_PMD_OUTPUT);
  
  TXON_DISABLE;
  WAKEUP_ENABLE;
  for(i=0;i<0x5000;i++);

  GPIO_SetMode(PC, (1<<0), GPIO_PMD_OUTPUT); 	//PC0	-	CS
  GPIO_SetMode(PC, (1<<1), GPIO_PMD_OUTPUT);	//PC1	-	SCK
  GPIO_SetMode(PC, (1<<3), GPIO_PMD_OUTPUT);	//PC3	-	MOSI

  PC0=1;				//CS High
  PC1=0;				//SCK Low
  PC3=0;				//MOSI Low

	memcpy(ET6603_buf,"\x21\x80\x98\xAE\x09\x04\x82",7);
	WriteET6603_Sequence(7,ET6603_buf);
/****************************************************************/
	WriteET6603(0x0740);delayus(1);
/****************************************************************/
#ifdef ET_6603_VERSON_05
  memcpy(ET6603_buf,"\x28\x2F\xE1\x0E\xB5\x88",6);
#else
  memcpy(ET6603_buf,"\x28\x2F\xE1\x0C\xB5\x88",6);
#endif
  WriteET6603_Sequence(6,ET6603_buf);

/****************************************************************/
	WriteET6603(0x0A00);delayus(1);
/****************************************************************/
  TXON_DISABLE;
  PC9=0;//SCK_LOW;
  PC11=0;//DATA_LOW;
  PC8=1;//CS_HIGH;
  
  RXON_ENABLE;
  
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk)|tmp;
}

void WriteET6602(uint32_t value)
{
	int i;
	DATA_LOW;
	CS_LOW;
  delayus(20);
	for(i=0; i<24; i++)
	{
		SCK_LOW;
		if (value & 0x00800000)
			DATA_HIGH;
		else
			DATA_LOW;
    delayus(3);
		SCK_HIGH;
    delayus(3);
		value <<= 1;
	}
	SCK_LOW;
  delayus(40);
	CS_HIGH;
	// DATA_LOW;
}

void ET6602_Init(unsigned char send_power){
  
  uint32_t i,tmp;
  //PB10:DSRC_RF TX_EN,PB9:DSRC_RF RX_EN
  tmp= SYS->GPB_MFPH & (SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  GPIO_SetMode(PB, (1<<10)|(1<<9), GPIO_PMD_OUTPUT);
  
  //PB2,WAKEUP_EN#
  PB2=0;
  SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk);
  GPIO_SetMode(PB, (1<<2), GPIO_PMD_OUTPUT);
  
  TXON_DISABLE;
  RXON_ENABLE;
  WAKEUP_ENABLE;
  for(i=0;i<0x10000;i++);
  
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~0x0000FFFF) | 0x00000000;
  SYS->GPD_MFPH = (SYS->GPD_MFPH & ~0xFF000000) | 0x00000000;
  
  PC0=1;
  PC1=0;
  PC3=0;
  GPIO_SetMode(PC, (1<<0), GPIO_PMD_OUTPUT); 	//PC0	-	CS
  GPIO_SetMode(PC, (1<<1), GPIO_PMD_OUTPUT);	//PC1	-	SCK
  GPIO_SetMode(PC, (1<<3), GPIO_PMD_OUTPUT);	//PC3	-	MOSI

	WriteET6602(0x009880);
	WriteET6602(0x0109AE);
	WriteET6602(0x02C200);
	WriteET6602(0x032F66);
  WriteET6602(0x048E3B); //output SPI
	//WriteET6602(0x048E38);	//output FM0
  
  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk))|tmp;
}

void ET6602FM0_Init(unsigned char send_power){  
  uint32_t i,tmp;
 //PB10:DSRC_RF TX_EN,PB9:DSRC_RF RX_EN
  tmp= SYS->GPB_MFPH & (SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  GPIO_SetMode(PB, (1<<10)|(1<<9), GPIO_PMD_OUTPUT);
  
  //PB2,WAKEUP_EN#
  PB2=0;
  SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk);
  GPIO_SetMode(PB, (1<<2), GPIO_PMD_OUTPUT);
  
  TXON_DISABLE;
  WAKEUP_ENABLE;
  for(i=0;i<0x10000;i++);
  
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~0x0000FFFF) | 0x00000000;
  SYS->GPD_MFPH = (SYS->GPD_MFPH & ~0xFF000000) | 0x00000000;

  GPIO_SetMode(PC, (1<<0), GPIO_PMD_OUTPUT); 	//PC0	-	CS
  GPIO_SetMode(PC, (1<<1), GPIO_PMD_OUTPUT);	//PC1	-	SCK
  GPIO_SetMode(PC, (1<<3), GPIO_PMD_OUTPUT);	//PC3	-	MOSI

	WriteET6602(0x009880);
	WriteET6602(0x0109AE);
	WriteET6602(0x02C200);
  WriteET6602(0x032F86);
  #if 0
	WriteET6602(0x048E38);	//output FM0
  #else
  WriteET6602(0x04813C);	//output FM0
  #endif
  
  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk))|tmp;
}
void ET6603_Init(unsigned char send_power)
{
  unsigned char ET6603_buf[16];

  uint32_t i,tmp;
  //PB10:DSRC_RF TX_EN,PB9:DSRC_RF RX_EN
  tmp= SYS->GPB_MFPH & (SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
  GPIO_SetMode(PB, (1<<10)|(1<<9), GPIO_PMD_OUTPUT);
  
  //PB2,WAKEUP_EN#
  PB2=0;
  SYS->GPC_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk);
  GPIO_SetMode(PB, (1<<2), GPIO_PMD_OUTPUT);
  
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~0x0000FFFF) | 0x00000000;
  SYS->GPD_MFPH = (SYS->GPD_MFPH & ~0xFF000000) | 0x00000000;

	TXON_DISABLE;
	WAKEUP_ENABLE;
  for(i=0;i<0x100000;i++);
  GPIO_SetMode(PC, (1<<0), GPIO_PMD_OUTPUT); 	//PC0	-	CS
  GPIO_SetMode(PC, (1<<1), GPIO_PMD_OUTPUT);	//PC1	-	SCK
  GPIO_SetMode(PC, (1<<3), GPIO_PMD_OUTPUT);	//PC3	-	MOSI

  PC0=1;				//CS High
  PC1=0;				//SCK Low
  PC3=0;				//MOSI Low

	//memcpy(ET6603_buf,"\x21\x80\x98\xAE\x09\x01\x82",7);//rising edge wakeup
	memcpy(ET6603_buf,"\x21\x80\x98\xAE\x09\x00\x82",7);//falling edge wakeup
	WriteET6603_Sequence(7,ET6603_buf);
/****************************************************************/
	WriteET6603(0x0740);delayus(1);
	//WriteET6603(0x0743);delayus(1);//6db
/****************************************************************/
#ifdef ET_6603_VERSON_05
  memcpy(ET6603_buf,"\x28\x2F\xE1\x0E\xB5\x88",6);
#else
  memcpy(ET6603_buf,"\x28\x2F\xE1\x0C\xB5\x88",6);
#endif
  WriteET6603_Sequence(6,ET6603_buf);

  TXON_DISABLE;
  RXON_DISABLE;
  PC9=0;//SCK_LOW;
  PC11=0;//DATA_LOW;
  PC8=1;//CS_HIGH;

  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk))|tmp;
}
void ET6602_Init_its(unsigned char send_power)
{
}

void T_x1001_OFF(void)
{
}

void ET6603_set_dsrc_tx_channel(uint8_t ch)
{
	unsigned char ET6603_buf[6];
	if(ch)
    {
			memcpy(ET6603_buf,"\x21\xA0\x00\xB6\x01",5);
			WriteET6603_Sequence(5,ET6603_buf);
    }
    else
    { 
			memcpy(ET6603_buf,"\x21\x80\x98\xAE\x09",5);
			WriteET6603_Sequence(5,ET6603_buf);
    }
}

void ET6602_set_dsrc_tx_channel(uint8_t ch)
{
  if(ch)
    {  
        WriteET6602(0x0000A0);
        WriteET6602(0x0101B6);
    }
    else
    {  
        WriteET6602(0x009880);
        WriteET6602(0x0109AE);
    }
}



