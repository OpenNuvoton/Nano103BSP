/**************************************************************************//**
 * @file     dsrc.h
 * @version  V1.00
 * $Revision: 10 $
 * $Date: 16/01/28 3:38p $
 * @brief    NANO103 series DSRC driver header file
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
 
#ifndef __DSRC_H__
#define __DSRC_H__

#ifdef __cplusplus
extern "C"
{
#endif
  
#define DSRC_IRQn ((IRQn_Type)25)     /*!< DSRC interrupt                                            */
  
#define SYS_IPRST2_DSRCRST_Pos           (7)                                               /*!< SYS_T::IPRST2: DSRCRST Position        */
#define SYS_IPRST2_DSRCRST_Msk           (0x1ul << SYS_IPRST2_DSRCRST_Pos)                 /*!< SYS_T::IPRST2: DSRCRST Mask            */
  
#define CLK_APBCLK_DSRCCKEN_Pos          (7)                                               /*!< CLK_T::APBCLK: DSRCCKEN Position       */
#define CLK_APBCLK_DSRCCKEN_Msk          (0x1ul << CLK_APBCLK_DSRCCKEN_Pos)                /*!< CLK_T::APBCLK: DSRCCKEN Mask           */
  
#define CLK_CLKSEL2_DSRCSEL_Pos          (7)                                               /*!< CLK_T::CLKSEL2: DSRCSEL Position       */
#define CLK_CLKSEL2_DSRCSEL_Msk          (0x1ul << CLK_CLKSEL2_DSRCSEL_Pos)                /*!< CLK_T::CLKSEL2: DSRCSEL Mask           */
  
#define CLK_CLKDIV1_DSRCDIV_Pos          (24)                                              /*!< CLK_T::CLKDIV1: DSRCDIV Position       */
#define CLK_CLKDIV1_DSRCDIV_Msk          (0x1ful << CLK_CLKDIV1_DSRCDIV_Pos)               /*!< CLK_T::CLKDIV1: DSRCDIV Mask           */
  
#define TIMER_CTL_DSRCCTLF_Pos             (31)                                            /*!< TIMER_T::CTL DSRCCTLF Position           */
#define TIMER_CTL_DSRCCTLF_Msk             (0x1ul << TIMER_CTL_DSRCCTLF_Pos)               /*!< TIMER_T::CTL DSRCCTLF Mask               */
  
/*clk.h*/
/********************* Bit definition of APBCLK register **********************/
#define CLK_APBCLK_DSRC_EN        ((uint32_t)0x00000080)      /*!<DSRC clock Enable Control */
/********************* Bit definition of CLKSEL2 register **********************/
#define CLK_CLKSEL2_DSRCSEL_HIRC         (0x1UL<<CLK_CLKSEL2_DSRCSEL_Pos)         /*!<Select DSRC clock source from high speed oscillator */
#define CLK_CLKSEL2_DSRCSEL_MIRC         (0x0UL<<CLK_CLKSEL2_DSRCSEL_Pos)         /*!<Select DSRC clock source from medium speed oscillator */
/********************* Bit definition of CLKDIV0/CLKDIV1 register **********************/
#define CLK_DSRC_CLK_DIVIDER(x)        ((((uint32_t)x-1)<<CLK_CLKDIV1_DSRCDIV_Pos) & CLK_CLKDIV1_DSRCDIV_Msk)          /* CLKDIV1 Setting for DSRC clock divider. It could be 1~31*/

/*-------------------------------------------------------------------------------------------------------------------------------*/
/*   AHBCLK/APBCLK(1) | CLKSEL(2) | CLKSEL_Msk(4) |  CLKSEL_Pos(5) | CLKDIV(2) | CLKDIV_Msk(8) |  CLKDIV_Pos(5)  |  IP_EN_Pos(5) */
/*-------------------------------------------------------------------------------------------------------------------------------*/
#define DSRC_MODULE      (( 1UL<<31)|( 2<<29)|(            1<<25)|( 7<<20)|( 1<<18)|(         0x1F<<10)|(24<<5)|( 7<<0)) /*!< DSRC Module  \hideinitializer */

/*sys.h*/
/*  Module Reset Control Resister constant definitions.                                                    */
#define DSRC_RST  ((0x4<<24) | SYS_IPRST2_DSRCRST_Pos  ) /*!< DSRC reset is one of the SYS_ResetModule parameter */

/********************* Bit definition of DSRC MFP register **********************/
#define SYS_GPA_MFPL_PA0MFP_DSRC_TEST_EN      (0x07UL<<SYS_GPA_MFPL_PA0MFP_Pos) /* DSRC enable test mode pin. */ 
#define SYS_GPA_MFPL_PA2MFP_DSRC_TEST_CS0     (0x07UL<<SYS_GPA_MFPL_PA2MFP_Pos) /* DSRC test cs0 pin. */ 
#define SYS_GPA_MFPL_PA3MFP_DSRC_TEST_CS1     (0x07UL<<SYS_GPA_MFPL_PA3MFP_Pos) /* DSRC test cs1 pin. */ 
#define SYS_GPA_MFPL_PA4MFP_DSRC_TEST_CS2     (0x07UL<<SYS_GPA_MFPL_PA4MFP_Pos) /* DSRC test cs2 pin. */ 
#define SYS_GPA_MFPL_PA5MFP_DSRC_TEST_CS3     (0x07UL<<SYS_GPA_MFPL_PA5MFP_Pos) /* DSRC test cs3 pin. */ 
#define SYS_GPB_MFPL_PB0MFP_DSRC_FM0_RX       (0x07UL<<SYS_GPB_MFPL_PB0MFP_Pos) /* DSRC FM0 receive pin. */ 
#define SYS_GPB_MFPL_PB1MFP_DSRC_FM0_TX       (0x07UL<<SYS_GPB_MFPL_PB1MFP_Pos) /* DSRC FM0 transfer pin. */ 
#define SYS_GPB_MFPL_PB2MFP_DSRC_WAKEUP       (0x02UL<<SYS_GPB_MFPL_PB2MFP_Pos) /* DSRC RF wakeup pin. */ 
#define SYS_GPB_MFPL_PB4MFP_DSRC_TEST_TIME_OUT (0x07UL<<SYS_GPB_MFPL_PB4MFP_Pos) /* DSRC test time out pin. */ 
#define SYS_GPB_MFPL_PB5MFP_DSRC_TEST_TIME_START (0x07UL<<SYS_GPB_MFPL_PB5MFP_Pos) /* DSRC time start pin. */ 
#define SYS_GPB_MFPH_PB9MFP_DSRC_RX_EN        (0x07UL<<SYS_GPB_MFPH_PB9MFP_Pos) /* DSRC RF RX enable pin. */ 
#define SYS_GPB_MFPH_PB10MFP_DSRC_TX_EN       (0x07UL<<SYS_GPB_MFPH_PB10MFP_Pos) /* DSRC RF TX enable pin. */ 
#define SYS_GPC_MFPH_PC10MFP_DSRC_FM0_TX      (0x07UL<<SYS_GPC_MFPH_PC10MFP_Pos) /* DSRC FM0 transfer pin. */ 
#define SYS_GPC_MFPH_PC11MFP_DSRC_FM0_RX      (0x07UL<<SYS_GPC_MFPH_PC11MFP_Pos) /* DSRC FM0 receive pin. */ 
#define SYS_GPE_MFPL_PE5MFP_DSRC_WAKEUP       (0x02UL<<SYS_GPE_MFPL_PE5MFP_Pos) /* DSRC RF wakeup pin. */ 
  
/// @cond HIDDEN_SYMBOLS

/** @addtogroup NANO103_Peripherals NANO103 Peripherals
  NANO103 Device Specific Peripheral registers structures
  @{
*/

/**
    @addtogroup Dedicated Short Range Communication Controller (DSRC)
    Memory Mapped Structure for DSRC Controller
@{ */
#if defined ( __CC_ARM  )
#pragma anon_unions
#endif
typedef struct {
    __IO uint32_t CTL;
    __IO uint32_t INTEN;
    __IO uint32_t STATUS;
    __IO uint32_t PREAMBLE;
    __IO uint32_t CRCSEED;
    __IO uint32_t RBCNT;
    __IO uint32_t CHKSUM;
         uint32_t Resived0[1];
    __IO uint32_t TX;
    __IO uint32_t RX;
         uint32_t Resived1[2];
    __IO uint32_t TO[5];
} DSRC_T;

typedef struct {
    __IO uint32_t CTL2;
         uint32_t Resived1[3];
    __IO uint32_t TXR;
    __IO uint32_t RXR;
    __IO uint32_t ICR;
} DSRCS_T;

/**
    @addtogroup DSRC_CONST DSRC Bit Field Definition
    Constant Definitions for DSRC Controller
@{ */

#define DSRC_CTL_DSRCEN_Pos				(0)                                               /*!< DSRC_T::CTL: DSRCEN Position                  */
#define DSRC_CTL_DSRCEN_Msk				(0x1ul<<DSRC_CTL_DSRCEN_Pos)                      /*!< DSRC_T::CTL: DSRCEN Mask                      */
#define DSRC_CTL_SWRXEN_Pos				(1)                                               /*!< DSRC_T::CTL: SWRXEN Position                  */
#define DSRC_CTL_SWRXEN_Msk				(0x1ul<<DSRC_CTL_SWRXEN_Pos)                      /*!< DSRC_T::CTL: SWRXEN Mask                      */
#define DSRC_CTL_CODECEN_Pos			(2)                                               /*!< DSRC_T::CTL: CODECEN Position                 */
#define DSRC_CTL_CODECEN_Msk			(0x1ul<<DSRC_CTL_CODECEN_Pos)                     /*!< DSRC_T::CTL: CODECEN Mask                     */
#define DSRC_CTL_CODECFMT_Pos			(3)                                               /*!< DSRC_T::CTL: CODECFMT Position                */
#define DSRC_CTL_CODECFMT_Msk			(0x1ul<<DSRC_CTL_CODECFMT_Pos)                    /*!< DSRC_T::CTL: CODECFMT Mask                    */
#define DSRC_CTL_BRDETEN_Pos			(4)                                               /*!< DSRC_T::CTL: BRDETEN Position                 */
#define DSRC_CTL_BRDETEN_Msk			(0x1ul<<DSRC_CTL_BRDETEN_Pos)                     /*!< DSRC_T::CTL: BRDETEN Mask                     */
#define DSRC_CTL_BRATEMOD_Pos			(5)                                               /*!< DSRC_T::CTL: BRATEMOD Position                */
#define DSRC_CTL_BRATEMOD_Msk			(0x1ul<<DSRC_CTL_BRATEMOD_Pos)                    /*!< DSRC_T::CTL: BRATEMOD Mask                    */
#define DSRC_CTL_CRCEN_Pos				(8)                                               /*!< DSRC_T::CTL: CRCEN Position                   */
#define DSRC_CTL_CRCEN_Msk				(0x1ul<<DSRC_CTL_CRCEN_Pos)                       /*!< DSRC_T::CTL: CRCEN Mask                       */
#define DSRC_CTL_CRCMSB_Pos				(9)                                               /*!< DSRC_T::CTL: CRCMSB Position                  */
#define DSRC_CTL_CRCMSB_Msk				(0x1ul<<DSRC_CTL_CRCMSB_Pos)                      /*!< DSRC_T::CTL: CRCMSB Mask                      */
#define DSRC_CTL_CHKSREV_Pos			(10)                                              /*!< DSRC_T::CTL: CHKSREV Position                 */
#define DSRC_CTL_CHKSREV_Msk			(0x1ul<<DSRC_CTL_CHKSREV_Pos)                     /*!< DSRC_T::CTL: CHKSREV Mask                     */
#define DSRC_CTL_CHKFMT_Pos				(11)                                              /*!< DSRC_T::CTL: CHKFMT Position                  */
#define DSRC_CTL_CHKFMT_Msk				(0x1ul<<DSRC_CTL_CHKFMT_Pos)                      /*!< DSRC_T::CTL: CHKFMT Mask                      */
#define DSRC_CTL_SEEDM_Pos				(12)                                              /*!< DSRC_T::CTL: SEEDM Position                   */
#define DSRC_CTL_SEEDM_Msk				(0x1ul<<DSRC_CTL_SEEDM_Pos)                       /*!< DSRC_T::CTL: SEEDM Mask                       */
#define DSRC_CTL_PREAMFMT_Pos			(13)                                              /*!< DSRC_T::CTL: PREAMFMT Position                */
#define DSRC_CTL_PREAMFMT_Msk			(0x1ul<<DSRC_CTL_PREAMFMT_Pos)                    /*!< DSRC_T::CTL: PREAMFMT Mask                    */
#define DSRC_CTL_PDRXOFF_Pos			(14)                                              /*!< DSRC_T::CTL: PDRXOFF Position                 */
#define DSRC_CTL_PDRXOFF_Msk			(0x1ul<<DSRC_CTL_PDRXOFF_Pos)                     /*!< DSRC_T::CTL: PDRXOFF Mask                     */
#define DSRC_CTL_WKHXTEN_Pos			(15)                                              /*!< DSRC_T::CTL: WKHXTEN Position                 */
#define DSRC_CTL_WKHXTEN_Msk			(0x1ul<<DSRC_CTL_WKHXTEN_Pos)                     /*!< DSRC_T::CTL: WKHXTEN Mask                     */
#define DSRC_CTL_TBPEN_Pos				(16)                                              /*!< DSRC_T::CTL: TBPEN Position                   */
#define DSRC_CTL_TBPEN_Msk				(0x1ul<<DSRC_CTL_TBPEN_Pos)                       /*!< DSRC_T::CTL: TBPEN Mask                       */
#define DSRC_CTL_TRANSFIN_Pos			(17)                                              /*!< DSRC_T::CTL: TRANSFIN Position                */
#define DSRC_CTL_TRANSFIN_Msk			(0x1ul<<DSRC_CTL_TRANSFIN_Pos)                    /*!< DSRC_T::CTL: TRANSFIN Mask                    */
#define DSRC_CTL_TTDMAEN_Pos			(18)                                              /*!< DSRC_T::CTL: TTDMAEN Position                 */
#define DSRC_CTL_TTDMAEN_Msk			(0x1ul<<DSRC_CTL_TTDMAEN_Pos)                     /*!< DSRC_T::CTL: TTDMAEN Mask                     */
#define DSRC_CTL_TRDMAEN_Pos			(19)                                              /*!< DSRC_T::CTL: TRDMAEN Position                 */
#define DSRC_CTL_TRDMAEN_Msk			(0x1ul<<DSRC_CTL_TRDMAEN_Pos)                     /*!< DSRC_T::CTL: TRDMAEN Mask                     */
#define DSRC_CTL_CRCBSWAP_Pos			(20)                                              /*!< DSRC_T::CTL: CRCBSWAP Position                */
#define DSRC_CTL_CRCBSWAP_Msk			(0x1ul<<DSRC_CTL_CRCBSWAP_Pos)                    /*!< DSRC_T::CTL: CRCBSWAP Mask                    */
#define DSRC_CTL_FORCETX_Pos			(21)                                              /*!< DSRC_T::CTL: FORCETX Position                 */
#define DSRC_CTL_FORCETX_Msk			(0x1ul<<DSRC_CTL_FORCETX_Pos)                     /*!< DSRC_T::CTL: FORCETX Mask                     */
#define DSRC_CTL_FORCERX_Pos			(22)                                              /*!< DSRC_T::CTL: FORCERX Position                 */
#define DSRC_CTL_FORCERX_Msk			(0x1ul<<DSRC_CTL_FORCERX_Pos)                     /*!< DSRC_T::CTL: FORCERX Mask                     */
#define DSRC_CTL_RXON_Pos					(23)                                              /*!< DSRC_T::CTL: RXON Position                    */
#define DSRC_CTL_RXON_Msk					(0x1ul<<DSRC_CTL_RXON_Pos)                        /*!< DSRC_T::CTL: RXON Mask                        */
#define DSRC_CTL_WKPOL_Pos				(26)                                              /*!< DSRC_T::CTL: WKPOL Position                   */
#define DSRC_CTL_WKPOL_Msk				(0x3ul<<DSRC_CTL_WKPOL_Pos)                       /*!< DSRC_T::CTL: WKPOL Mask                       */
#define DSRC_CTL_BRATEACC_Pos			(28)                                              /*!< DSRC_T::CTL: BRATEACC Position                */
#define DSRC_CTL_BRATEACC_Msk			(0x7ul<<DSRC_CTL_BRATEACC_Pos)                    /*!< DSRC_T::CTL: BRATEACC Mask                    */

#define DSRC_INTEN_CRCCORIE_Pos				(0)                                               /*!< DSRC_T::INTEN: CRCCORIE Position                  */
#define DSRC_INTEN_CRCCORIE_Msk				(0x1ul<<DSRC_INTEN_CRCCORIE_Pos)                  /*!< DSRC_T::INTEN: CRCCORIE Mask                      */
#define DSRC_INTEN_CRCERRIE_Pos				(1)                                               /*!< DSRC_T::INTEN: CRCERRIE Position                  */
#define DSRC_INTEN_CRCERRIE_Msk				(0x1ul<<DSRC_INTEN_CRCERRIE_Pos)                  /*!< DSRC_T::INTEN: CRCERRIE Mask                      */
#define DSRC_INTEN_STRFRMIE_Pos				(2)                                               /*!< DSRC_T::INTEN: STRFRMIE Position                 */
#define DSRC_INTEN_STRFRMIE_Msk				(0x1ul<<DSRC_INTEN_STRFRMIE_Pos)                  /*!< DSRC_T::INTEN: STRFRMIE Mask                     */
#define DSRC_INTEN_STPFRMIE_Pos				(3)                                               /*!< DSRC_T::INTEN: STPFRMIE Position                */
#define DSRC_INTEN_STPFRMIE_Msk				(0x1ul<<DSRC_INTEN_STPFRMIE_Pos)                  /*!< DSRC_T::INTEN: STPFRMIE Mask                    */
#define DSRC_INTEN_RXDATERRIE_Pos			(4)                                               /*!< DSRC_T::INTEN: RXDATERRIE Position                 */
#define DSRC_INTEN_RXDATERRIE_Msk			(0x1ul<<DSRC_INTEN_RXDATERRIE_Pos)                /*!< DSRC_T::INTEN: RXDATERRIE Mask                     */
#define DSRC_INTEN_BRATERRIE_Pos			(5)                                               /*!< DSRC_T::INTEN: BRATERRIE Position                */
#define DSRC_INTEN_BRATERRIE_Msk			(0x1ul<<DSRC_INTEN_BRATERRIE_Pos)                 /*!< DSRC_T::INTEN: BRATERRIE Mask                    */
#define DSRC_INTEN_TTBPDIE_Pos				(8)                                               /*!< DSRC_T::INTEN: TTBPDIE Position                   */
#define DSRC_INTEN_TTBPDIE_Msk				(0x1ul<<DSRC_INTEN_TTBPDIE_Pos)                   /*!< DSRC_T::INTEN: TTBPDIE Mask                       */
#define DSRC_INTEN_RTBPDIE_Pos				(9)                                               /*!< DSRC_T::INTEN: RTBPDIE Position                  */
#define DSRC_INTEN_RTBPDIE_Msk				(0x1ul<<DSRC_INTEN_RTBPDIE_Pos)                   /*!< DSRC_T::INTEN: RTBPDIE Mask                      */
#define DSRC_INTEN_TXDONEIE_Pos				(12)                                              /*!< DSRC_T::INTEN: TXDONEIE Position                   */
#define DSRC_INTEN_TXDONEIE_Msk				(0x1ul<<DSRC_INTEN_TXDONEIE_Pos)                  /*!< DSRC_T::INTEN: TXDONEIE Mask                       */
#define DSRC_INTEN_T2TOIE_Pos					(18)                                              /*!< DSRC_T::INTEN: T2TOIE Position                 */
#define DSRC_INTEN_T2TOIE_Msk					(0x1ul<<DSRC_INTEN_T2TOIE_Pos)                    /*!< DSRC_T::INTEN: T2TOIE Mask                    */
#define DSRC_INTEN_T3TOIE_Pos					(19)                                              /*!< DSRC_T::INTEN: T3TOIE Position                 */
#define DSRC_INTEN_T3TOIE_Msk					(0x1ul<<DSRC_INTEN_T3TOIE_Pos)                    /*!< DSRC_T::INTEN: T3TOIE Mask                     */
#define DSRC_INTEN_T4TOIE_Pos					(20)                                              /*!< DSRC_T::INTEN: T4TOIE Position                 */
#define DSRC_INTEN_T4TOIE_Msk					(0x1ul<<DSRC_INTEN_T4TOIE_Pos)                    /*!< DSRC_T::INTEN: T4TOIE Mask                     */
#define DSRC_INTEN_EPWKIE_Pos					(26)                                              /*!< DSRC_T::INTEN: EPWKIE Position                   */
#define DSRC_INTEN_EPWKIE_Msk					(0x1ul<<DSRC_INTEN_EPWKIE_Pos)                    /*!< DSRC_T::INTEN: EPWKIE Mask                       */


#define DSRC_STATUS_CRCCOR_Pos				(0)                                               /*!< DSRC_T::STATUS: CRCCOR Position                  */
#define DSRC_STATUS_CRCCOR_Msk				(0x1ul<<DSRC_STATUS_CRCCOR_Pos)                   /*!< DSRC_T::STATUS: CRCCOR Mask                      */
#define DSRC_STATUS_CRCERR_Pos				(1)                                               /*!< DSRC_T::STATUS: CRCERR Position                  */
#define DSRC_STATUS_CRCERR_Msk				(0x1ul<<DSRC_STATUS_CRCERR_Pos)                   /*!< DSRC_T::STATUS: CRCERR Mask                      */
#define DSRC_STATUS_STRFRM_Pos				(2)                                               /*!< DSRC_T::STATUS: STRFRM Position                 */
#define DSRC_STATUS_STRFRM_Msk				(0x1ul<<DSRC_STATUS_STRFRM_Pos)                   /*!< DSRC_T::STATUS: STRFRM Mask                     */
#define DSRC_STATUS_STPFRM_Pos				(3)                                               /*!< DSRC_T::STATUS: STPFRM Position                */
#define DSRC_STATUS_STPFRM_Msk				(0x1ul<<DSRC_STATUS_STPFRM_Pos)                   /*!< DSRC_T::STATUS: STPFRM Mask                    */
#define DSRC_STATUS_RXDATERR_Pos			(4)                                               /*!< DSRC_T::STATUS: RXDATERR Position                 */
#define DSRC_STATUS_RXDATERR_Msk			(0x1ul<<DSRC_STATUS_RXDATERR_Pos)                 /*!< DSRC_T::STATUS: RXDATERR Mask                     */
#define DSRC_STATUS_BRATERR_Pos			  (5)                                               /*!< DSRC_T::STATUS: BRATERR Position                */
#define DSRC_STATUS_BRATERR_Msk			  (0x1ul<<DSRC_STATUS_BRATERR_Pos)                  /*!< DSRC_T::STATUS: BRATERR Mask                    */
#define DSRC_STATUS_TTBPD_Pos				  (8)                                               /*!< DSRC_T::STATUS: TTBPD Position                   */
#define DSRC_STATUS_TTBPD_Msk				  (0x1ul<<DSRC_STATUS_TTBPD_Pos)                    /*!< DSRC_T::STATUS: TTBPD Mask                       */
#define DSRC_STATUS_RTBPD_Pos				  (9)                                               /*!< DSRC_T::STATUS: RTBPD Position                  */
#define DSRC_STATUS_RTBPD_Msk				  (0x1ul<<DSRC_STATUS_RTBPD_Pos)                    /*!< DSRC_T::STATUS: RTBPD Mask                      */
#define DSRC_STATUS_TXDONE_Pos				(12)                                              /*!< DSRC_T::STATUS: TXDONE Position                   */
#define DSRC_STATUS_TXDONE_Msk				(0x1ul<<DSRC_STATUS_TXDONE_Pos)                   /*!< DSRC_T::STATUS: TXDONE Mask                       */
#define DSRC_STATUS_TTBPFULL_Pos			(15)                                              /*!< DSRC_T::STATUS: TTBPFULL Position                   */
#define DSRC_STATUS_TTBPFULL_Msk			(0x1ul<<DSRC_STATUS_TTBPFULL_Pos)                 /*!< DSRC_T::STATUS: TTBPFULL Mask                       */
#define DSRC_STATUS_T2TO_Pos					(18)                                              /*!< DSRC_T::STATUS: T2TO Position                 */
#define DSRC_STATUS_T2TO_Msk					(0x1ul<<DSRC_STATUS_T2TO_Pos)                     /*!< DSRC_T::STATUS: T2TO Mask                    */
#define DSRC_STATUS_T3TO_Pos					(19)                                              /*!< DSRC_T::STATUS: T3TO Position                 */
#define DSRC_STATUS_T3TO_Msk					(0x1ul<<DSRC_STATUS_T3TO_Pos)                     /*!< DSRC_T::STATUS: T3TOIE Mask                     */
#define DSRC_STATUS_T4TO_Pos					(20)                                              /*!< DSRC_T::STATUS: T4TOIE Position                 */
#define DSRC_STATUS_T4TO_Msk					(0x1ul<<DSRC_STATUS_T4TO_Pos)                     /*!< DSRC_T::STATUS: T4TOIE Mask                     */
#define DSRC_STATUS_CRC0_OK_Pos				(24)                                              /*!< DSRC_T::STATUS: CRC0_OK Position                   */
#define DSRC_STATUS_CRC0_OK_Msk				(0x1ul<<DSRC_STATUS_CRC0_OK_Pos)                  /*!< DSRC_T::STATUS: CRC0_OK Mask                       */
#define DSRC_STATUS_CRC1_OK_Pos				(25)                                              /*!< DSRC_T::STATUS: CRC1_OK Position                   */
#define DSRC_STATUS_CRC1_OK_Msk				(0x1ul<<DSRC_STATUS_CRC1_OK_Pos)                  /*!< DSRC_T::STATUS: CRC1_OK Mask                       */
#define DSRC_STATUS_EPWKF_Pos					(26)                                              /*!< DSRC_T::STATUS: EPWKF Position                   */
#define DSRC_STATUS_EPWKF_Msk					(0x1ul<<DSRC_STATUS_EPWKF_Pos)                    /*!< DSRC_T::STATUS: EPWKF Mask                       */

#define DSRC_PREAMBLE_PREPAT_Pos			(0)                                               /*!< DSRC_T::PREAMBLE: PREPAT Position                   */
#define DSRC_PREAMBLE_PREPAT_Msk			(0xFFFFFFFFul<<DSRC_PREAMBLE_PREPAT_Pos)          /*!< DSRC_T::PREAMBLE: PREPAT Mask                       */
                                                                                        
#define DSRC_CRCSEED_CRCSEED0_Pos			(0)                                               /*!< DSRC_T::CRCSEED: CRCSEED0 Position                   */
#define DSRC_CRCSEED_CRCSEED0_Msk			(0xFFFFul<<DSRC_CRCSEED_CRCSEED0_Pos)             /*!< DSRC_T::CRCSEED: CRCSEED0 Mask                       */
#define DSRC_CRCSEED_CRCSEED1_Pos			(16)                                              /*!< DSRC_T::CRCSEED: CRCSEED1 Position                   */
#define DSRC_CRCSEED_CRCSEED1_Msk			(0xFFFFul<<DSRC_CRCSEED_CRCSEED1_Pos)             /*!< DSRC_T::CRCSEED: CRCSEED1 Mask                       */
                                                                                        
#define DSRC_RBCNT_RBCNT_Pos					(0)                                               /*!< DSRC_T::RBCNT: RBCNT Position                   */
#define DSRC_RBCNT_RBCNT_Msk					(0x1FFul<<DSRCRBCNT_RBCNT_Pos)         	          /*!< DSRC_T::RBCNT: RBCNT Mask                       */
                                                                                        
#define DSRC_CHKSUM_CHKSUM0_Pos			  (0)                                               /*!< DSRC_T::CHKSUM: CHKSUM0 Position                   */
#define DSRC_CHKSUM_CHKSUM0_Msk			  (0xFFFFul<<DSRC_CHKSUM_CHKSUM0_Pos)               /*!< DSRC_T::CHKSUM: CHKSUM0 Mask                       */
#define DSRC_CHKSUM_CHKSUM1_Pos			  (16)                                              /*!< DSRC_T::CHKSUM: CHKSUM1 Position                   */
#define DSRC_CHKSUM_CHKSUM1_Msk			  (0xFFFFul<<DSRC_CHKSUM_CHKSUM1_Pos)               /*!< DSRC_T::CHKSUM: CHKSUM1 Mask                       */
                                                                                        
#define DSRC_TX_TX_Pos			  				(0)                                               /*!< DSRC_T::TX: TX Position                   */
#define DSRC_TX_TX_Msk			  				(0xFFul<<DSRC_TX_TX_Pos)                          /*!< DSRC_T::TX: TX Mask                       */
                                                                                        
#define DSRC_RX_RX_Pos			  				(0)                                               /*!< DSRC_T::RX: RX Position                   */
#define DSRC_RX_RX_Msk			  				(0xFFul<<DSRC_RX_RX_Pos)                          /*!< DSRC_T::RX: RX Mask                       */
                                                                                        
#define DSRC_TMR2_CMPDAT_Pos			  	(0)                                               /*!< DSRC_T::TMR2: CMPDAT Position                */
#define DSRC_TMR2_CMPDAT_Msk			  	(0xFFFFFFul<<DSRC_TMR2_CMPDAT_Pos)                /*!< DSRC_T::TMR2: CMPDAT Mask                    */
#define DSRC_TMR2_PSC_Pos			  			(24)                                              /*!< DSRC_T::TMR2: PSC Position                   */
#define DSRC_TMR2_PSC_Msk			  			(0xFFul<<DSRC_TMR2_PSC_Pos)                       /*!< DSRC_T::TMR2: PSC Mask                       */
                                                                                        
#define DSRC_TMR3_CMPDAT_Pos			  	(0)                                               /*!< DSRC_T::TMR3: CMPDAT Position                */
#define DSRC_TMR3_CMPDAT_Msk			  	(0xFFFFFFul<<DSRC_TMR3_CMPDAT_Pos)                /*!< DSRC_T::TMR3: CMPDAT Mask                    */
#define DSRC_TMR3_PSC_Pos			  			(24)                                              /*!< DSRC_T::TMR3: PSC Position                   */
#define DSRC_TMR3_PSC_Msk			  			(0xFFul<<DSRC_TMR3_PSC_Pos)                       /*!< DSRC_T::TMR3: PSC Mask                       */
                                                                                        
#define DSRC_TMR4_CMPDAT_Pos			  	(0)                                               /*!< DSRC_T::TMR4: CMPDAT Position                */
#define DSRC_TMR4_CMPDAT_Msk			  	(0xFFFFFFul<<DSRC_TMR4_CMPDAT_Pos)                /*!< DSRC_T::TMR4: CMPDAT Mask                    */
#define DSRC_TMR4_PSC_Pos			  			(24)                                              /*!< DSRC_T::TMR4: PSC Position                   */
#define DSRC_TMR4_PSC_Msk			  			(0xFFul<<DSRC_TMR4_PSC_Pos)                       /*!< DSRC_T::TMR4: PSC Mask                       */
                                                                                        
#define DSRC_CTL2_SOFNUM_Pos			  	(0)                                               /*!< DSRC_T::CTL2: SOFNUM Position                */
#define DSRC_CTL2_SOFNUM_Msk			  	(0x3ul<<DSRC_CTL2_SOFNUM_Pos)                     /*!< DSRC_T::CTL2: SOFNUM Mask                    */
#define DSRC_CTL2_CODECDEGSEL_Pos			(8)                                               /*!< DSRC_T::CTL2: CODECDEGSEL Position                */
#define DSRC_CTL2_CODECDEGSEL_Msk			(0x3ul<<DSRC_CTL2_CODECDEGSEL_Pos)                /*!< DSRC_T::CTL2: CODECDEGSEL Mask                    */
                                                                                        
#define DSRC_TXR_TXATR_Pos			  		(0)                                               /*!< DSRC_T::TXR: TXATR Position                   */
#define DSRC_TXR_TXATR_Msk			 			(0xFFul<<DSRC_TXR_TXATR_Pos)                      /*!< DSRC_T::TXR: TXATR Mask                       */
                                                                                                                                         
#define DSRC_RXR_RXATR_Pos			  		(0)                                               /*!< DSRC_T::RXR: RXATR Position                   */
#define DSRC_RXR_RXATR_Msk			  		(0xFFul<<DSRC_RX_RXATR_Pos)                       /*!< DSRC_T::RXR: RXATR Mask                       */
                                                                                        
#define DSRC_ICR_DSRCCS_Pos			  		(0)                                               /*!< DSRC_T::RXR: DSRCCS Position                   */
#define DSRC_ICR_DSRCCS_Msk			  		(0xFul<<DSRC_RX_DSRCCS_Pos)                       /*!< DSRC_T::RXR: DSRCCS Mask                       */

/**@}*/ /* DSRC_CONST */

/**@}*/ /* end of DSRC register group */
/// @endcond //HIDDEN_SYMBOLS
#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#define DSRC_BASE              (0x401F0000)          ///< DSRC register base address
#define DSRCS_BASE             (DSRC_BASE+0x200)     ///< DSRC register base address
#define DSRC                   ((DSRC_T *) DSRC_BASE)              ///< Pointer to DSRC register structure
#define DSRCS                  ((DSRCS_T *) DSRCS_BASE)        ///< Pointer to DSRC debug register structure


/** @addtogroup NANO103_Device_Driver NANO103 Device Driver
  @{
*/

/** @addtogroup NANO103_DSRC_Driver DSRC Driver
  @{
*/

/** @addtogroup NANO103_DSRC_EXPORTED_CONSTANTS DSRC Exported Constants
  @{
*/
#define DSRC_MODE_FM0         (DSRC_CTL_CODECEN_Msk)
#define DSRC_MODE_MONCHESTER  (DSRC_CTL_CODECEN_Msk|DSRC_CTL_CODECFMT_Msk)
#define DSRC_MODE_SPI         (0)
#define DSRC_MODE_Msk         (DSRC_CTL_CODECEN_Msk|DSRC_CTL_CODECFMT_Msk)

  
#define DSRC_CTL_WKPOL_RISING   (0x0<<DSRC_CTL_WKPOL_Pos)
#define DSRC_CTL_WKPOL_FALLING  (0x1<<DSRC_CTL_WKPOL_Pos)
#define DSRC_CTL_WKPOL_BOTH     (0x2<<DSRC_CTL_WKPOL_Pos)


#define DSRC_CTL_PREAMFMT_16     (0x0<<DSRC_CTL_PREAMFMT_Pos)
#define DSRC_CTL_PREAMFMT_32     (0x1<<DSRC_CTL_PREAMFMT_Pos)
  
/*@}*/ /* end of group NANO103_DSRC_EXPORTED_CONSTANTS */


/** @addtogroup NANO103_DSRC_EXPORTED_FUNCTIONS DSRC Exported Functions
  @{
*/

/**
  * @brief Enable DSRC related interrupts specified by u32Mask parameter.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Mask is the combination of all related interrupt enable bits.
  *         Each bit corresponds to a interrupt bit.
  *         This parameter decides which interrupts will be enabled. Valid values are:
  *           - \ref DSRC_INTEN_CRCCORIE_Msk
  *           - \ref DSRC_INTEN_CRCERRIE_Msk
  *           - \ref DSRC_INTEN_STRFRMIE_Msk
  *           - \ref DSRC_INTEN_STPFRMIE_Msk
  *           - \ref DSRC_INTEN_RXDATERRIE_Msk
  *           - \ref DSRC_INTEN_BRATERRIE_Msk
  *           - \ref DSRC_INTEN_TTBPDIE_Msk
  *           - \ref DSRC_INTEN_RTBPDIE_Msk
  *           - \ref DSRC_INTEN_TXDONEIE_Msk
  *           - \ref DSRC_INTEN_T2TOIE_Msk
  *           - \ref DSRC_INTEN_T3TOIE_Msk
  *           - \ref DSRC_INTEN_T4TOIE_Msk
  * @return none
  */
#define DSRC_EnableInt(dsrc,u32Mask) (DSRC->INTEN |= u32Mask)

/**
  * @brief Disable DSRC related interrupts specified by u32Mask parameter.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Mask is the combination of all related interrupt enable bits.
  *         Each bit corresponds to a interrupt bit.
  *         This parameter decides which interrupts will be enabled. Valid values are:
  *           - \ref DSRC_INTEN_CRCCORIE_Msk
  *           - \ref DSRC_INTEN_CRCERRIE_Msk
  *           - \ref DSRC_INTEN_STRFRMIE_Msk
  *           - \ref DSRC_INTEN_STPFRMIE_Msk
  *           - \ref DSRC_INTEN_RXDATERRIE_Msk
  *           - \ref DSRC_INTEN_BRATERRIE_Msk
  *           - \ref DSRC_INTEN_TTBPDIE_Msk
  *           - \ref DSRC_INTEN_RTBPDIE_Msk
  *           - \ref DSRC_INTEN_TXDONEIE_Msk
  *           - \ref DSRC_INTEN_T2TOIE_Msk
  *           - \ref DSRC_INTEN_T3TOIE_Msk
  *           - \ref DSRC_INTEN_T4TOIE_Msk
  * @return none
  */
#define DSRC_DisableInt(dsrc,u32Mask) (DSRC->INTEN &= ~u32Mask)

/**
  * @brief Get DSRC related interrupts specified by u32Mask parameter.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Mask is the combination of all related interrupt enable bits.
  *         Each bit corresponds to a interrupt bit.
  *         This parameter decides which interrupts will be enabled. Valid values are:
  *           - \ref DSRC_INTEN_CRCCORIE_Msk
  *           - \ref DSRC_INTEN_CRCERRIE_Msk
  *           - \ref DSRC_INTEN_STRFRMIE_Msk
  *           - \ref DSRC_INTEN_STPFRMIE_Msk
  *           - \ref DSRC_INTEN_RXDATERRIE_Msk
  *           - \ref DSRC_INTEN_BRATERRIE_Msk
  *           - \ref DSRC_INTEN_TTBPDIE_Msk
  *           - \ref DSRC_INTEN_RTBPDIE_Msk
  *           - \ref DSRC_INTEN_TXDONEIE_Msk
  *           - \ref DSRC_INTEN_T2TOIE_Msk
  *           - \ref DSRC_INTEN_T3TOIE_Msk
  *           - \ref DSRC_INTEN_T4TOIE_Msk
                     
  * @return Interrupt Enable bit have been enable.
  * @retval 0 Disable
  * @retval 1 Enable
  */
#define DSRC_GET_INT(dsrc,u32Mask) ((DSRC->INTEN & (u32Mask))== (u32Mask) ? 1:0)


/**
  * @brief Get DSRC related interrupts specified by u32Mask parameter.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Mask is the combination of all related interrupt status bits.
  *         Each bit corresponds to a interrupt bit.
  *         This parameter decides which interrupts will be interrupt. Valid values are:
  *           - \ref DSRC_STATUS_CRCCOR_Msk
  *           - \ref DSRC_STATUS_CRCERR_Msk
  *           - \ref DSRC_STATUS_STRFRM_Msk
  *           - \ref DSRC_STATUS_STPFRM_Msk
  *           - \ref DSRC_STATUS_RXDATERR_Msk
  *           - \ref DSRC_STATUS_BRATERR_Msk
  *           - \ref DSRC_STATUS_TTBPD_Msk
  *           - \ref DSRC_STATUS_RTBPD_Msk
  *           - \ref DSRC_STATUS_TXDONE_Msk
  *           - \ref DSRC_STATUS_TTBPFULL_Msk
  *           - \ref DSRC_STATUS_T2TO_Msk
  *           - \ref DSRC_STATUS_T3TO_Msk
  *           - \ref DSRC_STATUS_T4TO_Msk
  *           - \ref DSRC_STATUS_CRC0_OK_Msk
  *           - \ref DSRC_STATUS_CRC1_OK_Msk
  *           - \ref DSRC_STATUS_EPWKF_Msk
  * @return Interrupt have been happen.
  * @retval 0 No interrupt
  * @retval 1 Interrupted
  */
#define DSRC_GET_FLAG(dsrc,u32Mask) ((DSRC->STATUS & (u32Mask))== (u32Mask) ? 1:0)


/**
  * @brief Clear DSRC related interrupts specified by u32Mask parameter.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Mask is the combination of all related interrupt enable bits.
  *         Each bit corresponds to a interrupt bit.
  *         This parameter decides which interrupts will be enabled. Valid values are:
  *           - \ref DSRC_STATUS_CRCCOR_Msk
  *           - \ref DSRC_STATUS_CRCERR_Msk
  *           - \ref DSRC_STATUS_STRFRM_Msk
  *           - \ref DSRC_STATUS_STPFRM_Msk
  *           - \ref DSRC_STATUS_RXDATERR_Msk
  *           - \ref DSRC_STATUS_BRATERR_Msk
  *           - \ref DSRC_STATUS_TTBPD_Msk
  *           - \ref DSRC_STATUS_RTBPD_Msk
  *           - \ref DSRC_STATUS_TXDONE_Msk
  *           - \ref DSRC_STATUS_TTBPFULL_Msk
  *           - \ref DSRC_STATUS_T2TO_Msk
  *           - \ref DSRC_STATUS_T3TO_Msk
  *           - \ref DSRC_STATUS_T4TO_Msk
  *           - \ref DSRC_STATUS_CRC0_OK_Msk
  *           - \ref DSRC_STATUS_CRC1_OK_Msk
  *           - \ref DSRC_STATUS_EPWKF_Msk
  * @return none
  */
#define DSRC_CLR_FLAG(dsrc,u32Mask) (DSRC->STATUS = (u32Mask))

/**
  * @brief  Trigger RX PDMA transfer.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_TRIGGER_RX_PDMA(dsrc) ( (DSRC)->CTL |= (1<<DSRC_CTL_TRDMAEN_Pos) )

/**
  * @brief  Trigger TX PDMA transfer.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_TRIGGER_TX_PDMA(dsrc) ( (DSRC)->CTL |= (1<<DSRC_CTL_TTDMAEN_Pos) )

/**
  * @brief  Force RX state.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_FORCE_RX(dsrc) ((DSRC)->CTL |= (1<<DSRC_CTL_FORCERX_Pos))

/**
  * @brief  Force TX state.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_FORCE_TX(dsrc) ((DSRC)->CTL |= (1<<DSRC_CTL_FORCETX_Pos))

/**
  * @brief  Set DSRC RX on.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_RXON(dsrc) ((DSRC)->CTL |= (1<<DSRC_CTL_RXON_Pos))

/**
  * @brief  Set DSRC RX off.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_RXOFF(dsrc) ((DSRC)->CTL &= ~(1<<DSRC_CTL_RXON_Pos))

/**
  * @brief  Set DSRC wakeup Trigger type.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  type parameter decides which trigger type for DSRC wakeup. Valid values are:
  *           - \ref DSRC_CTL_WKPOL_RISING
  *           - \ref DSRC_CTL_WKPOL_FALLING
  *           - \ref DSRC_CTL_WKPOL_BOTH
  * @return none
  * \hideinitializer
  */
#define DSRC_WKPOL_TRIGGER(dsrc,type) ((DSRC)->CTL = (DSRC)->CTL & ~(0x3<<26) | (type))


/**
  * @brief Set time out evnet.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Idx is time out number, it is should be 2,3,4.
  * @param[in]  u32Prescale is Prescale Counter, it is should be 0~255.
  * @param[in]  u32Compared is Timer Compared Value, it is should be 2~0xFFFFFF.
  * @return none
  */
#define DSRC_SET_TIMEROUT(dsrc,u32Idx,u32Prescale, u32Compared) (DSRC->TO[(u32Idx)]= ((u32Prescale)<<24)|(u32Compared))


/**
  * @brief Set CRC format.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32CRCMSB is CRC Generation on Data MSB. 0: Disable. 1: Enable.
  * @param[in]  u32CRCREV is CRC checksum reverse. 0: Disable. 1: Enable.
  * @param[in]  u32CRCXOR is 1'complement for CRC checksum. 0: Disable. 1: Enable.
  * @param[in]  u32CRCBSWAP is the received CRC value swap. 0: Disable. 1: Enable.
  * @return none
  */
#define DSRC_SET_CRC_FMT(dsrc,u32CRCMSB,u32CRCREV,u32CRCXOR,u32CRCBSWAP) \
              ( DSRC->CTL =( DSRC->CTL & ~((1<<DSRC_CTL_CRCMSB_Pos)|(1<<DSRC_CTL_CHKSREV_Pos)|(1<<DSRC_CTL_CHKFMT_Pos)|(1<<DSRC_CTL_CRCBSWAP_Pos)) ) \
              | (((u32CRCMSB)<<9)|((u32CRCREV)<<10)|((u32CRCXOR)<<11)|((u32CRCBSWAP)<<20)) )

/**
  * @brief Set Bit Rate Accuracy Control Bits for FM0.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Val is Bit rate accuracy value as bellow.
  * @return none
  * @details This parameter decides which bit rate accuracy will be enabled in following table:
  *-
  * |    |        | CLK_CLKSEL2[7]=0|     |        |CLK_CLKSEL2[7]=1 |
  * | :--|:------ |:--------------- | :-- |:------ |:--------------- |
  * | 0  | >75%   | 204 ~ 341 kH    |  0  | >75%   | 204 ~ 341 kHz   |
  * | 1  | >75%   | 204 ~ 341 kH    |  1  | >80%   | 213 ~ 320 kHz   |
  * | 2  | >87.5% | 227 ~ 292 kHz   |  2  | >84%   | 220 ~ 304 kHz   |
  * | 3  | >87.5% | 227 ~ 292 kHz   |  3  | >87.5% | 227 ~ 292 kHz   |
  * | 4  | X      |   Reserved      |  4  | >71%   | 198 ~ 355 kHz   |
  */
#define DSRC_SET_BATE_CTL(dsrc,u32Val) (DSRC->CTL = (DSRC->CTL & ~DSRC_CTL_BRATEACC_Msk) | ((u32Val)<<DSRC_CTL_BRATEACC_Pos))


/**
  * @brief Set FM0 Codec Deglitch.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Val is FM0 Codec Deglitch.
  *             This bits field is used to define how much width of glitch would be filtered.
  *             0 = disable to the codec deglitch selection.
  *             1 = Filter the glitches that the width is 0.25us or less.
  *             2 = Filter the glitches that the width is 0.50us or less.
  *             3 = Filter the glitches that the width is 0.75us or less.
  *             4 = Filter the glitches that the width is 1.00us or less.
  *             5 = Filter the glitches that the width is 1.25us or less.
  * @return none
  */
#define DSRC_SET_DEGLITCH(dsrc,u32Val) (DSRCS->CTL2 = (DSRCS->CTL2 & ~DSRC_CTL2_CODECDEGSEL_Msk) | ((u32Val)<<DSRC_CTL2_CODECDEGSEL_Pos))


/**
  * @brief  Enable CRC module.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_ENABLE_CRC(dsrc)  ((DSRC)->CTL |= DSRC_CTL_CRCEN_Msk)

/**
  * @brief  Disable CRC module.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_DISABLE_CRC(dsrc)  ((DSRC)->CTL &= ~DSRC_CTL_CRCEN_Msk)

/**
  * @brief  Enable TPB module.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_ENABLE_TBP(dsrc)  ((DSRC)->CTL |= DSRC_CTL_TBPEN_Msk)

/**
  * @brief  Disable TPB module.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_DISABLE_TBP(dsrc)  ((DSRC)->CTL &= ~DSRC_CTL_TBPEN_Msk)


/**
  * @brief  Enable software control RX_ON Enable Bit.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_ENABLE_SWRX(dsrc) ((DSRC)->CTL |= DSRC_CTL_SWRXEN_Msk)

/**
  * @brief  Disable software control RX_ON Enable Bit.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  * \hideinitializer
  */
#define DSRC_DISABLE_SWRX(dsrc) ((DSRC)->CTL &= ~DSRC_CTL_SWRXEN_Msk)


/**
  * @brief  Get DSRC System Current State Machine Status.
  * @param[in]  dsrcs is the base address of DSRC module.
  * @return State Machine Status.
  * @retval 0 = DSRC_RST state.
  * @retval 1 = DSRC_START_RX state.
  * @retval 2 = DSRC_RX7E_BRDET state.
  * @retval 3 = DSRC_RX7E_HEAD state.
  * @retval 4 = DSRC_RX7E_TAIL state.
  * @retval 5 = DSRC_TXWAIT state.
  * @retval 6 = DSRC_TX state.

  * \hideinitializer
  */
#define DSRC_GET_CurStateMachine(dsrcs) (DSRCS->ICR & 0xF)

/**
  * @brief  Set CRC Seed Mode.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Val is transmit CRC seed initial value. 0: Seed0 1: Seed1
  * @return none
  * \hideinitializer
  */
#define DSRC_SET_SEED(dsrc,u32Val) ((DSRC)->CTL = ((DSRC)->CTL & ~DSRC_CTL_SEEDM_Msk)| ((u32Val)<<DSRC_CTL_SEEDM_Pos) )

void DSRC_Open(DSRC_T *dsrc,uint32_t u32Mode,uint32_t u32crcEn,uint32_t u32pdmaEn);
void DSRC_SetPreamble(DSRC_T *dsrc,uint32_t u32Mode,uint32_t u32Val);
void DSRC_Close(DSRC_T *dsrc);
/*@}*/ /* end of group NANO103_DSRC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NANO103_DSRC_Driver */

/*@}*/ /* end of group NANO103_Device_Driver */

#ifdef __cplusplus
}
#endif

#endif //__DSRC_H__

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
