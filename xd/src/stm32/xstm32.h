#ifndef _X_STM32_H
#define _X_STM32_H

#include <stm32f10x.h>
#include "../c/xglobal.h"

#if defined (USE_RTX)
	#define delay_ms(n)	os_dly_wait(n)
#endif

typedef enum{NoRemap,PartialRemap,FullRemap}RemapType;


__inline uint32_t xstm32_gpio_to_rcc(GPIO_TypeDef* port){
	if(GPIOA==port)	return RCC_APB2Periph_GPIOA;
	if(GPIOB==port)	return RCC_APB2Periph_GPIOB;
	if(GPIOC==port)	return RCC_APB2Periph_GPIOC;
	if(GPIOD==port)	return RCC_APB2Periph_GPIOD;
	if(GPIOE==port)	return RCC_APB2Periph_GPIOE;
	if(GPIOF==port)	return RCC_APB2Periph_GPIOF;
	if(GPIOG==port)	return RCC_APB2Periph_GPIOG;
	assert_param(false);
	return 0;
}

#endif //_X_STM32_H
