/****************************************************************************
*                                                                           *
* Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   test_fun.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The header file of CUnit test function
*
* HISTORY
*   2007-06-25    Ver 1.0 Created by NS21 WTLiu
*
* REMARK
*   None
****************************************************************************/

#ifndef _TEST_FUN_H_
#define _TEST_FUN_H_

extern CU_SuiteInfo suites[];
extern CU_TestInfo FMC_ConstantTests[];
extern CU_TestInfo FMC_MacroTests[];
extern CU_TestInfo FMC_ApiTests[];

#define TEST_CONFIG0		0xfffffffa		
#define TEST_CONFIG1        0xC000

#endif
