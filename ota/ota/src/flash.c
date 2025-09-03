#include "flash.h"

#define FLASH_PAGE_SIZE   1024
#define FLASH_BASE_ADDR   0x08000000
#define FLASH_TOTAL_SIZE  (64 * 1024)     // 64KB flash
#define APP_ADDRESS       0x08004000      // app start sau 16KB


/**
 * @brief  Xoa toan bo vung Application trong Flash
 * @note   Vung APP tinh tu APP_ADDRESS den het Flash	
 */
void Flash_Erase(uint32_t startAddr, uint32_t size)
{
	FLASH_Unlock();
	
	uint32_t endAddr = startAddr + size;
	if (endAddr > (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE))
	{
		endAddr = FLASH_BASE_ADDR + FLASH_TOTAL_SIZE;
	}
	for (uint32_t addr = startAddr; addr < endAddr; addr += FLASH_PAGE_SIZE)
	{
		FLASH_ErasePage(addr);
	}
	FLASH_Lock();
}

/**
 * @brief  Ghi du lieu vao Flash 
 * @param  addr: dia chi Flash can ghi (chan) 
 * @param  data: buffer chua du lieu
 * @param  len: so byte can ghi
 */
void Flash_Write(uint32_t addr, uint8_t *data, uint16_t len)
{
	if (addr % 2 != 0)
	{
		return;
	}
	FLASH_Unlock();
	
	for (uint16_t i = 0; i < len; i+= 2)
	{
		int b1 = data[i];
		int b2 = (i + 1 < len) ? data[i + 1] : 0xFF;
		
		uint16_t halfword = (uint16_t)(b1 | (b2 << 8));
		
		FLASH_Status status = FLASH_ProgramHalfWord(addr + i, halfword);
		if (status != FLASH_COMPLETE)
		{
			break;
		}
	}
	
	FLASH_Lock();
}

/**
 * @brief  Doc du lieu tu FLash vao buffer
 * @param  addr: dia chi Flash can ghi (chan) 
 * @param  data: buffer chua du lieu
 * @param  len: so byte can ghi
 */
void Flash_ReadBuffer(uint32_t addr, uint8_t *buf, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
	{
		buf[i] = *(volatile uint8_t *)(addr + i);
	}
}
