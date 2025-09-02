/**
  ******************************************************************************
  * @file    main.c
  * @author  haihbv
  * @date    3/9/2025
	* @brief	 bootloader
  ******************************************************************************
  */

#include "main.h"
#include <string.h>

/****************************************************************************************
 * Prototypes
 ****************************************************************************************/
void GPIOA_Init(void);
void Setup(void);
void Loop(void);
bool Button_Read(void);

int main(void)
{
	Setup();
	while (1)
	{
		Loop();
	}
}

/****************************************************************************************
 * API
 ****************************************************************************************/
void GPIOA_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/* PA0 - Output Push Pull */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/* PA1 - Input Pull Up */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Setup(void)
{
	SystemInit();
	Delay_Init();
	GPIOA_Init();
	USART_Setup(115200);
	
	// printf("Nhan nut '0' tren Bluetooth de nhay sang APP\n");
	printf("Nhan nut (PA1) hoac go '(boot)' nhay sang APP\n");
}
bool Button_Read(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}

void Loop(void)
{
	/* Blink LED de bao hieu bootloader dang chay */
	static uint32_t start = 0;
	if (millis() - start >= 100)
	{
		GPIOA->ODR ^= GPIO_Pin_0;
		start = millis();
	}
	
	/* Nhan nut PA1 -> nhay sang Application */
	if (!Button_Read()) // nhan nut
	{
		Jump_To_Application();
	}

	/* Khong dung scanf 
	if (USART_Available() != 0)
	{
		char c = USART_GetChar();
		if (c == '0')
		{
			printf("Dang nhay qua APP...\n");
      Delay_Ms(10);
			Jump_To_Application();
		}
	}
	*/
	
	if (USART_Available() != 0)
	{
		char cmd[16] = {0};
		scanf("%s", cmd);
		if (strcmp(cmd, "boot") == 0)
		{
			printf("Dang nhay qua APP...\r\n");
			Delay_Ms(10);
			Jump_To_Application();
		}
		else
		{
			printf("Lenh khong hop le\n");
			printf("Vui long go lenh 'boot' de nhay sang APP\n");
		}
	}
}
