#ifndef __MAIN_H
#define __MAIN_H

/****************************************************************************************
 * Library
 ****************************************************************************************/
#include "stm32f10x.h"
#include "flash.h"
#include "delay.h"
#include "uart.h"
#include "jump.h"     

/*
=========================== BOOTLOADER FLOW ===========================

   +-------------------+
   |   Reset MCU       |
   +-------------------+
            |
            v
   +-------------------+
   |   SystemInit()    |  --> cau hinh clock, PLL
   +-------------------+
            |
            v
   +-------------------+
   |   Delay_Init()    |  --> khoi tao SysTick delay
   +-------------------+
            |
            v
   +-------------------+
   |   GPIOA_Init()    |  --> PA0 (LED), PA1 (nut bam)
   +-------------------+
            |
            v
   +-------------------+
   |   USART_Setup()   |  --> UART 115200bps
   +-------------------+
            |
            v
   +-------------------+
   |   While(1):       |
   |   - Nham nhay LED |
   |   - Kiem tra UART |
   +-------------------+
            |
            v
   Neu nhan duoc 'O' / 'o'
            |
            v
   +-------------------+
   | Jump_To_Application()
   +-------------------+

=========================== NHAY SANG APP ============================

Jump_To_Application():
   1. Doc MSP va Reset_Handler tu APP_ADDRESS
      - app_msp   = *(APP_ADDRESS)
      - app_reset = *(APP_ADDRESS + 4)

   2. Kiem tra app_msp co nam trong SRAM khong
      (bit cao phai bat dau bang 0x2000xxxx)

   3. Disable IRQ, reset SysTick, reset clock, reset peripheral

   4. Clear co loi trong SCB

   5. Chuyen Vector Table (SCB->VTOR) sang APP_ADDRESS

   6. Set MSP = app_msp

   7. Nhay vao Reset_Handler cua App:
         app_entry = (pFunction) app_reset
         app_entry()

=========================== KET THUC ===========================

Sau khi nhay, APP chay nhu binh thuong
Bootloader se KHONG tro lai nua
*/


typedef enum
{
	false = 0,
	true = !false
} bool;

#endif // __MAIN_H
