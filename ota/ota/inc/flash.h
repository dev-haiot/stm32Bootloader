#ifndef _FLASH_H
#define _FLASH_H

#include "stm32f10x.h"

/* API */
void Flash_Erase(uint32_t startAddr, uint32_t size);
void Flash_Write(uint32_t addr, uint8_t *data, uint16_t len);
void Flash_ReadBuffer(uint32_t addr, uint8_t *buf, uint16_t len);

#endif 
