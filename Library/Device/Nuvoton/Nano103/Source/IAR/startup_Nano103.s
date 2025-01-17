;/****************************************************************************//**
; * @file     startup_Nano103.s
; * @version  V2.00
; * $Revision: 4 $
; * $Date: 16/01/28 3:56p $
; * @brief    CMSIS ARM Cortex-M0 Core Device Startup File
; *
; * @note
; * SPDX-License-Identifier: Apache-2.0
; * Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
;*****************************************************************************/

    MODULE  ?cstartup

    ;; Forward declaration of sections.
    SECTION CSTACK:DATA:NOROOT(3) ;; 8 bytes alignment

    SECTION .intvec:CODE:NOROOT(2);; 4 bytes alignment

    EXTERN  __iar_program_start
    EXTERN  SystemInit
    EXTERN  ProcessHardFault
    PUBLIC  __vector_table

    DATA
__vector_table
    DCD     sfe(CSTACK)
    DCD     Reset_Handler

    DCD     NMI_Handler
    DCD     HardFault_Handler
    DCD     0
    DCD     0
    DCD     0
    DCD     0
    DCD     0
    DCD     0
    DCD     0
    DCD     SVC_Handler
    DCD     0
    DCD     0
    DCD     PendSV_Handler
    DCD     SysTick_Handler

    ; External Interrupts
    DCD     BOD_IRQHandler            ; Brownout low voltage detected interrupt
    DCD     WDT_IRQHandler            ; Watch Dog Timer interrupt
    DCD     EINT0_IRQHandler          ; External signal interrupt from PB.14 pin
    DCD     EINT1_IRQHandler          ; External signal interrupt from PB.15 pin
    DCD     GPABC_IRQHandler          ; External interrupt from PA[15:0]/PB[15:0]/PC[15:0]
    DCD     GPDEF_IRQHandler          ; External interrupt from PD[15:0]/PE[15:0]/PF[7:0]
    DCD     PWM0_IRQHandler           ; PWM 0 interrupt
    DCD     0                         ; Reserved
    DCD     TMR0_IRQHandler           ; Timer 0 interrupt
    DCD     TMR1_IRQHandler           ; Timer 1 interrupt
    DCD     TMR2_IRQHandler           ; Timer 2 interrupt
    DCD     TMR3_IRQHandler           ; Timer 3 interrupt
    DCD     UART0_IRQHandler          ; UART0 interrupt
    DCD     UART1_IRQHandler          ; UART1 interrupt
    DCD     SPI0_IRQHandler           ; SPI0 interrupt
    DCD     SPI1_IRQHandler           ; SPI1 interrupt
    DCD     SPI2_IRQHandler           ; SPI2 interrupt
    DCD     HIRC_IRQHandler           ; HIRC interrupt
    DCD     I2C0_IRQHandler           ; I2C0 interrupt
    DCD     I2C1_IRQHandler           ; I2C1 interrupt
    DCD     Default_Handler           ; Reserved
    DCD     SC0_IRQHandler            ; SC0 interrupt
    DCD     SC1_IRQHandler            ; SC1 interrupt
    DCD     Default_Handler           ; Reserved
    DCD     Default_Handler           ; Reserved
    DCD     Default_Handler           ; Reserved
    DCD     PDMA_IRQHandler           ; PDMA interrupt
    DCD     SPI3_IRQHandler              ; SPI3 interrupt
    DCD     PDWU_IRQHandler           ; Power Down Wake up interrupt
    DCD     ADC_IRQHandler            ; ADC interrupt
    DCD     ACMP_IRQHandler           ; ACMP interrupt
    DCD     RTC_IRQHandler            ; Real time clock interrupt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
    THUMB
    PUBWEAK Reset_Handler
    SECTION .text:CODE:REORDER:NOROOT(2)       ; 4 bytes alignment
Reset_Handler
        LDR     R0, =0x50000100
        ; Unlock Register
        LDR     R1, =0x59
        STR     R1, [R0]
        LDR     R1, =0x16
        STR     R1, [R0]
        LDR     R1, =0x88
        STR     R1, [R0]

        ; Disable POR
        LDR     R2, =0x50000060
        LDR     R1, =0x00005AA5
        STR     R1, [R2]
        LDR     R2, =0x50000014
        LDR     R1, =0x000000C0
        STR     R1, [R2]
        ; Unlock PA9 in 32 pin package
        LDR     R0, =SystemInit
        BLX     R0
        ; Lock register
        LDR     R0, =0x50000100
        MOVS    R1, #0
        STR     R1, [R0]

        LDR      R0, =__iar_program_start
        BX       R0

    PUBWEAK HardFault_Handler
    SECTION .text:CODE:REORDER:NOROOT(2)
HardFault_Handler
        MOV     R0, LR
        MRS     R1, MSP
        MRS     R2, PSP
        LDR     R3, =ProcessHardFault
        BLX     R3
        BX      R0

    PUBWEAK NMI_Handler
    PUBWEAK SVC_Handler
    PUBWEAK PendSV_Handler
    PUBWEAK SysTick_Handler
    PUBWEAK BOD_IRQHandler
    PUBWEAK WDT_IRQHandler
    PUBWEAK EINT0_IRQHandler
    PUBWEAK EINT1_IRQHandler
    PUBWEAK GPABC_IRQHandler
    PUBWEAK GPDEF_IRQHandler
    PUBWEAK PWM0_IRQHandler
    PUBWEAK TMR0_IRQHandler
    PUBWEAK TMR1_IRQHandler
    PUBWEAK TMR2_IRQHandler
    PUBWEAK TMR3_IRQHandler
    PUBWEAK UART0_IRQHandler
    PUBWEAK UART1_IRQHandler
    PUBWEAK SPI0_IRQHandler
    PUBWEAK SPI1_IRQHandler
    PUBWEAK SPI2_IRQHandler
    PUBWEAK HIRC_IRQHandler
    PUBWEAK I2C0_IRQHandler
    PUBWEAK I2C1_IRQHandler
    PUBWEAK SC0_IRQHandler
    PUBWEAK SC1_IRQHandler
    PUBWEAK PDMA_IRQHandler
    PUBWEAK SPI3_IRQHandler
    PUBWEAK PDWU_IRQHandler
    PUBWEAK ADC_IRQHandler
    PUBWEAK ACMP_IRQHandler
    PUBWEAK RTC_IRQHandler

    SECTION .text:CODE:REORDER:NOROOT(2)

NMI_Handler
SVC_Handler
PendSV_Handler
SysTick_Handler
BOD_IRQHandler
WDT_IRQHandler
EINT0_IRQHandler
EINT1_IRQHandler
GPABC_IRQHandler
GPDEF_IRQHandler
PWM0_IRQHandler
TMR0_IRQHandler
TMR1_IRQHandler
TMR2_IRQHandler
TMR3_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
HIRC_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SC0_IRQHandler
SC1_IRQHandler
PDMA_IRQHandler
SPI3_IRQHandler
PDWU_IRQHandler
ADC_IRQHandler
ACMP_IRQHandler
RTC_IRQHandler
Default_Handler
    B Default_Handler


;int32_t SH_DoCommand(int32_t n32In_R0, int32_t n32In_R1, int32_t *pn32Out_R0)
    PUBWEAK SH_DoCommand
    SECTION .text:CODE:REORDER:ROOT(2)
SH_DoCommand
    IMPORT      SH_Return

    BKPT    0xAB                ; Wait ICE or HardFault
    LDR     R3, =SH_Return
		PUSH    {R3 ,lr}
    BLX     R3                  ; Call SH_Return. The return value is in R0
		POP     {R3 ,PC}            ; Return value = R0

    END

;/*** (C) COPYRIGHT 2024 Nuvoton Technology Corp. ***/
