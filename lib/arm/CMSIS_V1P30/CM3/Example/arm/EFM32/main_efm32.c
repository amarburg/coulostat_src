/*************************************************************************//**
* @file:    main_efm32.c
* @purpose: CMSIS Cortex-M3 Core Peripheral Access Layer Source File
*           Basic SysTick interrupt
 * @version 1.0.0
* @date:    27. July 2009
*----------------------------------------------------------------------------
*
* Copyright (C) 2009 ARM Limited. All rights reserved.
*
* ARM Limited (ARM) is supplying this software for use with Cortex-M3
* processor based microcontrollers.  This file can be freely distributed
* within development tools that are supporting such ARM based processors.
*
* THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
*****************************************************************************/

#include "efm32.h"

/**************************************************************************//**
 * Defines FPGA register bank for Energy Micro Development Kit (DVK) board
 *
 *****************************************************************************/
typedef struct
{
  __IO uint16_t SPI_CTRL;
  __IO uint16_t SPI_STATUS;
  __IO uint16_t SPI_DATA;

  __IO uint16_t UIF_LEDS;
  __IO uint16_t UIF_PB;
  __IO uint16_t UIF_DIP;
  __IO uint16_t UIF_JOYSTICK;
  __IO uint16_t UIF_AEM;

  __IO uint16_t IREG_DISPLAY_CTRL;
  __IO uint16_t EBI_CFG;
  __IO uint16_t BUS_CFG;

  __IO uint16_t INTREG;
  __IO uint16_t PERICON;
  __IO uint16_t AEM_STATE;

} FPGARegister_TypeDef;

#define EBI_REGISTER_BASE    0x8c000000
#define FPGARegister    ((FPGARegister_TypeDef *) EBI_REGISTER_BASE)
/**************************************************************************//**
 * @brief Initializes External Bus Interface for access to FPGA Registers
 * on the EFM32-Gxxx-DK development kits 
 *****************************************************************************/
void EBI_init(void)
{
  GPIO_TypeDef *gpio = GPIO;
  EBI_TypeDef  *ebi  = EBI;
  CMU_TypeDef  *cmu  = CMU;

  /* Enable clocks */
  cmu->HFCORECLKEN0 |= CMU_HFCORECLKEN0_EBI;
  cmu->HFPERCLKEN0  |= CMU_HFPERCLKEN0_GPIO;

  /* Configure GPIO pins as push pull */
  /* EBI AD9..15 */
  gpio->P[0].MODEL = 
    GPIO_P_MODEL_MODE0_PUSHPULL |
    GPIO_P_MODEL_MODE1_PUSHPULL |
    GPIO_P_MODEL_MODE2_PUSHPULL |
    GPIO_P_MODEL_MODE3_PUSHPULL |
    GPIO_P_MODEL_MODE4_PUSHPULL |
    GPIO_P_MODEL_MODE5_PUSHPULL |
    GPIO_P_MODEL_MODE6_PUSHPULL;
  /* EBI AD8 */
  gpio->P[0].MODEH = 
    GPIO_P_MODEH_MODE15_PUSHPULL;
  /* EBI CS3 */
  gpio->P[3].MODEH = 
    GPIO_P_MODEH_MODE12_PUSHPULL;
  /* EBI AD0..7 */
  gpio->P[4].MODEH =
    GPIO_P_MODEH_MODE8_PUSHPULL |
    GPIO_P_MODEH_MODE9_PUSHPULL |
    GPIO_P_MODEH_MODE10_PUSHPULL |
    GPIO_P_MODEH_MODE11_PUSHPULL |
    GPIO_P_MODEH_MODE12_PUSHPULL |
    GPIO_P_MODEH_MODE13_PUSHPULL |
    GPIO_P_MODEH_MODE14_PUSHPULL |
    GPIO_P_MODEH_MODE15_PUSHPULL;
  /* EBI ARDY/ALEN/Wen/Ren */
  gpio->P[5].MODEL = 
    GPIO_P_MODEL_MODE2_PUSHPULL |
    GPIO_P_MODEL_MODE3_PUSHPULL |
    GPIO_P_MODEL_MODE4_PUSHPULL |
    GPIO_P_MODEL_MODE5_PUSHPULL;

  /* Configure bus connect - PC bit 12 - active low */
  gpio->P[2].MODEH = 
    GPIO_P_MODEH_MODE12_PUSHPULL;
  gpio->P[2].DOUT = 0x0000;

  /* Configure EBI controller */
  /* 16 bit address, 16 bit data mode */
  /* Enable bank 3 address map 0x8c000000, FPGA Registers */
  ebi->CTRL =
    EBI_CTRL_MODE_D16A16ALE |
    EBI_CTRL_BANK3EN;

  /* Zero setup time, 1 clock hold address time - default */
  ebi->ADDRTIMING = 1 << _EBI_ADDRTIMING_ADDRHOLD_SHIFT;

  /* Default values for all write timing registers, read timing conservative */
  ebi->RDTIMING = 7 << _EBI_RDTIMING_RDSTRB_SHIFT | 2 << _EBI_RDTIMING_RDHOLD_SHIFT | 2 << _EBI_RDTIMING_RDSETUP_SHIFT;
  ebi->WRTIMING = _EBI_WRTIMING_RESETVALUE;
  ebi->POLARITY = _EBI_POLARITY_RESETVALUE;

  /* Toggle on all chip selects for all bank 3, registers */
  ebi->ROUTE =
    EBI_ROUTE_CS3PEN |
    EBI_ROUTE_ALEPEN |
    EBI_ROUTE_EBIPEN;
}

volatile uint32_t msTicks;                        /**< Ccounts 1ms timeTicks */
/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;                        /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (happens every 1 ms)
 * @param uint32_t dlyTicks
 *  Number of ticks to delay
 *****************************************************************************/
__INLINE static void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  FPGARegister_TypeDef *fpgareg = FPGARegister;

  SystemCoreClockUpdate();
  if (SysTick_Config(SystemCoreClock / 1000))   /* Setup SysTick Timer for 1 msec interrupts  */
  {
    while (1) ;                                 /* Capture error */
  }

  /* Initialize access to EFM32 DVK registers */
  EBI_init();
  while (1)
  {
    Delay(500);                    /* delay  0.5 sec */
    fpgareg->UIF_LEDS = 0xffff;    /* Enable "User LEDS" on DVK board */
    Delay(500);                    /* delay  0.5 Msec */
    fpgareg->UIF_LEDS = 0x0000;    /* Disable "User LEDS" on DVK board */
  }
}

