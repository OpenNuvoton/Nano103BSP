/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (c) 2015 by Nuvoton Technology Corporation                                                   */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   gpio_cunit.c                                                                                         */
/*            The test function of GPIO for CUnit.                                                          */
/* Project:   FA8253                                                                                       */
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
#include "gpio_cunit.h"


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
    {"GPIO API", suite_success_init, suite_success_clean, NULL, NULL, GPIO_ApiTests},
    {"GPIO MACRO", suite_success_init, suite_success_clean, NULL, NULL, GPIO_MacroTests},


    CU_SUITE_INFO_NULL
};


#define GpioPortNum 6
GPIO_T *GpioPort[GpioPortNum] = {
    PA, PB, PC, PD, PE, PF 
};


uint32_t CheckPortPinReserved( uint32_t u32GpioPort, uint32_t u32GpioPin )
{
    //reserved pin return 1 
    
    //PA 15~0
    if( (u32GpioPort==0) && (u32GpioPin == 7) )
        return 1;
    //PB 15~0
    if( (u32GpioPort==1) && (u32GpioPin == 12) )
        return 1;
    //PC 15~0
    if( (u32GpioPort==2) && (u32GpioPin == 4) )
        return 1;
    if( (u32GpioPort==2) && (u32GpioPin == 5) )
        return 1;
    if( (u32GpioPort==2) && (u32GpioPin == 12) )
        return 1;
    if( (u32GpioPort==2) && (u32GpioPin == 13) )
        return 1;
    //PD 15~0
    if( (u32GpioPort==3) && u32GpioPin < 6 )
        return 1;
    if( (u32GpioPort==3) && (u32GpioPin > 7) &&  (u32GpioPin < 14))
        return 1;
    //PE 9~0
    if( (u32GpioPort==4) && u32GpioPin < 5 )
        return 1;
    if( (u32GpioPort==4) && u32GpioPin > 5 )
        return 1;
    //PF 5~0
    if( (u32GpioPort==5) && u32GpioPin == 4 )
        return 1;
    if( (u32GpioPort==5) && u32GpioPin == 5 )
        return 1;
    if( (u32GpioPort==5) && u32GpioPin > 7 )
        return 1;
    //used pin return 0
    return 0;
}


const uint32_t au32GpioModeSel[] = {
    GPIO_PMD_INPUT,
    GPIO_PMD_OUTPUT,
    GPIO_PMD_OPEN_DRAIN,
};

void TestFunc_GPIO_SetMode() 
{
    uint32_t u32TestPort;       //port
    uint8_t u8TestPin;          //pin  
    uint8_t u8GpioModeSelIdx;   //I/O mode     
    
    /* test loop */
    for( u32TestPort=0; u32TestPort<GpioPortNum; u32TestPort++ )
    {         
        for( u8TestPin=0; u8TestPin<GPIO_PIN_MAX; u8TestPin++ )
        {
            //skip reserved pin
            if(CheckPortPinReserved(u32TestPort,u8TestPin)) continue;             
             
            for( u8GpioModeSelIdx=0; u8GpioModeSelIdx<3; u8GpioModeSelIdx++ )
            {
                GPIO_SetMode( GpioPort[u32TestPort], 1<<u8TestPin, au32GpioModeSel[u8GpioModeSelIdx] );
                CU_ASSERT( ((GpioPort[u32TestPort]->MODE) & (3<<(u8TestPin<<1))) == (au32GpioModeSel[u8GpioModeSelIdx]<<(u8TestPin<<1)) ); 
                GPIO_SetMode( GpioPort[u32TestPort], 1<<u8TestPin, GPIO_PMD_OUTPUT );
            }                            
        }
    }
}

const uint32_t au32GpioINTSel[20] = { //5*4
    //interrupt mode, IMD setting, IEN rising setting, IEN falling setting
    GPIO_INT_RISING, GPIO_IMD_EDGE, 1,0,
    GPIO_INT_FALLING, GPIO_IMD_EDGE, 0, 1,
    GPIO_INT_BOTH_EDGE, GPIO_IMD_EDGE, 1, 1,
    GPIO_INT_HIGH, GPIO_IMD_LEVEL, 1, 0,
    GPIO_INT_LOW, GPIO_IMD_LEVEL, 0, 1,
};

void TestFunc_GPIO_EnableInt() 
{
    uint32_t u32TestPort;       //port
    uint8_t u8TestPin;          //pin  
    uint8_t u8GpioINTSelIdx;    //interrupt mode     

    /* test loop */
    for( u32TestPort=0; u32TestPort<GpioPortNum; u32TestPort++ )
    {                        
        for( u8TestPin=0; u8TestPin<GPIO_PIN_MAX; u8TestPin++ )
        { 
            //skip reserved pin
            if(CheckPortPinReserved(u32TestPort,u8TestPin)) continue; 
            
            for( u8GpioINTSelIdx=0; u8GpioINTSelIdx<20; u8GpioINTSelIdx+=4 )
            {
                //GPIO_EnableInt
                GPIO_EnableInt( GpioPort[u32TestPort], u8TestPin, au32GpioINTSel[u8GpioINTSelIdx]);  
                CU_ASSERT( ((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin)) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin) ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin)) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin) ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16))) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16)) ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin)) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin) ); 

                //GPIO_DisableInt
                GPIO_DisableInt( GpioPort[u32TestPort], u8TestPin);  
                CU_ASSERT( ((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin)) == 0 ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16))) == 0 ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin)) == 0 ); 

                //GPIO_EnableEINT0
                GPIO_EnableEINT0( GpioPort[u32TestPort], u8TestPin, au32GpioINTSel[u8GpioINTSelIdx]);  
                CU_ASSERT( ((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin)) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin) ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16))) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16)) ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin)) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin) ); 

                //GPIO_DisableEINT0
                GPIO_DisableEINT0( GpioPort[u32TestPort], u8TestPin);  
                CU_ASSERT( ((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin)) == 0 ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16))) == 0 ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin)) == 0 ); 

                //GPIO_EnableEINT1
                GPIO_EnableEINT1( GpioPort[u32TestPort], u8TestPin, au32GpioINTSel[u8GpioINTSelIdx]);  
                CU_ASSERT( ((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin)) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin) ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16))) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16)) ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin)) 
                                                          == (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin) ); 

                //GPIO_DisableEINT1
                GPIO_DisableEINT1( GpioPort[u32TestPort], u8TestPin);  
                CU_ASSERT( ((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx+1]<<u8TestPin)) == 0 ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+2]<<(u8TestPin+16))) == 0 ); 
                CU_ASSERT( ((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx+3]<<u8TestPin)) == 0 ); 
            }
       }
    }            
    
}

const uint32_t au32GpioDbClkSel[2] = { 
    GPIO_DBCLKSRC_HCLK,
    GPIO_DBCLKSRC_IRC10K,
};

const uint32_t au32GpioDbCycleSel[16] = { 
    GPIO_DBCLKSEL_1,
    GPIO_DBCLKSEL_2,
    GPIO_DBCLKSEL_4,
    GPIO_DBCLKSEL_8,
    GPIO_DBCLKSEL_16,
    GPIO_DBCLKSEL_32, 
    GPIO_DBCLKSEL_64,
    GPIO_DBCLKSEL_128,
    GPIO_DBCLKSEL_256,
    GPIO_DBCLKSEL_512,
    GPIO_DBCLKSEL_1024,
    GPIO_DBCLKSEL_2048,
    GPIO_DBCLKSEL_4096,
    GPIO_DBCLKSEL_8192,
    GPIO_DBCLKSEL_16384,
    GPIO_DBCLKSEL_32768,
};

void TestFunc_GPIO_TestMacro() 
{ 
    uint32_t u32TestPort;           //port
    uint8_t u8TestPin;              //pin  
    uint8_t u8GpioDbClkSelIdx;      //debounce clock souce  
    uint8_t u8GpioDbCycleSelIdx;    //debounce sample cycle       

    //set PF2 and PF3 to GPIO
    SYS->GPF_MFPL = 0;
    
    /* test pin value : GPIO_PIN_DATA */ 
    for( u32TestPort=0; u32TestPort<GpioPortNum; u32TestPort++ )
    {         
        for( u8TestPin=0; u8TestPin<GPIO_PIN_MAX; u8TestPin++ )
        { 
            //skip reserved pin
            if(CheckPortPinReserved(u32TestPort,u8TestPin)) continue;             
            
            //skip UART and ICE pin
            if( u32TestPort==1 && (u8TestPin==0 || u8TestPin==1) ) continue;
            if( u32TestPort==5 && (u8TestPin==0 || u8TestPin==1) ) continue;             
           
            //output 0
            GPIO_PIN_ADDR( u32TestPort, u8TestPin)=0;
            CLK_SysTickDelay(100);
            CU_ASSERT( (inp32( 0x50004200 + (u32TestPort<<6) + (u8TestPin<<2) )) == 0 );           

            //output 1
            GPIO_PIN_ADDR( u32TestPort, u8TestPin)=1; 
            CLK_SysTickDelay(100);
            CU_ASSERT( (inp32( 0x50004200 + (u32TestPort<<6) + (u8TestPin<<2) )) == 1 );            
        }            
    }    

    /* test loop */
    for( u32TestPort=0; u32TestPort<GpioPortNum; u32TestPort++ )
    {                
        for( u8TestPin=0; u8TestPin<GPIO_PIN_MAX; u8TestPin++ )
        { 
            //skip reserved pin
            if(CheckPortPinReserved(u32TestPort,u8TestPin)) continue;             
            
            //skip UART and ICE pin
            if( u32TestPort==1 && (u8TestPin==0 || u8TestPin==1) ) continue;
            if( u32TestPort==5 && (u8TestPin==0 || u8TestPin==1) ) continue;             

            //enable debounce
            GPIO_ENABLE_DEBOUNCE( GpioPort[u32TestPort], (1<<u8TestPin) );  
            CU_ASSERT( ((GpioPort[u32TestPort]->DBEN) & (1<<u8TestPin)) == (1<<u8TestPin) );              
  
            //disable debounce
            GPIO_DISABLE_DEBOUNCE( GpioPort[u32TestPort], (1<<u8TestPin) );  
            CU_ASSERT( ((GpioPort[u32TestPort]->DBEN) & (1<<u8TestPin)) == 0 ); 

            //disable digital path
            GPIO_DISABLE_DIGITAL_PATH( GpioPort[u32TestPort], (1<<u8TestPin) );
            CU_ASSERT( ((GpioPort[u32TestPort]->DINOFF) & (1<<(u8TestPin+16))) == (1<<(u8TestPin+16)) );              
             
            //enable digital path
            GPIO_ENABLE_DIGITAL_PATH( GpioPort[u32TestPort], (1<<u8TestPin) );
            CU_ASSERT( ((GpioPort[u32TestPort]->DINOFF) & (1<<(u8TestPin+16))) == 0 );
 
            //enable data write mask
            GPIO_ENABLE_DOUT_MASK( GpioPort[u32TestPort], (1<<u8TestPin) );
            CU_ASSERT( ((GpioPort[u32TestPort]->DATMSK) & (1<<u8TestPin)) ==  (1<<u8TestPin) );                
            
            //disable data write mask
            GPIO_DISABLE_DOUT_MASK( GpioPort[u32TestPort], (1<<u8TestPin) );
            CU_ASSERT( ((GpioPort[u32TestPort]->DATMSK) & (1<<u8TestPin)) == 0 );     

            //enable pull-up
            GPIO_ENABLE_PULL_UP( GpioPort[u32TestPort], (1<<u8TestPin) );
            CU_ASSERT( ((GpioPort[u32TestPort]->PUEN) & (1<<u8TestPin)) ==  (1<<u8TestPin) );                
            
            //disable pull-up
            GPIO_DISABLE_PULL_UP( GpioPort[u32TestPort], (1<<u8TestPin) );
            CU_ASSERT( ((GpioPort[u32TestPort]->PUEN) & (1<<u8TestPin)) == 0 );     

            //printf("%x, %x, %x\n", u32TestPort, u8TestPin, GpioPort[u32TestPort]->ISRC);
  
        }
    }

    //set debounce
    for( u8GpioDbClkSelIdx=0; u8GpioDbClkSelIdx<2; u8GpioDbClkSelIdx++ )
    {
        for( u8GpioDbCycleSelIdx=0; u8GpioDbCycleSelIdx<GPIO_PIN_MAX; u8GpioDbCycleSelIdx++ )
        {
            GPIO_SET_DEBOUNCE_TIME( au32GpioDbClkSel[u8GpioDbClkSelIdx], au32GpioDbCycleSel[u8GpioDbCycleSelIdx]);
            CU_ASSERT( (GPIO->DBCTL & GPIO_DBCTL_DBCLKSRC_Msk) == au32GpioDbClkSel[u8GpioDbClkSelIdx] );   
            CU_ASSERT( (GPIO->DBCTL & GPIO_DBCTL_DBCLKSEL_Msk) == au32GpioDbCycleSel[u8GpioDbCycleSelIdx] );               
        }        
    }
   
}

volatile uint32_t  *au32GpioPortPin[(16*5)+8] = { 
    &PA0, &PA1, &PA2, &PA3, &PA4, &PA5, &PA6, 0, &PA8, &PA9, &PA10, &PA11, &PA12, &PA13, &PA14, &PA15,
    &PB0, &PB1, &PB2, &PB3, &PB4, &PB5, &PB6, &PB7, &PB8, &PB9, &PB10, &PB11, 0, &PB13, &PB14, &PB15, 
    &PC0, &PC1, &PC2, &PC3, 0, 0, &PC6, &PC7, &PC8, &PC9, &PC10, &PC11, 0, 0, &PC14, &PC15, 
    0, 0, 0, 0, 0, 0, &PD6, &PD7, 0, 0, 0, 0, 0, 0, &PD14, &PD15,
    0, 0, 0, 0, 0, &PE5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    &PF0, &PF1, &PF2, &PF3, 0, 0, &PF6, &PF7
};

void TestFunc_GPIO_TestINT() 
{ 
    uint32_t u32TestPort;           //port
    uint8_t u8TestPin;              //pin      
    
    //set interrupt clock on mode
    GPIO->DBCTL = (GPIO->DBCTL & (~GPIO_DBCTL_ICLKON_Msk)) | GPIO_ICLK_OFF;
    CU_ASSERT( !(GPIO->DBCTL & GPIO_DBCTL_ICLKON_Msk) );
    GPIO->DBCTL = (GPIO->DBCTL & (~GPIO_DBCTL_ICLKON_Msk)) | GPIO_ICLK_ON;
    CU_ASSERT( GPIO->DBCTL & GPIO_DBCTL_ICLKON_Msk );
    
    /* test loop */ 
    for( u32TestPort=0; u32TestPort<GpioPortNum; u32TestPort++ )
    {                 
        for( u8TestPin=0; u8TestPin<GPIO_PIN_MAX; u8TestPin++ )
        { 
            //skip reserved pin
            if(CheckPortPinReserved(u32TestPort,u8TestPin)) continue; 
                       
            //skip UART and ICE pin
            if( u32TestPort==1 && (u8TestPin==0 || u8TestPin==1) ) continue;
            if( u32TestPort==5 && (u8TestPin==0 || u8TestPin==1) ) continue;             
          
            //set interrupt enable
            GPIO_EnableInt( GpioPort[u32TestPort], u8TestPin, GPIO_INT_BOTH_EDGE );  
  
            //set GPIO mode
            GPIO_SetMode( GpioPort[u32TestPort], 1<<u8TestPin, GPIO_PMD_OUTPUT);
 
            //output 0
            GPIO_SET_OUT_DATA( GpioPort[u32TestPort], 1<<u8TestPin );            
            CU_ASSERT( (GPIO_GET_IN_DATA( GpioPort[u32TestPort] )) & (1<<u8TestPin) );   
           
            //output 1
            GPIO_SET_OUT_DATA( GpioPort[u32TestPort], 0<<u8TestPin );
            CLK_SysTickDelay(100);
            CU_ASSERT( !((GPIO_GET_IN_DATA( GpioPort[u32TestPort] )) & (1<<u8TestPin)) );            
                     
            //Get interrupt flag
            CU_ASSERT( GPIO_GET_INT_FLAG( GpioPort[u32TestPort], 1<<u8TestPin) & (1<<u8TestPin) );           

            //Clear interrupt flag
            GPIO_CLR_INT_FLAG( GpioPort[u32TestPort], 1<<u8TestPin);
            CU_ASSERT( GPIO_GET_INT_FLAG( GpioPort[u32TestPort], 1<<u8TestPin) == 0 );         
            
            //Toggle GPIO
            CLK_SysTickDelay(100);
            GPIO_TOGGLE( *(au32GpioPortPin[ (u32TestPort<<4) + u8TestPin ]) );       
            
            //Get interrupt flag
            CLK_SysTickDelay(100);
            CU_ASSERT( GPIO_GET_INT_FLAG( GpioPort[u32TestPort], 1<<u8TestPin) & (1<<u8TestPin) );          

            //Clear interrupt flag
            GPIO_CLR_INT_FLAG( GpioPort[u32TestPort], 1<<u8TestPin);
            CU_ASSERT( GPIO_GET_INT_FLAG( GpioPort[u32TestPort], 1<<u8TestPin) == 0 );        
                       
        }            
    }    
    
 
}




CU_TestInfo GPIO_ApiTests[] =
{
    {"Testing GPIO_SetMode Function:", TestFunc_GPIO_SetMode},  
    {"Testing GPIO_EnableInt Function:", TestFunc_GPIO_EnableInt},  


    CU_TEST_INFO_NULL
};

CU_TestInfo GPIO_MacroTests[] =
{ 
   {"Testing GPIO_TestMacro Function:", TestFunc_GPIO_TestMacro},
   {"Testing GPIO_TestINT Function:", TestFunc_GPIO_TestINT}, 


    CU_TEST_INFO_NULL
};
