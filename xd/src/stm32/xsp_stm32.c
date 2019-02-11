#include "../c/xsp.h"
#include <rtl.h>
#include "stm32f10x.h"
#include "stm32/xstm32.h"

static OS_MUT send_mtx;
static OS_MUT rev_buf_mtx;

static bool remap = true;

void xsp_sleep(int t_ms){os_dly_wait(t_ms);}


bool xsp_comm_send(const uint8_t* data, uint8_t len)
{
	int i=0;
	for(i=0;i<len;++i){
		USART1->DR=data[i];
		while(((USART1->SR)&0x40)==0);
	}
	return true;
}

bool xsp_open(int port_num,int buf_size)
{
	GPIO_TypeDef* port=0;
	uint16_t	pinTx=0;
    uint16_t    pinRx=0;
	uint8_t		irqn=0;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	if(_xsp_open(buf_size)==false){
		return false;
	}
	os_mut_init(&send_mtx);
	os_mut_init(&rev_buf_mtx);
	
	if(port_num==0){
		irqn=USART1_IRQn;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
        if(remap){
            port=GPIOB;
            pinTx=GPIO_Pin_6;
            pinRx=GPIO_Pin_7;
            //GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
        }else{
            port=GPIOA;
            pinTx=GPIO_Pin_9;
            pinRx=GPIO_Pin_10;
        }
	}
    RCC_APB2PeriphClockCmd(xstm32_gpio_to_rcc(port),ENABLE);
    if(remap){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
        if(port_num==0){
            GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
        }
    }
    
	GPIO_InitStructure.GPIO_Pin=pinTx;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(port,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=pinRx;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(port,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=irqn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	return true;
}

void xsp_close(void)
{
	_xsp_close();
}
#include <stdlib.h>
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		xsp_feed(USART1->DR);
	}
}

bool xsp_send(const uint8_t* data, uint8_t len, bool pms)
{
	bool retval=false;
	os_mut_wait(&send_mtx,0xFFFF);
	retval = _xsp_send(data, len, pms);
	os_mut_release(&send_mtx);
	return retval;
}
bool xsp_recv(uint8_t buf[], uint8_t* len)
{
	bool retval=false;
	os_mut_wait(&rev_buf_mtx,0xFFFF);
	retval = _xsp_recv(buf, len);
	os_mut_release(&rev_buf_mtx);
	return retval;
}

