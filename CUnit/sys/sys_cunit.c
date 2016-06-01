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
#include "sys_cunit.h"




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
    {"Test IP: SYS", suite_success_init, suite_success_clean,NULL, NULL, SYS_TestCases},
    CU_SUITE_INFO_NULL
};

#define FLAG_ADDR       0x20000FFC

void TestFunc_SYS_TestReset() 
{
    uint32_t u32ResetFlag = 0xFFFFFFFF;    
    u32ResetFlag = M32(FLAG_ADDR);
    SYS_UnlockReg();
    switch((u32ResetFlag))
    {

        case SYS_RSTSTS_PORF_Msk:
            
            /* Check Reset Status */
            CU_ASSERT( SYS_IS_POR_RST() != 0 );
            SYS_ClearResetSrc(SYS_RSTSTS_PORF_Msk);
        
            /* Save System Reset Flag */
            u32ResetFlag = SYS_RSTSTS_SYSRF_Msk; 
            M32(FLAG_ADDR) = u32ResetFlag;
            printf(" System Reset");
             
            /* Set System Reset */
            UART_WAIT_TX_EMPTY(UART0);  
            outp32(0xE000ED0C, 0x05FA0004); 
            
        break;   

        case SYS_RSTSTS_SYSRF_Msk:

            /* Check Reset Status */
            CU_ASSERT( SYS_IS_SYSTEM_RST() != 0 );
            SYS_ClearResetSrc(SYS_RSTSTS_SYSRF_Msk);
        
            /* Save CPU Reset Flag */
            u32ResetFlag = SYS_RSTSTS_CPURF_Msk; 
            M32(FLAG_ADDR) = u32ResetFlag;
            printf(" PIN Reset");
            
            /* Set CPU Reset */
            UART_WAIT_TX_EMPTY(UART0);
            SYS_ResetCPU();
            
        break;   
            
        case SYS_RSTSTS_CPURF_Msk:
    
            /* Check Reset Status */
            CU_ASSERT( SYS_IS_CPU_RST() != 0 );
            SYS_ClearResetSrc(SYS_RSTSTS_CPURF_Msk);
            
            /* Save WDT Flag */
            u32ResetFlag = SYS_RSTSTS_WDTRF_Msk; 
            M32(FLAG_ADDR) = u32ResetFlag;
            printf(" WDT Reset");
                     
            /* Set WDT Reset */     
            UART_WAIT_TX_EMPTY(UART0);        
            CLK->PWRCTL |= CLK_PWRCTL_LXTEN_Msk;
            while((CLK->STATUS & CLK_STATUS_LIRCSTB_Msk) != CLK_STATUS_LIRCSTB_Msk);
            CLK->APBCLK |= CLK_APBCLK_WDTCKEN_Msk; 
            //CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_WDT_S_Msk) | CLK_CLKSEL1_WDTSEL_LIRC; 
            WDT->CTL = (5UL << WDT_CTL_WTIS_Pos) | WDT_CTL_WDTEN_Msk |WDT_CTL_WKEN_Msk | WDT_CTL_RSTEN_Msk; // 6.55sec at 10KHz

            while(1);
            
        //break;
            
        case SYS_RSTSTS_WDTRF_Msk:

            /* Check Reset Status */
            CU_ASSERT( SYS_IS_WDT_RST() != 0 );                 
            SYS_ClearResetSrc(SYS_RSTSTS_WDTRF_Msk);  
            printf(" Pass");          
                        
            /* Clear Reset Flag */
            u32ResetFlag = 0xFFFFFFFF; 
            SYS->RSTSTS = SYS->RSTSTS;          
            M32(FLAG_ADDR) = u32ResetFlag;        
            UART_WAIT_TX_EMPTY(UART0);      
                      
        break;
                        
        default:  
       
            CU_ASSERT( SYS_IS_RSTPIN_RST() != 0 );
            SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_PINRF_Msk);
            CU_ASSERT( SYS_IS_RSTPIN_RST() == 0 );        
        
            /* Clear Flag before Test */
            SYS->RSTSTS = SYS->RSTSTS;  
        
            /* Save Chip Reset Flag */
            u32ResetFlag = SYS_RSTSTS_PORF_Msk;
            M32(FLAG_ADDR) = u32ResetFlag;  
            printf(" Chip Reset"); 

            /* Set Chip Reset */
            UART_WAIT_TX_EMPTY(UART0);
            SYS_ResetChip();
            
        break;            
    }
    SYS_LockReg();
}


// void NMI_Handler()
// {

//     CU_ASSERT( SYS_GET_BOD_INT_FLAG() != 0 );  
//     
//     //clear BOD flag
//     SYS_CLEAR_BOD_INT_FLAG();
//     CU_ASSERT( SYS_GET_BOD_INT_FLAG() == 0 );      
// }

// void BOD_IRQHandler()
// {
//     SYS_CLEAR_BOD_INT_FLAG();
// }

void TestFunc_SYS_GetBODStatus() 
{
    //wait UART print message finish before test
    UART_WAIT_TX_EMPTY(UART0);    
    
    /* BOD condition */
    //SYS->BODCTL = SYS->BODCTL & (~SYS_BODCTL_BODVL_Msk) | SYS_BODCTL_BODVL_4_5V;
    //SYS->BODCTL = SYS->BODCTL & (~SYS_BODCTL_BODEN_Msk) | SYS_BODCTL_BODEN_Msk;

    //wait BOD happen
    //while((SYS->BODCTL & SYS_BODCTL_BODOUT_Msk) != SYS_BODCTL_BODOUT_Msk);
    //CU_ASSERT(SYS_GetBODStatus() != 0);

    /* not BOD condition */
    SYS->BODCTL &= ~SYS_BODCTL_BODEN_Msk;
    CU_ASSERT(SYS_GetBODStatus() == 0);
}

void TestFunc_SYS_GetResetSrc() 
{
    CU_ASSERT( SYS->RSTSTS == SYS_GetResetSrc() );   
}

void TestFunc_SYS_IsRegLocked() 
{
    /* Lock protected registers */
    SYS_LockReg();
    CU_ASSERT( SYS_IsRegLocked() == 1 );  
    
    /* Unlock protected registers */
    SYS_UnlockReg();    
    CU_ASSERT( SYS_IsRegLocked() == 0 );     
}

void TestFunc_SYS_ReadPDID() 
{
    CU_ASSERT( SYS->PDID == SYS_ReadPDID() );
}

//#define ModuleRstSelNum 23*3
uint32_t au32ModuleRstSel[] = {
// CHIP_RST ,(SYS_BASE+0x08),SYS_IPRST1_CHIPRST_Msk  ,
// CPU_RST  ,(SYS_BASE+0x08),SYS_IPRST1_CPURST_Msk ,
DMA_RST  ,(SYS_BASE+0x08),SYS_IPRST1_PDMARST_Msk  ,

GPIO_RST ,(SYS_BASE+0x0C),SYS_IPRST2_GPIORST_Msk ,
TMR0_RST ,(SYS_BASE+0x0C),SYS_IPRST2_TMR0RST_Msk ,	
TMR1_RST ,(SYS_BASE+0x0C),SYS_IPRST2_TMR1RST_Msk ,
TMR2_RST ,(SYS_BASE+0x0C),SYS_IPRST2_TMR2RST_Msk ,
TMR3_RST ,(SYS_BASE+0x0C),SYS_IPRST2_TMR3RST_Msk ,
//DSRC_RST ,(SYS_BASE+0x0C),SYS_IPRST2_DSRCRST_Msk ,
I2C0_RST ,(SYS_BASE+0x0C),SYS_IPRST2_I2C0RST_Msk ,	
I2C1_RST ,(SYS_BASE+0x0C),SYS_IPRST2_I2C1RST_Msk ,
SPI0_RST ,(SYS_BASE+0x0C),SYS_IPRST2_SPI0RST_Msk ,
SPI1_RST ,(SYS_BASE+0x0C),SYS_IPRST2_SPI1RST_Msk ,
SPI2_RST ,(SYS_BASE+0x0C),SYS_IPRST2_SPI2RST_Msk ,
SPI3_RST ,(SYS_BASE+0x0C),SYS_IPRST2_SPI3RST_Msk ,
// UART0_RST,(SYS_BASE+0x0C),SYS_IPRST2_UART0RST_Msk,
UART1_RST,(SYS_BASE+0x0C),SYS_IPRST2_UART1RST_Msk,
PWM0_RST ,(SYS_BASE+0x0C),SYS_IPRST2_PWM0RST_Msk ,	
ACMP0_RST  ,(SYS_BASE+0x0C),SYS_IPRST2_ACMP0RST_Msk ,	
ADC_RST  ,(SYS_BASE+0x0C),SYS_IPRST2_ADCRST_Msk  ,	
SC0_RST  ,(SYS_BASE+0x0C),SYS_IPRST2_SC0RST_Msk  ,
SC1_RST  ,(SYS_BASE+0x0C),SYS_IPRST2_SC1RST_Msk  ,
};

void TestFunc_SYS_ResetModule() 
{
    uint8_t u8ModuleRstSelIdx;

		SYS_UnlockReg();
	
    //wait UART print message finish before test 
    UART_WAIT_TX_EMPTY(UART0);  
           
    /* Test loop */       
    for( u8ModuleRstSelIdx=0; u8ModuleRstSelIdx<sizeof(au32ModuleRstSel)/sizeof(uint32_t); u8ModuleRstSelIdx+=3)
    {        
        *(volatile uint32_t *)((uint32_t)&SYS->IPRST1 + (au32ModuleRstSel[u8ModuleRstSelIdx] >> 24)) |= 1 << (au32ModuleRstSel[u8ModuleRstSelIdx] & 0x00ffffff);
        CU_ASSERT( (inp32(au32ModuleRstSel[u8ModuleRstSelIdx+1]) & au32ModuleRstSel[u8ModuleRstSelIdx+2]) == au32ModuleRstSel[u8ModuleRstSelIdx+2] );
// 			  printf("%x  -> %x  \n", inp32(au32ModuleRstSel[u8ModuleRstSelIdx+1]) & au32ModuleRstSel[u8ModuleRstSelIdx+2], au32ModuleRstSel[u8ModuleRstSelIdx+2]);
        *(volatile uint32_t *)((uint32_t)&SYS->IPRST1 + (au32ModuleRstSel[u8ModuleRstSelIdx] >> 24)) &= ~(1 << (au32ModuleRstSel[u8ModuleRstSelIdx]& 0x00ffffff));
        CU_ASSERT( (inp32(au32ModuleRstSel[u8ModuleRstSelIdx+1]) & au32ModuleRstSel[u8ModuleRstSelIdx+2]) == 0 );   
// 			  printf("%x  -> 0  \n", inp32(au32ModuleRstSel[u8ModuleRstSelIdx+1]) & au32ModuleRstSel[u8ModuleRstSelIdx+2]);
    }
    
    //restore UART setting after test   		    
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk) | CLK_CLKSEL1_UART0SEL_HXT;
    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_UART0DIV_Msk) | 0;
    UART0->BAUD = UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER(12000000, 115200);
    UART0->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

const uint32_t au32BODFnSel[2] = {    
    SYS_BODCTL_BOD_INTERRUPT_EN,
    SYS_BODCTL_BOD_RST_EN,
};

const uint32_t au32BODLvSel[15] = {
	SYS_BODCTL_BODVL_1_7V  ,
	SYS_BODCTL_BODVL_1_8V  ,
	SYS_BODCTL_BODVL_1_9V  ,
	SYS_BODCTL_BODVL_2_0V  ,
	SYS_BODCTL_BODVL_2_1V  ,
	SYS_BODCTL_BODVL_2_2V  ,
	SYS_BODCTL_BODVL_2_3V  ,
	SYS_BODCTL_BODVL_2_4V  ,
	SYS_BODCTL_BODVL_2_5V  ,
	SYS_BODCTL_BODVL_2_6V  ,
	SYS_BODCTL_BODVL_2_7V  ,
	SYS_BODCTL_BODVL_2_8V  ,
	SYS_BODCTL_BODVL_2_9V  ,
	SYS_BODCTL_BODVL_3_0V  ,
	SYS_BODCTL_BODVL_3_1V  ,
};

void TestFunc_SYS_EnableBOD() 
{
    uint8_t u8BODFnSelIdx; //A BOD function
    uint8_t u8BODLvSelIdx; //B BOD level
	
		SYS_UnlockReg();
  
    /* Test loop */
    for( u8BODFnSelIdx=0; u8BODFnSelIdx<2; u8BODFnSelIdx++)
    {
        for( u8BODLvSelIdx=0; u8BODLvSelIdx<15; u8BODLvSelIdx++)
        {
//             //skip reset function 3.8V, 4.5V case
//             if( u8BODFnSelIdx==1 && u8BODLvSelIdx>1 && u8BODLvSelIdx<4 ) continue;

            SYS_EnableBOD( au32BODFnSel[u8BODFnSelIdx], au32BODLvSel[u8BODLvSelIdx]);
            CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODEN_Msk) == SYS_BODCTL_BODEN_Msk );
            CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODREN_Msk) == (u8BODFnSelIdx<<SYS_BODCTL_BODREN_Pos) );
            CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == (u8BODLvSelIdx<<SYS_BODCTL_BODVL_Pos) );
        }
    }

    //disable BOD after test
    SYS_DisableBOD();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODEN_Msk) == 0 );
}

void TestFunc_SYS_TestMacro()
{
		SYS_UnlockReg();
	
    /* BOD condition */
    //BOD reset function
    SYS_ENABLE_BOD_RST();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODREN_Msk) == SYS_BODCTL_BODREN_Msk );

    //BOD interrupt function
    SYS_DISABLE_BOD_RST();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODREN_Msk) == 0 );

    //BOD level
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_1_7V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_1_7V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_1_8V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_1_8V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_1_9V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_1_9V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_0V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_0V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_1V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_1V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_2V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_2V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_3V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_3V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_4V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) ==SYS_BODCTL_BODVL_2_4V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_5V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_5V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_6V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_6V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_7V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_7V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_8V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_8V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_2_9V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_2_9V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_3_0V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_3_0V );
    SYS_SET_BOD_LEVEL(SYS_BODCTL_BODVL_3_1V);
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODVL_Msk) == SYS_BODCTL_BODVL_3_1V );
    
    //BOD enable
    SYS_ENABLE_BOD();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODEN_Msk) == SYS_BODCTL_BODEN_Msk );

//     //wait BOD happen
// 		SYS->BODCTL |= 0x8085;  
//     while( (SYS->BODCTL & SYS_BODCTL_BODOUT_Msk) != SYS_BODCTL_BODOUT_Msk );
//     CU_ASSERT( SYS_GET_BOD_OUTPUT() != 0 );
//     CU_ASSERT( SYS_GET_BOD_INT_FLAG() != 0 );   

    //clear BOD flag
    SYS_CLEAR_BOD_INT_FLAG();
    CU_ASSERT( SYS_GET_BOD_INT_FLAG() == 0 );

    /* not BOD condition */
    //BOD disable
    SYS_DISABLE_BOD();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_BODEN_Msk) == 0 );
    CU_ASSERT( SYS_GET_BOD_OUTPUT() == 0 );    

    //enable low power mode
    SYS_ENABLE_LPBOD();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_LPBODEN_Msk) == SYS_BODCTL_LPBODEN_Msk );

    //disable low power mode
    SYS_DISABLE_LPBOD();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_LPBODEN_Msk) == 0 );

    //diable LVR
    SYS_DISABLE_LVR();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_LVREN_Msk) == 0 );

    //enable LVR
    SYS_ENABLE_LVR();
    CU_ASSERT( (SYS->BODCTL & SYS_BODCTL_LVREN_Msk) == SYS_BODCTL_LVREN_Msk );

    //disable POR
    SYS_DISABLE_POR();
    CU_ASSERT( SYS->PORCTL == 0x5AA5 );

    //enable POR
    SYS_ENABLE_POR();
    CU_ASSERT( SYS->PORCTL != 0x5AA5 );

    //clear reset source
    SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_CPURF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_CPURF_Msk) == 0 );
    SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_SYSRF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_SYSRF_Msk) == 0 );
    SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_BODRF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_BODRF_Msk) == 0 );
    SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_LVRF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_LVRF_Msk) == 0 );
    SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_WDTRF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_WDTRF_Msk) == 0 );
    SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_PINRF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_PINRF_Msk) == 0 );
    SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_PORF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_PORF_Msk) == 0 );
		SYS_CLEAR_RST_SOURCE(SYS_RSTSTS_LOCKRF_Msk);
    CU_ASSERT( (SYS->RSTSTS & SYS_RSTSTS_LOCKRF_Msk) == 0 );
}

void TestFunc_SYS_TestConstant1() 
{
		//wait UART print message finish before test
    UART_WAIT_TX_EMPTY(UART0);
    
    /*----- New Multi-Function constant definitions -----*/    

    /* PA0 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk)) | SYS_GPA_MFPL_PA0MFP_ADC_CH0;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA0MFP_Msk) == (1<<SYS_GPA_MFPL_PA0MFP_Pos) );    
	  SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk)) | SYS_GPA_MFPL_PA0MFP_ACMP0_P;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA0MFP_Msk) == (2<<SYS_GPA_MFPL_PA0MFP_Pos) );
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk)) | SYS_GPA_MFPL_PA0MFP_TM2_EXT;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA0MFP_Msk) == (3<<SYS_GPA_MFPL_PA0MFP_Pos) );        
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk)) | SYS_GPA_MFPL_PA0MFP_PWM0_CH2;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA0MFP_Msk) == (5<<SYS_GPA_MFPL_PA0MFP_Pos) );     
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk)) | SYS_GPA_MFPL_PA0MFP_SPI3_MOSI1;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA0MFP_Msk) == (6<<SYS_GPA_MFPL_PA0MFP_Pos) );  
//    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk)) | SYS_GPA_MFPL_PA0MFP_DSRC_TEST_EN;
//    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA0MFP_Msk) == (7<<SYS_GPA_MFPL_PA0MFP_Pos) );
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk)) | SYS_GPA_MFPL_PA0MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA0MFP_Msk) == 0 );    
    
    /* PA1 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA1MFP_Msk)) | SYS_GPA_MFPL_PA1MFP_ADC_CH1;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA1MFP_Msk) == (1<<SYS_GPA_MFPL_PA1MFP_Pos) );    
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA1MFP_Msk)) | SYS_GPA_MFPL_PA1MFP_ACMP0_N;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA1MFP_Msk) == (2<<SYS_GPA_MFPL_PA1MFP_Pos) );   
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA1MFP_Msk)) | SYS_GPA_MFPL_PA1MFP_SPI3_MISO1;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA1MFP_Msk) == (6<<SYS_GPA_MFPL_PA1MFP_Pos) );  
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA1MFP_Msk)) | SYS_GPA_MFPL_PA1MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA1MFP_Msk) == 0 ); 

    /* PA2 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA2MFP_Msk)) | SYS_GPA_MFPL_PA2MFP_ADC_CH2;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA2MFP_Msk) == (1<<SYS_GPA_MFPL_PA2MFP_Pos) );    
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA2MFP_Msk)) | SYS_GPA_MFPL_PA2MFP_UART1_RXD;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA2MFP_Msk) == (5<<SYS_GPA_MFPL_PA2MFP_Pos) );     
//    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA2MFP_Msk)) | SYS_GPA_MFPL_PA2MFP_DSRC_TEST_CS0;
//    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA2MFP_Msk) == (7<<SYS_GPA_MFPL_PA2MFP_Pos) ); 
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA2MFP_Msk)) | SYS_GPA_MFPL_PA2MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA2MFP_Msk) == 0 ); 

    /* PA3 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk)) | SYS_GPA_MFPL_PA3MFP_ADC_CH3;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA3MFP_Msk) == (1<<SYS_GPA_MFPL_PA3MFP_Pos) );     
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk)) | SYS_GPA_MFPL_PA3MFP_UART1_TXD;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA3MFP_Msk) == (5<<SYS_GPA_MFPL_PA3MFP_Pos) );        
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk)) | SYS_GPA_MFPL_PA3MFP_SPI3_MOSI0;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA3MFP_Msk) == (6<<SYS_GPA_MFPL_PA3MFP_Pos) );  
//    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk)) | SYS_GPA_MFPL_PA3MFP_DSRC_TEST_CS1;
//    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA3MFP_Msk) == (7<<SYS_GPA_MFPL_PA3MFP_Pos) );  
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk)) | SYS_GPA_MFPL_PA3MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA3MFP_Msk) == 0 );  

    /* PA4 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA4MFP_Msk)) | SYS_GPA_MFPL_PA4MFP_ADC_CH4;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA4MFP_Msk) == (1<<SYS_GPA_MFPL_PA4MFP_Pos) );   
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA4MFP_Msk)) | SYS_GPA_MFPL_PA4MFP_I2C0_SDA;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA4MFP_Msk) == (5<<SYS_GPA_MFPL_PA4MFP_Pos) );
		SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA4MFP_Msk)) | SYS_GPA_MFPL_PA4MFP_SPI3_MISO0;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA4MFP_Msk) == (6<<SYS_GPA_MFPL_PA4MFP_Pos) );
//		SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA4MFP_Msk)) | SYS_GPA_MFPL_PA4MFP_DSRC_TEST_CS2;
//    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA4MFP_Msk) == (7<<SYS_GPA_MFPL_PA4MFP_Pos) );
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA4MFP_Msk)) | SYS_GPA_MFPL_PA4MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA4MFP_Msk) == 0 );   
 
    /* PA5 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA5MFP_Msk)) | SYS_GPA_MFPL_PA5MFP_ADC_CH5;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA5MFP_Msk) == (1<<SYS_GPA_MFPL_PA5MFP_Pos) );  
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA5MFP_Msk)) | SYS_GPA_MFPL_PA5MFP_I2C0_SCL;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA5MFP_Msk) == (5<<SYS_GPA_MFPL_PA5MFP_Pos) );  
		SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA5MFP_Msk)) | SYS_GPA_MFPL_PA5MFP_SPI3_CLK;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA5MFP_Msk) == (6<<SYS_GPA_MFPL_PA5MFP_Pos) );  
//    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA5MFP_Msk)) | SYS_GPA_MFPL_PA5MFP_DSRC_TEST_CS3;
//    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA5MFP_Msk) == (7<<SYS_GPA_MFPL_PA5MFP_Pos) ); 
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA5MFP_Msk)) | SYS_GPA_MFPL_PA5MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA5MFP_Msk) == 0 ); 

    /* PA6 */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_ADC_CH6;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == (1<<SYS_GPA_MFPL_PA6MFP_Pos) );
		SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_ACMP0_O;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == (2<<SYS_GPA_MFPL_PA6MFP_Pos) );		
		SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_TM3_EXT;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == (3<<SYS_GPA_MFPL_PA6MFP_Pos) );
		SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_TM3_CNT;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == (4<<SYS_GPA_MFPL_PA6MFP_Pos) );
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_PWM0_CH3;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == (5<<SYS_GPA_MFPL_PA6MFP_Pos) ); 
		SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_SPI3_SS0;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == (6<<SYS_GPA_MFPL_PA6MFP_Pos) ); 
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_TM3_OUT;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == (7<<SYS_GPA_MFPL_PA6MFP_Pos) ); 
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk)) | SYS_GPA_MFPL_PA6MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPL & SYS_GPA_MFPL_PA6MFP_Msk) == 0 ); 
    
     /* PA7 */

    
     /* PA8 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk)) | SYS_GPA_MFPH_PA8MFP_I2C0_SDA;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA8MFP_Msk) == (1<<SYS_GPA_MFPH_PA8MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk)) | SYS_GPA_MFPH_PA8MFP_TM0_CNT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA8MFP_Msk) == (2<<SYS_GPA_MFPH_PA8MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk)) | SYS_GPA_MFPH_PA8MFP_SC0_CLK;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA8MFP_Msk) == (3<<SYS_GPA_MFPH_PA8MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk)) | SYS_GPA_MFPH_PA8MFP_SPI2_SS0;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA8MFP_Msk) == (4<<SYS_GPA_MFPH_PA8MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk)) | SYS_GPA_MFPH_PA8MFP_TM0_OUT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA8MFP_Msk) == (5<<SYS_GPA_MFPH_PA8MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk)) | SYS_GPA_MFPH_PA8MFP_UART1_nCTS;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA8MFP_Msk) == (6<<SYS_GPA_MFPH_PA8MFP_Pos) );
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk)) | SYS_GPA_MFPH_PA8MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA8MFP_Msk) == 0 );    
    
     /* PA9 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_I2C0_SCL;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == (1<<SYS_GPA_MFPH_PA9MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_TM1_CNT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == (2<<SYS_GPA_MFPH_PA9MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_SC0_DAT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == (3<<SYS_GPA_MFPH_PA9MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_SPI2_CLK;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == (4<<SYS_GPA_MFPH_PA9MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_TM1_OUT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == (5<<SYS_GPA_MFPH_PA9MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_UART1_nRTS;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == (6<<SYS_GPA_MFPH_PA9MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_SNOOPER;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == (7<<SYS_GPA_MFPH_PA9MFP_Pos) );
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk)) | SYS_GPA_MFPH_PA9MFP_GPIO ;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA9MFP_Msk) == 0 );     
    
     /* PA10 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA10MFP_Msk)) | SYS_GPA_MFPH_PA10MFP_I2C1_SDA;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA10MFP_Msk) == (1<<SYS_GPA_MFPH_PA10MFP_Pos) );
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA10MFP_Msk)) | SYS_GPA_MFPH_PA10MFP_TM2_CNT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA10MFP_Msk) == (2<<SYS_GPA_MFPH_PA10MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA10MFP_Msk)) | SYS_GPA_MFPH_PA10MFP_SC0_PWR;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA10MFP_Msk) == (3<<SYS_GPA_MFPH_PA10MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA10MFP_Msk)) | SYS_GPA_MFPH_PA10MFP_SPI2_MISO0;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA10MFP_Msk) == (4<<SYS_GPA_MFPH_PA10MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA10MFP_Msk)) | SYS_GPA_MFPH_PA10MFP_TM2_OUT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA10MFP_Msk) == (5<<SYS_GPA_MFPH_PA10MFP_Pos) );
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA10MFP_Msk)) | SYS_GPA_MFPH_PA10MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA10MFP_Msk) == 0 );     
 
     /* PA11 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA11MFP_Msk)) | SYS_GPA_MFPH_PA11MFP_I2C1_SCL;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA11MFP_Msk) == (1<<SYS_GPA_MFPH_PA11MFP_Pos) );
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA11MFP_Msk)) | SYS_GPA_MFPH_PA11MFP_TM3_CNT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA11MFP_Msk) == (2<<SYS_GPA_MFPH_PA11MFP_Pos) ); 
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA11MFP_Msk)) | SYS_GPA_MFPH_PA11MFP_SC0_RST;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA11MFP_Msk) == (3<<SYS_GPA_MFPH_PA11MFP_Pos) ); 
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA11MFP_Msk)) | SYS_GPA_MFPH_PA11MFP_SPI2_MOSI0;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA11MFP_Msk) == (4<<SYS_GPA_MFPH_PA11MFP_Pos) ); 
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA11MFP_Msk)) | SYS_GPA_MFPH_PA11MFP_TM3_OUT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA11MFP_Msk) == (5<<SYS_GPA_MFPH_PA11MFP_Pos) ); 		
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA11MFP_Msk)) | SYS_GPA_MFPH_PA11MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA11MFP_Msk) == 0 ); 

     /* PA12 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA12MFP_Msk)) | SYS_GPA_MFPH_PA12MFP_PWM0_CH0;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA12MFP_Msk) == (1<<SYS_GPA_MFPH_PA12MFP_Pos) );
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA12MFP_Msk)) | SYS_GPA_MFPH_PA12MFP_TM0_EXT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA12MFP_Msk) == (3<<SYS_GPA_MFPH_PA12MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA12MFP_Msk)) | SYS_GPA_MFPH_PA12MFP_I2C0_SDA;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA12MFP_Msk) == (5<<SYS_GPA_MFPH_PA12MFP_Pos) );
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA12MFP_Msk)) | SYS_GPA_MFPH_PA12MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA12MFP_Msk) == 0 );
    
     /* PA13 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA13MFP_Msk)) | SYS_GPA_MFPH_PA13MFP_PWM0_CH1;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA13MFP_Msk) == (1<<SYS_GPA_MFPH_PA13MFP_Pos) );   
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA13MFP_Msk)) | SYS_GPA_MFPH_PA13MFP_TM1_EXT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA13MFP_Msk) == (3<<SYS_GPA_MFPH_PA13MFP_Pos) );   
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA13MFP_Msk)) | SYS_GPA_MFPH_PA13MFP_I2C0_SCL;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA13MFP_Msk) == (5<<SYS_GPA_MFPH_PA13MFP_Pos) );   
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA13MFP_Msk)) | SYS_GPA_MFPH_PA13MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA13MFP_Msk) == 0 );      

     /* PA14 */
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_PWM0_CH2;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA14MFP_Msk) == (1<<SYS_GPA_MFPH_PA14MFP_Pos) );     
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_I2C1_SDA;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA14MFP_Msk) == (2<<SYS_GPA_MFPH_PA14MFP_Pos) );    
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_TM2_EXT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA14MFP_Msk) == (3<<SYS_GPA_MFPH_PA14MFP_Pos) );
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_TM2_CNT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA14MFP_Msk) == (5<<SYS_GPA_MFPH_PA14MFP_Pos) );    
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_UART0_RXD;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA14MFP_Msk) == (6<<SYS_GPA_MFPH_PA14MFP_Pos) );		
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_TM2_OUT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA14MFP_Msk) == (7<<SYS_GPA_MFPH_PA14MFP_Pos) );		
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA14MFP_Msk)) | SYS_GPA_MFPH_PA14MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA14MFP_Msk) == 0 );     

     /* PA15 */
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_PWM0_CH3;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == (1<<SYS_GPA_MFPH_PA15MFP_Pos) );    
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_I2C1_SCL;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == (2<<SYS_GPA_MFPH_PA15MFP_Pos) ); 
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_TM3_EXT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == (3<<SYS_GPA_MFPH_PA15MFP_Pos) );    
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_SC0_PWR;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == (4<<SYS_GPA_MFPH_PA15MFP_Pos) ); 
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_TM3_CNT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == (5<<SYS_GPA_MFPH_PA15MFP_Pos) );    
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_UART0_TXD;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == (6<<SYS_GPA_MFPH_PA15MFP_Pos) ); 
		SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_TM3_OUT;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == (7<<SYS_GPA_MFPH_PA15MFP_Pos) ); 
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA15MFP_Msk)) | SYS_GPA_MFPH_PA15MFP_GPIO;
    CU_ASSERT( (SYS->GPA_MFPH & SYS_GPA_MFPH_PA15MFP_Msk) == 0 ); 

//     /* PB0 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk)) | SYS_GPB_MFPL_PB0MFP_UART0_RXD;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB0MFP_Msk) == (1<<SYS_GPB_MFPL_PB0MFP_Pos) );      
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk)) | SYS_GPB_MFPL_PB0MFP_SPI1_MOSI0;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB0MFP_Msk) == (3<<SYS_GPB_MFPL_PB0MFP_Pos) );  
//    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk)) | SYS_GPB_MFPL_PB0MFP_DSRC_FM0_RX;
//    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB0MFP_Msk) == (7<<SYS_GPB_MFPL_PB0MFP_Pos) ); 
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk)) | SYS_GPB_MFPL_PB0MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB0MFP_Msk) == 0 ); 
		SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk)) | SYS_GPB_MFPL_PB0MFP_UART0_RXD;

    /* PB1 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_UART0_TXD;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB1MFP_Msk) == (1<<SYS_GPB_MFPL_PB1MFP_Pos) );         
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_SPI1_MISO0;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB1MFP_Msk) == (3<<SYS_GPB_MFPL_PB1MFP_Pos) ); 
//    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_DSRC_FM0_TX;
//    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB1MFP_Msk) == (7<<SYS_GPB_MFPL_PB1MFP_Pos) );    
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB1MFP_Msk) == 0 );       
		SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_UART0_TXD;
    
    /* PB2 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB2MFP_Msk)) | SYS_GPB_MFPL_PB2MFP_UART0_nRTS;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB2MFP_Msk) == (1<<SYS_GPB_MFPL_PB2MFP_Pos) );    
//    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB2MFP_Msk)) | SYS_GPB_MFPL_PB2MFP_DSRC_WAKEUP;
//    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB2MFP_Msk) == (2<<SYS_GPB_MFPL_PB2MFP_Pos) );    
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB2MFP_Msk)) | SYS_GPB_MFPL_PB2MFP_SPI1_CLK;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB2MFP_Msk) == (3<<SYS_GPB_MFPL_PB2MFP_Pos) );   
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB2MFP_Msk)) | SYS_GPB_MFPL_PB2MFP_CLKO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB2MFP_Msk) == (4<<SYS_GPB_MFPL_PB2MFP_Pos) );     
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB2MFP_Msk)) | SYS_GPB_MFPL_PB2MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB2MFP_Msk) == 0 ); 

    /* PB3 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB3MFP_Msk)) | SYS_GPB_MFPL_PB3MFP_UART0_nCTS;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB3MFP_Msk) == (1<<SYS_GPB_MFPL_PB3MFP_Pos) );        
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB3MFP_Msk)) | SYS_GPB_MFPL_PB3MFP_SPI1_SS0;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB3MFP_Msk) == (3<<SYS_GPB_MFPL_PB3MFP_Pos) );
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB3MFP_Msk)) | SYS_GPB_MFPL_PB3MFP_SC1_CD;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB3MFP_Msk) == (4<<SYS_GPB_MFPL_PB3MFP_Pos) );    
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB3MFP_Msk)) | SYS_GPB_MFPL_PB3MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB3MFP_Msk) ==0 );    
    
    /* PB4 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB4MFP_Msk)) | SYS_GPB_MFPL_PB4MFP_UART1_RXD;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB4MFP_Msk) == (1<<SYS_GPB_MFPL_PB4MFP_Pos) );       
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB4MFP_Msk)) | SYS_GPB_MFPL_PB4MFP_SC0_CD;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB4MFP_Msk) == (3<<SYS_GPB_MFPL_PB4MFP_Pos) ); 
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB4MFP_Msk)) | SYS_GPB_MFPL_PB4MFP_SPI2_SS0;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB4MFP_Msk) == (4<<SYS_GPB_MFPL_PB4MFP_Pos) ); 
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB4MFP_Msk)) | SYS_GPB_MFPL_PB4MFP_RTC_HZ;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB4MFP_Msk) == (6<<SYS_GPB_MFPL_PB4MFP_Pos) ); 
//    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB4MFP_Msk)) | SYS_GPB_MFPL_PB4MFP_DSRC_TEST_TIME_OUT;
//    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB4MFP_Msk) == (7<<SYS_GPB_MFPL_PB4MFP_Pos) ); 
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB4MFP_Msk)) | SYS_GPB_MFPL_PB4MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB4MFP_Msk) == 0 ); 

    /* PB5 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB5MFP_Msk)) | SYS_GPB_MFPL_PB5MFP_UART1_TXD;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB5MFP_Msk) == (1<<SYS_GPB_MFPL_PB5MFP_Pos) );        
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB5MFP_Msk)) | SYS_GPB_MFPL_PB5MFP_SC0_RST;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB5MFP_Msk) == (3<<SYS_GPB_MFPL_PB5MFP_Pos) ); 
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB5MFP_Msk)) | SYS_GPB_MFPL_PB5MFP_SPI2_CLK;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB5MFP_Msk) == (4<<SYS_GPB_MFPL_PB5MFP_Pos) ); 
//    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB5MFP_Msk)) | SYS_GPB_MFPL_PB5MFP_DSRC_TEST_TIME_START;
//    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB5MFP_Msk) == (7<<SYS_GPB_MFPL_PB5MFP_Pos) );    
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB5MFP_Msk)) | SYS_GPB_MFPL_PB5MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB5MFP_Msk) == 0 );

    /* PB6 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB6MFP_Msk)) | SYS_GPB_MFPL_PB6MFP_UART1_nRTS;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB6MFP_Msk) == (1<<SYS_GPB_MFPL_PB6MFP_Pos) );    
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB6MFP_Msk)) | SYS_GPB_MFPL_PB6MFP_SPI2_MISO0;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB6MFP_Msk) == (4<<SYS_GPB_MFPL_PB6MFP_Pos) ); 
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB6MFP_Msk)) | SYS_GPB_MFPL_PB6MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB6MFP_Msk) == 0 );

    /* PB7 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB7MFP_Msk)) | SYS_GPB_MFPL_PB7MFP_UART1_nCTS;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB7MFP_Msk) == (1<<SYS_GPB_MFPL_PB7MFP_Pos) );        
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB7MFP_Msk)) | SYS_GPB_MFPL_PB7MFP_SPI2_MOSI0;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB7MFP_Msk) == (4<<SYS_GPB_MFPL_PB7MFP_Pos) ); 
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB7MFP_Msk)) | SYS_GPB_MFPL_PB7MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPL & SYS_GPB_MFPL_PB7MFP_Msk) == 0 ); 
    
    /* PB8 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB8MFP_Msk)) | SYS_GPB_MFPH_PB8MFP_STADC;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB8MFP_Msk) == (1<<SYS_GPB_MFPH_PB8MFP_Pos) );    
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB8MFP_Msk)) | SYS_GPB_MFPH_PB8MFP_TM0_CNT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB8MFP_Msk) == (2<<SYS_GPB_MFPH_PB8MFP_Pos) );        
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB8MFP_Msk)) | SYS_GPB_MFPH_PB8MFP_INT0;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB8MFP_Msk) == (3<<SYS_GPB_MFPH_PB8MFP_Pos) );
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB8MFP_Msk)) | SYS_GPB_MFPH_PB8MFP_TM0_OUT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB8MFP_Msk) == (4<<SYS_GPB_MFPH_PB8MFP_Pos) );        
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB8MFP_Msk)) | SYS_GPB_MFPH_PB8MFP_SNOOPER;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB8MFP_Msk) == (7<<SYS_GPB_MFPH_PB8MFP_Pos) );    
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB8MFP_Msk)) | SYS_GPB_MFPH_PB8MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB8MFP_Msk) == 0 );   

    /* PB9 */
    SYS->GPB_MFPH =( SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB9MFP_Msk)) | SYS_GPB_MFPH_PB9MFP_SPI1_SS1;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB9MFP_Msk) == (1<<SYS_GPB_MFPH_PB9MFP_Pos) );    
		SYS->GPB_MFPH =( SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB9MFP_Msk)) | SYS_GPB_MFPH_PB9MFP_TM1_CNT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB9MFP_Msk) == (2<<SYS_GPB_MFPH_PB9MFP_Pos) );
		SYS->GPB_MFPH =( SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB9MFP_Msk)) | SYS_GPB_MFPH_PB9MFP_TM1_OUT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB9MFP_Msk) == (4<<SYS_GPB_MFPH_PB9MFP_Pos) );
		SYS->GPB_MFPH =( SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB9MFP_Msk)) | SYS_GPB_MFPH_PB9MFP_INT0;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB9MFP_Msk) == (5<<SYS_GPB_MFPH_PB9MFP_Pos) );
//		SYS->GPB_MFPH =( SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB9MFP_Msk)) | SYS_GPB_MFPH_PB9MFP_DSRC_RX_EN;
//    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB9MFP_Msk) == (7<<SYS_GPB_MFPH_PB9MFP_Pos) );
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB9MFP_Msk)) | SYS_GPB_MFPH_PB9MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB9MFP_Msk) == 0 ); 

    /* PB10 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB10MFP_Msk)) | SYS_GPB_MFPH_PB10MFP_SPI0_MOSI0;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB10MFP_Msk) == (1<<SYS_GPB_MFPH_PB10MFP_Pos) );    
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB10MFP_Msk)) | SYS_GPB_MFPH_PB10MFP_TM2_CNT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB10MFP_Msk) == (2<<SYS_GPB_MFPH_PB10MFP_Pos) ); 
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB10MFP_Msk)) | SYS_GPB_MFPH_PB10MFP_TM2_OUT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB10MFP_Msk) == (4<<SYS_GPB_MFPH_PB10MFP_Pos) ); 
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB10MFP_Msk)) | SYS_GPB_MFPH_PB10MFP_SPI0_SS1;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB10MFP_Msk) == (5<<SYS_GPB_MFPH_PB10MFP_Pos) ); 
//		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB10MFP_Msk)) | SYS_GPB_MFPH_PB10MFP_DSRC_TX_EN;
//    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB10MFP_Msk) == (7<<SYS_GPB_MFPH_PB10MFP_Pos) ); 
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB10MFP_Msk)) | SYS_GPB_MFPH_PB10MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB10MFP_Msk) == 0 );    
    
    /* PB11 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB11MFP_Msk)) | SYS_GPB_MFPH_PB11MFP_PWM0_CH4;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB11MFP_Msk) == (1<<SYS_GPB_MFPH_PB11MFP_Pos) );    
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB11MFP_Msk)) | SYS_GPB_MFPH_PB11MFP_TM3_CNT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB11MFP_Msk) == (2<<SYS_GPB_MFPH_PB11MFP_Pos) ); 
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB11MFP_Msk)) | SYS_GPB_MFPH_PB11MFP_TM3_OUT;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB11MFP_Msk) == (4<<SYS_GPB_MFPH_PB11MFP_Pos) ); 
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB11MFP_Msk)) | SYS_GPB_MFPH_PB11MFP_SPI0_MISO0;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB11MFP_Msk) == (5<<SYS_GPB_MFPH_PB11MFP_Pos) ); 		
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB11MFP_Msk)) | SYS_GPB_MFPH_PB11MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB11MFP_Msk) == 0 );     
  
    /* PB12 */

    /* PB13 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB13MFP_Msk)) | SYS_GPB_MFPH_PB13MFP_SPI2_MISO1;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB13MFP_Msk) == (3<<SYS_GPB_MFPH_PB13MFP_Pos) );    
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB13MFP_Msk)) | SYS_GPB_MFPH_PB13MFP_SNOOPER;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB13MFP_Msk) == (7<<SYS_GPB_MFPH_PB13MFP_Pos) ); 
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB13MFP_Msk)) | SYS_GPB_MFPH_PB13MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB13MFP_Msk) == 0 );    

    /* PB14 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB14MFP_Msk)) | SYS_GPB_MFPH_PB14MFP_INT0;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB14MFP_Msk) == (1<<SYS_GPB_MFPH_PB14MFP_Pos) );
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB14MFP_Msk)) | SYS_GPB_MFPH_PB14MFP_SPI2_MOSI1;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB14MFP_Msk) == (3<<SYS_GPB_MFPH_PB14MFP_Pos) ); 
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB14MFP_Msk)) | SYS_GPB_MFPH_PB14MFP_SPI2_SS1;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB14MFP_Msk) == (4<<SYS_GPB_MFPH_PB14MFP_Pos) ); 		
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB14MFP_Msk)) | SYS_GPB_MFPH_PB14MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB14MFP_Msk) == 0 ); 
   
    /* PB15 */   
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB15MFP_Msk)) | SYS_GPB_MFPH_PB15MFP_INT1;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB15MFP_Msk) == (1<<SYS_GPB_MFPH_PB15MFP_Pos) );      
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB15MFP_Msk)) | SYS_GPB_MFPH_PB15MFP_SNOOPER;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB15MFP_Msk) == (3<<SYS_GPB_MFPH_PB15MFP_Pos) );      
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB15MFP_Msk)) | SYS_GPB_MFPH_PB15MFP_SC1_CD;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB15MFP_Msk) == (4<<SYS_GPB_MFPH_PB15MFP_Pos) );         
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB15MFP_Msk)) | SYS_GPB_MFPH_PB15MFP_GPIO;
    CU_ASSERT( (SYS->GPB_MFPH & SYS_GPB_MFPH_PB15MFP_Msk) == 0 ); 
}

void TestFunc_SYS_TestConstant2()
{
	    /* PC0 */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC0MFP_Msk)) | SYS_GPC_MFPL_PC0MFP_SPI0_SS0;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC0MFP_Msk) == (1<<SYS_GPC_MFPL_PC0MFP_Pos) );    
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC0MFP_Msk)) | SYS_GPC_MFPL_PC0MFP_SC1_CLK;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC0MFP_Msk) == (4<<SYS_GPC_MFPL_PC0MFP_Pos) );    
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC0MFP_Msk)) | SYS_GPC_MFPL_PC0MFP_PWM0_BRAKE1;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC0MFP_Msk) == (5<<SYS_GPC_MFPL_PC0MFP_Pos) );      
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC0MFP_Msk)) | SYS_GPC_MFPL_PC0MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC0MFP_Msk) == 0 ); 

    /* PC1 */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC1MFP_Msk)) | SYS_GPC_MFPL_PC1MFP_SPI0_CLK;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC1MFP_Msk) == (1<<SYS_GPC_MFPL_PC1MFP_Pos) ); 
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC1MFP_Msk)) | SYS_GPC_MFPL_PC1MFP_SC1_DAT;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC1MFP_Msk) == (4<<SYS_GPC_MFPL_PC1MFP_Pos) );     
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC1MFP_Msk)) | SYS_GPC_MFPL_PC1MFP_PWM0_BRAKE1;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC1MFP_Msk) == (5<<SYS_GPC_MFPL_PC1MFP_Pos) );  
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC1MFP_Msk)) | SYS_GPC_MFPL_PC0MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC1MFP_Msk) == 0 );

    /* PC2 */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC2MFP_Msk)) | SYS_GPC_MFPL_PC2MFP_SPI0_MISO0;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC2MFP_Msk) == (1<<SYS_GPC_MFPL_PC2MFP_Pos) ); 
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC2MFP_Msk)) | SYS_GPC_MFPL_PC2MFP_SC1_PWR;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC2MFP_Msk) == (4<<SYS_GPC_MFPL_PC2MFP_Pos) ); 
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC2MFP_Msk)) | SYS_GPC_MFPL_PC2MFP_PWM0_BRAKE0;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC2MFP_Msk) == (5<<SYS_GPC_MFPL_PC2MFP_Pos) );    
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC2MFP_Msk)) | SYS_GPC_MFPL_PC2MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC2MFP_Msk) == 0 );

    /* PC3 */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC3MFP_Msk)) | SYS_GPC_MFPL_PC3MFP_SPI0_MOSI0;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC3MFP_Msk) == (1<<SYS_GPC_MFPL_PC3MFP_Pos) ); 
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC3MFP_Msk)) | SYS_GPC_MFPL_PC3MFP_SC1_RST;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC3MFP_Msk) == (4<<SYS_GPC_MFPL_PC3MFP_Pos) ); 
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC3MFP_Msk)) | SYS_GPC_MFPL_PC3MFP_PWM0_BRAKE0;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC3MFP_Msk) == (5<<SYS_GPC_MFPL_PC3MFP_Pos) );     
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC3MFP_Msk)) | SYS_GPC_MFPL_PC3MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC3MFP_Msk) == (0<<SYS_GPC_MFPL_PC3MFP_Pos) );

    /* PC4 */
    
    /* PC5 */
    
    /* PC6 */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC6MFP_Msk)) | SYS_GPC_MFPL_PC6MFP_UART1_RXD;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC6MFP_Msk) == (1<<SYS_GPC_MFPL_PC6MFP_Pos) ); 
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC6MFP_Msk)) | SYS_GPC_MFPL_PC6MFP_TM0_EXT;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC6MFP_Msk) == (3<<SYS_GPC_MFPL_PC6MFP_Pos) );     
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC6MFP_Msk)) | SYS_GPC_MFPL_PC6MFP_SC1_CD;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC6MFP_Msk) == (4<<SYS_GPC_MFPL_PC6MFP_Pos) );       
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC6MFP_Msk)) | SYS_GPC_MFPL_PC6MFP_PWM0_CH0;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC6MFP_Msk) == (5<<SYS_GPC_MFPL_PC6MFP_Pos) );         
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC6MFP_Msk)) | SYS_GPC_MFPL_PC6MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC6MFP_Msk) == 0 );     
    
    /* PC7 */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC7MFP_Msk)) | SYS_GPC_MFPL_PC7MFP_UART1_TXD;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC7MFP_Msk) == (1<<SYS_GPC_MFPL_PC7MFP_Pos) ); 
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC7MFP_Msk)) | SYS_GPC_MFPL_PC7MFP_ADC_CH7;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC7MFP_Msk) == (2<<SYS_GPC_MFPL_PC7MFP_Pos) );    
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC7MFP_Msk)) | SYS_GPC_MFPL_PC7MFP_TM1_EXT;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC7MFP_Msk) == (3<<SYS_GPC_MFPL_PC7MFP_Pos) );  
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC7MFP_Msk)) | SYS_GPC_MFPL_PC7MFP_PWM0_CH1;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC7MFP_Msk) == (5UL<<SYS_GPC_MFPL_PC7MFP_Pos) );      
    SYS->GPC_MFPL = (SYS->GPC_MFPL & (~SYS_GPC_MFPL_PC7MFP_Msk)) | SYS_GPC_MFPL_PC7MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPL & SYS_GPC_MFPL_PC7MFP_Msk) == 0 ); 

    /* PC8 */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC8MFP_Msk)) | SYS_GPC_MFPH_PC8MFP_SPI1_SS0;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC8MFP_Msk) == (1<<SYS_GPC_MFPH_PC8MFP_Pos) ); 
		SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC8MFP_Msk)) | SYS_GPC_MFPH_PC8MFP_I2C1_SDA;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC8MFP_Msk) == (5<<SYS_GPC_MFPH_PC8MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC8MFP_Msk)) | SYS_GPC_MFPH_PC8MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC8MFP_Msk) == 0 ); 

    /* PC9 */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC9MFP_Msk)) | SYS_GPC_MFPH_PC9MFP_SPI1_CLK;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC9MFP_Msk) == (1<<SYS_GPC_MFPH_PC9MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC9MFP_Msk)) | SYS_GPC_MFPH_PC9MFP_I2C1_SCL;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC9MFP_Msk) == (5<<SYS_GPC_MFPH_PC9MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC9MFP_Msk)) | SYS_GPC_MFPH_PC9MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC9MFP_Msk) == 0 ); 

    /* PC10 */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC10MFP_Msk)) | SYS_GPC_MFPH_PC10MFP_SPI1_MISO0;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC10MFP_Msk) == (1<<SYS_GPC_MFPH_PC10MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC10MFP_Msk)) | SYS_GPC_MFPH_PC10MFP_UART1_RXD;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC10MFP_Msk) == (5<<SYS_GPC_MFPH_PC10MFP_Pos) ); 
//		SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC10MFP_Msk)) | SYS_GPC_MFPH_PC10MFP_DSRC_FM0_TX;
//    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC10MFP_Msk) == (7<<SYS_GPC_MFPH_PC10MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC10MFP_Msk)) | SYS_GPC_MFPH_PC10MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC10MFP_Msk) == 0 );     

    /* PC11 */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC11MFP_Msk)) | SYS_GPC_MFPH_PC11MFP_SPI1_MOSI0;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC11MFP_Msk) == (1<<SYS_GPC_MFPH_PC11MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC11MFP_Msk)) | SYS_GPC_MFPH_PC11MFP_UART1_TXD;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC11MFP_Msk) == (5<<SYS_GPC_MFPH_PC11MFP_Pos) );     
//		SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC11MFP_Msk)) | SYS_GPC_MFPH_PC11MFP_DSRC_FM0_RX;
//    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC11MFP_Msk) == (7<<SYS_GPC_MFPH_PC11MFP_Pos) );     
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC11MFP_Msk)) | SYS_GPC_MFPH_PC11MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC11MFP_Msk) == 0 );     
    
    /* PC12 */

    /* PC13 */

    /* PC14 */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC14MFP_Msk)) | SYS_GPC_MFPH_PC14MFP_UART1_nCTS;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC14MFP_Msk) == (1<<SYS_GPC_MFPH_PC14MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC14MFP_Msk)) | SYS_GPC_MFPH_PC14MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC14MFP_Msk) == 0 ); 

    /* PC15 */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC15MFP_Msk)) | SYS_GPC_MFPH_PC15MFP_UART1_nRTS;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC15MFP_Msk) == (1<<SYS_GPC_MFPH_PC15MFP_Pos) ); 
		SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC15MFP_Msk)) | SYS_GPC_MFPH_PC15MFP_TM0_EXT;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC15MFP_Msk) == (3<<SYS_GPC_MFPH_PC15MFP_Pos) ); 
    SYS->GPC_MFPH = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC15MFP_Msk)) | SYS_GPC_MFPH_PC15MFP_GPIO;
    CU_ASSERT( (SYS->GPC_MFPH & SYS_GPC_MFPH_PC15MFP_Msk) == 0 ); 
}

void TestFunc_SYS_TestConstant3() 
{
    /* PD0 */    
    /* PD1 */
    /* PD2 */
    /* PD3 */    
    /* PD4 */    
    /* PD5 */
  
    /* PD6 */
    SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD6MFP_Msk)) | SYS_GPD_MFPL_PD6MFP_SPI1_MOSI1;
    CU_ASSERT( (SYS->GPD_MFPL & SYS_GPD_MFPL_PD6MFP_Msk) == (3<<SYS_GPD_MFPL_PD6MFP_Pos) ); 
    SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD6MFP_Msk)) | SYS_GPD_MFPL_PD6MFP_SC1_RST;
    CU_ASSERT( (SYS->GPD_MFPL & SYS_GPD_MFPL_PD6MFP_Msk) == (4<<SYS_GPD_MFPL_PD6MFP_Pos) );    
    SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD6MFP_Msk)) | SYS_GPD_MFPL_PD6MFP_GPIO;
    CU_ASSERT( (SYS->GPD_MFPL & SYS_GPD_MFPL_PD6MFP_Msk) == 0 );

    /* PD7 */
    SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD7MFP_Msk)) | SYS_GPD_MFPL_PD7MFP_SPI1_MISO1;
    CU_ASSERT( (SYS->GPD_MFPL & SYS_GPD_MFPL_PD7MFP_Msk) == (3<<SYS_GPD_MFPL_PD7MFP_Pos) ); 
    SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD7MFP_Msk)) | SYS_GPD_MFPL_PD7MFP_SC1_PWR;
    CU_ASSERT( (SYS->GPD_MFPL & SYS_GPD_MFPL_PD7MFP_Msk) == (4<<SYS_GPD_MFPL_PD7MFP_Pos) );              
    SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD7MFP_Msk)) | SYS_GPD_MFPL_PD7MFP_GPIO;
    CU_ASSERT( (SYS->GPD_MFPL & SYS_GPD_MFPL_PD7MFP_Msk) == 0 );    
        
    /* PD8 */
    /* PD9 */
    /* PD10 */
    /* PD11 */
    /* PD12 */
    /* PD13 */
    
    /* PD14 */
    SYS->GPD_MFPH = (SYS->GPD_MFPH & (~SYS_GPD_MFPH_PD14MFP_Msk)) | SYS_GPD_MFPH_PD14MFP_SPI0_MOSI1;
    CU_ASSERT( (SYS->GPD_MFPH & SYS_GPD_MFPH_PD14MFP_Msk) == (1<<SYS_GPD_MFPH_PD14MFP_Pos) ); 
    SYS->GPD_MFPH = (SYS->GPD_MFPH & (~SYS_GPD_MFPH_PD14MFP_Msk)) | SYS_GPD_MFPH_PD14MFP_SC1_DAT;
    CU_ASSERT( (SYS->GPD_MFPH & SYS_GPD_MFPH_PD14MFP_Msk) == (4<<SYS_GPD_MFPH_PD14MFP_Pos) ); 
    SYS->GPD_MFPH = (SYS->GPD_MFPH & (~SYS_GPD_MFPH_PD14MFP_Msk)) | SYS_GPD_MFPH_PD14MFP_GPIO;
    CU_ASSERT( (SYS->GPD_MFPH & SYS_GPD_MFPH_PD14MFP_Msk) == 0 ); 
     
    /* PD15 */
    SYS->GPD_MFPH = (SYS->GPD_MFPH & (~SYS_GPD_MFPH_PD15MFP_Msk)) | SYS_GPD_MFPH_PD15MFP_SPI0_MISO1;
    CU_ASSERT( (SYS->GPD_MFPH & SYS_GPD_MFPH_PD15MFP_Msk) == (1<<SYS_GPD_MFPH_PD15MFP_Pos) ); 
    SYS->GPD_MFPH = (SYS->GPD_MFPH & (~SYS_GPD_MFPH_PD15MFP_Msk)) | SYS_GPD_MFPH_PD15MFP_SC1_CLK;
    CU_ASSERT( (SYS->GPD_MFPH & SYS_GPD_MFPH_PD15MFP_Msk) == (4<<SYS_GPD_MFPH_PD15MFP_Pos) );    
    SYS->GPD_MFPH = SYS->GPD_MFPH & (~SYS_GPD_MFPH_PD15MFP_Msk) | SYS_GPD_MFPH_PD15MFP_GPIO;
    CU_ASSERT( (SYS->GPD_MFPH & SYS_GPD_MFPH_PD15MFP_Msk) == 0 );     

    /* PE0 */
		/* PE1 */
		/* PE2 */
		/* PE3 */
		/* PE4 */
    
    /* PE5 */
    SYS->GPE_MFPL = (SYS->GPE_MFPL & (~SYS_GPE_MFPL_PE5MFP_Msk)) | SYS_GPE_MFPL_PE5MFP_PWM0_CH5;
    CU_ASSERT( (SYS->GPE_MFPL & SYS_GPE_MFPL_PE5MFP_Msk) == (1<<SYS_GPE_MFPL_PE5MFP_Pos) );
//    SYS->GPE_MFPL = (SYS->GPE_MFPL & (~SYS_GPE_MFPL_PE5MFP_Msk)) | SYS_GPE_MFPL_PE5MFP_DSRC_WAKEUP;
//    CU_ASSERT( (SYS->GPE_MFPL & SYS_GPE_MFPL_PE5MFP_Msk) == (2<<SYS_GPE_MFPL_PE5MFP_Pos) );
    SYS->GPE_MFPL = (SYS->GPE_MFPL & (~SYS_GPE_MFPL_PE5MFP_Msk)) | SYS_GPE_MFPL_PE5MFP_RTC_HZ;
    CU_ASSERT( (SYS->GPE_MFPL & SYS_GPE_MFPL_PE5MFP_Msk) == (6<<SYS_GPE_MFPL_PE5MFP_Pos) );
    SYS->GPE_MFPL = (SYS->GPE_MFPL & (~SYS_GPE_MFPL_PE5MFP_Msk)) | SYS_GPE_MFPL_PE5MFP_GPIO;
    CU_ASSERT( (SYS->GPE_MFPL & SYS_GPE_MFPL_PE5MFP_Msk) == 0 );
   
    /* PE6 */
    /* PE7 */
    /* PE8 */
    /* PE9 */
    /* PE10 */
    /* PE11 */
    /* PE12 */
    /* PE13 */    
    /* PE14 */
    
    /* PF0 */
    SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF0MFP_Msk)) | SYS_GPF_MFPL_PF0MFP_GPIO;
    CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF0MFP_Msk) == 0 );     
    SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF0MFP_Msk)) | SYS_GPF_MFPL_PF0MFP_INT0;
    CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF0MFP_Msk) == (5<<SYS_GPF_MFPL_PF0MFP_Pos) );     
    SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF0MFP_Msk)) | SYS_GPF_MFPL_PF0MFP_ICE_DAT;
    CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF0MFP_Msk) == (7<<SYS_GPF_MFPL_PF0MFP_Pos) );    

    /* PF1 */
    SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF1MFP_Msk)) | SYS_GPF_MFPL_PF1MFP_GPIO;
    CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF1MFP_Msk) == 0 );       
    SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF1MFP_Msk)) | SYS_GPF_MFPL_PF1MFP_INT1;
    CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF1MFP_Msk) == (5<<SYS_GPF_MFPL_PF1MFP_Pos) );    
		SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF1MFP_Msk)) | SYS_GPF_MFPL_PF1MFP_ICE_CLK;
    CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF1MFP_Msk) == (7<<SYS_GPF_MFPL_PF1MFP_Pos) ); 
		
//     /* PF2 */      
//     SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF2MFP_Msk)) | SYS_GPF_MFPL_PF2MFP_XT1_OUT;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF2MFP_Msk) == (7<<SYS_GPF_MFPL_PF2MFP_Pos) ); 
//     SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF2MFP_Msk)) | SYS_GPF_MFPL_PF2MFP_GPIO;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF2MFP_Msk) == 0 ); 

//     /* PF3 */      
//     SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF3MFP_Msk)) | SYS_GPF_MFPL_PF3MFP_XT1_IN;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF3MFP_Msk) == (7<<SYS_GPF_MFPL_PF3MFP_Pos) ); 
//     SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF3MFP_Msk)) | SYS_GPF_MFPL_PF3MFP_GPIO;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF3MFP_Msk) == 0 ); 
//     
    /* PF4 */         
    /* PF5 */
    
    /* PF6 */         
//     SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF6MFP_Msk)) | SYS_GPF_MFPL_PF6MFP_GPIO;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF6MFP_Msk) == 0 );    
//     printf(" %x  --> %x \n", (SYS->GPF_MFPL & SYS_GPF_MFPL_PF6MFP_Msk), (0<<SYS_GPF_MFPL_PF6MFP_Pos));		
//     SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF6MFP_Msk)) | SYS_GPF_MFPL_PF6MFP_I2C1_SDA;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF6MFP_Msk) == (1<<SYS_GPF_MFPL_PF6MFP_Pos) ); 
// 		printf(" %x  --> %x \n", (SYS->GPF_MFPL & SYS_GPF_MFPL_PF6MFP_Msk), (1<<SYS_GPF_MFPL_PF6MFP_Pos));
// 		SYS->GPF_MFPL = (SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF6MFP_Msk)) | SYS_GPF_MFPL_PF6MFP_X32_OUT;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF6MFP_Msk) == (7<<SYS_GPF_MFPL_PF6MFP_Pos) );
// 		printf(" %x  --> %x \n", (SYS->GPF_MFPL & SYS_GPF_MFPL_PF6MFP_Msk), (7<<SYS_GPF_MFPL_PF6MFP_Pos));
		
    /* PF7 */              
//     SYS->GPF_MFPL = SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF7MFP_Msk) | SYS_GPF_MFPL_PF7MFP_GPIO;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk) == 0 ); 
// 		printf(" %x  --> %x \n", (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk), (0<<SYS_GPF_MFPL_PF7MFP_Pos));
// 		SYS->GPF_MFPL = SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF7MFP_Msk) | SYS_GPF_MFPL_PF7MFP_I2C1_SCL;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk) == 1 );
// 		printf(" %x  --> %x \n", (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk), (1<<SYS_GPF_MFPL_PF7MFP_Pos));
// 		SYS->GPF_MFPL = SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF7MFP_Msk) | SYS_GPF_MFPL_PF7MFP_SC0_CD;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk) == 3 );
// 		printf(" %x  --> %x \n", (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk), (3<<SYS_GPF_MFPL_PF7MFP_Pos));
// 		SYS->GPF_MFPL = SYS->GPF_MFPL & (~SYS_GPF_MFPL_PF7MFP_Msk) | SYS_GPF_MFPL_PF7MFP_X32_IN;
//     CU_ASSERT( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk) == (7<<SYS_GPF_MFPL_PF7MFP_Pos)  );
// 		printf(" %x  --> %x \n", (SYS->GPF_MFPL & SYS_GPF_MFPL_PF7MFP_Msk), (7<<SYS_GPF_MFPL_PF7MFP_Pos));
		
    /* VREFCTL */
		CU_ASSERT(SYS_IVREFCTL_BGP_EN==1<<SYS_IVREFCTL_BGPEN_Pos);
		CU_ASSERT(SYS_IVREFCTL_REG_EN==1<<SYS_IVREFCTL_REGEN_Pos);

    SYS_UnlockReg();
	 
		SYS->IVREFCTL = (SYS->IVREFCTL & ~SYS_IVREFCTL_SEL25_Msk )| SYS_IVREFCTL_SEL25;
		CU_ASSERT((SYS->IVREFCTL & SYS_IVREFCTL_SEL25_Msk)== SYS_IVREFCTL_SEL25);    
		SYS->IVREFCTL = (SYS->IVREFCTL & ~SYS_IVREFCTL_SEL25_Msk )| SYS_IVREFCTL_SEL18;
		CU_ASSERT((SYS->IVREFCTL & SYS_IVREFCTL_SEL25_Msk)==SYS_IVREFCTL_SEL18);
		
		SYS->IVREFCTL = (SYS->IVREFCTL & ~SYS_IVREFCTL_SEL25_Msk )| SYS_IVREFCTL_SEL15;
		CU_ASSERT((SYS->IVREFCTL & SYS_IVREFCTL_SEL25_Msk)==(0<<SYS_IVREFCTL_SEL25_Pos));

		CU_ASSERT(SYS_IVREFCTL_EXTMODE == (1<<SYS_IVREFCTL_EXTMODE_Pos));

    /* LDOCTL */
		CU_ASSERT(SYS_LDOCTL_LDO_LEVEL12 == (0<<2));
		CU_ASSERT(SYS_LDOCTL_LDO_LEVEL16 == (1<<2));
		CU_ASSERT(SYS_LDOCTL_LDO_LEVEL18 == (2<<2));
}

uint32_t au32HIRC0TrimCTLSel[] = {
SYS_IRC0TCTL_TRIM_11_0592M, 1,(SYS_BASE+0x80),
SYS_IRC0TCTL_TRIM_12M,      2,(SYS_BASE+0x80),
SYS_IRC0TCTL_TRIM_12_288M,  3,(SYS_BASE+0x80),
SYS_IRC0TCTL_TRIM_16M,      4,(SYS_BASE+0x80),
};

uint32_t au32HIRC0TrimLoopSel[] = {
SYS_IRCTCTL_LOOP_4CLK,     0,(SYS_BASE+0x80),
SYS_IRCTCTL_LOOP_8CLK,     1,(SYS_BASE+0x80),
SYS_IRCTCTL_LOOP_16CLK,    2,(SYS_BASE+0x80),
SYS_IRCTCTL_LOOP_32CLK,    3,(SYS_BASE+0x80),
};

uint32_t au32HIRC0TrimRetrySel[] = {
SYS_IRCTCTL_RETRY_64,    0,(SYS_BASE+0x80),
SYS_IRCTCTL_RETRY_128,   1,(SYS_BASE+0x80),
SYS_IRCTCTL_RETRY_256,   2,(SYS_BASE+0x80),
SYS_IRCTCTL_RETRY_512,   3,(SYS_BASE+0x80),
};

uint32_t au32HIRC0TrimIENSel[] = {
SYS_IRCTIEN_DISABLE,     0,(SYS_BASE+0x84),
SYS_IRCTIEN_FAIL_EN,     2,(SYS_BASE+0x84),
SYS_IRCTIEN_32KERR_EN,   4,(SYS_BASE+0x84),
(SYS_IRCTIEN_32KERR_EN|SYS_IRCTIEN_FAIL_EN),   6,(SYS_BASE+0x84),
};

void TestFunc_SYS_HIRC0TRIMStatus()
{
  uint32_t i,j;
  SYS_UnlockReg();
  for(i=0;i<sizeof(au32HIRC0TrimCTLSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC0TrimCTLSel[i+2],(inpw(au32HIRC0TrimCTLSel[i+2]) & ~SYS_IRC0TCTL_FREQSEL_Msk)| au32HIRC0TrimCTLSel[i] );
    CU_ASSERT((inpw(au32HIRC0TrimCTLSel[i+2]) & (au32HIRC0TrimCTLSel[i+1])<<SYS_IRC0TCTL_FREQSEL_Pos)==au32HIRC0TrimCTLSel[i]);
  }
  for(i=0;i<sizeof(au32HIRC0TrimLoopSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC0TrimLoopSel[i+2],(inpw(au32HIRC0TrimLoopSel[i+2]) & ~SYS_IRC0TCTL_LOOPSEL_Msk)| au32HIRC0TrimLoopSel[i] );
    CU_ASSERT((inpw(au32HIRC0TrimLoopSel[i+2]) & (au32HIRC0TrimLoopSel[i+1])<<SYS_IRC0TCTL_LOOPSEL_Pos)==au32HIRC0TrimLoopSel[i]);
  }
  for(i=0;i<sizeof(au32HIRC0TrimRetrySel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC0TrimRetrySel[i+2],(inpw(au32HIRC0TrimRetrySel[i+2]) & ~SYS_IRC0TCTL_RETRYCNT_Msk)| au32HIRC0TrimRetrySel[i] );
    CU_ASSERT((inpw(au32HIRC0TrimRetrySel[i+2]) & (au32HIRC0TrimRetrySel[i+1])<<SYS_IRC0TCTL_RETRYCNT_Pos)==au32HIRC0TrimRetrySel[i]);
  }
  for(i=0;i<sizeof(au32HIRC0TrimIENSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC0TrimIENSel[i+2],((inpw(au32HIRC0TrimIENSel[i+2]) & ~(0x6))| au32HIRC0TrimIENSel[i]) );
    CU_ASSERT((inpw(au32HIRC0TrimIENSel[i+2]) & (au32HIRC0TrimIENSel[i+1]))==au32HIRC0TrimIENSel[i]);
  }

  for(i=0;i<sizeof(au32HIRC0TrimCTLSel)/sizeof(uint32_t);i+=3)
    for(j=0;j<sizeof(au32HIRC0TrimIENSel)/sizeof(uint32_t);j+=3)
    {
       SYS_EnableHIRC0Trim(au32HIRC0TrimCTLSel[i],au32HIRC0TrimIENSel[j]);
       CU_ASSERT((inpw(au32HIRC0TrimCTLSel[i+2]) & (au32HIRC0TrimCTLSel[i+1])<<SYS_IRC0TCTL_FREQSEL_Pos)==au32HIRC0TrimCTLSel[i]);
       CU_ASSERT((inpw(au32HIRC0TrimIENSel[j+2]) & (au32HIRC0TrimIENSel[j+1]))==au32HIRC0TrimIENSel[j]);
    }
  SYS_DisableHIRC0Trim();
  CU_ASSERT(SYS->IRC0TCTL == 0);

  CU_ASSERT(SYS_GET_IRC0TRIM_INT_FLAG()==SYS->IRC0TISTS);
  //SYS_CLEAR_IRCTRIM_INT_FLAG()  //Clear IRCTrim flag , can't not test.
		
  SYS->IRC0TIEN = SYS->IRC0TIEN | SYS_IRCTIEN_FAIL_EN;
  CU_ASSERT( (SYS->IRC0TIEN & 1<<SYS_IRC0TIEN_TFAILIEN_Pos) == SYS_IRC0TIEN_TFAILIEN_Msk);
  SYS->IRC0TIEN = SYS->IRC0TIEN & ~SYS_IRCTIEN_FAIL_EN;
  CU_ASSERT( (SYS->IRC0TIEN & 1<<SYS_IRC0TIEN_TFAILIEN_Pos)==0);

  SYS->IRC0TIEN = SYS->IRC0TIEN | SYS_IRCTIEN_32KERR_EN;
  CU_ASSERT( (SYS->IRC0TIEN & 1<<SYS_IRC0TIEN_CLKEIEN_Pos) == SYS_IRC0TIEN_CLKEIEN_Msk);
  SYS->IRC0TIEN = SYS->IRC0TIEN & ~SYS_IRCTIEN_32KERR_EN;
  CU_ASSERT( (SYS->IRC0TIEN & 1<<SYS_IRC0TIEN_CLKEIEN_Pos)==0);

  CU_ASSERT(SYS_IRCTISTS_FREQLOCK == (1<<SYS_IRC0TISTS_FREQLOCK_Pos));
  CU_ASSERT(SYS_IRCTISTS_FAIL_INT == (1<<SYS_IRC0TISTS_TFAILIF_Pos));
  CU_ASSERT(SYS_IRCTISTS_32KERR_INT == (1<<SYS_IRC0TISTS_CLKERRIF_Pos));

  CU_ASSERT(SYS_IRCTCTL_CLKERR_STOP == (1<<8));

  SYS_LockReg();
}


uint32_t au32HIRC1TrimCTLSel[] = {
SYS_IRC1TCTL_TRIM_36M,     2,(SYS_BASE+0x90),
};

uint32_t au32HIRC1TrimLoopSel[] = {
SYS_IRCTCTL_LOOP_4CLK,     0,(SYS_BASE+0x90),
SYS_IRCTCTL_LOOP_8CLK,     1,(SYS_BASE+0x90),
SYS_IRCTCTL_LOOP_16CLK,    2,(SYS_BASE+0x90),
SYS_IRCTCTL_LOOP_32CLK,    3,(SYS_BASE+0x90),
};

uint32_t au32HIRC1TrimRetrySel[] = {
SYS_IRCTCTL_RETRY_64,    0,(SYS_BASE+0x90),
SYS_IRCTCTL_RETRY_128,   1,(SYS_BASE+0x90),
SYS_IRCTCTL_RETRY_256,   2,(SYS_BASE+0x90),
SYS_IRCTCTL_RETRY_512,   3,(SYS_BASE+0x90),
};

uint32_t au32HIRC1TrimIENSel[] = {
SYS_IRCTIEN_DISABLE,     0,(SYS_BASE+0x94),
SYS_IRCTIEN_FAIL_EN,     2,(SYS_BASE+0x94),
SYS_IRCTIEN_32KERR_EN,   4,(SYS_BASE+0x94),
(SYS_IRCTIEN_32KERR_EN|SYS_IRCTIEN_FAIL_EN),   6,(SYS_BASE+0x94),
};

void TestFunc_SYS_HIRC1TRIMStatus()
{
  uint32_t i,j;
  SYS_UnlockReg();
  for(i=0;i<sizeof(au32HIRC1TrimCTLSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC1TrimCTLSel[i+2],(inpw(au32HIRC1TrimCTLSel[i+2]) & ~SYS_IRC1TCTL_FREQSEL_Msk)| au32HIRC1TrimCTLSel[i] );
    CU_ASSERT((inpw(au32HIRC1TrimCTLSel[i+2]) & (au32HIRC1TrimCTLSel[i+1])<<SYS_IRC1TCTL_FREQSEL_Pos)==au32HIRC1TrimCTLSel[i]);
  }
  for(i=0;i<sizeof(au32HIRC1TrimLoopSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC1TrimLoopSel[i+2],(inpw(au32HIRC1TrimLoopSel[i+2]) & ~SYS_IRC1TCTL_LOOPSEL_Msk)| au32HIRC1TrimLoopSel[i] );
    CU_ASSERT((inpw(au32HIRC1TrimLoopSel[i+2]) & (au32HIRC1TrimLoopSel[i+1])<<SYS_IRC1TCTL_LOOPSEL_Pos)==au32HIRC1TrimLoopSel[i]);
  }
  for(i=0;i<sizeof(au32HIRC0TrimRetrySel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC1TrimRetrySel[i+2],(inpw(au32HIRC1TrimRetrySel[i+2]) & ~SYS_IRC1TCTL_RETRYCNT_Msk)| au32HIRC1TrimRetrySel[i] );
    CU_ASSERT((inpw(au32HIRC1TrimRetrySel[i+2]) & (au32HIRC1TrimRetrySel[i+1])<<SYS_IRC1TCTL_RETRYCNT_Pos)==au32HIRC1TrimRetrySel[i]);
  }
  for(i=0;i<sizeof(au32HIRC1TrimIENSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32HIRC1TrimIENSel[i+2],((inpw(au32HIRC1TrimIENSel[i+2]) & ~(0x6))| au32HIRC1TrimIENSel[i]) );
    CU_ASSERT((inpw(au32HIRC1TrimIENSel[i+2]) & (au32HIRC1TrimIENSel[i+1]))==au32HIRC1TrimIENSel[i]);
  }

  for(i=0;i<sizeof(au32HIRC1TrimCTLSel)/sizeof(uint32_t);i+=3)
    for(j=0;j<sizeof(au32HIRC1TrimIENSel)/sizeof(uint32_t);j+=3)
    {
       SYS_EnableHIRC1Trim(au32HIRC1TrimCTLSel[i],au32HIRC1TrimIENSel[j]);
       CU_ASSERT((inpw(au32HIRC1TrimCTLSel[i+2]) & (au32HIRC1TrimCTLSel[i+1])<<SYS_IRC1TCTL_FREQSEL_Pos)==au32HIRC1TrimCTLSel[i]);
       CU_ASSERT((inpw(au32HIRC1TrimIENSel[j+2]) & (au32HIRC1TrimIENSel[j+1]))==au32HIRC1TrimIENSel[j]);
    }
  SYS_DisableHIRC1Trim();
  CU_ASSERT(SYS->IRC1TCTL == 0);

  CU_ASSERT(SYS_GET_IRC1TRIM_INT_FLAG()==SYS->IRC1TISTS);
  //SYS_CLEAR_IRCTRIM_INT_FLAG()  //Clear IRCTrim flag , can't not test.
		
  SYS->IRC1TIEN = SYS->IRC1TIEN | SYS_IRCTIEN_FAIL_EN;
  CU_ASSERT( (SYS->IRC1TIEN & 1<<SYS_IRC1TIEN_TFAILIEN_Pos) == SYS_IRC1TIEN_TFAILIEN_Msk);
  SYS->IRC1TIEN = SYS->IRC1TIEN & ~SYS_IRCTIEN_FAIL_EN;
  CU_ASSERT( (SYS->IRC1TIEN & 1<<SYS_IRC1TIEN_TFAILIEN_Pos)==0);

  SYS->IRC1TIEN = SYS->IRC1TIEN | SYS_IRCTIEN_32KERR_EN;
  CU_ASSERT( (SYS->IRC1TIEN & 1<<SYS_IRC1TIEN_CLKEIEN_Pos) == SYS_IRC1TIEN_CLKEIEN_Msk);
  SYS->IRC1TIEN = SYS->IRC1TIEN & ~SYS_IRCTIEN_32KERR_EN;
  CU_ASSERT( (SYS->IRC1TIEN & 1<<SYS_IRC1TIEN_CLKEIEN_Pos)==0);

  CU_ASSERT(SYS_IRCTISTS_FREQLOCK == (1<<SYS_IRC1TISTS_FREQLOCK_Pos));
  CU_ASSERT(SYS_IRCTISTS_FAIL_INT == (1<<SYS_IRC1TISTS_TFAILIF_Pos));
  CU_ASSERT(SYS_IRCTISTS_32KERR_INT == (1<<SYS_IRC1TISTS_CLKERRIF_Pos));

  CU_ASSERT(SYS_IRCTCTL_CLKERR_STOP == (1<<8));

  SYS_LockReg();
}

uint32_t au32MIRCTrimCTLSel[] = {
SYS_MIRCTCTL_TRIM_4M,     2,(SYS_BASE+0x90),
};

uint32_t au32MIRCTrimLoopSel[] = {
SYS_IRCTCTL_LOOP_4CLK,     0,(SYS_BASE+0xA0),
SYS_IRCTCTL_LOOP_8CLK,     1,(SYS_BASE+0xA0),
SYS_IRCTCTL_LOOP_16CLK,    2,(SYS_BASE+0xA0),
SYS_IRCTCTL_LOOP_32CLK,    3,(SYS_BASE+0xA0),
};

uint32_t au32MIRCTrimRetrySel[] = {
SYS_IRCTCTL_RETRY_64,    0,(SYS_BASE+0xA0),
SYS_IRCTCTL_RETRY_128,   1,(SYS_BASE+0xA0),
SYS_IRCTCTL_RETRY_256,   2,(SYS_BASE+0xA0),
SYS_IRCTCTL_RETRY_512,   3,(SYS_BASE+0xA0),
};

uint32_t au32MIRCTrimIENSel[] = {
SYS_IRCTIEN_DISABLE,     0,(SYS_BASE+0xA4),
SYS_IRCTIEN_FAIL_EN,     2,(SYS_BASE+0xA4),
SYS_IRCTIEN_32KERR_EN,   4,(SYS_BASE+0xA4),
(SYS_IRCTIEN_32KERR_EN|SYS_IRCTIEN_FAIL_EN),   6,(SYS_BASE+0xA4),
};

void TestFunc_SYS_MIRCTRIMStatus()
{
  uint32_t i,j;
  SYS_UnlockReg();
  for(i=0;i<sizeof(au32MIRCTrimCTLSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32MIRCTrimCTLSel[i+2],(inpw(au32MIRCTrimCTLSel[i+2]) & ~SYS_MIRCTCTL_FREQSEL_Msk)| au32MIRCTrimCTLSel[i] );
    CU_ASSERT((inpw(au32MIRCTrimCTLSel[i+2]) & (au32MIRCTrimCTLSel[i+1])<<SYS_MIRCTCTL_FREQSEL_Pos)==au32MIRCTrimCTLSel[i]);
  }
  for(i=0;i<sizeof(au32MIRCTrimLoopSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32MIRCTrimLoopSel[i+2],(inpw(au32MIRCTrimLoopSel[i+2]) & ~SYS_MIRCTCTL_LOOPSEL_Msk)| au32MIRCTrimLoopSel[i] );
    CU_ASSERT((inpw(au32MIRCTrimLoopSel[i+2]) & (au32MIRCTrimLoopSel[i+1])<<SYS_MIRCTCTL_LOOPSEL_Pos)==au32MIRCTrimLoopSel[i]);
  }
  for(i=0;i<sizeof(au32MIRCTrimRetrySel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32MIRCTrimRetrySel[i+2],(inpw(au32MIRCTrimRetrySel[i+2]) & ~SYS_MIRCTCTL_RETRYCNT_Msk)| au32MIRCTrimRetrySel[i] );
    CU_ASSERT((inpw(au32MIRCTrimRetrySel[i+2]) & (au32MIRCTrimRetrySel[i+1])<<SYS_MIRCTCTL_RETRYCNT_Pos)==au32MIRCTrimRetrySel[i]);
  }
  for(i=0;i<sizeof(au32MIRCTrimIENSel)/sizeof(uint32_t);i+=3)
  {
    outpw(au32MIRCTrimIENSel[i+2],((inpw(au32MIRCTrimIENSel[i+2]) & ~(0x6))| au32MIRCTrimIENSel[i]) );
    CU_ASSERT((inpw(au32MIRCTrimIENSel[i+2]) & (au32MIRCTrimIENSel[i+1]))==au32MIRCTrimIENSel[i]);
  }

  for(i=0;i<sizeof(au32HIRC1TrimCTLSel)/sizeof(uint32_t);i+=3)
    for(j=0;j<sizeof(au32MIRCTrimIENSel)/sizeof(uint32_t);j+=3)
    {
       SYS_EnableHIRC1Trim(au32MIRCTrimCTLSel[i],au32MIRCTrimIENSel[j]);
       CU_ASSERT((inpw(au32MIRCTrimCTLSel[i+2]) & (au32MIRCTrimCTLSel[i+1])<<SYS_MIRCTCTL_FREQSEL_Pos)==au32MIRCTrimCTLSel[i]);
       CU_ASSERT((inpw(au32MIRCTrimIENSel[j+2]) & (au32MIRCTrimIENSel[j+1]))==au32MIRCTrimIENSel[j]);
    }
  SYS_DisableMIRCTrim();
  CU_ASSERT(SYS->MIRCTCTL == 0);

  CU_ASSERT(SYS_GET_MIRCTRIM_INT_FLAG()==SYS->MIRCTISTS);
  //SYS_CLEAR_IRCTRIM_INT_FLAG()  //Clear IRCTrim flag , can't not test.
		
  SYS->MIRCTIEN = SYS->MIRCTIEN | SYS_IRCTIEN_FAIL_EN;
  CU_ASSERT( (SYS->MIRCTIEN & 1<<SYS_MIRCTIEN_TFAILIEN_Pos) == SYS_MIRCTIEN_TFAILIEN_Msk);
  SYS->MIRCTIEN = SYS->MIRCTIEN & ~SYS_IRCTIEN_FAIL_EN;
  CU_ASSERT( (SYS->MIRCTIEN & 1<<SYS_MIRCTIEN_TFAILIEN_Pos)==0);

  SYS->MIRCTIEN = SYS->MIRCTIEN | SYS_IRCTIEN_32KERR_EN;
  CU_ASSERT( (SYS->MIRCTIEN & 1<<SYS_MIRCTIEN_CLKEIEN_Pos) == SYS_MIRCTIEN_CLKEIEN_Msk);
  SYS->MIRCTIEN = SYS->MIRCTIEN & ~SYS_IRCTIEN_32KERR_EN;
  CU_ASSERT( (SYS->MIRCTIEN & 1<<SYS_MIRCTIEN_CLKEIEN_Pos)==0);

  CU_ASSERT(SYS_IRCTISTS_FREQLOCK == (1<<SYS_MIRCTISTS_FREQLOCK_Pos));
  CU_ASSERT(SYS_IRCTISTS_FAIL_INT == (1<<SYS_MIRCTISTS_TFAILIF_Pos));
  CU_ASSERT(SYS_IRCTISTS_32KERR_INT == (1<<SYS_MIRCTISTS_CLKERRIF_Pos));

  CU_ASSERT(SYS_IRCTCTL_CLKERR_STOP == (1<<8));

  SYS_LockReg();
}

void TestFunc_SYS_TestOthers()
{

}

CU_TestInfo SYS_TestCases[] =
{
#if OPTION==1
    {"Testing SYS_TestReset Function:", TestFunc_SYS_TestReset}, // unplug ICE to test this item   
#endif
#if OPTION==2
    {"Testing SYS_GetBODStatus Function:", TestFunc_SYS_GetBODStatus},
    {"Testing SYS_HIRC0TRIMStatus Function:", TestFunc_SYS_HIRC0TRIMStatus},
    {"Testing SYS_HIRC1TRIMStatus Function:", TestFunc_SYS_HIRC1TRIMStatus},
    {"Testing SYS_MIRCTRIMStatus Function:", TestFunc_SYS_MIRCTRIMStatus},
    {"Testing SYS_GetResetSrc Function:", TestFunc_SYS_GetResetSrc}, 
#endif
#if OPTION==3
    {"Testing SYS_IsRegLocked Function:", TestFunc_SYS_IsRegLocked},
    {"Testing SYS_ReadPDID Function:", TestFunc_SYS_ReadPDID},
    {"Testing SYS_ResetModule Function:", TestFunc_SYS_ResetModule},
#endif
#if OPTION==4
    {"Testing SYS_EnableBOD Function:", TestFunc_SYS_EnableBOD},
    {"Testing SYS_TestMacro Function:", TestFunc_SYS_TestMacro},
#endif
#if OPTION==5
    {"Testing SYS_TestConstant1 Function:", TestFunc_SYS_TestConstant1},
#endif
#if OPTION==6
    {"Testing SYS_TestConstant2 Function:", TestFunc_SYS_TestConstant2},
#endif
#if OPTION==7
    {"Testing SYS_TestConstant3 Function:", TestFunc_SYS_TestConstant3}, // PF2,PF3 always Xin, Xout   PF6,PF7 always X32in, X32out(board modify for LXT)
#endif
    CU_TEST_INFO_NULL
};

