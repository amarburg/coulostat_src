;/*****************************************************************************
; * @file:    startup_TMPM330.s
; * @purpose: CMSIS Cortex-M3 Core Device Startup File for the
; *           TOSHIBA 'TMPM330' Device Series 
; * @version: V2.0.0
; * @date:    2009/10/14
; * 
; * THE SOURCE CODE AND ITS RELATED DOCUMENTATION IS PROVIDED "AS IS". TOSHIBA
; * CORPORATION MAKES NO OTHER WARRANTY OF ANY KIND, WHETHER EXPRESS, IMPLIED OR,
; * STATUTORY AND DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF MERCHANTABILITY,
; * SATISFACTORY QUALITY, NON INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
; * 
; * THE SOURCE CODE AND DOCUMENTATION MAY INCLUDE ERRORS. TOSHIBA CORPORATION
; * RESERVES THE RIGHT TO INCORPORATE MODIFICATIONS TO THE SOURCE CODE IN LATER
; * REVISIONS OF IT, AND TO MAKE IMPROVEMENTS OR CHANGES IN THE DOCUMENTATION OR
; * THE PRODUCTS OR TECHNOLOGIES DESCRIBED THEREIN AT ANY TIME.
; * 
; * TOSHIBA CORPORATION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
; * CONSEQUENTIAL DAMAGE OR LIABILITY ARISING FROM YOUR USE OF THE SOURCE CODE OR
; * ANY DOCUMENTATION, INCLUDING BUT NOT LIMITED TO, LOST REVENUES, DATA OR
; * PROFITS, DAMAGES OF ANY SPECIAL, INCIDENTAL OR CONSEQUENTIAL NATURE, PUNITIVE
; * DAMAGES, LOSS OF PROPERTY OR LOSS OF PROFITS ARISING OUT OF OR IN CONNECTION
; * WITH THIS AGREEMENT, OR BEING UNUSABLE, EVEN IF ADVISED OF THE POSSIBILITY OR
; * PROBABILITY OF SUCH DAMAGES AND WHETHER A CLAIM FOR SUCH DAMAGE IS BASED UPON
; * WARRANTY, CONTRACT, TORT, NEGLIGENCE OR OTHERWISE.
; * 
; * (C)Copyright TOSHIBA CORPORATION 2009 All rights reserved
; ******************************************************************************/
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; Cortex-M version
;

                MODULE  ?cstartup

                ;; Forward declaration of sections.
                SECTION CSTACK:DATA:NOROOT(3)

                SECTION .intvec:CODE:NOROOT(2)

                EXTERN  __iar_program_start
                EXTERN  SystemInit
                PUBLIC  __vector_table

                DATA
__vector_table  DCD     sfe(CSTACK)
        DCD     Reset_Handler

                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     INT0_IRQHandler           ; 0:  Interrupt pin (PJ0/70 pin)
                DCD     INT1_IRQHandler           ; 1:  Interrupt pin (PJ1/49 pin)
                DCD     INT2_IRQHandler           ; 2:  Interrupt pin (PJ2/86 pin)
                DCD     INT3_IRQHandler           ; 3:  Interrupt pin (PJ3/87 pin)
                DCD     INT4_IRQHandler           ; 4:  Interrupt pin (PG3/6  pin)
                DCD     INT5_IRQHandler           ; 5:  Interrupt pin (PF7/19 pin)
                DCD     INTRX0_IRQHandler         ; 6:  Serial reception (channel.0)
                DCD     INTTX0_IRQHandler         ; 7:  Serial transmission (channel.0)
                DCD     INTRX1_IRQHandler         ; 8:  Serial reception (channel.1)
                DCD     INTTX1_IRQHandler         ; 9:  Serial transmission (channel.1)
                DCD     INTSBI0_IRQHandler        ; 10: Serial bus interface 0
                DCD     INTSBI1_IRQHandler        ; 11: Serial bus interface 1
                DCD     INTCECRX_IRQHandler       ; 12: CEC reception
                DCD     INTCECTX_IRQHandler       ; 13: CEC transmission
                DCD     INTRMCRX0_IRQHandler      ; 14: Remote control signal reception (channel.0)
                DCD     INTADHP_IRQHandler        ; 15: Highest priority AD conversion complete interrupt
                DCD     INTADM0_IRQHandler        ; 16: AD conversion monitoring function interrupt 0
                DCD     INTADM1_IRQHandler        ; 17: AD conversion monitoring function interrupt 1
                DCD     INTTB0_IRQHandler         ; 18: 16bit TMRB match detection 0
                DCD     INTTB1_IRQHandler         ; 19: 16bit TMRB match detection 1
                DCD     INTTB2_IRQHandler         ; 20: 16bit TMRB match detection 2
                DCD     INTTB3_IRQHandler         ; 21: 16bit TMRB match detection 3
                DCD     INTTB4_IRQHandler         ; 22: 16bit TMRB match detection 4
                DCD     INTTB5_IRQHandler         ; 23: 16bit TMRB match detection 5
                DCD     INTTB6_IRQHandler         ; 24: 16bit TMRB match detection 6
                DCD     INTRTC_IRQHandler         ; 25: Real time clock timer
                DCD     INTCAP00_IRQHandler       ; 26: 16bit TMRB input capture 00
                DCD     INTCAP01_IRQHandler       ; 27: 16bit TMRB input capture 01
                DCD     INTCAP10_IRQHandler       ; 28: 16bit TMRB input capture 10
                DCD     INTCAP11_IRQHandler       ; 29: 16bit TMRB input capture 11
                DCD     INTCAP50_IRQHandler       ; 30: 16bit TMRB input capture 50
                DCD     INTCAP51_IRQHandler       ; 31: 16bit TMRB input capture 51
                DCD     INTCAP60_IRQHandler       ; 32: 16bit TMRB input capture 60
                DCD     INTCAP61_IRQHandler       ; 33: 16bit TMRB input capture 61
                DCD     INT6_IRQHandler           ; 34: Interrupt pin (PJ6/39 pin)
                DCD     INT7_IRQHandler           ; 35: Interrupt pin (PJ7/58 pin)
                DCD     INTRX2_IRQHandler         ; 36: Serial reception (channel.2)
                DCD     INTTX2_IRQHandler         ; 37: Serial transmission (channel.2)
                DCD     INTSBI2_IRQHandler        ; 38: Serial bus interface 2
                DCD     INTRMCRX1_IRQHandler      ; 39: Remote control signal reception (channel.1)
                DCD     INTTB7_IRQHandler         ; 40: 16bit TMRB match detection 7
                DCD     INTTB8_IRQHandler         ; 41: 16bit TMRB match detection 8
                DCD     INTTB9_IRQHandler         ; 42: 16bit TMRB match detection 9
                DCD     INTCAP20_IRQHandler       ; 43: 16bit TMRB input capture 20
                DCD     INTCAP21_IRQHandler       ; 44: 16bit TMRB input capture 21
                DCD     INTCAP30_IRQHandler       ; 45: 16bit TMRB input capture 30
                DCD     INTCAP31_IRQHandler       ; 46: 16bit TMRB input capture 31
                DCD     INTCAP40_IRQHandler       ; 47: 16bit TMRB input capture 40
                DCD     INTCAP41_IRQHandler       ; 48: 16bit TMRB input capture 41
                DCD     INTAD_IRQHandler          ; 49: A/D conversion completion

                THUMB
; Dummy Exception Handlers (infinite loops which can be modified)

                PUBWEAK Reset_Handler
                SECTION .text:CODE:REORDER(2)
Reset_Handler
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__iar_program_start
                BX      R0

                PUBWEAK NMI_Handler
                SECTION .text:CODE:REORDER(1)
NMI_Handler
                B       NMI_Handler

                PUBWEAK HardFault_Handler
                SECTION .text:CODE:REORDER(1)
HardFault_Handler
                B       HardFault_Handler

                PUBWEAK MemManage_Handler
                SECTION .text:CODE:REORDER(1)
MemManage_Handler
                B       MemManage_Handler

                PUBWEAK BusFault_Handler
                SECTION .text:CODE:REORDER(1)
BusFault_Handler
                B       BusFault_Handler

                PUBWEAK UsageFault_Handler
                SECTION .text:CODE:REORDER(1)
UsageFault_Handler
                B       UsageFault_Handler

                PUBWEAK SVC_Handler
                SECTION .text:CODE:REORDER(1)
SVC_Handler
                B       SVC_Handler

                PUBWEAK DebugMon_Handler
                SECTION .text:CODE:REORDER(1)
DebugMon_Handler
                B       DebugMon_Handler

                PUBWEAK PendSV_Handler
                SECTION .text:CODE:REORDER(1)
PendSV_Handler
                B       PendSV_Handler

                PUBWEAK SysTick_Handler
                SECTION .text:CODE:REORDER(1)
SysTick_Handler
                B       SysTick_Handler

                PUBWEAK INT0_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT0_IRQHandler
                B       INT0_IRQHandler

                PUBWEAK INT1_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT1_IRQHandler
                B       INT1_IRQHandler

                PUBWEAK INT2_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT2_IRQHandler
                B       INT2_IRQHandler

                PUBWEAK INT3_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT3_IRQHandler
                B       INT3_IRQHandler

                PUBWEAK INT4_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT4_IRQHandler
                B       INT4_IRQHandler

                PUBWEAK INT5_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT5_IRQHandler
                B       INT5_IRQHandler

                PUBWEAK INTRX0_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTRX0_IRQHandler
                B       INTRX0_IRQHandler

                PUBWEAK INTTX0_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTX0_IRQHandler
                B       INTTX0_IRQHandler

                PUBWEAK INTRX1_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTRX1_IRQHandler
                B       INTRX1_IRQHandler

                PUBWEAK INTTX1_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTX1_IRQHandler
                B       INTTX1_IRQHandler

                PUBWEAK INTSBI0_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTSBI0_IRQHandler
                B       INTSBI0_IRQHandler

                PUBWEAK INTSBI1_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTSBI1_IRQHandler
                B       INTSBI1_IRQHandler

                PUBWEAK INTCECRX_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCECRX_IRQHandler
                B       INTCECRX_IRQHandler

                PUBWEAK INTCECTX_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCECTX_IRQHandler
                B       INTCECTX_IRQHandler

                PUBWEAK INTRMCRX0_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTRMCRX0_IRQHandler
                B       INTRMCRX0_IRQHandler

                PUBWEAK INTADHP_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTADHP_IRQHandler
                B       INTADHP_IRQHandler

                PUBWEAK INTADM0_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTADM0_IRQHandler
                B       INTADM0_IRQHandler

                PUBWEAK INTADM1_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTADM1_IRQHandler
                B       INTADM1_IRQHandler

                PUBWEAK INTTB0_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB0_IRQHandler
                B       INTTB0_IRQHandler

                PUBWEAK INTTB1_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB1_IRQHandler
                B       INTTB1_IRQHandler

                PUBWEAK INTTB2_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB2_IRQHandler
                B       INTTB2_IRQHandler

                PUBWEAK INTTB3_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB3_IRQHandler
                B       INTTB3_IRQHandler

                PUBWEAK INTTB4_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB4_IRQHandler
                B       INTTB4_IRQHandler

                PUBWEAK INTTB5_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB5_IRQHandler
                B       INTTB5_IRQHandler

                PUBWEAK INTTB6_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB6_IRQHandler
                B       INTTB6_IRQHandler

                PUBWEAK INTRTC_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTRTC_IRQHandler
                B       INTRTC_IRQHandler

                PUBWEAK INTCAP00_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP00_IRQHandler
                B       INTCAP00_IRQHandler

                PUBWEAK INTCAP01_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP01_IRQHandler
                B       INTCAP01_IRQHandler

                PUBWEAK INTCAP10_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP10_IRQHandler
                B       INTCAP10_IRQHandler

                PUBWEAK INTCAP11_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP11_IRQHandler
                B       INTCAP11_IRQHandler

                PUBWEAK INTCAP50_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP50_IRQHandler
                B       INTCAP50_IRQHandler

                PUBWEAK INTCAP51_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP51_IRQHandler
                B       INTCAP51_IRQHandler

                PUBWEAK INTCAP60_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP60_IRQHandler
                B       INTCAP60_IRQHandler

                PUBWEAK INTCAP61_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP61_IRQHandler
                B       INTCAP61_IRQHandler

                PUBWEAK INT6_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT6_IRQHandler
                B       INT6_IRQHandler

                PUBWEAK INT7_IRQHandler
                SECTION .text:CODE:REORDER(1)
INT7_IRQHandler
                B       INT7_IRQHandler

                PUBWEAK INTRX2_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTRX2_IRQHandler
                B       INTRX2_IRQHandler

                PUBWEAK INTTX2_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTX2_IRQHandler
                B       INTTX2_IRQHandler

                PUBWEAK INTSBI2_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTSBI2_IRQHandler
                B       INTSBI2_IRQHandler

                PUBWEAK INTRMCRX1_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTRMCRX1_IRQHandler
                B       INTRMCRX1_IRQHandler

                PUBWEAK INTTB7_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB7_IRQHandler
                B       INTTB7_IRQHandler

                PUBWEAK INTTB8_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB8_IRQHandler
                B       INTTB8_IRQHandler

                PUBWEAK INTTB9_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTTB9_IRQHandler
                B       INTTB9_IRQHandler

                PUBWEAK INTCAP20_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP20_IRQHandler
                B       INTCAP20_IRQHandler

                PUBWEAK INTCAP21_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP21_IRQHandler
                B       INTCAP21_IRQHandler

                PUBWEAK INTCAP30_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP30_IRQHandler
                B       INTCAP30_IRQHandler

                PUBWEAK INTCAP31_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP31_IRQHandler
                B       INTCAP31_IRQHandler

                PUBWEAK INTCAP40_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP40_IRQHandler
                B       INTCAP40_IRQHandler

                PUBWEAK INTCAP41_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTCAP41_IRQHandler
                B       INTCAP41_IRQHandler

                PUBWEAK INTAD_IRQHandler
                SECTION .text:CODE:REORDER(1)
INTAD_IRQHandler
                B       INTAD_IRQHandler

                END
