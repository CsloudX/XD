#ifndef _X_SENSOR_H
#define _X_SENSOR_H

#include "xstm32.h"

typedef struct{
	GPIO_TypeDef*	port;
	uint16_t		pin;
}xsensor_t;

__inline void xsensor_init(const xsensor_t* sensor){
	GPIO_InitTypeDef GPIO_InitStructure;
	assert_param(sensor);
	RCC_APB2PeriphClockCmd(xstm32_gpio_to_rcc(sensor->port),ENABLE);
	GPIO_InitStructure.GPIO_Pin=sensor->pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(sensor->port,&GPIO_InitStructure);
}

__inline bool xsensor_status(const xsensor_t* sensor){
	assert_param(sensor);
	return GPIO_ReadInputDataBit(sensor->port,sensor->pin)?true:false;
}

#endif //_X_SENSOR_H
