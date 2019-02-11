#ifndef _X_C_GOBAL_H
#define _X_C_GOBAL_H


//#if defined ( _MSC_VER )
//	#include <cassert>
//	#define X_ASSERT(cond)	assert(b)
//#elif defined ( __CC_ARM   )
//	#include "stm32f10x.h"
//	#define X_ASSERT(b)	assert_param(b)
//#elif defined ( __ICCARM__ )
////  #define __ASM           __asm                                       /*!< asm keyword for IAR Compiler          */
////  #define __INLINE        inline                                      /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */
//
//#elif defined   (  __GNUC__  )
////  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
////  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
//
//#elif defined   (  __TASKING__  )
////  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
////  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */
//
//#endif

#include <stdint.h>

typedef unsigned int	uint;
typedef unsigned char	uchar;

#define XLO8(n)			(uint8_t)((n)&0xFF)
#define XHI8(n)			(uint8_t)((n>>8)&0xFF)
#define XLO16(n)		(uint16_t)((n)&0xFFFF)
#define XHI16(n)		(uint16_t)((n>>16)&0xFFFF)
#define XMAKE16(h,l)	(uint16_t)(((l)&0xFF)|(((h)&0xFF)<<8))
#define XMAKE32(h,l)	(uint32_t)(((l)&0xFFFF)|(((h)&0xFFFF)<<16))


// useage: X_MACRO_EXPAND(MIN(1,2))
#define X_TOSTRING(str) #str
#define X_TOWSTRING(str) L#str
#define X_MACRO_EXPAND(macro)	X_TOSTRING(macro)
#define X_MACRO_EXPAND2(macro)	X_TOWSTRING(macro)

#define X_VERSION(major,minor,patch)	((major<<16)|(minor<<8)|(patch))
#define X_VERSION_MAJOR(version)    ((version>>16)&0xFF)
#define X_VERSION_MINOR(version)    ((version>>8)&0xFF)
#define X_VERSION_PATCH(version)    ((version)&0xFF)

#define X_ABS(n) ((n>=0)?n:-n)

#define X_SET_BIT(n,v)      ((n)|=(v))
#define X_UNSET_BIT(n,v)    ((n)&=(~(v)))
#define X_CHECK_BIT(n,v)    ((n)&(v))

#ifndef __cplusplus
typedef enum{false=0,true=!false} bool;
#define nullptr 0
#endif 

#define X_FREE(p)		if(p){free(p);p=nullptr;}




__inline int count_ones(int n)
{
	int ret = 0;
	while (n)
	{
		n &= (n - 1);
		++ret;
	}
	return ret;
}

#endif //_X_C_GOBAL_H

