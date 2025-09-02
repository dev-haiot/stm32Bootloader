#include "stm32_driver_flash.h"

void Flash_Write(uint16_t data, uint32_t page_addr)
{
	FLASH_Unlock();
	FLASH_ErasePage(page_addr);
	FLASH_ProgramHalfWord(page_addr, data);
	FLASH_Lock();
}
uint16_t Flash_Read(uint32_t page_addr)
{
	return *(uint16_t*)page_addr;
}
