/*****************************************************************
 * File: FoodlineControl.c
 * Date: 2019/12/03 16:01
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "FoodlineControl.h"
#include "StateMachine.h"
// control
OS_MsgBoxHandle gControlSend_Queue;

OS_MsgBoxHandle gControlStartRe_Queue;
OS_MsgBoxHandle gControlStopRe_Queue;
OS_MsgBoxHandle gControlSendSeq_Queue;
OS_MsgBoxHandle gControlSendNotify_Queue;

static u8 gFlineCtrlSeq; // 序号，累加
extern bool NormalStopFlag;   //  位置  StateMachine.h

//static u32 retryTime = 1000;//400;   // *5ms重发间隔时间，根据路径中设备个数确定

#define DELAY_RESEND                    (1000)
static bool DeviceSendCmd(u8 areaIndex,u8 index, SEND_TYPE type);
static bool NextEnabled(u8 areaIndex,u8 index);
u32 timerCommunication;

#define  		MAX_SEQ_MAP_SIZE			5
typedef struct 
{
	u8 Seq[MAX_SEQ_MAP_SIZE];
	u8 SegIndex;
}ScmSeqMap;
static ScmSeqMap SeqMap;

void SeqMapInit(void)
{
	u8 i;
	for(i = 0; i < MAX_SEQ_MAP_SIZE; i ++)
	{
		SeqMap.Seq[i] = gFlineCtrlSeq - 0x80;
	}
	SeqMap.SegIndex = MAX_SEQ_MAP_SIZE;
}
void PutSeqInMap(u8 seq)
{
	if(++SeqMap.SegIndex >= MAX_SEQ_MAP_SIZE)
	{
		SeqMap.SegIndex = 0;
	}
	SeqMap.Seq[SeqMap.SegIndex] = seq;
}

bool IsSeqInMap(u8 seq)
{
	u8 i;

	for(i = 0;i < MAX_SEQ_MAP_SIZE; i++)
	{
		if(SeqMap.Seq[i] == seq)
		{
			return TRUE;
		}
	}
	return FALSE;
}

typedef struct
{
    DevicePara dPara[PATH_PER_INDEX];
    u8 index;
}ControlDevice;


static DeviceControlPara gDeviceControlPara;

DeviceControlPara *DeviceControlParaGet(void)
{
    return &gDeviceControlPara;
}


void ControlMsgBoxInit(void)
{
    // Control
    gControlSend_Queue = OS_MsgBoxCreate("CsndEvt", sizeof(INPUT_EVENT), CONTROL_MSGSND_COUNT);
    if (gControlSend_Queue == NULL)
        SYSTEMINIT_Debug("gControlSend_Queue NULL\n\r");
    
 
    
    gControlStartRe_Queue = OS_MsgBoxCreate("StartRe_Queue", sizeof(INPUT_EVENT), 12);
    if (gControlStartRe_Queue == NULL)
        SYSTEMINIT_Debug("gControlStartRe_Queue NULL\n\r");
    
    gControlStopRe_Queue = OS_MsgBoxCreate("StopRe_Queue", sizeof(INPUT_EVENT), 12);
    if (gControlStopRe_Queue == NULL)
        SYSTEMINIT_Debug("gControlStopRe_Queue NULL\n\r");
    
    gControlSendSeq_Queue = OS_MsgBoxCreate("SendSeq_Queue", sizeof(INPUT_EVENT), 12);
    if (gControlSendSeq_Queue == NULL)
        SYSTEMINIT_Debug("gControlSendSeq_Queue NULL\n\r");
    gControlSendNotify_Queue = OS_MsgBoxCreate("SendNotify_Queue", sizeof(INPUT_EVENT), 5);
    if (gControlSendNotify_Queue == NULL)
        SYSTEMINIT_Debug("gControlSendNotify_Queue NULL\n\r");
	OS_MemSet(&SeqMap,0x80,sizeof(ScmSeqMap));
	
}
void StateMachine_Task(void *Task_Parameters)
{

    StateMachineInit();

    while (1)
    {

        StateMachineProcess();
    }
}




bool SendTypeInquire(u8 areaIndex,u8 index)
{
    return DeviceSendCmd(areaIndex,index,SEND_TYPE_INQUIRE);
}
static bool DeviceSendCmd(u8 areaIndex, u8 index, SEND_TYPE type)
{
	INPUT_EVENT event;
	u8 indexBuf,place;
	u8 path;
	u8 seq, err, exit, cnt;
    u8 rcOvertimeNum = 0 ;
    u8 rcOvertimeCnt;

    DeviceControlParaGet()->rcDevice.id = AllTheControlParaGet(areaIndex,index)->cDevice.id;    //DeviceControlParaGet()->rcDevice.id

    place = index;
    path = PathTrav(AllTheControlParaGet(areaIndex,place)->cDevice.id);  //  判断是地几条路线
    rcOvertimeNum = IndexTrav(path,AllTheControlParaGet(areaIndex,place)->cDevice.id);
    rcOvertimeCnt = rcOvertimeNum;
    if(path == PATH_NUMBERS)
        return FALSE;
    indexBuf = 0;
    WirlessParaGet()->buffer[indexBuf++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[indexBuf++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[indexBuf++] = (AllTheControlParaGet(areaIndex,place)->cDevice.id>>8)&0xFF; 
    WirlessParaGet()->buffer[indexBuf++] = AllTheControlParaGet(areaIndex,place)->cDevice.id&0xFF;
    WirlessParaGet()->buffer[indexBuf++] = gFlineCtrlSeq;
    WirlessParaGet()->buffer[indexBuf++] = path;
    if(type == SEND_TYPE_INQUIRE)
    {
        WirlessParaGet()->buffer[indexBuf++] = DEVICE_CMD_FLINE_READ;
    }
    else if(type == SEND_TYPE_CONTROL_START)
    {
        WirlessParaGet()->buffer[indexBuf++] = DEVICE_CMD_FLINE_SET;
		if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_FOREWARD)  //如果是要求正转
			WirlessParaGet()->buffer[indexBuf++] = TOWERSOUT_CONTROL_FOREWARD;    //正转
		else  
			WirlessParaGet()->buffer[indexBuf++] = TOWERSOUT_CONTROL_REVERSAL;     //反转
    }
    else if(type == SEND_TYPE_CONTROL_STOP)
    {
        WirlessParaGet()->buffer[indexBuf++] = DEVICE_CMD_FLINE_SET;
        WirlessParaGet()->buffer[indexBuf++] = 0;   
    }

    if(AllTheControlParaGet(areaIndex,place)->cDevice.place.type == DEVICE_NAME_FLINE)
    {
        WirlessParaGet()->buffer[indexBuf++] = 0;   
        WirlessParaGet()->buffer[indexBuf++] = 0;   
        WirlessParaGet()->buffer[indexBuf++] = 0;   
    }
    else if(AllTheControlParaGet(areaIndex,place)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
    {
        WirlessParaGet()->buffer[indexBuf++] = 0;   
        WirlessParaGet()->buffer[indexBuf++] = 0;   
    }

    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = indexBuf;
    DeviceControlParaGet()->maxTime[0] = OSTimeGet();
    DeviceControlParaGet()->maxTime[4] = gFlineCtrlSeq-1;
//    seq = gFlineCtrlSeq-1;
    exit = 0;
    cnt = 0;
    PathParameterGet()->alarm.next = 0xAA;   
	SeqMapInit(); 
	while(OS_MsgBoxReceive(gControlSendNotify_Queue, &event, OS_NO_DELAY) == OS_NO_ERR);
    while (cnt++ < MAX_SEQ_MAP_SIZE)
    {
        seq = gFlineCtrlSeq++;
        WirlessParaGet()->buffer[4] = seq;
		PutSeqInMap(seq);
//        timerCommunication = OSTimeGet();
        ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);  // 发送时目标地址永远是第一个设备
        
        while (1)
        {
//            err = OS_MsgBoxReceive(gControlSendNotify_Queue, &event, 150*(index+1));
            err = OS_MsgBoxReceive(gControlSendNotify_Queue, &event, 60);
            if (err == OS_TIMEOUT)
            {
                if(PathParameterGet()->alarm.next == 0)
                {
                    if(rcOvertimeCnt)
                    {
                        rcOvertimeCnt--;
                        continue;
                    }
                    else
                    {
                        rcOvertimeCnt = rcOvertimeNum ;
                        PathParameterGet()->alarm.next = 0xAA;   
                        break;
                    }   
                }
                else
                {
					rcOvertimeCnt = rcOvertimeNum ;
                    PathParameterGet()->alarm.next = 0xAA; 
                    break;
                }
                
            }
            if ((err == OS_NO_ERR) && (event.Num == 1))
            {
                if (IsSeqInMap(event.Info.b[0]) == TRUE)   //   验证序列号是否一致
                {
                    exit = 1;
                    AllTheControlParaGet(areaIndex,index)->commTimes = 0;
                    AllTheControlParaGet(areaIndex,index)->isCommAlarm = FALSE;
                    break;                 
                }
				else 
				{
					err = err;
				}
            }
        }
        if (exit)
		{
			OSTimeDly(4);
            return TRUE;
		}
        if(AllTheControlParaGet(areaIndex,index)->isCommAlarm == TRUE)
            return FALSE;
        
    }
    if(AllTheControlParaGet(areaIndex,index)->commTimes < COMME_TIMEOUT)
        AllTheControlParaGet(areaIndex,index)->commTimes++;
    
    if(AllTheControlParaGet(areaIndex,index)->commTimes >= COMME_TIMEOUT)
        AllTheControlParaGet(areaIndex,index)->isCommAlarm = TRUE;

    
    return FALSE;
}


bool SingleDeviceStop(u8 areaIndex,u8 index)
{
    if(DeviceSendCmd(areaIndex,index, SEND_TYPE_CONTROL_STOP))
        return TRUE;
    else
        return FALSE;
}
bool DeviceStopStateInquire(u8 areaIndex,u8 index)
{
    if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
        ||AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
    {
        if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_CLOSE 
            || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_CLOSING
            )
        {                    
            return FALSE;
            
        }
        else
        {
            return TRUE;
        }
    }
    else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE)
    {
        if(AllTheControlParaGet(areaIndex,index)->stateByte != FOODLINE_PARA_CLOSE)
        {                    
            return TRUE;
        }
    }
    else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
    {	
        if(AllTheControlParaGet(areaIndex,index)->stateByte != TOWERSOUT_PARA_SOTP)
        {  
			if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_FOREWARD)   // 判断转向是否和要求的一致
			{
				if( AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSALING
				|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSAL)
				{
					return FALSE;
				}			
			}
			if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_REVERSAL)   // 判断转向是否和要求的一致
			{
				if( AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARDING
				|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARD)
				{
					return FALSE;
				}			
			}				
            return TRUE;
        }
    }
    return FALSE;
}
bool SingleDeviceControl(u8 areaIndex,u8 index)
{
    if(AllTheControlParaGet(areaIndex,index)->isSelect == TRUE)
    {
        if(DeviceSendCmd(areaIndex,index, SEND_TYPE_CONTROL_START))
            return TRUE;
    }
//    else
//    {
//        if(DeviceSendCmd(areaIndex,index, SEND_TYPE_CONTROL_STOP))
//            return TRUE;
//    }
    return FALSE;
}

// 对于料塔三通，绞龙，副料线的缺料满料都将视为不可控制
bool DeviceControlStateInquire(u8 areaIndex,u8 index)
{
    if(AllTheControlParaGet(areaIndex,index)->isSelect == TRUE)
    {
        if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS )      //  料塔三通
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                    
                return FALSE;
            }
            else
			{
				if(AllTheControlParaGet(areaIndex,index)->onoff.b.b0 == FOOD_UP_PLACE_OK)   //如果料塔满料则不能再进行开动作
					return FALSE;
				else
					return TRUE;
			}
        }
		else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)   // 主副闸三通
		{
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                    
                return FALSE;
            }
            else
                return TRUE;		
		}
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE)  //料线驱动
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte != FOODLINE_PARA_OPEN)
            {                    
                return TRUE;
            }
        }
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)   // 副料线驱动
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte != VICE_FOODLINE_PARA_OPEN)
            {
				if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE
					||AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)   // 如果满料或者关联料塔缺料则禁止打开
					return FALSE;
				else
					return TRUE;
            }
        }		
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)    //绞龙
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARD 
                || AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARDING)
            {                    			
				if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_REVERSAL)   //如果控制方向为反转，实际为正转，则停止当前料线
				{
					NormalStopFlag = TRUE ;  //  开始正常关闭。
					DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //标记需要关闭的区域				
				}				
                return FALSE;
            }
			else if( AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSALING
				|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSAL)
			{
				if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_FOREWARD)   //如果控制方向为正转，实际为反转，则停止当前料线
				{
					NormalStopFlag = TRUE ;  //  开始正常关闭。
					DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //标记需要关闭的区域				
				}				
                return FALSE;			
			}
            else
			{
				if(AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)     //如果关联料塔缺料则禁止打开
					return FALSE;
				else
					return TRUE; 
			}		
        }
    }
	else    // 如果没有选择，但控件是开启转态则关闭该控件
	{
        if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS )      //  料塔三通
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                    
                SingleDeviceStop(areaIndex,index);   //停止当前控件
            }
        }
		else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)   // 主副闸三通
		{
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                    
                SingleDeviceStop(areaIndex,index);   //停止当前控件
            }	
		}
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE)  //料线驱动
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == FOODLINE_PARA_OPEN)
            {                    
                SingleDeviceStop(areaIndex,index);   //停止当前控件	
            }
        }
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)   // 副料线驱动
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == VICE_FOODLINE_PARA_OPEN)
            {
				SingleDeviceStop(areaIndex,index);   //停止当前控件	
            }
        }		
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)    //绞龙
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARD 
                || AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARDING)
            {                    
                SingleDeviceStop(areaIndex,index);   //停止当前控件	
            }			
        }		
	}
    
    return FALSE;
}
/*******************************************************************************************************
函数名：continueControlJudge
功能：  根据查询回来的信息判断是否需要继续控制和查询，条件不符合就退出控制，进入自动停止状态
条件：所有料塔不满料也不缺料
	个别料塔缺料或满料的处理情况：1、缺料，将缺料信息赋给关联设备。
								2、满料，如果选中的料塔还有未满的，则关闭满料的控件，打料继续
										如果选中的料塔都满料了，则先打开所有选中的料塔，然后启动自动关闭程序。
**************************************************************************************************************/
// 判断继续控制的条件是否符合，也就是判断是否有缺料或者满料的情况
static void continueControlJudge(u8 areaIndex,u8 index)
{
	u8 foodOverNum = 0 ,foodLackFlag = 0,towersOutFoodLackNum;
	u8 towersOutNum = 0 ;   // 记录绞龙数量
	u16 towersOutPrevDeviceId = 0;   //绞龙关联设备ID
	u8 i,j;
	//料塔或副料线满料处理程序
	if(AllTheControlParaGet(areaIndex,index)->onoff.b.b0 == FOOD_UP_PLACE_OK
		&& AllTheControlParaGet(areaIndex,index)->isSelect == TRUE )   //检测上料位是否到位
	{
		//检测是否还有其他的三通未满
		//如果有则关闭这一个
		//如果没有则关闭所有
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS)    // 三通处理情况
		{
			for(i = 0;i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
			{
				if(AllTheControlParaGet(areaIndex,i)->onoff.b.b0 == FOOD_UP_PLACE_NO
					&& AllTheControlParaGet(areaIndex,i)->isSelect == TRUE)   //如果当前料线还有未满并选中的料塔
				{
					foodOverNum++;    //选中但未满的料塔数量
				}
			}
			if(foodOverNum != 0)   // 如果还有未满的料塔，则只关闭当前的料塔
			{
				SingleDeviceStop(areaIndex,index);   //停止当前三通	
			}
			else
			{
				NormalStopFlag = TRUE ;  //  开始正常关闭。
				DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //标记需要关闭的区域
			}
		}
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)    // 副料线处理情况
		{
			SingleDeviceStop(areaIndex,index);
			towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID;
			if(towersOutPrevDeviceId != 0)
			{
				for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
				{
					if( towersOutPrevDeviceId == AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID)    // 如果有关联						
					{
						//towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,i)->prevDevice.placeNew.useID;     //  索引下一个关联设备
						SingleDeviceStop(areaIndex,i);  // 关闭相关联的绞龙
					}
				}
			}
		}		
	}	
	//料塔缺料处理程序  如果缺料则将信息传给给关联设备
	if(AllTheControlParaGet(areaIndex,index)->onoff.b.b1 == FOOD_DOWN_PLACE_NO
		&& AllTheControlParaGet(areaIndex,index)->isSelect == TRUE )   //检测下料位是否缺料并且是否被选
	{
		//先判断是否有关联设备
		if(AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID != 0)
		{
			for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER - 1; j++)
			{
				for(i = 0;i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
				{
					if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID == AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID 
						&& AllTheControlParaGet(j,i)->isSelect == TRUE)   //如果检索到关联设备并且以选择，那么将信号赋给关联设备
					{
						AllTheControlParaGet(j,i)->prevGetAlarm = FOOD_TOWER_LACK_FOOD;    //将关联设备的相关位赋值。
						foodLackFlag = 1;
						break;
					}
				}
				if(foodLackFlag == 1)
					break;
			}
		}

	}
	// 与料塔关联的绞龙或副料线缺料
	if(AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)   //如果关联料塔的缺料
	{
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)//如果是绞龙，如果同组还有未缺料并选择的绞龙则停止当前设备，如果所选的绞龙全都缺料则正常停止
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
			{
				if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT    
					&& AllTheControlParaGet(areaIndex,i)->isSelect == TRUE)				//  选出当前组的被选的绞龙
				{
					towersOutNum++;    // 记录当前组的绞龙数量
					if(AllTheControlParaGet(areaIndex,i)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)  // 被选中的绞龙是否缺料   
					{
						towersOutFoodLackNum++;   //记录缺料的绞龙
					}
				}				
			}
			if(towersOutFoodLackNum == towersOutNum)  // 如果缺料的绞龙个该组被选的绞龙数一致则正常关打料程序
			{
				NormalStopFlag = TRUE;						
				DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //标记需要关闭的区域
			}
			else  // 如果只有一个绞龙缺料，则只关当前一个绞龙
			{
				SingleDeviceStop(areaIndex,index);
			}
		}	
		else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE) // 如果是副料线，则停止副料线，以及关联副驱动
		{
			SingleDeviceStop(areaIndex,index);
			towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID;
			if(towersOutPrevDeviceId != 0)
			{
				for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
				{
					if( towersOutPrevDeviceId == AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID)    // 如果有关联					
					{
						//towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,i)->prevDevice.placeNew.useID;     //  索引下一个关联设备
						SingleDeviceStop(areaIndex,i);  // 关闭相关联的绞龙
						break;
					}
				}
			}
		}

	}
}



void DeviceControlAndInquire(void)
{
    u8 err;
    INPUT_EVENT event;
    static u8 index = SING_LINK_DEVICE_TOTAL_NUMBER-1,  areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
   // static bool isFinish;
    static u8 i = 0,j = 0, k = 0;
	u8 gIndex = 0;
	static bool myselfMachineFlag = FALSE;
	u8 foodOverNum = 0;
	
    err = OS_MsgBoxReceive(gControlStartRe_Queue, &event, OS_NO_DELAY);
    if (err == OS_NO_ERR)
    {	
		// 得到需要控制的料线条数，以及相应料线中的设备数量，如果是分机那么只要控制
	
		while(DeviceControlParaGet()->controlArea[areaIndex] == FALSE)
		{
			areaIndex--;			
			if(areaIndex == 0)
			{
				areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
				break;
			}
			OSTimeDly(20);
		}
		while(AllTheControlParaGet(areaIndex,gIndex)->cDevice.placeNew.useID != 0) 
		{
			if(gIndex < (SING_LINK_DEVICE_TOTAL_NUMBER-1))
			{
				gIndex++;
			}
			else
			{
				gIndex = 0;
				break;
			}
		}	
		index = gIndex - 1;		
    }

	if(DeviceSendCmd(areaIndex ,index, SEND_TYPE_INQUIRE) == FALSE)    //查询状态，无论有没有完成控制功能都会先查询状态。  
		return;    //   加入通讯失败异常代码

	if(AllTheControlParaGet(areaIndex,index)->manualAuto != AUTO_GEARS)   //  不再自动状态则退出
	{
		//加入手动状态提示图标。
		DeviceControlParaGet()->isHaveAlarm = TRUE;
		index = SING_LINK_DEVICE_TOTAL_NUMBER - 1;
		areaIndex = areaIndex - 1;
		return;
	}  
	continueControlJudge(areaIndex,index);   //  检查料塔、副料线的缺料满料情况，并处理
	
    //  控制还未完成则继续控制
	if(DeviceControlStateInquire(areaIndex,index) == TRUE)    //如果此设备可以控制   设备是被选择并且是打开状态。
	{            
		if(SingleDeviceControl(areaIndex,index) == FALSE)   //  发送控制命令并加入超时异常代码    
			return  ;      
	}
		//  如果是主副闸三通阀，则不是开状态就不能进行下一个控件的操作    此判断不合理，有些料线主副闸的开合是用来决定不同料线的输料情况
	//if(NextEnabled(areaIndex,index))   
	{
		if(index == 0)
		{
			index = SING_LINK_DEVICE_TOTAL_NUMBER-1;
			while(1)
			{
				if(areaIndex > 0)
				{
					areaIndex -= 1;
				}
				else
				{
					//isFinish = TRUE;
					areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
					break;
				}																
				if(AllTheControlParaGet(areaIndex,0)->cDevice.placeNew.device != 0)          //  此处条件可换成判断是否是分机地址
				{
					if(AllTheControlParaGet(areaIndex,0)->cDevice.place.area == DEVICE_AREA_S || 
						AllTheControlParaGet(areaIndex,0)->cDevice.place.area == DEVICE_AREA_C  ||
						DeviceControlParaGet()->controlArea[areaIndex] == TRUE   )
						// 如果不是主机区域，其余的不再预览，节约资源。
					{
						while(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.device != 0)   // 得到当前料线的设备数量
						{
							if(i < (SING_LINK_DEVICE_TOTAL_NUMBER-1))
							{
								i++;
							}
							else
							{
								i = SING_LINK_DEVICE_TOTAL_NUMBER-1;
								break;
							}
						}
						index = i;
						DeviceControlParaGet()->controlArea[areaIndex] = FALSE ;  //当前区域控制完成，
						i = 0;
						break;
					}
				}			
			}
		}
		else
		{
			index -= 1;
		}    
	}            
    
}

static bool NextEnabled(u8 areaIndex,u8 index)
{
     if(AllTheControlParaGet(areaIndex,index)->isSelect == TRUE)
     {
         if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS )
           // || AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
         {
                if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN)
                    return TRUE ;
                else 
                    return FALSE ;
         }
     }
    return TRUE;
}
void DeviceStopAndInquire()
{
    u8 err;
    INPUT_EVENT event;
	RTC_TIME overTimer;
    static u8 index = 0,areaIndex = 0;
	   
    err = OS_MsgBoxReceive(gControlStartRe_Queue, &event, OS_NO_DELAY);
    if (err == OS_NO_ERR)
    {
        index = 0;
		areaIndex = 0;		
		while(DeviceControlParaGet()->controlStopArea[areaIndex] == FALSE)
		{
			areaIndex++;			
			if(areaIndex > AREA_DEVICE_TOTAL_NUMBER - 1)
			{
				areaIndex = 0;
				break;
			}
			OSTimeDly(20);
		}
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_CONTROL)
			index++;		
    }
    if(DeviceSendCmd(areaIndex,index, SEND_TYPE_INQUIRE) == FALSE)
    {           
        return; 
    }
	continueControlJudge(areaIndex,index);   //  检查料塔、副料线的缺料满料情况，并处理 
	// 延时处理功能需要更改	
    if(AllTheControlParaGet(areaIndex,index)->time)
    {        
		DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = TRUE;	// 将正在延时的料线标记好
		if(AllTheControlParaGet(areaIndex,index)->startStopTime == 0)
		{
			AllTheControlParaGet(areaIndex,index)->startStopTime = RTC_GetTime(&overTimer);
		}
        if(AllTheControlParaGet(areaIndex,index)->stateByte == FOODLINE_PARA_OPEN)   //  如果料线驱动是开状态则开始时间递减
        {
            if(DeviceControlParaGet()->isClickShutdown == TRUE)     //如果按下了急停件，则停止计时
            {
                AllTheControlParaGet(areaIndex,index)->time = 0;  
				AllTheControlParaGet(areaIndex,index)->startStopTime = 0;
				DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = FALSE;	// 将正在延时的料线取消标记
            }
			if(AllTheControlParaGet(areaIndex,index)->time > 0)
			{
				AllTheControlParaGet(areaIndex,index)->time -= (RTC_GetTime(&overTimer) - AllTheControlParaGet(areaIndex,index)->startStopTime) ;
			}
			else
			{
				AllTheControlParaGet(areaIndex,index)->startStopTime = 0;
				DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = FALSE;	// 将正在延时的料线取消标记
			}
        }

    }
	else
	{	  
		if(DeviceControlParaGet()->controlStopArea[areaIndex] == TRUE)   //如果相关区域没有收到停止命令则不判断是否需要停止。
		{
			if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type != DEVICE_NAME_VICE_FLINE)   // 不是副料线才处理
			{

				if(DeviceStopStateInquire(areaIndex,index) == TRUE)    // 检测是否需要停止
				{				
					if(SingleDeviceStop(areaIndex,index))      // 发送停止命令   发送完之后索引要向后移
					{				
						return ;
					}
				}
			}
		}
	}
    
	if(index < SING_LINK_DEVICE_TOTAL_NUMBER - 1)
	{
		index += 1;	
	}
	else
	{	
		index = 0;									 
	}    	
	if(AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID == 0)
	{
		DeviceControlParaGet()->controlStopArea[areaIndex] = FALSE;   //相关区域停机控制完成。
		if(areaIndex  < (AREA_DEVICE_TOTAL_NUMBER - 1))
		{
			areaIndex += 1;		
		}
		else
		{
			areaIndex = 0;					
		}		
		index = 0;
	}	             
   
    
}
// 强制关机
void DeviceShutdown(void)
{
    s8 i,j, stopTimer = 0;
    DeviceControlParaGet()->isHaveAlarm = TRUE;
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)
	{
		if(DeviceControlParaGet()->controlShutdownArea[j] == TRUE)//  只关闭被标记的区域
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
			{
				if(AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_FLINE
					||AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
				  DeviceSendCmd(j, i, SEND_TYPE_CONTROL_STOP);
			}
		}
	}
    OSTimeDly(100);
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)
	{
		if(DeviceControlParaGet()->controlShutdownArea[j] == TRUE)//  只关闭被标记的区域
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
			{ 
				if(DeviceSendCmd(j, i, SEND_TYPE_INQUIRE) == FALSE)
				{           
					return; 
				}   

				if(AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_FLINE)
				{
					if(AllTheControlParaGet(j,i)->cState != DEVICE_STATE_CLOSE)
					{                    
						DeviceSendCmd(j,i, SEND_TYPE_CONTROL_STOP);
						stopTimer++;
					}
				}
				else if(AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
				{
					if(AllTheControlParaGet(j,i)->cState != DEVICE_STATE_CLOSE)
					{                    
						DeviceSendCmd(j,i, SEND_TYPE_CONTROL_STOP);
						stopTimer++;
					}
				}
				
			}
		}		
	}	
    if(stopTimer == 0)
    {
       DeviceControlParaGet()->isHaveAlarm = FALSE; 
    }
}


