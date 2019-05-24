/***************************************************************************//**
 * @file     targetdev.h
 * @brief    UART ISP slave header file
 * @version  2.0.0
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "Nano103.h"
#include "uart_transfer.h"
#include "ISP_USER.h"

/* rename for uart_transfer.c */
#define UART_N                          UART0
#define UART_N_IRQHandler       UART0_IRQHandler
#define UART_N_IRQn                 UART0_IRQn

/* 0: Disable, 1: Enable */
#define USE_ISP_UART                (1)
