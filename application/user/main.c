#include "stm32f10x.h"                  // Device header
#include "delay.h"

static void GPIO_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio = {0};
	gpio.GPIO_Pin = GPIO_Pin_15;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
}

int main()
{
	SystemInit();
	SCB->VTOR = 0x08004000;
	Delay_Init();
	GPIO_Config();
	
	uint32_t start = millis();
	
	while (1)
	{
		if (millis() - start > 500)
		{
			start = millis();
			GPIOC->ODR ^= GPIO_Pin_15;
		}
	}
	
}
