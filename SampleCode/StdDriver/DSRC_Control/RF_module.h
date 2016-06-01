#ifndef __RF_module_H
#define __RF_module_H

//#include "DrvGPIO.h"
#if 0
#define TXON_ENABLE      PC7=1 //DrvGPIO_SetBit(E_GPA, 15)
#define TXON_DISABLE     PC7=0 //DrvGPIO_ClrBit(E_GPA, 15)
#define RXON_ENABLE      PC6=1  //DrvGPIO_SetBit(E_GPC, 8)
#define RXON_DISABLE     PC6=0  //DrvGPIO_ClrBit(E_GPC, 8)
#define WAKEUP_ENABLE    PC15=1 //DrvGPIO_ClrBit(E_GPA, 14)
#define WAKEUP_DISABLE   PC15=0 //DrvGPIO_SetBit(E_GPA, 14)
#else
#define TXON_ENABLE      PB10=1 //DrvGPIO_SetBit(E_GPA, 15)
#define TXON_DISABLE     PB10=0 //DrvGPIO_ClrBit(E_GPA, 15)
#define RXON_ENABLE      PB9=1  //DrvGPIO_SetBit(E_GPC, 8)
#define RXON_DISABLE     PB9=0  //DrvGPIO_ClrBit(E_GPC, 8)
#define WAKEUP_ENABLE    PB2=0 //DrvGPIO_ClrBit(E_GPA, 14)
#define WAKEUP_DISABLE   PB2=1 //DrvGPIO_SetBit(E_GPA, 14)
#endif

extern void ET6603_Init(unsigned char send_power);
extern void ET6603FM0_Init(unsigned char send_power);
extern void ET6602_Init(unsigned char send_power);
extern void ET6602FM0_Init(unsigned char send_power);
extern void SKY6605FM0_Init(unsigned char send_power);

#endif
