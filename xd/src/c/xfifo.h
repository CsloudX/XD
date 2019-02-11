#ifndef _X_FIFO_H
#define _X_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "xglobal.h"


#define X_DECL_FIFO(T)	\
typedef struct T##_xfifo_node{		\
	T	data;						\
	struct T##_xfifo_node* next;	\
}T##_xfifo_node;					\
typedef struct{						\
	T##_xfifo_node* head_node;		\
	T##_xfifo_node* tail_node;		\
}T##_xfifo_t;						\
void T##_xfifo_init(T##_xfifo_t* fifo){	  \
	fifo->head_node = nullptr;			  \
	fifo->tail_node = nullptr;			  \
}										  \
bool T##_xfifo_in(T##_xfifo_t* fifo,const T* data){ \
	T##_xfifo_node* node = (T##_xfifo_node*)malloc(sizeof(T##_xfifo_node)); \
	if (node == nullptr)								 \
		return false;									 \
	node->data = *data;									 \
	node->next = nullptr;								 \
	if (fifo->tail_node) fifo->tail_node->next = node;	 \
	else fifo->head_node = node;						 \
	fifo->tail_node = node;								 \
	return true;										 \
}												   \
bool T##_xfifo_out(T##_xfifo_t* fifo, T* data){	   \
	T##_xfifo_node* node = fifo->head_node;		   \
	if (node == nullptr)						   \
		return false;							   \
	*data = node->data;							   \
	fifo->head_node = node->next;				   \
	if (fifo->head_node == nullptr)				   \
		fifo->tail_node = nullptr;				   \
	free(node);									   \
	return true;								   \
}												   \
int T##_xfifo_count(T##_xfifo_t* fifo){        \
	int count=0;                               \
	T##_xfifo_node* node = fifo->head_node;    \
	while(node->next){                         \
		++count;                               \
		node=node->next;                       \
	}                                          \
	return count;                              \
}                                              \


#ifdef __cplusplus
}
#endif


#endif //_X_FIFO_H
