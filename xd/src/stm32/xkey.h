#ifndef _X_KEY_H
#define _X_KEY_H

#include "xstm32.h"

typedef struct{
	GPIO_TypeDef*	port;
	uint16_t		pin;
}xkey_t;

__inline void xkey_init(const xkey_t* key){
	GPIO_InitTypeDef GPIO_InitStructure;
	assert_param(key);
	RCC_APB2PeriphClockCmd(xstm32_gpio_to_rcc(key->port),ENABLE);
	GPIO_InitStructure.GPIO_Pin = key->pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(key->port,&GPIO_InitStructure);
}

__inline bool xkey_is_pressed(const xkey_t* key){
	assert_param(key);
	return GPIO_ReadInputDataBit(key->port,key->pin)?false:true;
}

#endif // _X_KEY_H
