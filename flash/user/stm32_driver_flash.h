#ifndef FLASH_H
#define FLASH_H

#include "stm32f10x_flash.h"            // Keil::Device:StdPeriph Drivers:Flash

void Flash_Write(uint16_t data, uint32_t page_addr);
uint16_t Flash_Read(uint32_t page_addr);

#endif // FLASH_H
