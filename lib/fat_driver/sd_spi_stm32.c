/* AM's modified version of Martin Thomas' original SD-in-SPI for STM32
 * code from:
 * http://gandalf.arubi.uni-kl.de/avr_projects/arm_projects/arm_memcards/index.html*
 * With modifications to use Libmaple rather than CMSIS.
 *
 *
 * Martin's original copyright starts here: */
/*-----------------------------------------------------------------------*/
/* MMC/SDSC/SDHC (in SPI mode) control module for STM32 Version 1.1.6    */
/* (C) Martin Thomas, 2010 - based on the AVR MMC module (C)ChaN, 2007   */
/*-----------------------------------------------------------------------*/

/* Copyright (c) 2010, Martin Thomas, ChaN
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#include <stdbool.h>
#include "libmaple.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "fat_fs/ffconf.h"
#include "fat_fs/diskio.h"
#include "main.h"

// demo uses a command line option to define this (see Makefile):
// #define STM32_SD_USE_DMA

#ifdef STM32_SD_USE_DMA
// #warning "Information only: using DMA"
#pragma message "*** Using DMA ***"
#endif

/* set to 1 to provide a disk_ioctrl function even if not needed by the FatFs */
#define STM32_SD_DISK_IOCTRL_FORCE      0

// demo uses a command line option to define this (see Makefile):
//#define USE_EK_STM32F
//#define USE_STM32_P103
//#define USE_MINI_STM32

#if defined(BOARD_maple)

/* Power switchable? */
#define CARD_SUPPLY_SWITCHABLE   1
/* Use maple pin 23 == PC15 */
#define SD_PWR_GPIO              GPIOC
#define SD_PWR_PIN               15
#define SD_PWR_MODE              GPIO_OUTPUT_OD

/* Write protect? */
#define SOCKET_WP_CONNECTED      0

/* Card present? */
#define SOCKET_CP_CONNECTED      1

/* I'll connect it to Maple pin 35 == PC6 and 
 * use the internal pull-up */
#define SD_CP_GPIO               GPIOC
#define SD_CP_PIN                6
#define SD_CP_MODE               GPIO_INPUT_PU

#define SD_SPI                   SPI2

#define DMA_Channel_SPI_SD_RX    DMA1_Channel2
#define DMA_Channel_SPI_SD_TX    DMA1_Channel3
#define DMA_FLAG_SPI_SD_TC_RX    DMA1_FLAG_TC2
#define DMA_FLAG_SPI_SD_TC_TX    DMA1_FLAG_TC3

/* Chip select? */
#define SD_CS_GPIO               GPIOB
#define SD_CS                    0

#define SD_SPI_BASE              GPIOB
#define SD_SPI_SCK               13
#define SD_SPI_MISO              14
#define SD_SPI_MOSI              15 

#define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB2PeriphClockCmd
#define RCC_APBPeriph_SPI_SD     RCC_APB2Periph_SPI2

/* - for SPI1 and full-speed APB2: 72MHz/4 */
#define SPI_FAST_PRESCALER  SPI_BAUD_PCLK_DIV_2
#define SPI_SLOW_PRESCALER  SPI_BAUD_PCLK_DIV_256

#else
#error "unsupported board"
#endif


/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

/* Card-Select Controls  (Platform dependent) */
#define SELECT()       gpio_write_bit( SD_CS_GPIO, SD_CS, 0 ) 
#define DESELECT()     gpio_write_bit( SD_CS_GPIO, SD_CS, 1 ) 


#if (_MAX_SS != 512) || (_FS_READONLY == 0) || (STM32_SD_DISK_IOCTRL_FORCE == 1)
#define STM32_SD_DISK_IOCTRL   1
#else
#define STM32_SD_DISK_IOCTRL   0
#endif

/*--------------------------------------------------------------------------

  Module Private Functions and Variables

  ---------------------------------------------------------------------------*/

static const DWORD socket_state_mask_cp = (1 << 0);
static const DWORD socket_state_mask_wp = (1 << 1);

// Disk status
static volatile DSTATUS Stat = STA_NOINIT;	

// 100Hz decrement timers
static volatile DWORD Timer1, Timer2;	
//
// Card type flags
static byte_t CardType;			

enum speed_setting { INTERFACE_SLOW, INTERFACE_FAST };

static void spi_set_prescaler( spi_dev *dev, uint32_t spi_baud )
{
   uint32_t cr1 = dev->regs->CR1;
   cr1 &= ~(SPI_CR1_BR);
   cr1 |= spi_baud;
   dev->regs->CR1 = cr1;
}

static void interface_speed( enum speed_setting speed )
{
  if ( speed == INTERFACE_SLOW )  
    spi_set_prescaler( SD_SPI, SPI_SLOW_PRESCALER );
  else
    spi_set_prescaler( SD_SPI, SPI_FAST_PRESCALER );
}

#if SOCKET_WP_CONNECTED
/* Socket's Write-Protection Pin: high = write-protected, low = writable */
/* I haven't enabled WP on my socket, so this is untested  AMM 22/1/11 */
static void socket_wp_init(void)
{
  gpio_set_mode( SD_WP_GPIO, SD_WP_PIN, SD_WP_MODE );
}

static DWORD socket_is_write_protected(void)
{
  return ( gpio_read_bit( SD_WP_GPIO, SD_WP_PIN ) ) ? socket_state_mask_wp : false;
}

#else

static void socket_wp_init(void)
{
  return;
}

static inline DWORD socket_is_write_protected(void)
{
  return 0; /* fake not protected */
}

#endif /* SOCKET_WP_CONNECTED */


#if SOCKET_CP_CONNECTED
/* Socket's Card-Present Pin: high = socket empty, low = card inserted */
static void socket_cp_init(void)
{
  gpio_set_mode( SD_CP_GPIO, SD_CP_PIN, SD_CP_MODE );
}
// On this hardware, with a mild pull-up on CP, 1 = no card, 0 = card
static inline DWORD socket_is_empty(void)
{
  return ( gpio_read_bit( SD_CP_GPIO, SD_CP_PIN ) ) ? false : socket_state_mask_cp;
}

#else

static void socket_cp_init(void)
{
  return;
}

static inline DWORD socket_is_empty(void)
{
  return 0; /* fake inserted */
}

#endif /* SOCKET_CP_CONNECTED */


#if CARD_SUPPLY_SWITCHABLE

/*!!AMM this was static but I wanted to export it */
void card_power(uint8_t on)		/* switch FET for card-socket VCC */
{
  gpio_set_mode( SD_PWR_GPIO, SD_PWR_PIN, SD_PWR_MODE );
/*!!AMM GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; */

  if (on) {
    gpio_write_bit( SD_PWR_GPIO, SD_PWR_PIN, 0 );
  } else {
    // !!AMM Chip select internal pull-down (to avoid parasite powering) 
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
    // GPIO_Init(GPIO_CS, &GPIO_InitStructure);

    gpio_write_bit( SD_PWR_GPIO, SD_PWR_PIN, 1 );
  }
}

#if (STM32_SD_DISK_IOCTRL == 1)
int chk_power(void)		/* Socket power state: 0=off, 1=on */
{
  if ( gpio_read_bit( SD_PWR_GPIO, SD_PWR_PIN ) == 1 ) {
    debug_println("Power is on");
    return 1;
  } else {
    debug_println("Power is off");
    return 0;
  }
}
#endif

#else

void card_power(uint8_t on)
{
  on=on;
}

#if (STM32_SD_DISK_IOCTRL == 1)
static int chk_power(void)
{
  return 1; /* fake powered */
}
#endif

#endif /* CARD_SUPPLY_SWITCHABLE */

static inline uint8 spi_mode_fault( spi_dev *dev ) {
  return dev->regs->SR & SPI_SR_MODF;
}

/*-----------------------------------------------------------------------*/
/* Transmit/Receive a byte to MMC via SPI  (Platform dependent)          */
/*-----------------------------------------------------------------------*/
static byte_t stm32_spi_rw( byte_t out )
{
  uint8 retval = 0;

  // Follows the procedure from the reference manual RM0008 pp 665
  while( !spi_is_tx_empty( SD_SPI )) {  ; }
  spi_tx_reg( SD_SPI, out );

  while( !spi_is_rx_nonempty( SD_SPI ) && !spi_mode_fault( SD_SPI ) ) { ; }

  if( spi_mode_fault( SD_SPI ) ){
    debug_println("MODF");
    return;
  }

  retval = spi_rx_reg( SD_SPI );

/*  while( !spi_is_tx_empty( SD_SPI )) { ; }
  while( spi_is_busy( SD_SPI)) {;} */

  return retval;
}

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/
// Just transmit, no receive

#define xmit_spi(dat)  stm32_spi_rw(dat)
//static void xmit_spi( byte_t val )
//{
//  while( !spi_is_tx_empty( SD_SPI )) { ; } 
//  spi_tx_reg( SD_SPI, val );
//}

/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/
// Clock out an idle byte, receive a byte

static byte_t rcvr_spi(void)
{
  return stm32_spi_rw( 0xFF );
  //return stm32_spi_rw(0xff);
  //return spi_rx( SPI_SD );
}

/* Alternative macro to receive data fast */
#define rcvr_spi_m(dst)  *(dst)=stm32_spi_rw(0xff)



/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static byte_t wait_ready (void)
{
  byte_t res;

  Timer2 = 50;	/* Wait for ready in timeout of 500ms */

  rcvr_spi();

  do {
    res = rcvr_spi();
  } while ((res != 0xFF) && Timer2);

  return res;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static void release_spi (void)
{
  DESELECT();
  rcvr_spi();
}

#ifdef STM32_SD_USE_DMA
/*!!AMM haven't tested the DMA at all */
/*-----------------------------------------------------------------------*/
/* Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-----------------------------------------------------------------------*/
static void stm32_dma_transfer(
    bool receive,		/* false for buff->SPI, true for SPI->buff               */
    const byte_t *buff,	/* receive true  : 512 byte data block to be transmitted
                           receive false : Data buffer to store received data    */
    UINT btr 			/* receive true  : Byte count (must be multiple of 2)
                                   receive false : Byte count (must be 512)              */
    )
{
  DMA_InitTypeDef DMA_InitStructure;
  WORD rw_workbyte[] = { 0xffff };

  /* shared DMA configuration values */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (DWORD)(&(SD_SPI->DR));
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_BufferSize = btr;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_DeInit(DMA_Channel_SPI_SD_RX);
  DMA_DeInit(DMA_Channel_SPI_SD_TX);

  if ( receive ) {

    /* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
    /* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)buff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Init(DMA_Channel_SPI_SD_RX, &DMA_InitStructure);

    /* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
    /* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)rw_workbyte;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_Init(DMA_Channel_SPI_SD_TX, &DMA_InitStructure);

  } else {

#if _FS_READONLY == 0
    /* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
    /* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)rw_workbyte;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_Init(DMA_Channel_SPI_SD_RX, &DMA_InitStructure);

    /* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
    /* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)buff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Init(DMA_Channel_SPI_SD_TX, &DMA_InitStructure);
#endif

  }

  /* Enable DMA RX Channel */
  DMA_Cmd(DMA_Channel_SPI_SD_RX, ENABLE);
  /* Enable DMA TX Channel */
  DMA_Cmd(DMA_Channel_SPI_SD_TX, ENABLE);

  /* Enable SPI TX/RX request */
  SPI_I2S_DMACmd(SPI_SD, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

  /* Wait until DMA1_Channel 3 Transfer Complete */
  /// not needed: while (DMA_GetFlagStatus(DMA_FLAG_SPI_SD_TC_TX) == RESET) { ; }
  /* Wait until DMA1_Channel 2 Receive Complete */
  while (DMA_GetFlagStatus(DMA_FLAG_SPI_SD_TC_RX) == RESET) { ; }
  // same w/o function-call:
  // while ( ( ( DMA1->ISR ) & DMA_FLAG_SPI_SD_TC_RX ) == RESET ) { ; }

  /* Disable DMA RX Channel */
  DMA_Cmd(DMA_Channel_SPI_SD_RX, DISABLE);
  /* Disable DMA TX Channel */
  DMA_Cmd(DMA_Channel_SPI_SD_TX, DISABLE);

  /* Disable SPI RX/TX request */
  SPI_I2S_DMACmd(SPI_SD, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}
#endif /* STM32_SD_USE_DMA */


/*-----------------------------------------------------------------------*/
/* Power Control and interface-initialization (Platform dependent)       */
/*-----------------------------------------------------------------------*/

static void power_on (void)
{
  /*SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure; */
  volatile byte_t dummyread;

  // Clocks for the GPIO and SPI are set in gpio_init and spi_init
  spi_peripheral_disable( SD_SPI );

  card_power(1);
  for (Timer1 = 25; Timer1; );	/* Wait for 250ms */

  /* Configure I/O for Flash Chip select */
  /*!!AMM investigate the 50MHz flag... on all four GPIOs */
  gpio_init( SD_CS_GPIO );
  gpio_set_mode( SD_CS_GPIO, SD_CS, GPIO_OUTPUT_PP );
  DESELECT();

  gpio_init( SD_SPI_BASE );
  gpio_set_mode( SD_SPI_BASE, SD_SPI_SCK,  GPIO_AF_OUTPUT_PP );
  gpio_set_mode( SD_SPI_BASE, SD_SPI_MOSI, GPIO_AF_OUTPUT_PP );
  gpio_set_mode( SD_SPI_BASE, SD_SPI_MISO, GPIO_INPUT_PU );

  timer_set_mode( TIMER1, 1, TIMER_DISABLED );
  timer_set_mode( TIMER1, 2, TIMER_DISABLED );
  timer_set_mode( TIMER1, 3, TIMER_DISABLED );

  //usart_disable( USART3 );
  //i2c_disable( I2C2 );

  spi_irq_disable( SD_SPI, SPI_INTERRUPTS_ALL );
  // This handles GPIO setup on SCK, MISO and MOSI
  spi_init( SD_SPI );
  spi_master_enable( SD_SPI, SPI_SLOW_PRESCALER, 
                     SPI_MODE_0, SPI_FRAME_MSB | SPI_DFF_8_BIT | SPI_SW_SLAVE | SPI_SOFT_SS );
  
  // Ensure the SPI interface is cleared out...
  /*spi_tx_reg( SD_SPI, 0x00 );
  while( !spi_is_tx_empty( SD_SPI )) { ; } */

  //spi_rx_reg( SD_SPI ); 
  rcvr_spi();

#ifdef STM32_SD_USE_DMA
  /* enable DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif

}
static void power_off (void)
{

  if (!(Stat & STA_NOINIT)) {
    SELECT();
    wait_ready();
    release_spi();
  }

  /*SPI_I2S_DeInit(SPI_SD);
  SPI_Cmd(SPI_SD, DISABLE);
  RCC_APBPeriphClockCmd_SPI_SD(RCC_APBPeriph_SPI_SD, DISABLE); */
  spi_peripheral_disable( SD_SPI );

  /* All SPI-Pins to input with weak internal pull-downs */
  /*GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MISO | GPIO_Pin_SPI_SD_MOSI;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
  GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure); */

  gpio_set_mode( SD_SPI_BASE, SD_SPI_SCK,  GPIO_INPUT_PD );
  gpio_set_mode( SD_SPI_BASE, SD_SPI_MOSI, GPIO_INPUT_PD );
  gpio_set_mode( SD_SPI_BASE, SD_SPI_MISO, GPIO_INPUT_PD );
  //gpio_set_mode( SD_CS_GPIO,  SD_CS,       GPIO_INPUT_PD );

  card_power(0);

  Stat |= STA_NOINIT;		/* Set STA_NOINIT */
}


/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static bool rcvr_datablock (
    byte_t *buff,			/* Data buffer to store received data */
    UINT btr			/* Byte count (must be multiple of 4) */
    )
{
  byte_t token;


  Timer1 = 10;
  do {							/* Wait for data packet in timeout of 100ms */
    token = rcvr_spi();
  } while ((token == 0xFF) && Timer1);
  if(token != 0xFE) return false;	/* If not valid data token, return with error */

#ifdef STM32_SD_USE_DMA
  stm32_dma_transfer( true, buff, btr );
#else
  do {							/* Receive the data block into buffer */
    rcvr_spi_m(buff++);
    rcvr_spi_m(buff++);
    rcvr_spi_m(buff++);
    rcvr_spi_m(buff++);
  } while (btr -= 4);
#endif /* STM32_SD_USE_DMA */

  rcvr_spi();						/* Discard CRC */
  rcvr_spi();

  return true;					/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _FS_READONLY == 0
static bool xmit_datablock (
    const byte_t *buff,	/* 512 byte data block to be transmitted */
    byte_t token			/* Data/Stop token */
    )
{
  byte_t resp;
#ifndef STM32_SD_USE_DMA
  byte_t wc;
#endif

  if (wait_ready() != 0xFF) return false;

  xmit_spi(token);					/* transmit data token */
  if (token != 0xFD) {	/* Is data token */

#ifdef STM32_SD_USE_DMA
    stm32_dma_transfer( false, buff, 512 );
#else
    wc = 0;
    do {							/* transmit the 512 byte data block to MMC */
      xmit_spi(*buff++);
      xmit_spi(*buff++);
    } while (--wc);
#endif /* STM32_SD_USE_DMA */

    xmit_spi(0xFF);					/* CRC (Dummy) */
    xmit_spi(0xFF);
    resp = rcvr_spi();				/* Receive data response */
    if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
      return false;
  }

  return true;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
byte_t send_cmd (
    byte_t cmd,		/* Command byte */
    DWORD arg		/* Argument */
    )
{
  byte_t n, res = 0;


  if (cmd & 0x80) {	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
    cmd &= 0x7F;
    res = send_cmd(CMD55, 0);
    if (res > 1) return res;
  }

  /* Select the card and wait for ready */
  DESELECT();
  SELECT();

  if (wait_ready() != 0xFF) {
    DESELECT();
    return 0xFF;
  } 

  /* Send command packet */
  xmit_spi(cmd);						/* Start + Command index */
  xmit_spi((byte_t)(arg >> 24));		/* Argument[31..24] */
  xmit_spi((byte_t)(arg >> 16));		/* Argument[23..16] */
  xmit_spi((byte_t)(arg >> 8));			/* Argument[15..8] */
  xmit_spi((byte_t)arg);				/* Argument[7..0] */
  n = 0x01;							/* Dummy CRC + Stop */
  if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
  if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
  xmit_spi(n);

  /* Receive command response */
  if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */

  n = 10;								/* Wait for a valid response in timeout of 10 attempts */
  do
    res = rcvr_spi();
  while ((res & 0x80) && --n);

  return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

  Public Functions

  ---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    byte_t drv		/* Physical drive number (0) */
    )
{
  byte_t n, cmd, ty, ocr[4];

  if (drv) return STA_NOINIT;			/* Supports only single drive */
  if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

  if(!chk_power()) power_on();							/* Force socket power on and initialize interface */

  interface_speed(INTERFACE_SLOW);
  for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */

  ty = 0;
  if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
    //
    // M. Thomas has a 1 second timeout but I was having initialization errors with
    // a recent 2GB microSD.  Maybe my SPI bus is too slow?
    Timer1 = 500;						/* Initialization timeout of x milliseconds */

    // Only new cards understand CMD8, but you have to send it
    if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDHC */

      // Get trailing return value of R7 response 
      for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		

      // The card can work at VDD range of 2.7-3.6V 
      if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				
        do {
          cmd = send_cmd(ACMD41, 1UL<< 30 );
          //xprintf("%u %x\r\n",Timer1, cmd);
        } while (Timer1 && cmd);	/* Wait for leaving idle state (ACMD41 with HCS bit) */
       
        // Check CCS bit in the OCR.  If setm it's a high capacity card
        if (Timer1 && send_cmd(CMD58, 0) == 0) {		
          for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
          ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
        }
      }
    } else {							/* SDSC or MMC */

      // Older cards don't understand CMD8
      if (send_cmd(ACMD41, 0) <= 1) 	{
        ty = CT_SD1; cmd = ACMD41;	/* SDSC */
      } else {
        ty = CT_MMC; cmd = CMD1;	/* MMC */
      }
      while (Timer1 && send_cmd(cmd, 0));			/* Wait for leaving idle state */
      if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
        ty = 0;
    }
  } 
  CardType = ty;
  release_spi();

  if (ty) {			/* Initialization succeeded */
    Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
    interface_speed(INTERFACE_FAST);
  } else {			/* Initialization failed */
    power_off();
  }

  return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    byte_t drv		/* Physical drive number (0) */
    )
{
  if (drv) return STA_NOINIT;		/* Supports only single drive */
  return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    byte_t drv,			/* Physical drive number (0) */
    byte_t *buff,			/* Pointer to the data buffer to store read data */
    DWORD sector,		/* Start sector number (LBA) */
    byte_t count			/* Sector count (1..255) */
    )
{
  if (drv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;

  if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

  if (count == 1) {	/* Single block read */
    if (send_cmd(CMD17, sector) == 0)	{ /* READ_SINGLE_BLOCK */
      if (rcvr_datablock(buff, 512)) {
        count = 0;
      }
    }
  }
  else {				/* Multiple block read */
    if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
      do {
        if (!rcvr_datablock(buff, 512)) {
          break;
        }
        buff += 512;
      } while (--count);
      send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
    }
  }
  release_spi();

  return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _FS_READONLY == 0

DRESULT disk_write (
    byte_t drv,			/* Physical drive number (0) */
    const byte_t *buff,	/* Pointer to the data to be written */
    DWORD sector,		/* Start sector number (LBA) */
    byte_t count			/* Sector count (1..255) */
    )
{
  if (drv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  if (Stat & STA_PROTECT) return RES_WRPRT;

  if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

  if (count == 1) {	/* Single block write */
    if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
        && xmit_datablock(buff, 0xFE))
      count = 0;
  }
  else {				/* Multiple block write */
    if (CardType & CT_SDC) send_cmd(ACMD23, count);
    if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
      do {
        if (!xmit_datablock(buff, 0xFC)) break;
        buff += 512;
      } while (--count);
      if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
        count = 1;
    }
  }
  release_spi();

  return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY == 0 */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if (STM32_SD_DISK_IOCTRL == 1)
DRESULT disk_ioctl (
    byte_t drv,		/* Physical drive number (0) */
    byte_t ctrl,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
    )
{
  DRESULT res;
  byte_t n, csd[16], *ptr = (byte_t *)buff;
  WORD csize;

  if (drv) return RES_PARERR;

  res = RES_ERROR;

  if (ctrl == CTRL_POWER) {
    switch (*ptr) {
      case 0:		/* Sub control code == 0 (POWER_OFF) */
        if (chk_power()) {
          power_off();		/* Power off */
        }
        res = RES_OK;
        break;
      case 1:		/* Sub control code == 1 (POWER_ON) */
        debug_println("Power is off, we'll turn it on");
        power_on();				/* Power on */
        res = RES_OK;
        break;
      case 2:		/* Sub control code == 2 (POWER_GET) */
        *(ptr+1) = (byte_t)chk_power();
        res = RES_OK;
        break;
      default :
        res = RES_PARERR;
    }
  }
  else {
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    switch (ctrl) {
      case CTRL_SYNC :		/* Make sure that no pending write process */
        SELECT();
        if (wait_ready() == 0xFF)
          res = RES_OK;
        break;

      case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
        if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
          if ((csd[0] >> 6) == 1) {	/* SDC version 2.00 */
            csize = csd[9] + ((WORD)csd[8] << 8) + 1;
            *(DWORD*)buff = (DWORD)csize << 10;
          } else {					/* SDC version 1.XX or MMC*/
            n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
            csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
            *(DWORD*)buff = (DWORD)csize << (n - 9);
          }
          res = RES_OK;
        }
        break;

      case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

      case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
        if (CardType & CT_SD2) {	/* SDC version 2.00 */
          if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
            rcvr_spi();
            if (rcvr_datablock(csd, 16)) {				/* Read partial block */
              for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
              *(DWORD*)buff = 16UL << (csd[10] >> 4);
              res = RES_OK;
            }
          }
        } else {					/* SDC version 1.XX or MMC */
          if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
            if (CardType & CT_SD1) {	/* SDC version 1.XX */
              *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
            } else {					/* MMC */
              *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
            }
            res = RES_OK;
          }
        }
        break;

      case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
        *ptr = CardType;
        res = RES_OK;
        break;

      case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
        if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
            && rcvr_datablock(ptr, 16))
          res = RES_OK;
        break;

      case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
        if (send_cmd(CMD10, 0) == 0		/* READ_CID */
            && rcvr_datablock(ptr, 16))
          res = RES_OK;
        break;

      case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
        if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
          for (n = 4; n; n--) *ptr++ = rcvr_spi();
          res = RES_OK;
        }
        break;

      case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
        if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
          rcvr_spi();
          if (rcvr_datablock(ptr, 64))
            res = RES_OK;
        }
        break;

      default:
        res = RES_PARERR;
    }

    release_spi();
  }

  return res;
}
#endif /* _USE_IOCTL != 0 */


// Some hardware initialization which should bjust be run once at the outset
void disk_subsystem_init( void )
{
  socket_cp_init();
  socket_wp_init();
}

/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void disk_timerproc (void)
{
  static DWORD pv;
  DWORD ns;
  DWORD n; 
  byte_t s;


  n = Timer1;                /* 100Hz decrement timers */
  if (n) Timer1 = --n;
  n = Timer2;
  if (n) Timer2 = --n;

  ns = pv;
  pv = socket_is_empty() | socket_is_write_protected();	/* Sample socket switch */

  if (ns == pv) {                         /* Have contacts stabled? */
    s = Stat;

    if (pv & socket_state_mask_wp)      /* WP is H (write protected) */
      s |= STA_PROTECT;
    else                                /* WP is L (write enabled) */
      s &= ~STA_PROTECT;

    if (pv & socket_state_mask_cp)      /* INS = H (Socket empty) */
      s |= (STA_NODISK | STA_NOINIT);
    else                                /* INS = L (Card inserted) */
      s &= ~STA_NODISK;

    Stat = s;
  }
}

