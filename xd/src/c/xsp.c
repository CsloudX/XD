#include "xsp.h"
#include <string.h>
#include <stdlib.h>
#include "c/xbuf.h"

#define STX	0x02 // head -> ESC+0xE7
#define ETX	0x03 // tail -> ESC+0xE8
#define ESC	0x1B //		 -> ESC+0x00
//FRAME:
//[HEAD][ID][ATTR][CHK]<DATA>[TAIL]
// HEAD: STX
// ID: 0~255，相邻两帧不能一样
// ATTR:
//	bit0: 0-数据帧 1-应答帧
//	bit1: 是否需要应答（应答帧始终为0）
//	bit2: 1表示为重发帧
// CHK:CRC8校验
// TAIL: ESC
#define BIT_ACK_FRAME	0x01
#define BIT_NEED_ACK	0x02
#define BIT_RE_FRAME	0x04


#define XSP_SEND_BUF_SIZE	64
#define XSP_RECV_BUF_SIZE	64
static uint8_t send_buf[XSP_SEND_BUF_SIZE];
static uint8_t recv_buf[XSP_RECV_BUF_SIZE];
static int send_buf_idx;
static int recv_buf_idx;
static int get_recv_buf_idx;

static uint8_t send_id;
static uint8_t recv_id;

typedef struct{
	uint8_t id;
	uint8_t attr;
	uint8_t len;
	uint8_t chk;
	uint8_t data[20];
}xsp_data_t;
static xsp_data_t send_data;
static xsp_data_t recv_data;
static volatile bool data_send_ok;
static xsp_data_t ack_send_data;

X_DECL_BUF(xsp_data_t)
static xsp_data_t_xbuf_t recv_buf_data;

static uint8_t crc_table[16] = { 0x00, 0x9d, 0x23, 0xbe, 0x46, 0xdb, 0x65,
0xf8, 0x8c, 0x11, 0xaf, 0x32, 0xca, 0x57, 0xe9, 0x74 };

#define crc_calc(c)	\
	da=(uint8_t)(reg&0x0F);\
	reg>>=4;\
	reg^=crc_table[da^(c&0x0F)];\
	da=(uint8_t)(reg&0x0F);\
	reg>>=4;\
	reg^=crc_table[da^(c/16)];\


uint8_t xsp_check(const xsp_data_t* data)
{
	int i = 0;
	uint8_t reg = 0x00;
	uint8_t da = 0x00;

	crc_calc(data->id);
	crc_calc(data->attr);
	for (i = 0; i < data->len; ++i){
		crc_calc(data->data[i]);
	}
	return reg;
}

bool xsp_get_recv_buf_ch(uint8_t* ch)
{
	uint8_t next_ch = recv_buf[get_recv_buf_idx + 1];
	*ch = recv_buf[get_recv_buf_idx];
	if (*ch == ESC){
		switch (next_ch)
		{
		case 0xE7:
			*ch = STX;
			get_recv_buf_idx++;
			break;
		case 0xE8:
			*ch = ETX;
			get_recv_buf_idx++;
			break;
		case 0x00:
			*ch = ESC;
			get_recv_buf_idx++;
			break;
		default:
			break;
		}
	}

	if (get_recv_buf_idx > recv_buf_idx)
		return false;

	get_recv_buf_idx++;
	return true;
}


void xsp_fill_send_buf(uint8_t ch)
{
	switch (ch)
	{
	case STX:
		send_buf[send_buf_idx++] = ESC;
		send_buf[send_buf_idx++] = 0xE7;
		break;
	case ETX:
		send_buf[send_buf_idx++] = ESC;
		send_buf[send_buf_idx++] = 0xE8;
		break;
	case ESC:
		send_buf[send_buf_idx++] = ESC;
		send_buf[send_buf_idx++] = 0x00;
		break;
	default:
		send_buf[send_buf_idx++] = ch;
		break;
	}
}

bool xsp_send_xsp_data(xsp_data_t* data)
{
	int i = 0;
	send_buf_idx = 1;
	xsp_fill_send_buf(data->id);
	xsp_fill_send_buf(data->attr);
	xsp_fill_send_buf(data->len);
	xsp_fill_send_buf(data->chk);
	for (i = 0; i < data->len; i++)
	{
		xsp_fill_send_buf(data->data[i]);
	}
	send_buf[send_buf_idx++] = ETX;
	return xsp_comm_send(send_buf, send_buf_idx);
}


void xsp_parse_recv_buf()
{
	int i = 0;
	get_recv_buf_idx = 0;
	if (!xsp_get_recv_buf_ch(&recv_data.id))
		return;
	if (!xsp_get_recv_buf_ch(&recv_data.attr))
		return;
	if (!xsp_get_recv_buf_ch(&recv_data.len))
		return;
	if (!xsp_get_recv_buf_ch(&recv_data.chk))
		return;
	for (i = 0; i < recv_data.len; i++){
		if (!xsp_get_recv_buf_ch(&recv_data.data[i]))
			return;
	}

	if (recv_data.chk != xsp_check(&recv_data))
		return;

	if (recv_data.attr&BIT_ACK_FRAME){
		if (recv_data.id == (uint8_t)(send_id - 1))
			data_send_ok = true;
	}
	else{
		if (recv_data.attr&BIT_NEED_ACK){
			ack_send_data.id = recv_data.id;
			ack_send_data.attr = BIT_ACK_FRAME;
			ack_send_data.len = 0;
			ack_send_data.chk = xsp_check(&ack_send_data);
			xsp_send_xsp_data(&ack_send_data);
		}
		if ((recv_data.attr&BIT_RE_FRAME) && (recv_data.id == recv_id)){
		}else{
			recv_id = recv_data.id;
			if(xsp_data_t_xbuf_in(&recv_buf_data, &recv_data)==false){
				X_ASSERT(false);
			}
		}
	}
}

void xsp_feed(uint8_t ch)
{
	switch (ch)
	{
	case STX:
		recv_buf_idx = 0;
		break;
	case ETX:
		xsp_parse_recv_buf();
		recv_buf_idx = 0;
		break;
	default:
		recv_buf[recv_buf_idx++] = ch;
		if (recv_buf_idx > XSP_RECV_BUF_SIZE)
			recv_buf_idx = 0;
		break;
	}
}

bool _xsp_open(int buf_size)
{
	send_id = 0;
	recv_id = 0;
	send_buf[0] = STX;
	return xsp_data_t_xbuf_init(&recv_buf_data,buf_size);
}

void _xsp_close(void)
{
	xsp_data_t data;
	while (xsp_data_t_xbuf_out(&recv_buf_data, &data));
}

bool wait_send_data_ok(int timeout_ms)
{
	while (timeout_ms > 0){
		xsp_sleep(3);
		timeout_ms -= 3;
		if (data_send_ok)
			return true;
	}
	return false;
}

bool _xsp_send(const uint8_t* data, uint8_t len, bool pms)
{
	int i = 0;

	send_data.id = send_id;
	send_data.attr = 0x00;
	if (pms){
		send_data.attr |= BIT_NEED_ACK;
		++send_id;
	}
	send_data.len = len;
	memcpy(send_data.data, data, len);
	send_data.chk = xsp_check(&send_data);

	data_send_ok = false;
	if (!xsp_send_xsp_data(&send_data))
		return false;

	if (!pms)
		return true;

	if (wait_send_data_ok(1000))
		return true;

	send_data.attr |= BIT_RE_FRAME;
	send_data.chk = xsp_check(&send_data);

	for (i = 0; i < 3; i++){
		if (!xsp_send_xsp_data(&send_data))
			return false;
		if (wait_send_data_ok(1000))
			return true;
	}

	return false;
}

bool _xsp_recv(uint8_t buf[], uint8_t* len)
{
	xsp_data_t data;
	if (xsp_data_t_xbuf_out(&recv_buf_data, &data)){
		memcpy(buf, data.data, data.len);
		*len = data.len;
		return true;
	}
	return false;
}


