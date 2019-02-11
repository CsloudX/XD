#ifndef _X_SP_H
#define _X_SP_H

#include "c/xglobal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	XSP_ERR_FIFO
}XSP_ERR_CODE;

bool xsp_open(int port,int buf_size);
void xsp_close(void);
bool xsp_send(const uint8_t* data, uint8_t len, bool pms);
bool xsp_recv(uint8_t buf[], uint8_t* len);
// when data arrived, you should call this
void xsp_feed(uint8_t ch);
void xsp_errored(int err_code);

// you should implement this follow functions
bool xsp_comm_send(const uint8_t* data, uint8_t len);
void xsp_sleep(int timeout_ms);


bool _xsp_open(int buf_size);
void _xsp_close(void);
bool _xsp_send(const uint8_t* data, uint8_t len, bool pms);
bool _xsp_recv(uint8_t buf[], uint8_t* len);

#ifdef __cplusplus
}
#endif
#endif //_X_SP_H
