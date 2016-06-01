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
#include "i2c_cunit.h"


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

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
{
    return 0;
}

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

CU_SuiteInfo suites[] = {
    {"I2C API", suite_success_init, suite_success_clean,NULL, NULL, I2C_ApiTests},
	{"I2C MACRO", suite_success_init, suite_success_clean,NULL, NULL, I2C_MacroTests},
    CU_SUITE_INFO_NULL
};

void Test_API_I2C_Open_Close()
{
    uint32_t u32BusClock;

    /* To confirm I2C is disable for I2C_Open Test*/
    if((I2C0->CTL & I2C_CTL_I2CEN_Msk) != 0x00)
        while(1);

    /* API I2C_Open Test*/
    u32BusClock = 100000;
    CU_ASSERT_EQUAL(I2C_Open(I2C0,u32BusClock),u32BusClock);
    CU_ASSERT((I2C0->CTL & I2C_CTL_I2CEN_Msk) == I2C_CTL_I2CEN_Msk);

    /* API I2C_Close Test*/
    I2C_Close(I2C0);
    CU_ASSERT((I2C0->CTL & I2C_CTL_I2CEN_Msk) != I2C_CTL_I2CEN_Msk);
	CU_ASSERT(I2C_GET_BUS_FREE_FLAG(I2C0) == 1);
	
    if((I2C1->CTL & I2C_CTL_I2CEN_Msk) != 0x00)
        while(1);

    /* API I2C_Open Test*/
    u32BusClock = 100000;
    CU_ASSERT_EQUAL(I2C_Open(I2C1,u32BusClock),u32BusClock);
    CU_ASSERT((I2C1->CTL & I2C_CTL_I2CEN_Msk) == I2C_CTL_I2CEN_Msk);

    /* API I2C_Close Test*/
    I2C_Close(I2C1);
    CU_ASSERT((I2C1->CTL & I2C_CTL_I2CEN_Msk) != I2C_CTL_I2CEN_Msk);	    
    CU_ASSERT(I2C_GET_BUS_FREE_FLAG(I2C1) == 1);
}

void Test_API_I2C_GetBusClockFreq()
{
    uint32_t u32BusClock;

    u32BusClock = 100000;
    I2C_Open(I2C0,u32BusClock);
    CU_ASSERT_EQUAL(I2C_GetBusClockFreq(I2C0),I2C_Open(I2C0,u32BusClock));
    I2C_Close(I2C0);

    I2C_Open(I2C1,u32BusClock);
    CU_ASSERT_EQUAL(I2C_GetBusClockFreq(I2C1),I2C_Open(I2C1,u32BusClock));
    I2C_Close(I2C1);
}

void Test_API_I2C_SetBusClockFreq()
{
    uint32_t u32BusClock;

    u32BusClock = 100000;
    I2C_Open(I2C0,u32BusClock);
    CU_ASSERT_EQUAL(I2C_SetBusClockFreq(I2C0, u32BusClock),I2C_Open(I2C0,u32BusClock));
    I2C_Close(I2C0);
	
    I2C_Open(I2C1,u32BusClock);
    CU_ASSERT_EQUAL(I2C_SetBusClockFreq(I2C1, u32BusClock),I2C_Open(I2C1,u32BusClock));
    I2C_Close(I2C1);
}

void Test_API_I2C_INT()
{
    I2C_EnableInt(I2C0);
    CU_ASSERT_EQUAL((I2C0->CTL & I2C_CTL_INTEN_Msk), I2C_CTL_INTEN_Msk);
    I2C_EnableInt(I2C1);
    CU_ASSERT_EQUAL((I2C1->CTL & I2C_CTL_INTEN_Msk), I2C_CTL_INTEN_Msk);

    I2C_DisableInt(I2C0);
    CU_ASSERT_NOT_EQUAL((I2C0->CTL & I2C_CTL_INTEN_Msk), I2C_CTL_INTEN_Msk);
    I2C_DisableInt(I2C1);
    CU_ASSERT_NOT_EQUAL((I2C1->CTL & I2C_CTL_INTEN_Msk), I2C_CTL_INTEN_Msk);
}

void Test_API_I2C_SetSLVAddr()
{
    uint8_t i;
    uint8_t I2C0_SLVAddr[4] = {15, 35, 55, 75};
    uint8_t I2C1_SLVAddr[4] = {16, 36, 56, 76};
    for(i=0; i<4; i++) {
        I2C_SetSlaveAddr(I2C0, i, I2C0_SLVAddr[i], 0);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL((I2C0->ADDR0) >> I2C_ADDR0_ADDR_Pos ,I2C0_SLVAddr[i]);
            break;
        case 1:
            CU_ASSERT_EQUAL((I2C0->ADDR1) >> I2C_ADDR1_ADDR_Pos ,I2C0_SLVAddr[i]);
            break;        
        }

        I2C_SetSlaveAddr(I2C1, i, I2C1_SLVAddr[i], 0);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL((I2C1->ADDR0) >> I2C_ADDR0_ADDR_Pos ,I2C1_SLVAddr[i]);
            break;
        case 1:
            CU_ASSERT_EQUAL((I2C1->ADDR1) >> I2C_ADDR1_ADDR_Pos ,I2C1_SLVAddr[i]);
            break;       
        }
    }
}

void Test_API_I2C_SetSLVAddrMsk()
{
    uint8_t i;
    uint8_t I2C0_SLVAddrMsk[4] = {0x01, 0x04, 0x01, 0x04};
    uint8_t I2C1_SLVAddrMsk[4] = {0x04, 0x02, 0x04, 0x02};
    for(i=0; i<4; i++) {
        I2C_SetSlaveAddrMask(I2C0, i, I2C0_SLVAddrMsk[i]);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL((I2C0->ADDRMSK0) >> I2C_ADDRMSK0_ADDRMSK_Pos ,I2C0_SLVAddrMsk[i]);
            break;
        case 1:
            CU_ASSERT_EQUAL((I2C0->ADDRMSK1) >> I2C_ADDRMSK1_ADDRMSK_Pos ,I2C0_SLVAddrMsk[i]);
            break;       
        }
        I2C_SetSlaveAddrMask(I2C1, i, I2C1_SLVAddrMsk[i]);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL((I2C1->ADDRMSK0) >> I2C_ADDRMSK0_ADDRMSK_Pos ,I2C1_SLVAddrMsk[i]);
            break;
        case 1:
            CU_ASSERT_EQUAL((I2C1->ADDRMSK1) >> I2C_ADDRMSK1_ADDRMSK_Pos ,I2C1_SLVAddrMsk[i]);
            break;        
        }
    }
}

void Test_API_I2C_SetSLVAddrGC()
{
    uint8_t i;
    uint8_t I2C0_SLVAddr[4] = {15, 35, 55, 75};
    uint8_t I2C1_SLVAddr[4] = {16, 36, 56, 76};

    for(i=0; i<4; i++) {
        I2C_SetSlaveAddr(I2C0,i, I2C0_SLVAddr[i], 1);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL(I2C0->ADDR0 & I2C_ADDR0_GC_Msk, 1);
            break;
        case 1:
            CU_ASSERT_EQUAL(I2C0->ADDR1 & I2C_ADDR1_GC_Msk, 1);
            break;       
        }
        I2C_SetSlaveAddr(I2C1,i, I2C1_SLVAddr[i], 1);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL(I2C1->ADDR0 & I2C_ADDR0_GC_Msk, 1);
            break;
        case 1:
            CU_ASSERT_EQUAL(I2C1->ADDR1 & I2C_ADDR1_GC_Msk, 1);
            break;        
        }
    }

    for(i=0; i<4; i++) {
        I2C_SetSlaveAddr(I2C0,i, I2C0_SLVAddr[i], 0);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL(I2C0->ADDR0 & I2C_ADDR0_GC_Msk, 0);
            break;
        case 1:
            CU_ASSERT_EQUAL(I2C0->ADDR1 & I2C_ADDR1_GC_Msk, 0);
            break;        
        }
        I2C_SetSlaveAddr(I2C1,i, I2C1_SLVAddr[i], 0);
        switch(i) {
        case 0:
            CU_ASSERT_EQUAL(I2C1->ADDR0 & I2C_ADDR0_GC_Msk, 0);
            break;
        case 1:
            CU_ASSERT_EQUAL(I2C1->ADDR1 & I2C_ADDR1_GC_Msk, 0);
            break;        
        }
    }
}

void Test_API_I2C_Set_Get_Data()
{
    I2C_SetData(I2C0, 0x5A);
    CU_ASSERT_EQUAL(I2C0->DAT, 0x5A);
    CU_ASSERT_EQUAL(I2C_GetData(I2C0), 0x5A);

    I2C_SetData(I2C1, 0xA5);
    CU_ASSERT_EQUAL(I2C1->DAT, 0xA5);
    CU_ASSERT_EQUAL(I2C_GetData(I2C1), 0xA5);
}

void Test_API_I2C_Timeout()
{
    I2C_Open(I2C0, 100000);
    I2C_EnableTimeout(I2C0, 1);
    CU_ASSERT_EQUAL(I2C0->TOCTL & I2C_TOCTL_TOCDIV4_Msk, I2C_TOCTL_TOCDIV4_Msk);
    CU_ASSERT_EQUAL(I2C0->TOCTL & I2C_TOCTL_TOCEN_Msk, I2C_TOCTL_TOCEN_Msk);
	
	I2C_Trigger(I2C0, 1, 0, 0, 0);
    while(I2C_GetIntFlag(I2C0)==0);
    CU_ASSERT((I2C0->INTSTS & 0x1) == 0x1);
    CU_ASSERT(I2C_GetStatus(I2C0) == 0x08);
	
	CU_ASSERT(I2C_GET_BUS_FREE_FLAG(I2C0) == 0);
	
	I2C_Trigger(I2C0, 0, 0, 1, 0);
    while(I2C_GetIntFlag(I2C0)==0);
    CU_ASSERT((I2C0->INTSTS & 0x1) == 0x1);
    
	/* Set PA.9 as GPIO */
    SYS->GPA_MFPH &= ~SYS_GPA_MFPH_PA9MFP_Msk;	
	
    while((I2C0->INTSTS & I2C_INTSTS_TOIF_Msk) == 0);
    I2C_ClearTimeoutFlag(I2C0);
    CU_ASSERT_EQUAL(I2C0->INTSTS & I2C_INTSTS_TOIF_Msk, 0);

    I2C_DisableTimeout(I2C0);
    CU_ASSERT_EQUAL(I2C0->TOCTL & I2C_TOCTL_TOCEN_Msk, 0);
    I2C_Close(I2C0);
	
	/* Set PA.9 as I2C_CLK */
	SYS->GPA_MFPH = (SYS->GPA_MFPH & ~SYS_GPA_MFPH_PA9MFP_Msk) | SYS_GPA_MFPH_PA9MFP_I2C0_SCL;	
	
    I2C_Open(I2C1, 100000);
    I2C_EnableTimeout(I2C1, 1);
    CU_ASSERT_EQUAL(I2C1->TOCTL & I2C_TOCTL_TOCDIV4_Msk, I2C_TOCTL_TOCDIV4_Msk);
    CU_ASSERT_EQUAL(I2C1->TOCTL & I2C_TOCTL_TOCEN_Msk, I2C_TOCTL_TOCEN_Msk);
	
	I2C_Trigger(I2C1, 1, 0, 0, 0);
    while(I2C_GetIntFlag(I2C1)==0);
    CU_ASSERT((I2C1->INTSTS & 0x1) == 0x1);
    CU_ASSERT(I2C_GetStatus(I2C1) == 0x08);
	
	CU_ASSERT(I2C_GET_BUS_FREE_FLAG(I2C1) == 0);
	
	I2C_Trigger(I2C1, 0, 0, 1, 0);
    while(I2C_GetIntFlag(I2C1)==0);
    CU_ASSERT((I2C1->INTSTS & 0x1) == 0x1);
	
	/* Set PA.11 as GPIO */
    SYS->GPA_MFPH &= ~SYS_GPA_MFPH_PA11MFP_Msk;	
	
    while((I2C1->INTSTS & I2C_INTSTS_TOIF_Msk) == 0);
    while(I2C_GET_TIMEOUT_FLAG(I2C1) == 0);
    I2C_CLEAR_TIMEOUT_FLAG(I2C1);
    CU_ASSERT_EQUAL(I2C1->INTSTS & I2C_INTSTS_TOIF_Msk, 0);

    I2C_DisableTimeout(I2C1);
    CU_ASSERT_EQUAL(I2C1->TOCTL & I2C_TOCTL_TOCEN_Msk, 0);
    I2C_Close(I2C1);
	
	/* Set PA.11 as I2C_CLK */
	SYS->GPA_MFPH = (SYS->GPA_MFPH & ~SYS_GPA_MFPH_PA11MFP_Msk) | SYS_GPA_MFPH_PA11MFP_I2C1_SCL;
}

void Test_API_I2C_Control_Read_Status()
{
    /* Open I2C0 module and set bus clock */
    I2C_Open(I2C0, 100000);

    /* Set I2C0 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */

    /* Open I2C1 module and set bus clock */
    I2C_Open(I2C1, 200000);

    /* Set I2C1 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */

    /* Set I2C1 4 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);

    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_AA | I2C_SI);

    /* I2C as master sends START signal */
    I2C_Trigger(I2C0, 1, 0, 0, 0);
    while(I2C_GetIntFlag(I2C0)==0);
    CU_ASSERT((I2C0->INTSTS & 0x1) == 0x1);
    CU_ASSERT(I2C_GetStatus(I2C0) == 0x08);
    I2C_ClearIntFlag(I2C0);
	
	I2C_SetData(I2C0, 0x16<<1);
    CU_ASSERT(I2C0->DAT == (0x16<<1));
    CU_ASSERT(I2C_GetData(I2C0) == (0x16<<1));
    
	I2C_Trigger(I2C0, 0, 0, 1, 0);
    CU_ASSERT((I2C0->CTL & I2C_SI) == 0);    
	while(I2C_GetIntFlag(I2C0)==0);
    CU_ASSERT((I2C0->INTSTS & 0x1) == 0x1);
    CU_ASSERT(I2C_GetStatus(I2C0) == 0x18);

	I2C_Close(I2C0);
    I2C_Close(I2C1);
}

void Test_MACRO_I2C_Control_Read_Status()
{
    /* Open I2C0 module and set bus clock */
    I2C_Open(I2C0, 100000);

    /* Set I2C0 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */

    /* Open I2C1 module and set bus clock */
    I2C_Open(I2C1, 200000);

    /* Set I2C1 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */

    /* Set I2C1 4 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);

    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_AA | I2C_SI);

	I2C_START(I2C0);
    CU_ASSERT((I2C0->CTL & I2C_STA) == I2C_STA);

    while(I2C_GET_STATUS(I2C0) != 0x08);
    CU_ASSERT(I2C_GET_STATUS(I2C0) == 0x08);
    CU_ASSERT((I2C0->STATUS) == 0x08);
	I2C_ClearIntFlag(I2C0);
	
    I2C_SET_DATA(I2C0, 0x16<<1);
    CU_ASSERT((I2C0->DAT)==(0x16<<1));
    CU_ASSERT(I2C_GET_DATA(I2C0) == (0x16<<1));
    I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    I2C_WAIT_READY(I2C0);
	while(I2C_GET_STATUS(I2C1) != 0x60);
    while(I2C_GET_STATUS(I2C0) != 0x18);
	
	I2C_WAIT_READY(I2C1);
	I2C_SET_CONTROL_REG(I2C1, I2C_AA | I2C_SI);
	
	I2C_SET_DATA(I2C0, 0x17);
	I2C_SET_CONTROL_REG(I2C0, I2C_SI);
	I2C_WAIT_READY(I2C0);
    while(I2C_GET_STATUS(I2C1) != 0x80);
    I2C_ClearIntFlag(I2C0);
	
	I2C_WAIT_READY(I2C1);
	I2C_SET_CONTROL_REG(I2C1, I2C_AA | I2C_SI);
	
	I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    I2C_STOP(I2C0);    
	CU_ASSERT((I2C0->CTL & I2C_STO) != I2C_STO);
    while(I2C_GET_STATUS(I2C1) != 0xA0);
    I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    
	I2C_Close(I2C0);
    I2C_Close(I2C1);
}

void Test_MACRO_I2C_GC_MODE()
{
	 CU_ASSERT_EQUAL(I2C_GCMODE_ENABLE, 1);
	 CU_ASSERT_EQUAL(I2C_GCMODE_DISABLE , 0);
}

void Test_MACRO_I2C_DATA_MODE()
{
	 I2C_ENABLE_DATAMODE(I2C0);
     CU_ASSERT_EQUAL(I2C0->CTL2 & I2C_CTL2_DATMODE_Msk, 0x40);
     
     I2C_DISABLE_DATAMODE(I2C0);
     CU_ASSERT_EQUAL(I2C0->CTL2 & I2C_CTL2_DATMODE_Msk, 0x00);
     
     I2C_SET_DATAMODE_READ(I2C0);
     CU_ASSERT_EQUAL(I2C0->CTL2 & I2C_CTL2_MSDAT_Msk, 0x80);
     
     I2C_SET_DATAMODE_WRITE(I2C0);
     CU_ASSERT_EQUAL(I2C0->CTL2 & I2C_CTL2_MSDAT_Msk, 0x00);
}

void PDWU_IRQHandler(void)
{
    /* Clear I2C wake up flag */
    I2C_CLEAR_WAKEUP_FLAG(I2C0);

    /* Clear wake up flag */
    CLK->WKINTSTS = 1;
}

void Test_API_MACRO_I2C_Wakeup()
{
    volatile uint8_t u8RxData[3];
    volatile uint8_t u8ReadWrite;
    
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);

    /* Set I2C 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x25, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */

    /* Set I2C 4 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C0, 0, 0x01);
    I2C_SetSlaveAddrMask(I2C0, 1, 0x04);

    /* Set I2C0 enter Not Address SLAVE mode */
    I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA);

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable I2C wake-up */
    I2C_EnableWakeup(I2C0);
    CU_ASSERT((I2C0->CTL2 & I2C_CTL2_WKUPEN_Msk) == I2C_CTL2_WKUPEN_Msk);
	
	CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;
	NVIC_EnableIRQ(PDWU_IRQn);
	
    /* Processor use deep sleep */
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;

    /* System power down enable */
    CLK->PWRCTL |= (CLK_PWRCTL_PDEN_Msk | CLK_PWRCTL_PDWKDLY_Msk );
	
    printf("\n");
    printf("CHIP enter power down status.\n");

    /* Waiting for UART printf finish*/
    while(UART_GET_TX_EMPTY(UART0) == 0);

    if(((I2C0->CTL)&I2C_SI) != 0) {
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }

    /*  Use WFI instruction to idle the CPU. NOTE:
        If ICE is attached, system will wakeup immediately because ICE is a wakeup event. */
    __WFI();
	
	while(!I2C_GET_WAKEUP_ACK_DONE_FLAG(I2C0));
    CU_ASSERT((I2C0->INTSTS & 0x80) == 0x80);

    /* Remember master wants to read or write data */
    u8ReadWrite = I2C_GET_WAKEUP_RW_FLAG(I2C0);
    CU_ASSERT(u8ReadWrite == ((I2C0->STATUS2 & 0x8) >> 3));

    /* If master wants to read, we need to prepare Tx output data here before release SCK pin */
    if(u8ReadWrite)
        I2C_SET_DATA(I2C0, 0x11);

    /* I2C can release SCK pin and enter I2C normal operation */
    I2C_CLEAR_WAKEUP_ACK_DONE_FLAG(I2C0);
    CU_ASSERT((I2C0->INTSTS & 0x80) == 0);

    if(u8ReadWrite) {   // master wants to read
        /* 1st data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA);

        /* 2nd data */
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        I2C_SET_DATA(I2C0, 0x55);                   // prepare tx data
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it

        /* 3rd data */
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        I2C_SET_DATA(I2C0, 0xAA);                   // prepare tx data
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it

        /* stop */
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_STO);// trigger it
        while(I2C0->CTL & I2C_CTL_STO_Msk);        // wait until STOP happens
    } else {        // master wants to write
        /* 1st data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        u8RxData[0] = I2C_GET_DATA(I2C0);           // receive data

        /* 2nd data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        u8RxData[1] = I2C_GET_DATA(I2C0);           // receive data

        /* 3rd data */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_AA); // trigger it
        I2C_WAIT_READY(I2C0);                       // wait if interrupt happens
        u8RxData[2] = I2C_GET_DATA(I2C0);           // receive data

        /* stop */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI | I2C_STO);// trigger it
        while(I2C0->CTL & I2C_CTL_STO_Msk);        // wait until STOP happens
    }
      	
    /* Disable power wake-up interrupt */
    CLK->PWRCTL &= ~CLK_PWRCTL_PDWKIEN_Msk;
    NVIC_EnableIRQ(PDWU_IRQn);
    if((I2C0->STATUS2 & I2C_STATUS2_WKIF_Msk) == I2C_STATUS2_WKIF_Msk) {
        CU_ASSERT(I2C_GET_WAKEUP_FLAG(I2C0) == 1);
        I2C_CLEAR_WAKEUP_FLAG(I2C0);
        CU_ASSERT(I2C_GET_WAKEUP_FLAG(I2C0) == 0);
    }
    /* Lock protected registers */
    SYS_LockReg();

    I2C_DisableWakeup(I2C0);
    CU_ASSERT((I2C0->CTL2 & I2C_CTL2_WKUPEN_Msk) == 0);
    I2C_Close(I2C0);
}

void Test_API_I2C_FIFO()
{
    /* 
		Cross port-0 <--> port-1
	*/
	
	/* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);
    
	/* Open I2C1 module and set bus clock */
    I2C_Open(I2C1, 200000);
    /* Set I2C1 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */
    I2C_SetSlaveAddr(I2C1, 2, 0x56, 0);   /* Slave Address : 0x56 */
    I2C_SetSlaveAddr(I2C1, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_AA | I2C_SI);
	
    I2C_ENABLE_CLOCK_STRETCH(I2C0);
    CU_ASSERT_EQUAL(I2C0->CTL2 & 0x20, 0x0);
    
    I2C_DISABLE_CLOCK_STRETCH(I2C0);
    CU_ASSERT_EQUAL(I2C0->CTL2 & 0x20, 0x20);
    
    I2C_ENABLE_OVERRUN_INT(I2C0);
    CU_ASSERT(I2C0->CTL2 & 0x2);
    
    I2C_DISABLE_OVERRUN_INT(I2C0);
    CU_ASSERT((I2C0->CTL2 & 0x2) == 0);
    
    I2C_ENABLE_UNDERRUN_INT(I2C0);
    CU_ASSERT(I2C0->CTL2 & 0x4);
    
    I2C_DISABLE_UNDERRUN_INT(I2C0);
    CU_ASSERT((I2C0->CTL2 & 0x4) == 0);
    
    I2C_ENABLE_FIFO(I2C0);
    CU_ASSERT(I2C0->CTL2 & 0x10);
    
    /* START */
    I2C_SET_CONTROL_REG(I2C0, I2C_STA);
    while(I2C_GetIntFlag(I2C0)==0);
    
    I2C_SET_DATA(I2C0, 0x16 << 1);                
    I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    I2C_SET_DATA(I2C0, 0x11);
	while(I2C_GetIntFlag(I2C0)==0);
	
	 I2C_Trigger(I2C1, 0, 0, 1, 1);
    
	I2C_SET_DATA(I2C0, 0x12);
    I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    while(I2C_GetIntFlag(I2C0)==0);    
	
	 I2C_Trigger(I2C1, 0, 0, 1, 1);
	 
	I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
    while(I2C_GetIntFlag(I2C0)==0);

	 I2C_Trigger(I2C1, 0, 0, 1, 1);

    I2C_SET_CONTROL_REG(I2C0, I2C_SI);
	
	I2C_DISABLE_FIFO(I2C0);
    CU_ASSERT((I2C0->CTL2 & 0x10) == 0);
	
	I2C_Close(I2C0);
	I2C_Close(I2C1);
}

CU_TestInfo I2C_ApiTests[] = {
    {" 1: API I2C_Open_Close.", Test_API_I2C_Open_Close},
    {" 2: API I2C_Get_Bus_Clock", Test_API_I2C_GetBusClockFreq},
    {" 3: API I2C_Set_Bus_Clock", Test_API_I2C_SetBusClockFreq},
    {" 4: API I2C_INT_En_Dis_ABLE", Test_API_I2C_INT},
    {" 5: API I2C_Set_SLV_Address", Test_API_I2C_SetSLVAddr},
    {" 6: API I2C_Set_SLV_Address_Mask", Test_API_I2C_SetSLVAddrMsk},
    {" 7: API I2C_Set_SLV_GCMode", Test_API_I2C_SetSLVAddrGC},
    {" 8: API I2C_Set_Get_Data", Test_API_I2C_Set_Get_Data},
    {" 9: API I2C_Time-out_En_Dis_able", Test_API_I2C_Timeout},
    {"10: API I2C_Control_Read_STATUS", Test_API_I2C_Control_Read_Status},
    {"11: API I2C_FIFO", Test_API_I2C_FIFO},
    CU_TEST_INFO_NULL
};

CU_TestInfo I2C_MacroTests[]= {
	{" 1: MACRO I2C_Read_Status", Test_MACRO_I2C_Control_Read_Status},
//    {" 2: MACRO I2C_Wakeup", Test_API_MACRO_I2C_Wakeup},
    {" 3: MACRO I2C_DataMode", Test_MACRO_I2C_DATA_MODE},
    CU_TEST_INFO_NULL
};
