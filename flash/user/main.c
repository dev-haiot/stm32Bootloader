#include "stm32f10x.h"                  // Device header
#include "stm32_driver_flash.h"

#define FLASH_USER_START_ADDR		((uint32_t)0x0801FC00)
#define TEST_DATA								(0x1234)

static uint32_t read_data;

int main()
{
	Flash_Write(TEST_DATA, FLASH_USER_START_ADDR);
	
	read_data = Flash_Read(FLASH_USER_START_ADDR);
	
	if (read_data == TEST_DATA)
	{
		read_data = 0xAAAA;
	}
	else
	{
		read_data = 0xFFFF;
	}
	
	while (1)
	{
		
	}
}
