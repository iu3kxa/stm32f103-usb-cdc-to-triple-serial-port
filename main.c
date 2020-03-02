#include "main.h"
#include "stm32f10x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

int main(void)
{
	U32 dly;
	HardwareInit();
	
	//led
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIOC->CRH |= GPIO_CRH_MODE13_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;
	
	//disconnect usb
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIOA->CRH |=  0x02<<16 |  0x01<<18;		//PA12 output open drain to kill v_usb
	GPIOA->BSRR |= 1<<29;							//output to low
	for(dly=0;dly<20000000;dly++); 				//Approx 1 second delay
	GPIOC->ODR ^= 1<<13;								//toggle led 

	USB_Prepare();
	USB_Init();

	dly=0;
	while (1)
	{
		if (bDeviceState == CONFIGURED)
		{
			if (USB_USART1_Data.count_out != 0)
				USB_To_USARTx_Send_Data(&USB_USART1_Data);

			if (USB_USART2_Data.count_out != 0)
				USB_To_USARTx_Send_Data(&USB_USART2_Data);
			
			if (USB_USART3_Data.count_out != 0)
				USB_To_USARTx_Send_Data(&USB_USART3_Data);
		}
	}
}

int fputc(int ch, FILE *f)
{
    USART1->DR = (u8) ch;
    // Loop until the end of transmission
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
