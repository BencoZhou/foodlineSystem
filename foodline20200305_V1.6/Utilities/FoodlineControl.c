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

static u8 gFlineCtrlSeq; // ��ţ��ۼ�
extern bool NormalStopFlag;   //  λ��  StateMachine.h
static u8 startNum[AREA_DEVICE_TOTAL_NUMBER] = {0},stopNum[AREA_DEVICE_TOTAL_NUMBER] = {0}, selectNum[AREA_DEVICE_TOTAL_NUMBER] = {0};


//static u32 retryTime = 1000;//400;   // *5ms�ط����ʱ�䣬����·�����豸����ȷ��

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
    path = PathTrav(AllTheControlParaGet(areaIndex,place)->cDevice.id);  //  �ж��ǵؼ���·��
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
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT )
		{
			if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_FOREWARD)  //�����Ҫ����ת
				WirlessParaGet()->buffer[indexBuf++] = TOWERSOUT_CONTROL_FOREWARD;    //��ת
			else  
				WirlessParaGet()->buffer[indexBuf++] = TOWERSOUT_CONTROL_REVERSAL;     //��ת
		}
		else
		{
			WirlessParaGet()->buffer[indexBuf++] = 1;  
		}
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
        ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);  // ����ʱĿ���ַ��Զ�ǵ�һ���豸
        
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
                if (IsSeqInMap(event.Info.b[0]) == TRUE)   //   ��֤���к��Ƿ�һ��
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
			if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_FOREWARD)   // �ж�ת���Ƿ��Ҫ���һ��
			{
				if( AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSALING
				|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSAL)
				{
					return FALSE;
				}			
			}
			if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_REVERSAL)   // �ж�ת���Ƿ��Ҫ���һ��
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

// ����������ͨ�������������ߵ�ȱ�����϶�����Ϊ���ɿ���
bool DeviceControlStateInquire(u8 areaIndex,u8 index)
{

	
    if(AllTheControlParaGet(areaIndex,index)->isSelect == TRUE)
    { 
        if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS )      //  ������ͨ
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                 
                return FALSE;
            }
            else
			{
				if(AllTheControlParaGet(areaIndex,index)->onoff.b.b0 == FOOD_UP_PLACE_OK)   //����������������ٽ��п�����
					return FALSE;
				else
					return TRUE;
			}
        }
		else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)   // ����բ��ͨ
		{
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                    
                return FALSE;
            }
            else
                return TRUE;		
		}
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE)  //��������
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte != FOODLINE_PARA_OPEN)
            {                    
                return TRUE;
            }
        }
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)   // ����������
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte != VICE_FOODLINE_PARA_OPEN)
            {
				if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE
					||AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)   // ������ϻ��߹�������ȱ�����ֹ��
					return FALSE;
				else
					return TRUE;
            }
        }		
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)    //����
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARD 
                || AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARDING)
            {                    			
				if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_REVERSAL)   //������Ʒ���Ϊ��ת��ʵ��Ϊ��ת����ֹͣ��ǰ����
				{
					NormalStopFlag = TRUE ;  //  ��ʼ�����رա�
					DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //�����Ҫ�رյ�����				
				}				
                return FALSE;
            }
			else if( AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSALING
				|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSAL)
			{
				if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_FOREWARD)   //������Ʒ���Ϊ��ת��ʵ��Ϊ��ת����ֹͣ��ǰ����
				{
					NormalStopFlag = TRUE ;  //  ��ʼ�����رա�
					DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //�����Ҫ�رյ�����				
				}				
                return FALSE;			
			}
            else
			{
				if(AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)     //�����������ȱ�����ֹ��
					return FALSE;
				else
					return TRUE; 
			}		
        }
    }
	else    // ���û��ѡ�񣬵��ؼ��ǿ���ת̬��رոÿؼ�
	{
        if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS )      //  ������ͨ
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                    
                SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ�ؼ�
            }
        }
		else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)   // ����բ��ͨ
		{
            if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN 
                || AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPENING)
            {                    
                SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ�ؼ�
            }	
		}
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE)  //��������
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == FOODLINE_PARA_OPEN)
            {                    
                SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ�ؼ�	
            }
        }
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)   // ����������
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == VICE_FOODLINE_PARA_OPEN)
            {
				SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ�ؼ�	
            }
        }		
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)    //����
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARD 
                || AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARDING)
            {                    
                SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ�ؼ�	
            }			
        }		
	}
    
    return FALSE;
}
/*******************************************************************************************************
��������continueControlJudge
���ܣ�  ���ݲ�ѯ��������Ϣ�ж��Ƿ���Ҫ�������ƺͲ�ѯ�����������Ͼ��˳����ƣ������Զ�ֹͣ״̬
��������������������Ҳ��ȱ��
	��������ȱ�ϻ����ϵĴ��������1��ȱ�ϣ���ȱ����Ϣ���������豸��
								2�����ϣ����ѡ�е���������δ���ģ���ر����ϵĿؼ������ϼ���
										���ѡ�е������������ˣ����ȴ�����ѡ�е�������Ȼ�������Զ��رճ���
**************************************************************************************************************/
// �жϼ������Ƶ������Ƿ���ϣ�Ҳ�����ж��Ƿ���ȱ�ϻ������ϵ����
static void continueControlJudge(u8 areaIndex,u8 index)
{
	u8 foodOverNum = 0 ,foodLackFlag = 0,towersOutFoodLackNum;
	u8 towersOutNum = 0 ;   // ��¼��������
	u8 inTowersNum = 0;		//����������ͨ����
	u16 towersOutPrevDeviceId = 0;   //���������豸ID
	u8 i,j;
	//�������������ϴ������
	if(AllTheControlParaGet(areaIndex,index)->onoff.b.b0 == FOOD_UP_PLACE_OK
		&& AllTheControlParaGet(areaIndex,index)->isSelect == TRUE )   //�������λ�Ƿ�λ
	{
		//����Ƿ�����������ͨδ��
		//�������ر���һ��
		//���û����ر�����
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS)    // ��ͨ�������
		{
			for(i = 0;i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
			{
				if(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0) 
				{
					if( AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
						&& AllTheControlParaGet(areaIndex,i)->isSelect == TRUE												
						)   //�����ǰ���߻���δ����ѡ�е�����
					{
						inTowersNum++;
						if(AllTheControlParaGet(areaIndex,i)->onoff.b.b0 == FOOD_UP_PLACE_OK)
						foodOverNum++;    //ѡ�е�δ������������
					}
				}
			}
			if(foodOverNum < inTowersNum)   // �������δ������������ֻ�رյ�ǰ������
			{
				if(AllTheControlParaGet(areaIndex,index)->stateByte != SWITCH_VALVE_PARA_CLOSE)
				SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ��ͨ	
			}
			else
			{
				NormalStopFlag = TRUE ;  //  ��ʼ�����رա�
				DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //�����Ҫ�رյ�����
			}
		}
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)    // �����ߴ������
		{
			SingleDeviceStop(areaIndex,index);
			towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID;
			if(towersOutPrevDeviceId != 0)
			{
				for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
				{
					if( towersOutPrevDeviceId == AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID)    // ����й���						
					{
						//towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,i)->prevDevice.placeNew.useID;     //  ������һ�������豸
						SingleDeviceStop(areaIndex,i);  // �ر�������Ľ���
					}
				}
			}
		}		
	}	
	//����ȱ�ϴ������  ���ȱ������Ϣ�����������豸
	if(AllTheControlParaGet(areaIndex,index)->onoff.b.b1 == FOOD_DOWN_PLACE_NO
		&& AllTheControlParaGet(areaIndex,index)->isSelect == TRUE )   //�������λ�Ƿ�ȱ�ϲ����Ƿ�ѡ
	{
		//���ж��Ƿ��й����豸
		if(AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID != 0)
		{
			for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER - 1; j++)
			{
				for(i = 0;i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
				{
					if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID == AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID 
						&& AllTheControlParaGet(j,i)->isSelect == TRUE)   //��������������豸������ѡ����ô���źŸ��������豸
					{
						AllTheControlParaGet(j,i)->prevGetAlarm = FOOD_TOWER_LACK_FOOD;    //�������豸�����λ��ֵ��
						foodLackFlag = 1;
						break;
					}
				}
				if(foodLackFlag == 1)
					break;
			}
		}

	}
	// �����������Ľ���������ȱ��
	if(AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)   //�������������ȱ��
	{
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)//����ǽ��������ͬ�黹��δȱ�ϲ�ѡ��Ľ�����ֹͣ��ǰ�豸�������ѡ�Ľ���ȫ��ȱ��������ֹͣ
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
			{
				if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT    
					&& AllTheControlParaGet(areaIndex,i)->isSelect == TRUE)				//  ѡ����ǰ��ı�ѡ�Ľ���
				{
					towersOutNum++;    // ��¼��ǰ��Ľ�������
					if(AllTheControlParaGet(areaIndex,i)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)  // ��ѡ�еĽ����Ƿ�ȱ��   
					{
						towersOutFoodLackNum++;   //��¼ȱ�ϵĽ���
					}
				}				
			}
			if(towersOutFoodLackNum == towersOutNum)  // ���ȱ�ϵĽ��������鱻ѡ�Ľ�����һ���������ش��ϳ���
			{
				NormalStopFlag = TRUE;						
				DeviceControlParaGet()->controlStopArea[areaIndex] = TRUE;   //�����Ҫ�رյ�����
			}
			else  // ���ֻ��һ������ȱ�ϣ���ֻ�ص�ǰһ������
			{
				SingleDeviceStop(areaIndex,index);
			}
		}	
		else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE) // ����Ǹ����ߣ���ֹͣ�����ߣ��Լ�����������
		{
			SingleDeviceStop(areaIndex,index);
			towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID;
			if(towersOutPrevDeviceId != 0)
			{
				for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
				{
					if( towersOutPrevDeviceId == AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID)    // ����й���					
					{
						//towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,i)->prevDevice.placeNew.useID;     //  ������һ�������豸
						SingleDeviceStop(areaIndex,i);  // �ر�������Ľ���
						break;
					}
				}
			}
		}

	}
}

void judgeControlArea(u8 areaIndex,u8 index)//�ж�ĳ��������豸�Ƿ�ȫ�������ɹ�������ǣ��򽫸�����Ŀ�����־λ��0
{
	u8 i = 0, j = 0;
	selectNum[areaIndex] = 0;
	startNum[areaIndex] = 0;
	for(i = 0 ;i < SING_LINK_DEVICE_TOTAL_NUMBER-1; i++)
	{   
		//�ж��ж��ٸ��豸��ѡ��
		if(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0)
		{
			if(AllTheControlParaGet(areaIndex,i)->isSelect == TRUE)
			{
				selectNum[areaIndex]++;				
			}
		}
        if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS )      //  ������ͨ
        {
            if(AllTheControlParaGet(areaIndex,i)->stateByte == SWITCH_VALVE_PARA_OPEN )
            {                 
                startNum[areaIndex]++;
            }

        }
		else if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)   // ����բ��ͨ
		{
            if(AllTheControlParaGet(areaIndex,i)->stateByte == SWITCH_VALVE_PARA_OPEN )
            {                    
               startNum[areaIndex]++;
            }
		
		}
        else if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_FLINE)  //��������
        {
            if(AllTheControlParaGet(areaIndex,i)->stateByte == FOODLINE_PARA_OPEN)
            {                    
                startNum[areaIndex]++;
            }
        }
        else if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)   // ����������
        {
            if(AllTheControlParaGet(areaIndex,i)->stateByte == VICE_FOODLINE_PARA_OPEN)
            {
					startNum[areaIndex]++;
            }
        }		
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)    //����
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARD 
                || AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARDING
				|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSALING
				|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSAL)
            {                    						
                startNum[areaIndex]++;
            }         		
        }		
		OSTimeDly(5);
	}
	if(startNum[areaIndex] == selectNum[areaIndex])
	{
		DeviceControlParaGet()->controlArea[areaIndex] = FALSE ;  //��ǰ���������ɣ�
	}
}

void DeviceControlAndInquire(void)
{
    u8 err;
    INPUT_EVENT event;
    static u8 index = SING_LINK_DEVICE_TOTAL_NUMBER-1,  areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
   // static bool isFinish;
    u8 i = 0;
	u8 gIndex = 0;


	
    err = OS_MsgBoxReceive(gControlStartRe_Queue, &event, OS_NO_DELAY);
    if (err == OS_NO_ERR)
    {	
		// �õ���Ҫ���Ƶ������������Լ���Ӧ�����е��豸����������Ƿֻ���ôֻҪ����
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
	
	if(DeviceSendCmd(areaIndex ,index, SEND_TYPE_INQUIRE) == FALSE)    //��ѯ״̬��������û����ɿ��ƹ��ܶ����Ȳ�ѯ״̬��  
		return;    //   ����ͨѶʧ���쳣����
	judgeControlArea(areaIndex,index);//�ж�ĳ��������豸�Ƿ�ȫ�������ɹ�������ǣ��򽫸�����Ŀ�����־λ��0
	if(AllTheControlParaGet(areaIndex,index)->manualAuto != AUTO_GEARS)   //  �����Զ�״̬�������һ������
	{
		//�����ֶ�״̬��ʾͼ�ꡣ
	//	DeviceControlParaGet()->isHaveAlarm = TRUE;
		index = SING_LINK_DEVICE_TOTAL_NUMBER - 1;
		areaIndex = areaIndex - 1;
		OS_MsgBoxSend(gControlStartRe_Queue, &event, OS_NO_DELAY, FALSE);
		return;
	}  
	continueControlJudge(areaIndex,index);   //  ��������������ߵ�ȱ�����������������
	
    //  ���ƻ�δ������������
	if(DeviceControlStateInquire(areaIndex,index) == TRUE)    //������豸���Կ���   �豸�Ǳ�ѡ�����Ǵ�״̬��
	{            
		if(SingleDeviceControl(areaIndex,index) == FALSE)   //  ���Ϳ���������볬ʱ�쳣����    
			return  ;          
	}
		//  ���������բ��ͨ�������ǿ�״̬�Ͳ��ܽ�����һ���ؼ��Ĳ���,���û��ѡ������Խ�����һ���豸�Ĳ���

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
				areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
			}																
			if(AllTheControlParaGet(areaIndex,0)->cDevice.placeNew.useID != 0)          //  ֻ�ж����豸������
			{
				if(DeviceControlParaGet()->controlArea[areaIndex] == TRUE )
				{
					while(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0)   // �õ���ǰ���ߵ��豸����
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
						OSTimeDly(5);
					}
					index = (i>0)?(i-1):0;
					
					i = 0;
					break;
				}
			}	
			OSTimeDly(5);			
		}
	}
	else
	{
		if(NextEnabled(areaIndex,index))   
		{
			index -= 1;
			if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_CONTROL)  //�������������Ҫ�����ѯ����
			{
				if(areaIndex > 0)
					areaIndex -= 1;
				else
					areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
				while(1)
				{
					if(AllTheControlParaGet(areaIndex,0)->cDevice.placeNew.useID != 0)  
					{
						while(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0)   // �õ���ǰ���ߵ��豸����
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
						index = (i>0)?(i-1):0;	
						break;
					}
					else
					{
						if(areaIndex > 0)
							areaIndex -= 1;
						else
							areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;						
					}
					OSTimeDly(5);
				}				
			}
		}
	}  
		
	    
    
}

static bool NextEnabled(u8 areaIndex,u8 index)
{
     if(AllTheControlParaGet(areaIndex,index)->isSelect == TRUE)  
     {
         if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE )
           // || AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
         {
                if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_OPEN)
                    return TRUE ;
                else 
                    return FALSE ;
         }
     }
	 else
	 {
         if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE )
         {
                if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_CLOSE)
                    return TRUE ;
                else 
                    return FALSE ;
         }		
	 }
    return TRUE;
}


void judgeControlStopArea(u8 areaIndex,u8 index)//�ж�ĳ��������豸�Ƿ�ȫ�������ɹ�������ǣ��򽫸�����Ŀ�����־λ��0
{
	u8 i = 0, j = 0;
	selectNum[areaIndex] = 0;
	stopNum[areaIndex] = 0;
	for(i = 0 ;i < SING_LINK_DEVICE_TOTAL_NUMBER-1; i++)
	{   
		//�ж��ж��ٸ��豸��ѡ��
		if(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0)
		{
			if(AllTheControlParaGet(areaIndex,i)->isSelect == TRUE)
			{
				selectNum[areaIndex]++;				
			}
		}
        if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS )      //  ������ͨ
        {
            if(AllTheControlParaGet(areaIndex,i)->stateByte == SWITCH_VALVE_PARA_CLOSE )
            {                 
                stopNum[areaIndex]++;
            }

        }
		else if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)   // ����բ��ͨ
		{
            if(AllTheControlParaGet(areaIndex,i)->stateByte == SWITCH_VALVE_PARA_CLOSING )
            {                    
               stopNum[areaIndex]++;
            }
		
		}
        else if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_FLINE)  //��������
        {
            if(AllTheControlParaGet(areaIndex,i)->stateByte == FOODLINE_PARA_CLOSE)
            {                    
                stopNum[areaIndex]++;
            }
        }
        else if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)   // ����������
        {
            if(AllTheControlParaGet(areaIndex,i)->stateByte == VICE_FOODLINE_PARA_CLOSE)
            {
					stopNum[areaIndex]++;
            }
        }		
        else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)    //����
        {
            if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_SOTP )
            {                    						
                stopNum[areaIndex]++;
            }         		
        }		
		OSTimeDly(5);
	}
	if(stopNum[areaIndex] == selectNum[areaIndex])
	{
		DeviceControlParaGet()->controlStopArea[areaIndex] = FALSE ;  //��ǰ���������ɣ�
	}
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
	judgeControlStopArea(areaIndex,index);//�ж�ĳ��������豸�Ƿ�ȫ���رճɹ�������ǣ��򽫸�����Ŀ�����־λ��0
	continueControlJudge(areaIndex,index);   //  ��������������ߵ�ȱ����������������� 
	// ��ʱ��������Ҫ����	
    if(AllTheControlParaGet(areaIndex,index)->time)
    {        
		DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = TRUE;	// ��������ʱ�����߱�Ǻ�
//		DelayShutDown(areaIndex, index, overTimer);
    }
	else
	{	  
		if(DeviceControlParaGet()->controlStopArea[areaIndex] == TRUE)   //����������û���յ�ֹͣ�������ж��Ƿ���Ҫֹͣ��
		{
			if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type != DEVICE_NAME_VICE_FLINE)   // ���Ǹ����߲Ŵ���
			{

				if(DeviceStopStateInquire(areaIndex,index) == TRUE)    // ����Ƿ���Ҫֹͣ
				{				
					if(SingleDeviceStop(areaIndex,index))      // ����ֹͣ����   ������֮������Ҫ�����
					{				
						return ;
					}
				}
			}
		}
	}
    if( DeviceControlParaGet()->controlStopDelayFlag[areaIndex] == FALSE )
	{
		if(index < SING_LINK_DEVICE_TOTAL_NUMBER - 1)
		{
			index += 1;	
		}
		else
		{	
			index = 0;									 
		}    
	}	
	if(AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID == 0     //���ѭ������һ���豸��������������������������
		|| DeviceControlParaGet()->controlStopDelayFlag[areaIndex] == TRUE )  //���ѭ������һ���豸��������������������������
	{
		while(1)
		{
			if(areaIndex  < (AREA_DEVICE_TOTAL_NUMBER - 1))
			{
				areaIndex += 1;		
			}
			else
			{
				areaIndex = 0;					
			}
			if(AllTheControlParaGet(areaIndex,0)->cDevice.placeNew.useID != 0)
			{
				break;
			}
		}	
		index = 0;
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_CONTROL)
			index++;
	}	              
}

void DelayShutDown(u8 areaIndex,u8 index,RTC_TIME overTimer)
{
	u32 nowTime = 0;
	u16 timeQuantum = 0;
//	DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = TRUE;	// ��������ʱ�����߱�Ǻ�
	if(AllTheControlParaGet(areaIndex,index)->startStopTime == 0)
	{
		RTC_GetTime(&overTimer);
		AllTheControlParaGet(areaIndex,index)->startStopTime = (overTimer.hour * 3600 + overTimer.min * 60 + overTimer.sec);
	}
	if(AllTheControlParaGet(areaIndex,index)->stateByte == FOODLINE_PARA_OPEN)   //  ������������ǿ�״̬��ʼʱ��ݼ�
	{
		if(DeviceControlParaGet()->isClickShutdown == TRUE)     //��������˼�ͣ������ֹͣ��ʱ
		{
			AllTheControlParaGet(areaIndex,index)->time = 0;  
			AllTheControlParaGet(areaIndex,index)->startStopTime = 0;
			DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = FALSE;	// ��������ʱ������ȡ�����
		}
		if(AllTheControlParaGet(areaIndex,index)->time > 0)
		{
			RTC_GetTime(&overTimer);
			nowTime = (overTimer.hour * 3600 + overTimer.min * 60 + overTimer.sec);
			timeQuantum = (u16)(nowTime - AllTheControlParaGet(areaIndex,index)->startStopTime);
			if(timeQuantum <= AllTheControlParaGet(areaIndex,index)->time)
			{
				AllTheControlParaGet(areaIndex,index)->time -=  timeQuantum;
			}
			else
			{
				AllTheControlParaGet(areaIndex,index)->time = 0;
			}
		}
		else
		{
			AllTheControlParaGet(areaIndex,index)->startStopTime = 0;
			DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = FALSE;	// ��������ʱ������ȡ�����
		}
	}	
}
// ǿ�ƹػ�
void DeviceShutdown(void)
{
    s8 i,j, stopTimer = 0;
    DeviceControlParaGet()->isHaveAlarm = TRUE;
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)
	{
		if(DeviceControlParaGet()->controlShutdownArea[j] == TRUE)//  ֻ�رձ���ǵ�����
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
 		
		if(DeviceControlParaGet()->controlShutdownArea[j] == TRUE)//  ֻ�رձ���ǵ�����
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


