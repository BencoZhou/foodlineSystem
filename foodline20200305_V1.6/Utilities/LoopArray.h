#ifndef _LOOPARRAY_H_
#define _LOOPARRAY_H_

#include "System.h"

typedef struct _RHLoopArray
{
	u16	head;
	u16	tail;
	u16	number;	//数组已填充个数
	u16	length;	//数组申请长度
	u8	*buffer;
	bool hasBeenInit;
}RHLoopArray; 


void RHLoopArrayInit(RHLoopArray *rLoopArry, u8 *bufer, u16 length);
bool RHLoopArrayIn(RHLoopArray *rLoopArry, u8 item);
u8   RHLoopArrayOut(RHLoopArray *rLoopArry);
u8   RHLoopArrayPeek(RHLoopArray *rLoopArry, u16 index);
bool RHLoopArrayEmpty(RHLoopArray *rLoopArry);
u16  RHLoopArrayGetNum(RHLoopArray *rLoopArry);
bool RHLoopArrayClear(RHLoopArray *rLoopArry);





#endif
