#ifndef _X_BUF_H
#define _X_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "xglobal.h"

#define X_DECL_BUF(T)	\
typedef struct{			\
	T* buf;				\
	int size;			\
	int64_t in_count;	\
	int64_t out_count;	\
}T##_xbuf_t;			\
bool T##_xbuf_init(T##_xbuf_t* xbuf, int size){	\
	xbuf->buf = (T*)malloc(sizeof(T)*size);		\
	xbuf->size = size;							\
	xbuf->in_count = 0;							\
	xbuf->out_count = 0;						\
	return (xbuf->buf ? true : false);			\
}												\
void T##_xbuf_release(T##_xbuf_t* xbuf){	\
	X_FREE(xbuf->buf)						\
	xbuf->size = 0;							\
	xbuf->in_count = 0;						\
	xbuf->out_count = 0;					\
}											\
bool T##_xbuf_in(T##_xbuf_t* xbuf,const T* data){	\
	int idx = xbuf->in_count%xbuf->size;	\
	if (xbuf->in_count - xbuf->out_count >= xbuf->size)	\
		return false;	\
	memcpy(&(xbuf->buf[idx]), data, sizeof(T));	\
	++(xbuf->in_count);	\
	return true;			\
}	\
bool T##_xbuf_out(T##_xbuf_t* xbuf,T* data){	\
	if (xbuf->out_count < xbuf->in_count){	\
		memcpy(data, &(xbuf->buf[(xbuf->out_count)++%xbuf->size]), sizeof(T));	\
		return true;	\
	}	\
	return false;	\
}	\


#ifdef __cplusplus
}
#endif

#endif // _X_BUF_H
