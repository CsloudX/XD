#include "xcan.h"
#include <string.h>
#include "../c/xbuf.h"

X_DECL_BUF(xcan_data_t)
static xcan_data_t_xbuf_t recv_buf1;

bool xcan_init(const xcan_t* can,int buf_size)
{
	GPIO_TypeDef*	GPIO_port;
	uint16_t		pin_rx,pin_tx;
	uint32_t		remap = 0;
	uint8_t			irqn = 0;
	GPIO_InitTypeDef	GPIO_InitStructure;
	CAN_InitTypeDef		CAN_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
	
	if(can->port==CAN1){
		if(xcan_data_t_xbuf_init(&recv_buf1,buf_size)==false){
			return false;
		}
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
		irqn = USB_LP_CAN1_RX0_IRQn;
		switch(can->remap){
			case NoRemap:
				GPIO_port=GPIOA;
				pin_rx=GPIO_Pin_11;
				pin_tx=GPIO_Pin_12;
				break;
			case PartialRemap:
				GPIO_port=GPIOB;
				pin_rx=GPIO_Pin_8;
				pin_tx=GPIO_Pin_9;
				remap = GPIO_Remap1_CAN1;
				break;
			case FullRemap:
				GPIO_port=GPIOD;
				pin_rx=GPIO_Pin_0;
				pin_tx=GPIO_Pin_1;
				remap = GPIO_Remap2_CAN1;
				break;
		}
	}else if(can->port==CAN2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
		GPIO_port=GPIOB;
		switch(can->remap){
			case NoRemap:
				pin_rx=GPIO_Pin_12;
				pin_tx=GPIO_Pin_13;
				break;
			case PartialRemap:
			case FullRemap:
				pin_rx=GPIO_Pin_5;
				pin_tx=GPIO_Pin_6;
				remap = GPIO_Remap_CAN2;
				break;
		}
	}else{assert_param(false);}
	
	RCC_APB2PeriphClockCmd(xstm32_gpio_to_rcc(GPIO_port),ENABLE);
	if(can->remap!=NoRemap){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
		GPIO_PinRemapConfig(remap,ENABLE);
	}
	
	GPIO_InitStructure.GPIO_Pin=pin_rx;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIO_port,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=pin_tx;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_port,&GPIO_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=irqn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_DeInit(can->port);
	CAN_StructInit(&CAN_InitStructure);
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=ENABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;
	//bsp = apb1hz/(sjw+bs1+bs2)/prescalar = 36M/(1+2+1)/9 = 1M
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1=CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_1tq;
	CAN_InitStructure.CAN_Prescaler=9;
	CAN_Init(can->port,&CAN_InitStructure);
	
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInitStructure.CAN_FilterIdHigh   = 0x00;
	CAN_FilterInitStructure.CAN_FilterIdLow   = 0x00;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  = 0x00;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow   = 0x00;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(can->port,CAN_IT_FMP0,ENABLE);
	
	return true;
}


void xcan_set_filter(const xcan_t* can,const uint8_t filter_num, const uint32_t id)
{
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = filter_num;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInitStructure.CAN_FilterIdHigh   = (((u32)id<<21)&0xffff0000)>>16;
	CAN_FilterInitStructure.CAN_FilterIdLow   = (((u32)id<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  = 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow   = 0xFFFF;
	CAN_FilterInit(&CAN_FilterInitStructure);
}

bool xcan_send(const xcan_t* can,const xcan_data_t* can_data)
{
	u8 mailbox = 0;
	int i=0;
	
	CanTxMsg 	txMsg;
	txMsg.StdId = can_data->id;
	txMsg.RTR=CAN_RTR_Data;
	txMsg.IDE = CAN_ID_STD;
	txMsg.DLC = 8;
	memcpy(txMsg.Data,can_data->data,8);
	mailbox = CAN_Transmit(can->port,&txMsg);
	if(CAN_TxStatus_NoMailBox==mailbox){
		return false;
	}
	for(i=0;i<10; i++){
		if(CAN_TransmitStatus(can->port, mailbox)==CANTXOK){
			return true;
		}
		delay_ms(1);
	}
	return false;
}
bool xcan_recv(const xcan_t* can,xcan_data_t* can_data)
{
	assert_param(can->port==CAN1);
	if(can->port==CAN1){
		return xcan_data_t_xbuf_out(&recv_buf1,can_data);
	}
	return false;
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg can_rx_msg;
	xcan_data_t can_data;
	CAN_Receive(CAN1, CAN_FIFO0, &can_rx_msg);
	can_data.id=can_rx_msg.StdId;
	memcpy(can_data.data,can_rx_msg.Data,8);
	if(xcan_data_t_xbuf_in(&recv_buf1,&can_data)==false){
		assert_param(false);
	}
}
