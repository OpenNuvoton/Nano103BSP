////////////////////////////////////////////////////////////////////////////////
// SKY6605 Global Definitions                                                 //
////////////////////////////////////////////////////////////////////////////////
#ifndef __SKY6605_H
#define __SKY6605_H

#include "Nano103.h"

#define SKY6605_FIFO_MAX          128

// SKY6605 Command Lists
#define SKY6605_CMD_IDLE          (0x00)
#define SKY6605_CMD_CL_RXFF       (0x02)
#define SKY6605_CMD_CL_TXFF       (0x03)
#define SKY6605_CMD_TX            (0x04)
#define SKY6605_CMD_TX_TEST       (0x06)
#define SKY6605_CMD_RX            (0x08)
#define SKY6605_CMD_RX_TEST       (0x0A)
#define SKY6605_CMD_TX_RX         (0x0C)
#define SKY6605_CMD_RX_TX         (0x0D)
#define SKY6605_CMD_SW_RST        (0x0F)

// SKY6605 Register List
#define SKY6605_SYS_CTRL          (0x00)    
#define SKY6605_CRC_INIT_H        (0x01)
#define SKY6605_CRC_INIT_L        (0x02)
#define SKY6605_TIMER_CTRL        (0x03)    
#define SKY6605_PREAMB_LEN        (0x04)    
#define SKY6605_RX_PREAMB_PAT     (0x05)
#define SKY6605_TXFIFO_WL         (0x06)    
#define SKY6605_RXFIFO_WL         (0x07)    
#define SKY6605_TX_LEN            (0x08)    
#define SKY6605_IRQ_EN            (0x09)    
#define SKY6605_ERR_IRQ_EN        (0x0A)
#define SKY6605_TEST0             (0x0B)    
#define SKY6605_TEST1             (0x0C) 
#define SKY6605_TEST2             (0x0D)

#define SKY6605_IRQ_STA           (0x0E)    
#define SKY6605_ERR_STA           (0x0F)    
#define SKY6605_SYS_STA           (0x10)
#define SKY6605_FIFO_STA          (0x11)
#define SKY6605_TXFIFO_LEN        (0x12)    
#define SKY6605_RXFIFO_LEN        (0x13)
#define SKY6605_RX_BYTE_NUM       (0x14)    
#define SKY6605_TXFIFO            (0x15)
#define SKY6605_RXFIFO            (0x16)
#define SKY6605_ANA_RSSI          (0x17)
 
#define SKY6605_RX_VCO_FDFH       (0x20)
#define SKY6605_RX_VCO_FDFL       (0x21)
#define SKY6605_RX_VCO_FDI        (0x22)
#define SKY6605_TX_VCO_FDFH       (0x23)
#define SKY6605_TX_VCO_FDFL       (0x24)
#define SKY6605_TX_VCO_FDI        (0x25)
#define SKY6605_PLL1              (0x26)
#define SKY6605_PLL2              (0x27)                        
#define SKY6605_WAKEUP1           (0x28)
#define SKY6605_WAKEUP2           (0x29)                        
#define SKY6605_WAKEUP3           (0x2A)                        
#define SKY6605_WAKEUP4           (0x2B)                        
#define SKY6605_RX_AFE1           (0x2C)                        
#define SKY6605_RX_AFE2           (0x2D)                        
#define SKY6605_TX_AFE1           (0x2E)                        
#define SKY6605_TX_AFE2           (0x2F) 
//#define SKY6605_WAKEUP_TIMER      (0x30) // to be deleted!!!!!!

#define SKY6605_PWKU_CTRL         (0x33)
#define SKY6605_PWKU_T1T2         (0x34)
#define SKY6605_PWKU_T3T4         (0x35)
#define SKY6605_PWKU_CNT          (0x36)
#define SKY6605_PWKU_RSSI_CFG     (0x37)
#define SKY6605_PWKU_RSSI_MAX     (0x38)
#define SKY6605_PWKU_STA          (0x39)
#define SKY6605_PWKU_TEST         (0x3A)

#define SKY6605_CHIP_ID3          (0x3C)    
#define SKY6605_CHIP_ID2          (0x3D)    
#define SKY6605_CHIP_ID1          (0x3E)    
#define SKY6605_CHIP_ID0          (0x3F)  

// SKY6605 IRQ Definition
#define SKY6605_IRQ_OSC_STABLE    (1<<6)
#define SKY6605_IRQ_TXE           (1<<5)
#define SKY6605_IRQ_TXFWQ         (1<<4)
#define SKY6605_IRQ_RXFRQ         (1<<3)
#define SKY6605_IRQ_RXFSF         (1<<2)
#define SKY6605_IRQ_RXFEF         (1<<1)
#define SKY6605_IRQ_TRERR         (1<<0)

#define SKY6605_IRQ_RXFE_ERR      (1<<7)
#define SKY6605_IRQ_RXFF_ERR      (1<<6)
#define SKY6605_IRQ_TXFE_ERR      (1<<5)
#define SKY6605_IRQ_TXFF_ERR      (1<<4)
#define SKY6605_IRQ_RX_7ENBB_ERR  (1<<3)
#define SKY6605_IRQ_RX_FM0_ERR    (1<<2)
#define SKY6605_IRQ_RX_7S1_ERR    (1<<1)
#define SKY6605_IRQ_RX_CRC_ERR    (1<<0)

#define SKY6605_IRQ_COME          (P2IN&BIT5)
#define SKY6605_WAKEUP_IN         (P5IN&BIT4)

#define SKY6605_BLOCK_SIZE         25

////////////////////////////////////////////////////////////////////////////////
// SKY6605 Functions                                                          //
////////////////////////////////////////////////////////////////////////////////
// SKY6605 IO Control
void sky6605_set_chipen_high(void);
void sky6605_set_chipen_low(void);

// SKY6605 Register Control
void sky6605_set_tx_rate(int tx_rate);
void sky6605_set_rx_rate(int rx_rate);

void sky6605_disable_internal_crc(void);
void sky6605_enable_auto_sel_rx (void);

void sky6605_cfg_timer_ctrl(uint8_t tx_timer, uint8_t rx_timer); // unit: us; 15 per step
void sky6605_cfg_tx_len(uint8_t tx_len);
void sky6605_cfg_preamb_len(uint8_t tx_preamb_len, uint8_t rx_preamb_len);
void sky6605_cfg_rx_preamb_pat(uint8_t rx_preamb_pat);
void sky6605_cfg_txfifo_wl(uint8_t txfifo_wl);
void sky6605_cfg_rxfifo_wl(uint8_t rxfifo_wl);

void sky6605_period_wakeup_cfg(void);//----to be deleted!!!!
void sky6605_period_wakeup_enable(void);
void sky6605_period_wakeup_disable(void);
void sky6605_period_wakeup_config(uint8_t pwku_wku_en    , uint8_t pwku_mode  ,
                                  uint8_t pwku_t1        , uint8_t pwku_t2    ,
                                  uint8_t pwku_t3        , uint8_t pwku_t4    ,
                                  uint8_t pwku_cnt1      , uint8_t pwku_cnt2  ,
                                  uint8_t pwku_rssi_delta, uint8_t pwku_rssi_th);

void sky6605_analog_init(void);
void sky6605_init(void);

// SKY6605 pwku reg write lock and unlock
void sky6605_pwku_reg_wr_unlock(void);
void sky6605_pwku_reg_wr_lock(void);
uint8_t sky6605_pwku_reg_lock_sta(void);

// SKY6605 IRQ
void sky6605_disable_all_irq(void);
void sky6605_enable_rx_irq(void);
void sky6605_enable_rx_fef_irq(void);
void sky6605_enable_rx_err_irq(void);
void sky6605_enable_tx_irq(void);
void sky6605_clr_all_irq(void);
void sky6605_clr_rx_irq(void);
void sky6605_clr_rx_err_irq(void);

// SKY6605 TX RX
//uint16_t sky6605_rx_func(uint8_t rxfifo_wl);
void sky6605_tx_func(uint8_t tx_len, uint8_t txfifo_wl);

// SKY6605 Debug/Production Test/Bring Up
void sky6605_rxtx_test(uint8_t rxfifo_wl, uint8_t txfifo_wl);
void sky6605_period_wakeup_response(void);
void sky6605_set_FIFO_LB_mode (void);
void sky6605_disable_FIFO_LB_mode (void);
void sky6605_set_MFOUT_txdata (void);
void sky6605_set_MFOUT_rxdata (void);
void sky6605_rx_its(uint8_t FM0_decoder_en, uint8_t RX_decoder_en);
void sky6605_tx_constant(uint8_t constant1, uint8_t FM0_encode_en);

#endif
