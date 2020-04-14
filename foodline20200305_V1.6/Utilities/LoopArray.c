#include "LoopArray.h"

void RHLoopArrayInit(RHLoopArray *rLoopArry, u8 *buff, u16 length)
{
    if(rLoopArry==NULL || length <= 0)
	{
		return;
	} 
    rLoopArry->head	= 0;
	rLoopArry->tail	= 0;
	rLoopArry->number	= 0;
	rLoopArry->length	= length;
	rLoopArry->buffer	= buff;
    rLoopArry->hasBeenInit = TRUE;

}

bool RHLoopArrayFull(RHLoopArray *rLoopArry)
{
    if((rLoopArry->hasBeenInit!=TRUE)||(rLoopArry==NULL))
	{
		return FALSE;
	}
    if(rLoopArry->number==rLoopArry->length)
	    return TRUE;
    else 
        return FALSE;
}

bool RHLoopArrayEmpty(RHLoopArray *rLoopArry)
{
    if((rLoopArry->hasBeenInit!=TRUE)||(rLoopArry==NULL))    
	{
		return FALSE;
	}

    if(rLoopArry->number==0)
        return TRUE;
    else
        return FALSE;
}

u16 RHLoopArrayGetNum(RHLoopArray *rLoopArry)
{
    u16 ret;

	if((rLoopArry->hasBeenInit!=TRUE)||(rLoopArry==NULL))
	{
	    return FALSE;
	}
	ret=rLoopArry->number;
    return ret;
}

bool RHLoopArrayClear(RHLoopArray *rLoopArry)
{
    if((rLoopArry->hasBeenInit!=TRUE)||(rLoopArry==NULL))
	{
		return FALSE;
	}
	rLoopArry->head	= 0;
    rLoopArry->tail	= 0;
    rLoopArry->number= 0;
	return TRUE;
}


bool RHLoopArrayIn(RHLoopArray *rLoopArry, u8 item)
{    
	if((rLoopArry->hasBeenInit!=TRUE)||(rLoopArry==NULL))
	{
		return FALSE;
	}
	if(rLoopArry->length < rLoopArry->number)
	{
		return FALSE;
	}

	rLoopArry->buffer[rLoopArry->tail] = item;
	rLoopArry->tail	= (rLoopArry->tail+1) % rLoopArry->length;
	rLoopArry->number++;

	// 新数据覆盖旧数据(保留新数据)
	if (rLoopArry->number == rLoopArry->length)
	{
		rLoopArry->head = (rLoopArry->head+1) % rLoopArry->length;
		rLoopArry->number--;
	}

	return TRUE;
}

u8 RHLoopArrayOut(RHLoopArray *rLoopArry)
{
	u16 tempLoopArryHead;
    if((rLoopArry->hasBeenInit!=TRUE)||(rLoopArry==NULL))
	{
		return FALSE;
	}
	if(rLoopArry->number == 0) 
	{
		return FALSE; 
	}

	tempLoopArryHead = rLoopArry->head;
	rLoopArry->head	 = (rLoopArry->head+1) % rLoopArry->length;
	rLoopArry->number--;

	return (rLoopArry->buffer[tempLoopArryHead]);
}

u8 RHLoopArrayPeek(RHLoopArray *rLoopArry, u16 index)
{
	if((rLoopArry->hasBeenInit!=TRUE)||(rLoopArry==NULL))
	{
		return FALSE;
	}
	if(index > rLoopArry->number) 
	{
		return FALSE; 
	}
	
	return (rLoopArry->buffer[(rLoopArry->head+index) % rLoopArry->length]);
}


