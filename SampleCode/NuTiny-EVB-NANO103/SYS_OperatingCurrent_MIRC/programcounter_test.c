/******************************************************************************
 * @file     programcounter_test.c
 * @version  V0.10
 * $Revision: 1 $
 * $Date: 16/03/29 9:15a $
 * @brief    Program Counter Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"


void CPU_PC_Test(void);

#ifdef __ICCARM__
#pragma optimize=none
#endif

#ifdef __ARMCC_VERSION
#pragma O0
#endif

#define RETURNVALUE 10

//return a defined value
#ifdef __ICCARM__
#pragma default_function_attributes = @ "pc_test1"
void pc_test_return1(void)
{
    while (1);
}

#pragma default_function_attributes =

//return myself address
#pragma default_function_attributes = @ "pc_test2"
uint32_t pc_test_return2(void)
{
    return ((uint32_t) &pc_test_return2);
}
#pragma default_function_attributes =
#endif

#ifdef __ARMCC_VERSION
__attribute__((section("pc_test1")))
uint32_t pc_test_return1(void)
{
    return RETURNVALUE;
}

//return myself address
__attribute__((section("pc_test2")))
uint32_t pc_test_return2(void)
{
    return ((uint32_t) &pc_test_return2);
}
#endif

void CPU_PC_Test(void)
{    
    pc_test_return1();
    
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
