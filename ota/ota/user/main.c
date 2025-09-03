/**
  ******************************************************************************
  * @file    main.c
  * @author  haihbv
  * @date    3/9/2025
	* @brief	 bootloader
  ******************************************************************************
  */

#include "main.h"
#include <stdio.h>
#include <string.h>

/****************************************************************************************
 * Prototypes
 ****************************************************************************************/
 
#define APP_ADDRESS       0x08004000
#define STX 0x02
#define APP_SIZE_MAX (48*1024)

void Bootloader_Task(void);

int main(void)
{
	SystemInit();
	Delay_Init();
	USART_Setup(115200);
	printf("Bootloader started\r\n");
	
	uint8_t ch;
	if (USART_ReadByteTimeout(&ch, 3000))
	{
		if (ch == 'S')
		{
			char buf[6];
			buf[0] = 'S';
			for (int i = 1; i < 5; i++)
			{
				if (!USART_ReadByteTimeout((uint8_t*)&buf[i], 100))
				{
					return 0;
				}
			}
			buf[5] ='\0';
			if (strncmp(buf, "START", 5) == 0)
			{
				Bootloader_Task();
			}
		}			
	}
//	printf("No update request, jumping to App...\r\n");
//	Jump_To_Application();
}

void Bootloader_Task(void)
{
	printf("Enter firmware update mode\r\n");
	
	// Xoa vung APP
	Flash_Erase(APP_ADDRESS, APP_SIZE_MAX);
	USART_SendString("OK\n");
	
	uint32_t addr = APP_ADDRESS;
	uint8_t packet[260];
	
	while (1)
	{
		uint8_t ch;
		if (!USART_ReadByteTimeout(&ch, 5000))
		{
			continue;
		}
		
		if (ch == 'E')
		{
			USART_SendString("OK\r\n");
			printf("Update done, jumping to App...\r\n");
			Jump_To_Application();
		}
		
		if (ch == STX)
		{
			uint8_t seq, len, checksum;
			if (!USART_ReadByteTimeout(&seq, 100)) continue;
			if (!USART_ReadByteTimeout(&len, 100)) continue;
			
			for (int i = 0; i < len; i++)
			{
				if (!USART_ReadByteTimeout(&packet[i], 100)) 
				{
					continue;
				}
			}
			
			if (!USART_ReadByteTimeout(&checksum, 100))
			{
				continue;
			}
			
			// check checksum
			uint8_t calc = seq ^ len;
			for (int i = 0; i < len; i++)
			{
				calc ^= packet[i];
			}
			if (calc != checksum)
			{
				USART_SendString("ERR\n");
				continue;
			}
			
			// ghi flash
			Flash_Write(addr, packet, len);
			addr += len;
			USART_SendString("OK\n");
		}
	}
}
