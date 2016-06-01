//#include "io430x5418.h"
#include "string.h"
//#include "global.h"
#include "sky6605.h"
#include "stdlib.h"

uint8_t  tx_buf[256];
uint8_t  rx_buf[256];
//static int err_cnt=0, err_7enbb=0, err_fm0=0, err_7s1=0, err_crc=0, pkt_cnt=0;

extern void WriteET6603(unsigned short value);
extern void delayus(uint32_t us);

void delay_hw(uint16_t value)
{
  uint32_t i;
  for(i=0;i<10;i++);
}

void delay_100us(uint16_t value)
{
  uint16_t i;
    for(i=0; i<value; i++) delay_hw(0x19);
}

void delay_1ms(uint16_t value)
{
  uint16_t i;
  for(i=0; i<value; i++) delay_hw(0xFA);
}

void NEW_ROW()
{
//    UART_TX('\r'); UART_TX('\n');
}
void PUTS(uint8_t *str)
{
}

void SPI_write1(uint8_t add, uint8_t data)
{
  uint32_t offset=8;
  WriteET6603(add<<offset | data);delayus(1);
}

uint8_t SPI_read1(uint8_t add)
{
  return add;
}

void SPI_cmd(uint8_t cmd)
{

}

void sky6605_set_chipen_high(void)
{
//    P5OUT       |=  BIT6;
}

void sky6605_set_chipen_low(void)
{
//    P5OUT       &=  ~BIT6;
}

////////////////////////////////////////////////////////////////////////////////
// Register Control Functions                                                 //
////////////////////////////////////////////////////////////////////////////////
// tx_rate = 256,  00 -> 256kbps
//         = 512,  01 -> 512kbps (default)
//         = 1024, 10 -> 1024kbps
void sky6605_set_tx_rate(int tx_rate)
{
    uint8_t rd_data;
    rd_data = SPI_read1(SKY6605_SYS_CTRL);
    switch(tx_rate) {
      case(256) : { rd_data &= 0x3F; break;}
      case(512) : { rd_data &= 0x7F; rd_data |= 0x40; break;}
      case(1024): { rd_data |= 0x80; rd_data &= 0xBF; break;}
      default   : { rd_data &= 0x7F; rd_data |= 0x40; break;}
    }
    SPI_write1(SKY6605_SYS_CTRL,rd_data);
}

// rx_rate = 256,  00 -> 256kbps (default)
//         = 512,  01 -> 512kbps 
//         = 1024, 10 -> 1024kbps
void sky6605_set_rx_rate(int rx_rate)
{
    uint8_t rd_data;
    rd_data = SPI_read1(SKY6605_SYS_CTRL);
    switch(rx_rate) {
      case(256) : { rd_data &= 0xCF; break;}
      case(512) : { rd_data &= 0xDF; rd_data |= 0x10; break;}
      case(1024): { rd_data |= 0x20; rd_data &= 0xEF; break;}
      default   : { rd_data &= 0xCF; break;}
    }
    SPI_write1(SKY6605_SYS_CTRL,rd_data);
}

void sky6605_disable_internal_crc(void)
{
    uint8_t RD_DATA = SPI_read1(SKY6605_SYS_CTRL);
    RD_DATA &= ~BIT0;
    SPI_write1(SKY6605_SYS_CTRL,RD_DATA);
}

void sky6605_enable_auto_sel_rx (void)
{
    SPI_write1(SKY6605_SYS_CTRL, 0x4B);
}

void sky6605_cfg_timer_ctrl(uint8_t tx_timer, uint8_t rx_timer) // unit: us; 15 per step
{
    uint8_t val,tmp;
    tmp = tx_timer/15;
    val = rx_timer/15;
    val = (tmp << 4) + val;
    SPI_write1(SKY6605_TIMER_CTRL, val);
}

void sky6605_cfg_tx_len(uint8_t tx_len)  // The length of data to be transmit in bytes
{
    SPI_write1(SKY6605_TX_LEN, tx_len);
}

// tx_preamb_len: The length of preamble in bytes to be sent in TX.
//      =2 Bytes
//      =3 Bytes
//      =4 Bytes
//      =Others, RFU
// rx_preamb_len: The length of preamble in bits detected in RX.
//      =0 bits, Do not detect preamble                     -> 0
//      =Detect preamble as 2 bits in RX_PREAMB_PAT[1:0]    -> 1
//      =Detect preamble as 4 bits in RX_PREAMB_PAT[3:0]    -> 2
//      =Detect preamble as 6 bits in RX_PREAMB_PAT[5:0]    -> 3
//      =Detect preamble as 8 bits in RX_PREAMB_PAT[7:0]    -> 4
//      =Others, RFU
void sky6605_cfg_preamb_len(uint8_t tx_preamb_len, uint8_t rx_preamb_len)
{
    uint8_t val, tmp;
    switch(tx_preamb_len) {
        case  2: tmp = 0; break;
        case  3: tmp = 1; break;
        case  4: tmp = 2; break;
        default: tmp = 0; break;
    }
    val = tmp;
    switch(rx_preamb_len) {
        case  0: tmp = 0; break;
        case  2: tmp = 1; break;
        case  4: tmp = 2; break;
        case  6: tmp = 3; break;
        case  8: tmp = 4; break;
        default: tmp = 0; break;
    }
    val = (val << 4) + tmp;
    SPI_write1(SKY6605_PREAMB_LEN, val);
}

void sky6605_cfg_rx_preamb_pat(uint8_t rx_preamb_pat)
{
    SPI_write1(SKY6605_RX_PREAMB_PAT, rx_preamb_pat);
}

void sky6605_cfg_txfifo_wl(uint8_t txfifo_wl)
{
    uint8_t val;
    val = txfifo_wl;
    if (txfifo_wl>127) val = 127;
    SPI_write1(SKY6605_TXFIFO_WL, val);
}

void sky6605_cfg_rxfifo_wl(uint8_t rxfifo_wl)
{
    uint8_t val;
    val = rxfifo_wl;
    if (rxfifo_wl>127) val = 127;
    SPI_write1(SKY6605_RXFIFO_WL, val);
}

// analog circuit initial config, do not config period wakeup function
void sky6605_analog_init(void)
{
    SPI_write1(SKY6605_RX_VCO_FDFH,0xB4);
    SPI_write1(SKY6605_RX_VCO_FDFL,0xF4);
    SPI_write1(SKY6605_RX_VCO_FDI,0x36);
    SPI_write1(SKY6605_TX_VCO_FDFH,0xCA);
    SPI_write1(SKY6605_TX_VCO_FDFL,0x97);
    SPI_write1(SKY6605_TX_VCO_FDI,0x35);
    SPI_write1(SKY6605_PLL1,0x84);
    SPI_write1(SKY6605_PLL2,0x72);
    SPI_write1(SKY6605_WAKEUP1,0x82);
    SPI_write1(SKY6605_WAKEUP2,0x01);
    SPI_write1(SKY6605_WAKEUP3,0xE1);
    SPI_write1(SKY6605_WAKEUP4,0x80);
    SPI_write1(SKY6605_RX_AFE1,0x4A); // adc_clk
    SPI_write1(SKY6605_RX_AFE2,0x8B);
    SPI_write1(SKY6605_TX_AFE1,0x88);
    SPI_write1(SKY6605_TX_AFE2,0xC0); // 
}

void sky6605_init(void)
{
    //sky6605_set_tx_rate(512);
    //sky6605_set_rx_rate(256);
    SPI_write1(SKY6605_SYS_CTRL,0x43);
    sky6605_cfg_timer_ctrl(30,0);
    sky6605_cfg_tx_len(8);
    sky6605_cfg_preamb_len(2,2);
    sky6605_cfg_rx_preamb_pat(0x00);
    sky6605_cfg_txfifo_wl(8);
    sky6605_cfg_rxfifo_wl(126);
    sky6605_disable_all_irq();
    sky6605_analog_init();
}


////////////////////////////////////////////////////////////////////////////////
// IRQ Functions                                                              //
////////////////////////////////////////////////////////////////////////////////
void sky6605_disable_all_irq(void)
{
    SPI_write1(SKY6605_IRQ_EN        , 0x00);
    SPI_write1(SKY6605_ERR_IRQ_EN    , 0x00);
}

void sky6605_enable_rx_irq(void)
{
  SPI_write1(SKY6605_IRQ_EN , (SKY6605_IRQ_TRERR | SKY6605_IRQ_RXFRQ | SKY6605_IRQ_RXFEF ));
}

void sky6605_enable_rx_fef_irq(void)
{
  SPI_write1(SKY6605_IRQ_EN , SKY6605_IRQ_RXFEF);
}

void sky6605_enable_rx_err_irq(void)
{
  SPI_write1(SKY6605_ERR_IRQ_EN, (SKY6605_IRQ_RXFE_ERR | SKY6605_IRQ_RXFF_ERR | SKY6605_IRQ_RX_7ENBB_ERR | SKY6605_IRQ_RX_FM0_ERR | SKY6605_IRQ_RX_7S1_ERR | SKY6605_IRQ_RX_CRC_ERR ));
}

void sky6605_enable_tx_irq(void)
{
  SPI_write1(SKY6605_IRQ_EN , (SKY6605_IRQ_TXFWQ | SKY6605_IRQ_TXE));
}

void sky6605_clr_all_irq(void)
{
  SPI_write1(SKY6605_ERR_STA , 0xff);
  SPI_write1(SKY6605_IRQ_STA , 0xff);//(SKY6605_IRQ_TRERR | SKY6605_IRQ_RXFRQ | SKY6605_IRQ_RXFEF ));
}

void sky6605_clr_rx_irq(void)
{
  SPI_write1(SKY6605_IRQ_STA , (SKY6605_IRQ_TRERR | SKY6605_IRQ_RXFRQ | SKY6605_IRQ_RXFEF ));
}

void sky6605_clr_rx_err_irq(void)
{
  SPI_write1(SKY6605_ERR_STA, (SKY6605_IRQ_RXFE_ERR | SKY6605_IRQ_RXFF_ERR  | SKY6605_IRQ_RX_7ENBB_ERR | SKY6605_IRQ_RX_FM0_ERR | SKY6605_IRQ_RX_7S1_ERR | SKY6605_IRQ_RX_CRC_ERR ));
}

////////////////////////////////////////////////////////////////////////////////
// TX/RX Base Functions                                                       //
////////////////////////////////////////////////////////////////////////////////
// uint16_t sky6605_rx_func(uint8_t rxfifo_wl)
// {
// #if 0
//   int j, addr;
//   uint8_t RD_DATA;
//   uint16_t ret=-1;
//   addr = 0;
//   j=1;

//   sky6605_enable_rx_err_irq();
//   sky6605_enable_rx_irq();
//   sky6605_clr_all_irq();

//   delay_100us(4);
//   SPI_cmd(SKY6605_CMD_RX);
//   while(j){   
//     if(SKY6605_IRQ_COME) {
//       RD_DATA = SPI_read1(SKY6605_IRQ_STA);
//       if(RD_DATA & SKY6605_IRQ_RXFEF){
//         pkt_cnt = pkt_cnt + 1;        
//         SPI_write1(SKY6605_IRQ_STA, SKY6605_IRQ_RXFEF); // Clear RXFEF IRQ
//         RD_DATA = SPI_read1(SKY6605_RXFIFO_LEN);
//         if(RD_DATA) SPI_reads_rxfifo(&rx_buf[addr], RD_DATA);
//         addr = addr + RD_DATA;
//         j = 0;        
// #ifdef SKY6605_PRINT_RX_DATA
//         NEW_ROW();for(int i=0; i< addr; i++) PRINT(rx_buf[i]);NEW_ROW();
//         NEW_ROW();PUTS("recv_cnt=");PRINT(pkt_cnt);PUTS("err_cnt=");PRINT(err_cnt);PUTS("err_7enbb=");PRINT(err_7enbb);PUTS("err_fm0=");PRINT(err_fm0);PUTS("err_7s1=");PRINT(err_7s1);PUTS("err_crc=");PRINT(err_crc);PRINT(addr);
// #endif
//         ret = addr;
//       }
//       else if(RD_DATA & SKY6605_IRQ_TRERR) {
//         RD_DATA = SPI_read1(SKY6605_ERR_STA);
//         err_cnt = err_cnt + 1;     
//         if(RD_DATA & SKY6605_IRQ_RX_7ENBB_ERR) err_7enbb = err_7enbb + 1;
//         if(RD_DATA & SKY6605_IRQ_RX_FM0_ERR)   err_fm0   = err_fm0 + 1;
//         if(RD_DATA & SKY6605_IRQ_RX_7S1_ERR)   err_7s1   = err_7s1 + 1;
//         if(RD_DATA & SKY6605_IRQ_RX_CRC_ERR)   err_crc   = err_crc + 1;
//         if(RD_DATA & (SKY6605_IRQ_RXFE_ERR)){ NEW_ROW();PUTS("Failed RX FIFO empty error !!! \t");NEW_ROW();}
//         if(RD_DATA & (SKY6605_IRQ_RXFF_ERR)){ NEW_ROW();PUTS("Failed RX FIFO full error !!! \t");NEW_ROW();}
//         
//         SPI_write1(SKY6605_ERR_STA, 0xff);      // Clear Error Status
//         SPI_write1(SKY6605_IRQ_STA, (SKY6605_IRQ_TRERR | SKY6605_IRQ_RXFRQ)); // Clear Error IRQ and RXFHF
//         addr = 0;
//         ret = -1;
//       }
//       else if(RD_DATA & SKY6605_IRQ_RXFRQ){
//         SPI_write1(SKY6605_IRQ_STA, SKY6605_IRQ_RXFRQ);
//         if(!SKY6605_IRQ_COME){
//           SPI_reads_rxfifo(&rx_buf[addr], rxfifo_wl);
//           addr = addr + rxfifo_wl;
//           
//         }
//       }
//     }
//   }
//   return ret;
// #endif

// }

void sky6605_tx_func(uint8_t tx_len, uint8_t txfifo_wl)
{
#if 0
   uint8_t i,j,RD_DATA;
   int addr;
   sky6605_enable_tx_irq();
   sky6605_clr_all_irq();
   i = tx_len;
   addr = 0;
   sky6605_cfg_tx_len(tx_len);
   if(i <= SKY6605_FIFO_MAX){
     SPI_writes_txfifo(&tx_buf[addr], i);
     addr = i;
     i = 0;
   }
   else {
     SPI_writes_txfifo(&tx_buf[addr], SKY6605_FIFO_MAX);
     addr = SKY6605_FIFO_MAX;
     i = i - SKY6605_FIFO_MAX;
   }
   SPI_cmd(SKY6605_CMD_TX);
 
   j = 1;
   while(j){
     if(SKY6605_IRQ_COME){
       RD_DATA = SPI_read1(SKY6605_IRQ_STA);
       if(RD_DATA & SKY6605_IRQ_TXE){
         j = 0;
         SPI_write1(SKY6605_IRQ_STA, SKY6605_IRQ_TXE); // Clear TXE tx end IRQ
       }
       else {
         j = 1;
         SPI_write1(SKY6605_IRQ_STA, SKY6605_IRQ_TXFWQ); // Clear TXFWQ IRQ
         if(i > (int) (128 - txfifo_wl)){
           SPI_writes_txfifo(&tx_buf[addr], (128 - txfifo_wl));
           addr = addr + (128 - txfifo_wl);
           i = i- (128 - txfifo_wl);
         }
         else {
           if(i)SPI_writes_txfifo(&tx_buf[addr], i);
           i = 0;
         }
       }
     }
   }
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Period Wakeup Base Functions                                               //
////////////////////////////////////////////////////////////////////////////////
void sky6605_period_wakeup_enable(void)
{
    uint8_t val;
    val  = SPI_read1(SKY6605_PWKU_CTRL);
    val |= 0x80;
    SPI_write1(SKY6605_PWKU_CTRL,val);
}

void sky6605_period_wakeup_disable(void)
{
    uint8_t val;
    val  = SPI_read1(SKY6605_PWKU_CTRL);
    val &= 0x7F;
    SPI_write1(SKY6605_PWKU_CTRL,val);
}

void sky6605_period_wakeup_config(uint8_t pwku_wku_en    , uint8_t pwku_mode  ,
                                  uint8_t pwku_t1        , uint8_t pwku_t2    ,
                                  uint8_t pwku_t3        , uint8_t pwku_t4    ,
                                  uint8_t pwku_cnt1      , uint8_t pwku_cnt2  ,
                                  uint8_t pwku_rssi_delta, uint8_t pwku_rssi_th)
{
    uint8_t val;

    val = ((pwku_wku_en&0x01)<<3) + (pwku_mode&0x03);
    SPI_write1(SKY6605_PWKU_CTRL,val);

    val = ((pwku_t1&0x03)<<6) + (pwku_t2&0x1f);
    SPI_write1(SKY6605_PWKU_T1T2,val);

    val = ((pwku_t3&0x0f)<<4) + (pwku_t4&0x0f);
    SPI_write1(SKY6605_PWKU_T3T4,val);

    val = ((pwku_cnt1&0x0f)<<4) + (pwku_cnt2&0x0f);
    SPI_write1(SKY6605_PWKU_CNT,val);

    val = ((pwku_rssi_delta&0x0f)<<4) + (pwku_rssi_th&0x0f);
    SPI_write1(SKY6605_PWKU_RSSI_CFG,val);
    
    SPI_write1(SKY6605_RX_AFE1,0x4A); // cfg adc_clk
    SPI_write1(SKY6605_RX_AFE2,0x8B); // 48
}

// sky6605 rxtx test func used for chip bringup and test
// User should NOT use this func in production program.
void sky6605_rxtx_test(uint8_t rxfifo_wl, uint8_t txfifo_wl)
{
    int16_t ret=0;
//    ret = sky6605_rx_func(rxfifo_wl);
    if (ret == -1) {
        NEW_ROW();PUTS("No Answer !!! ");
    }
    else if(rx_buf[0]==0xff & rx_buf[1]==0xff & rx_buf[2]==0xff & rx_buf[3]==0xff ) {
        //NEW_ROW();PUTS("TX frame 1 begin !!! ");
        tx_buf[0  ] = 0x00;
        tx_buf[1  ] = 0x00;
        tx_buf[2  ] = 0x7E;
    #if 0
        tx_buf[3  ] = 0x03;     
        tx_buf[4  ] = 0x22;
        tx_buf[5  ] = 0xFF;
        tx_buf[6  ] = 0xFF;
    #endif
    #if 1
        tx_buf[3  ] = 0x12;     
        tx_buf[4  ] = 0x34;
        tx_buf[5  ] = 0x56;
        tx_buf[6  ] = 0x78;
    #endif
        tx_buf[7  ] = 0xc0;
        tx_buf[8  ] = 0x03;
        tx_buf[9  ] = 0x91;
        tx_buf[10 ] = 0xd0;
        tx_buf[11 ] = 0x00;
        tx_buf[12 ] = 0x01;
        tx_buf[13 ] = 0xc1;
        tx_buf[14 ] = 0x01;
        tx_buf[15 ] = 0x20;
        tx_buf[16 ] = 0x27;
        tx_buf[17 ] = 0x45;
        tx_buf[18 ] = 0x54;
        tx_buf[19 ] = 0x43;
        tx_buf[20 ] = 0x01;
        tx_buf[21 ] = 0x00;
        tx_buf[22 ] = 0x01;
        tx_buf[23 ] = 0x00;
        tx_buf[24 ] = 0x01;
        tx_buf[25 ] = 0x16;
        tx_buf[26 ] = 0x10;
        tx_buf[27 ] = 0x86;
        tx_buf[28 ] = 0x88;
        tx_buf[29 ] = 0x00;
        tx_buf[30 ] = 0x03;
        tx_buf[31 ] = 0x03;
        tx_buf[32 ] = 0x98;
        tx_buf[33 ] = 0x17;
        tx_buf[34 ] = 0x51;
        tx_buf[35 ] = 0x20;
        tx_buf[36 ] = 0x08;
        tx_buf[37 ] = 0x04;
        tx_buf[38 ] = 0x08;
        tx_buf[39 ] = 0x20;
        tx_buf[40 ] = 0x20;
        tx_buf[41 ] = 0x01;
        tx_buf[42 ] = 0x01;
        tx_buf[43 ] = 0x28;
        tx_buf[44 ] = 0xff;
        tx_buf[45 ] = 0xff;
        tx_buf[46 ] = 0xff;
        tx_buf[47 ] = 0xff;
        tx_buf[48 ] = 0xff;
        tx_buf[49 ] = 0xff;
        tx_buf[50 ] = 0xff;
        tx_buf[51 ] = 0xff;
        tx_buf[52 ] = 0xff;
        tx_buf[53 ] = 0xff;
        tx_buf[54 ] = 0xff;
        tx_buf[55 ] = 0xff;
        tx_buf[56 ] = 0xff;
        tx_buf[57 ] = 0xff;
        tx_buf[58 ] = 0xff;
        tx_buf[59 ] = 0xff;
        tx_buf[60 ] = 0xff;
        tx_buf[61 ] = 0xff;
        tx_buf[62 ] = 0xff;
        tx_buf[63 ] = 0xff;
        tx_buf[64 ] = 0xff;
        tx_buf[65 ] = 0xff;
        tx_buf[66 ] = 0xff;
        tx_buf[67 ] = 0xff;
        tx_buf[68 ] = 0xff;
        tx_buf[69 ] = 0xff;
        tx_buf[70 ] = 0xff;
        tx_buf[71 ] = 0xff;
        tx_buf[72 ] = 0x00;
        tx_buf[73 ] = 0x00;
        tx_buf[74 ] = 0x00;
        tx_buf[75 ] = 0x00;
        tx_buf[76 ] = 0x00;
        tx_buf[77 ] = 0x00;
        tx_buf[78 ] = 0x00;
        tx_buf[79 ] = 0x00;
        tx_buf[80 ] = 0x00;
        tx_buf[81 ] = 0x00;
        tx_buf[82 ] = 0x00;
        tx_buf[83 ] = 0x00;
        tx_buf[84 ] = 0x00;
        tx_buf[85 ] = 0x00;
        tx_buf[86 ] = 0x00;
        tx_buf[87 ] = 0x00;
        tx_buf[88 ] = 0x00;
        tx_buf[89 ] = 0x00;
        tx_buf[90 ] = 0x00;
        tx_buf[91 ] = 0x00;
        tx_buf[92 ] = 0x00;
        tx_buf[93 ] = 0x00;
        tx_buf[94 ] = 0x00;
        tx_buf[95 ] = 0x00;
        tx_buf[96 ] = 0x00;
        tx_buf[97 ] = 0x00;
        tx_buf[98 ] = 0x00;
        tx_buf[99 ] = 0x00;
        tx_buf[100] = 0x00;
        tx_buf[101] = 0x00;
        tx_buf[102] = 0x00;
        tx_buf[103] = 0x00;
        tx_buf[104] = 0x00;
        tx_buf[105] = 0x00;
        tx_buf[106] = 0x00;
        tx_buf[107] = 0x00;
        tx_buf[108] = 0x00;
        tx_buf[109] = 0x00;
        tx_buf[110] = 0x00;
        tx_buf[111] = 0x00;
        tx_buf[112] = 0x00;
        tx_buf[113] = 0x00;
        tx_buf[114] = 0x00;
        tx_buf[115] = 0x00;
        tx_buf[116] = 0x00;
        tx_buf[117] = 0x00;
        tx_buf[118] = 0x00;
    #if 0
        tx_buf[119] = 0x03;
        tx_buf[120] = 0x22;
        tx_buf[121] = 0xFF;
        tx_buf[122] = 0xFF;
    #endif
    #if 1
        tx_buf[119] = 0x12;
        tx_buf[120] = 0x34;
        tx_buf[121] = 0x56;
        tx_buf[122] = 0x78;
    #endif
        tx_buf[123] = 0x12;
        tx_buf[124] = 0x22;
        tx_buf[125] = 0x00;
        //tx_buf[126] = 0xd9;
        //tx_buf[127] = 0x84;
        tx_buf[126] = 0x7E;
        tx_buf[127] = 0x00;
        sky6605_tx_func(128,txfifo_wl);
    }
    else if(rx_buf[0]==0x12 & rx_buf[1]==0x34 & rx_buf[2]==0x56 & rx_buf[3]==0x78 ) {
        //NEW_ROW();PUTS("TX frame 2 begin !!! ");
        tx_buf[0  ] = 0x00;
        tx_buf[1  ] = 0x00;
        tx_buf[2  ] = 0x7E;
        tx_buf[3  ] = 0x12;     
        tx_buf[4  ] = 0x34;
        tx_buf[5  ] = 0x56;
        tx_buf[6  ] = 0x78;
        tx_buf[7  ] = 0xe0;
        tx_buf[8  ] = 0xf7;
        tx_buf[9  ] = 0x00;
        tx_buf[10 ] = 0x91;
        tx_buf[11 ] = 0x18;
        tx_buf[12 ] = 0x01;
        tx_buf[13 ] = 0x15;
        tx_buf[14 ] = 0x01;
        tx_buf[15 ] = 0x1d;
        tx_buf[16 ] = 0x5a;
        tx_buf[17 ] = 0xba;
        tx_buf[18 ] = 0x6e;
        tx_buf[19 ] = 0x86;
        tx_buf[20 ] = 0x9f;
        tx_buf[21 ] = 0x45;
        tx_buf[22 ] = 0xed;
        tx_buf[23 ] = 0x3e;
        tx_buf[24 ] = 0x38;
        tx_buf[25 ] = 0x8a;
        tx_buf[26 ] = 0xb4;
        tx_buf[27 ] = 0x23;
        tx_buf[28 ] = 0x37;
        tx_buf[29 ] = 0x3a;
        tx_buf[30 ] = 0x6a;
        tx_buf[31 ] = 0x2e;
        tx_buf[32 ] = 0x18;
        tx_buf[33 ] = 0x8a;
        tx_buf[34 ] = 0x12;
        tx_buf[35 ] = 0x84;
        tx_buf[36 ] = 0xf4;
        tx_buf[37 ] = 0xeb;
        tx_buf[38 ] = 0x36;
        tx_buf[39 ] = 0x24;
        tx_buf[40 ] = 0xc6;
        tx_buf[41 ] = 0x62;
        tx_buf[42 ] = 0x3d;
        tx_buf[43 ] = 0x41;
        tx_buf[44 ] = 0x4d;
        tx_buf[45 ] = 0x06;
        tx_buf[46 ] = 0x15;
        tx_buf[47 ] = 0x00;
        tx_buf[48 ] = 0x00;
        tx_buf[49 ] = 0x00;
        tx_buf[50 ] = 0x00;
        tx_buf[51 ] = 0x00;
        tx_buf[52 ] = 0x00;
        tx_buf[53 ] = 0x00;
        tx_buf[54 ] = 0x00;
        tx_buf[55 ] = 0x00;
        //tx_buf[56 ] = 0x0d;
        //tx_buf[57 ] = 0x19;
        tx_buf[56] = 0x7E;
        tx_buf[57] = 0x00;
        sky6605_tx_func(58,txfifo_wl);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Debug/Production Test/Bring Up Functions                                   //
////////////////////////////////////////////////////////////////////////////////
void sky6605_period_wakeup_response(void)
{ 
#ifdef SKY6605_PRINT_RX_DATA
    SPI_reads_rxfifo(&rx_buf[0], 32);
    NEW_ROW();for(int i=0; i< 32; i++) PRINT(rx_buf[i]);NEW_ROW();
#else
    SPI_cmd(SKY6605_CMD_CL_RXFF);
#endif
    tx_buf[0  ] = 0x00;
    tx_buf[1  ] = 0x00;
    tx_buf[2  ] = 0x7E;
    tx_buf[3  ] = 0x12; 
    tx_buf[4  ] = 0x34;
    tx_buf[5  ] = 0x56;
    tx_buf[6  ] = 0x78;
    tx_buf[7  ] = 0x7E;
    tx_buf[8  ] = 0x00;
    sky6605_tx_func(9,64);
}

// Please Be caureful to use this func. In FIFO Loopback mode, normal RX/TX func is not worked.
// Please Do not set other values for SKY6605_TEST2 !!!
void sky6605_set_FIFO_LB_mode (void)
{
    SPI_write1(SKY6605_TEST2, 0x1F);
}

void sky6605_disable_FIFO_LB_mode (void)
{
    SPI_write1(SKY6605_TEST2, 0x00);
}

void sky6605_set_MFOUT_txdata (void)
{
    SPI_write1(SKY6605_TEST1, 0x02);
}

void sky6605_set_MFOUT_rxdata (void)
{
    SPI_write1(SKY6605_TEST1, 0x01);
}

// sky6605 rx test mode
// Using MFOUT, MFCLK to monitor the RX performance / Debugging
void sky6605_rx_its(uint8_t FM0_decoder_en, uint8_t RX_decoder_en)
{
   
   if(RX_decoder_en == 0x00){
     if(FM0_decoder_en == 0x00) {         // disable fm0 decoder, disable decoder zero insert
       SPI_write1(SKY6605_TEST1, 0x41);   // MFCLK = 13.56MHz, MFOUT = rxdata from ananlog
     }
     else if(FM0_decoder_en != 0x00) {    // enable  fm0 decoder, disable decoder zero insert
       SPI_write1(SKY6605_TEST1, 0x23);   // MFCLK = fm0_dec_clk, MFOUT = fm0_dec_data
     }
   }
   else if(RX_decoder_en != 0x00){        // enable  fm0 decoder, enable decoder zero insert
     SPI_write1(SKY6605_TEST1, 0x34);     // MFCLK = pktp_rx_clk, MFOUT = (payload + crc) delete zero insert
   }
   delay_100us(4);
   SPI_write1(SKY6605_SYS_CTRL, 0x43); // rx 256k, tx 512k, crc enable
   SPI_cmd(SKY6605_CMD_RX_TEST);
   while(1) {};
}

// sky6605 tx test mode
// TX constant 0 ,constant 1
void sky6605_tx_constant(uint8_t constant1, uint8_t FM0_encode_en)
{
  if(FM0_encode_en == 0x00){ //FM0 encoder disable
    if(constant1 == 0x00){   //constant 0
      SPI_write1(SKY6605_TEST0, 0x11);   //constant0 to analog
    }
    else{                    //constant 1
      SPI_write1(SKY6605_TEST0, 0x12);   //constant1 to analog
    }
  }
  else{                      //FM0 encoder enable
    if(constant1 == 0x00){   //constant 0
      //MFIN tied to 0
      SPI_write1(SKY6605_TEST0, 0x10);   //fm0_tx_data to analog
    }
    else{                    //constant 1
      //MFIN tied to 1
      SPI_write1(SKY6605_TEST0, 0x10);   //fm0_tx_data to analog
    }
  }
  
  delay_100us(4);
  SPI_write1(SKY6605_TXFIFO, 0x00);   //drive top_fsm from wait_tx to data_tx
  SPI_write1(SKY6605_TX_LEN, 0x00);   //drive tx_fsm continal tx payload 
  SPI_cmd(SKY6605_CMD_TX_TEST);
  while(1) {};
}
