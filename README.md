# stm32f103_triple_vcp
An example to show how to use triple CDC VCP USB interfaces. 
## Hardware configuration
MCU: STM32F103C8T6 running at 72MHz

Compiled with mdk  
Using High Speed Clock (HSE): 8Mhz Crystal/Ceramic Resonator  
USB Device with 6+1 endpoints (default , 3 in, 3 out, 3 control) , see usbdesc.c  

include a personal certificate and signed inf, tested on windows 7 and windows 10 without unsigned driver warning. 
where
USART1 shown as "stm32f103 vcp serial port 3" on windows device manager  
USART2 shown as "stm32f103 vcp serial port 1" on windows device manager  
USART3 shown as "stm32f103 vcp serial port 2" on windows device manager  


Tested up to 57600 baud without errors.  
At 115200 in tx-rx loop sending large chunk of data result in data loss; i will thank anyone that know and point out the problem.  

Pin configuration  
USB_DM: PA11  
USB_PM: PA12 must have a 1.5k pullup resistor to 3.3v  
Disconnection is done by setting PA12 to output and pulling PA12 down for approx. 2 seconds.  

USART1_TX: PA9  
USART1_RX: PA10  

USART2_TX: PA2  
USART2_RX: PA3  

USART2_TX: PA2  
USART2_RX: PA3  

USART3_TX: PB10  
USART3_RX: PB11  
