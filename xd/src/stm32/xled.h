#ifndef _X_LED_H
#define _X_LED_H

#include "xstm32.h"

typedef struct{
	GPIO_TypeDef*	port;
	uint16_t		pin;
}xled_t;

__inline void xled_init(const xled_t* led){
	GPIO_InitTypeDef GPIO_InitStructure;
	assert_param(led);
	RCC_APB2PeriphClockCmd(xstm32_gpio_to_rcc(led->port),ENABLE);
	GPIO_InitStructure.GPIO_Pin=led->pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(led->port,&GPIO_InitStructure);
}

__inline void xled_turn(const xled_t* led,const bool on){
	assert_param(led);
	GPIO_WriteBit(led->port,led->pin,on?Bit_RESET:Bit_SET);
}

__inline bool xled_is_on(const xled_t* led){
	assert_param(led);
	return (GPIO_ReadOutputDataBit(led->port,led->pin)?false:true);
}

__inline void xled_toggle(const xled_t* led){
	assert_param(led);
	xled_turn(led,xled_is_on(led)?false:true);
}


#endif //_X_LED_H
