# stm32f103_triple_vcp
An example to show how to use triple CDC VCP USB interfaces. 
## Hardware configuration
MCU: STM32F103C6T6 running at 72MHz

Compiled with Keil uVision
Using High Speed Clock (HSE): 8Mhz Crystal/Ceramic Resonator
USB Device with 6+1 endpoints (default , 3 in, 3 out, 3 control) , see usbdesc.c
USART1
USART2
USART3
Mode: Asynchronous
tested up to 57600 baud without errors.

Pin configuration
USB_DM: PA11
USB_PM: PA12 must have a 1.5k pullup resistor to 3.3v

USART1_TX: PA9
USART1_RX: PA10

USART2_TX: PA2
USART2_RX: PA3

USART2_TX: PA2
USART2_RX: PA3

USART3_TX: PB10
USART3_RX: PB11
