#ifndef _X_CAN_H
#define _X_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "xstm32.h"

typedef struct{
	CAN_TypeDef* 	port;
	RemapType	remap;
}xcan_t;
	
typedef struct{
	uint32_t 	id;
	uint8_t 	data[8];
}xcan_data_t;


bool xcan_init(const xcan_t* can, int buf_size);
void xcan_set_filter(const xcan_t* can,const uint8_t filter_num, const uint32_t id);
bool xcan_send(const xcan_t* can,const xcan_data_t* can_data);
bool xcan_recv(const xcan_t* can,xcan_data_t* can_data);

#ifdef __cplusplus
}
#endif
	
#endif //_X_CAN_H
