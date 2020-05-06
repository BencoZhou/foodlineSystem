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

#define CONT_ERR 	1
// control
OS_MsgBoxHandle gControlSend_Queue;

OS_MsgBoxHandle gControlStartRe_Queue;
OS_MsgBoxHandle gControlStopRe_Queue;
OS_MsgBoxHandle gControlSendSeq_Queue;
OS_MsgBoxHandle gControlSendNotify_Queue;

static u8 gFlineCtrlSeq; // ��ţ��ۼ�
extern bool NormalStopFlag;   //  λ��  StateMachine.h
static u8 startNum[AREA_DEVICE_TOTAL_NUMBER] = {0},stopNum[AREA_DEVICE_TOTAL_NUMBER] = {0}, selectNum[AREA_DEVICE_TOTAL_NUMBER] = {0};
static u8 deciceNum[AREA_DEVICE_TOTAL_NUMBER] = {0};

//static u32 retryTime = 1000;//400;   // *5ms�ط����ʱ�䣬����·�����豸����ȷ��

#define DELAY_RESEND                    (1000)
static bool DeviceSendCmd(u8 areaIndex,u8 index, SEND_TYPE type);
static bool NextEnabled(u8 areaIndex,u8 index);
static u8 indexAdministration(u8 *areaIndex,u8 *index);    // �������豸�������������ƶ��豸����
static u8 AreaIndexAdministration(u8 *areaIndex,u8 *index);  //���������������ڸ�������
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

    if(AllTheControlParaGet(areaIndex,place)->cDevice.place.type == DEVICE_NAME_FLINE
		|| AllTheControlParaGet(areaIndex,place)->cDevice.place.type ==DEVICE_NAME_VICE_FLINE)
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
    else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE
		|| AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)
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

	if(DeviceControlParaGet()->controlArea[areaIndex] == TRUE)
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
					if(AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)   // ������ϻ��߹�������ȱ�����ֹ��
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
				if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_FOREWARD)   //�����־Ϊ��ת��
				{
					if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARD    //ʵ��ҲΪ��ת����ͣ�����豸
						|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_FOREWARDING)
					{                    
						SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ�ؼ�	
					}			
				}
				else if(AllTheControlParaGet(areaIndex,index)->rotationDirection == TOWERSOUT_CONTROL_REVERSAL)   //�����־Ϊ��ת��
				{
					if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSAL    //ʵ��ҲΪ��ת����ͣ�����豸
						|| AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_REVERSALING)
					{                    
						SingleDeviceStop(areaIndex,index);   //ֹͣ��ǰ�ؼ�	
					}					
				}
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
	if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
		&&AllTheControlParaGet(areaIndex,index)->onoff.b.b0 == FOOD_UP_PLACE_OK
		&& AllTheControlParaGet(areaIndex,index)->isSelect == TRUE )   //�������λ�Ƿ�λ
	{
		//����Ƿ�����������ͨδ��
		//�������ر���һ��
		//���û����ر�����
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS)    // �������� �������
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
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)    // �����ߴ������   ����������ӣ���Ҫ����дһ������
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
	if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
		&&AllTheControlParaGet(areaIndex,index)->onoff.b.b1 == FOOD_DOWN_PLACE_NO
		&& AllTheControlParaGet(areaIndex,index)->isSelect == TRUE )   //�������λ�Ƿ�ȱ�ϲ����Ƿ�ѡ
	{
		//���ж��Ƿ��й����豸
		if(AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID != 0)
		{
			for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER - 1; j++)
			{
				for(i = 0;i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
				{
					if(AllTheControlParaGet(j,i)->prevDevice.placeNew.useID == AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID 
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
	// ����������������������ȱ��
	if((AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE 
		||AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
		&&AllTheControlParaGet(areaIndex,index)->prevGetAlarm == FOOD_TOWER_LACK_FOOD)   //�������������ȱ��
	{
		if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)//����ǽ��������ͬ�黹��δȱ�ϲ�ѡ��Ľ�����ֹͣ��ǰ�豸�������ѡ�Ľ���ȫ��ȱ��������ֹͣ
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER - 1; i++)
			{
				if(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0)
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
				else
					break;
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
//			towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,index)->prevDevice.placeNew.useID;
//			if(towersOutPrevDeviceId != 0)
//			{
				for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
				{
					if(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0)
					{    
						
						if( AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID == AllTheControlParaGet(areaIndex,i)->prevDevice.placeNew.useID)    // ����й���					
						{
//							towersOutPrevDeviceId = AllTheControlParaGet(areaIndex,i)->prevDevice.placeNew.useID;     //  ������һ�������豸
							SingleDeviceStop(areaIndex,i);  // �ر�������ĸ���������
							break;
						}
					}
				}
//			}
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
		else
			break;
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
        else if(AllTheControlParaGet(areaIndex,i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)    //����
        {
            if(AllTheControlParaGet(areaIndex,i)->stateByte == TOWERSOUT_PARA_FOREWARD 
                || AllTheControlParaGet(areaIndex,i)->stateByte == TOWERSOUT_PARA_FOREWARDING
				|| AllTheControlParaGet(areaIndex,i)->stateByte == TOWERSOUT_PARA_REVERSALING
				|| AllTheControlParaGet(areaIndex,i)->stateByte == TOWERSOUT_PARA_REVERSAL)
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
static u8 FirstIndexConfirm(u8 *areaIndex,u8 *index)
{
	u8 gIndex = 0 ,j = 0;
	static u8 prevAreaIndex = 0;   //��¼��һ�������Ƶ�����
	// �õ���Ҫ���Ƶ������������Լ���Ӧ�����е��豸����������Ƿֻ���ôֻҪ����
	while(DeviceControlParaGet()->controlArea[*areaIndex] == FALSE)
	{			
		j++;
		if(*areaIndex == 0)
		{
			*areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
		}
		(*areaIndex)--;	
		prevAreaIndex = *areaIndex ;
		if(j >= AREA_DEVICE_TOTAL_NUMBER)   //�����������û��������־���˳�����״̬��
		{
//			while(AllTheControlParaGet(*areaIndex,0)->cDevice.placeNew.useID == 0)
//			{
//				if(*areaIndex == 0)
//				{
//					*areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
//				}
//				(*areaIndex)--;
//			}
//			while(AllTheControlParaGet(*areaIndex,gIndex)->cDevice.placeNew.useID != 0) 
//			{
//				
//				if(gIndex < (SING_LINK_DEVICE_TOTAL_NUMBER-1))
//				{
//					gIndex++;
//				}
//				else
//				{
//					gIndex = 0;
//					break;
//				}
//			}	
//			
//			if(gIndex > 0)
//				*index = gIndex - 1;	
//			else
//				*index = SING_LINK_DEVICE_TOTAL_NUMBER;		
//			while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == 0)//����ͺ��Ǹ��յ���˵�����豸����Ҫ��ѯ����ƣ�
//			{
//				if(*index > 0)
//					*index -= 1;
//				else
//				{
//					break;
//				}
//				OSTimeDly(5);
//			}			
				
			DeviceControlParaGet()->isClickStart = FALSE;
			return  CONT_ERR;
		}
		OSTimeDly(20);
	}	
	if( (DeviceControlParaGet()->controlIndexMemory[*areaIndex] != 0)
		&&(AllTheControlParaGet(*areaIndex,(DeviceControlParaGet()->controlIndexMemory[*areaIndex]))->cDevice.place.type != DEVICE_NAME_CONTROL)
	    && (DeviceControlParaGet()->controlRestartFlag[(DEVICE_AREA_S - 1)] == FALSE))
	{		
		*index = DeviceControlParaGet()->controlIndexMemory[*areaIndex];   //����һ�ε�����λ�ÿ�ʼ�顣
	}
	else
	{
		DeviceControlParaGet()->controlRestartFlag[(DEVICE_AREA_S - 1)] = FALSE;
		
		while(AllTheControlParaGet(*areaIndex,gIndex)->cDevice.placeNew.useID != 0) 
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
		
		if(gIndex > 0)
			*index = gIndex - 1;	
		else
			*index = SING_LINK_DEVICE_TOTAL_NUMBER;
	}
	while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == 0)//����ͺ��Ǹ��յ���˵�����豸����Ҫ��ѯ����ƣ�
	{
		if(*index > 0)
			*index -= 1;
		else
		{
			//������һ������ѡ��
			AreaIndexAdministration(areaIndex,index);
			break;
		}
		OSTimeDly(5);
	}
	return 0;
}
void DeviceControlAndInquire(void)
{
    u8 err;
    INPUT_EVENT event;
    static u8 index = SING_LINK_DEVICE_TOTAL_NUMBER-1,  areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
    u8 i = 0;
	
	
    err = OS_MsgBoxReceive(gControlStartRe_Queue, &event, OS_NO_DELAY);
    if (err == OS_NO_ERR)
    {	
		// �õ���Ҫ���Ƶ������������Լ���Ӧ�����е��豸����������Ƿֻ���ôֻҪ����
		if(FirstIndexConfirm(&areaIndex,&index) == CONT_ERR)
			return ;
    }
	
	if(DeviceSendCmd(areaIndex ,index, SEND_TYPE_INQUIRE) == FALSE)    //��ѯ״̬��������û����ɿ��ƹ��ܶ����Ȳ�ѯ״̬��  
		return;    //   ����ͨѶʧ���쳣����
//	judgeControlArea(areaIndex,index);//�ж�ĳ��������豸�Ƿ�ȫ�������ɹ�������ǣ��򽫸�����Ŀ�����־λ��0
	if(AllTheControlParaGet(areaIndex,index)->manualAuto != AUTO_GEARS)   //  �����Զ�״̬�������һ������
	{
		//�����ֶ�״̬��ʾͼ�ꡣ
	//	DeviceControlParaGet()->isHaveAlarm = TRUE;
//		index = SING_LINK_DEVICE_TOTAL_NUMBER - 1;
//		areaIndex = areaIndex - 1;		
//		if(areaIndex > 0)
//		{
//			areaIndex -= 1;
//		}
//		else
//		{
//			areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
//		}	
		AreaIndexAdministration(&areaIndex,&index);    //ѡ����һ������
		OS_MsgBoxSend(gControlStartRe_Queue, &event, OS_NO_DELAY, FALSE);
		return;
	}  
	continueControlJudge(areaIndex,index);   //  ��������������ߵ�ȱ�����������������
	
    //  ���ƻ�δ������������
	if(DeviceControlStateInquire(areaIndex,index) == TRUE)    //������豸���Կ���   �豸�Ǳ�ѡ�����Ǵ�״̬��
	{          
		if(DeviceControlParaGet()->controlArea[areaIndex] == TRUE)
		{
			if(SingleDeviceControl(areaIndex,index) == FALSE)   //  ���Ϳ���������볬ʱ�쳣����    
				return  ;      
		}			
	}
		
	if(index == 0)
	{
		AreaIndexAdministration(&areaIndex,&index);    //ѡ����һ������
	}
	else
	{
		if(NextEnabled(areaIndex,index))     //�������ͨ������ѡ������ȴ�����λ���ܽ�����һ���豸�Ŀ���
		{
			indexAdministration(&areaIndex,&index);    //�������������������������ƶ�
			DeviceControlParaGet()->controlIndexMemory[areaIndex] = index;				
		}
		else
		{
			AreaIndexAdministration(&areaIndex,&index);    //ѡ����һ������
		}
	}  
	DeviceControlParaGet()->controlIndexMemory[areaIndex] = index;	
	    
    
}
static u8 AreaIndexAdministration(u8 *areaIndex,u8 *index)
{
	u8 i = 0, j =0;
	*index = SING_LINK_DEVICE_TOTAL_NUMBER-1;

	while(1)
	{
		j++;
		if(*areaIndex > 0)
		{
			*areaIndex -= 1;
		}
		else
		{
			*areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
		}																
		if(AllTheControlParaGet(*areaIndex,0)->cDevice.placeNew.useID != 0)          //  ֻ�ж����豸������
		{
			if(DeviceControlParaGet()->controlArea[*areaIndex] == TRUE )       //ֻ�жϱ�־Ϊ����������
			{
				while(AllTheControlParaGet(*areaIndex,i)->cDevice.placeNew.useID != 0)   // �õ���ǰ���ߵ��豸����
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
				*index = (i>0)?(i-1):0;
				while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == 0)
				{
					if(*index > 0)
						*index -= 1;				
					OSTimeDly(5);
				}					
				break;
			}
		}	
		if( j >= AREA_DEVICE_TOTAL_NUMBER)    // �����������û�б���־Ϊ����   ���˳�����״̬
		{
//			while(AllTheControlParaGet(*areaIndex,0)->cDevice.placeNew.useID == 0)   // �õ���ǰ���ߵ��豸����
//			{
//				if(*areaIndex > 0)
//				{
//					*areaIndex -= 1;
//				}
//				else
//				{
//					*areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
//				}		
//				OSTimeDly(5);
//			}
//			i = 0 ;
//			while(AllTheControlParaGet(*areaIndex,i)->cDevice.placeNew.useID != 0)   // �õ���ǰ���ߵ��豸����
//			{
//				if(i < (SING_LINK_DEVICE_TOTAL_NUMBER-1))
//				{
//					i++;
//				}
//				else
//				{
//					i = SING_LINK_DEVICE_TOTAL_NUMBER-1;
//					break;
//				}
//				OSTimeDly(5);
//			}			
//			*index = (i>0)?(i-1):0;
//			while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == 0)
//			{
//				if(*index > 0)
//					*index -= 1;				
//				OSTimeDly(5);
//			}		
			DeviceControlParaGet()->isClickStart = FALSE;			
			return CONT_ERR;			
		}
		OSTimeDly(5);
	}
	return 0 ;	
}
static u8 indexAdministration(u8 *areaIndex,u8 *index)
{
	u8 i = 0, j = 0;
	*index -= 1;
	if(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_CONTROL)  //�������������Ҫ�����ѯ����
	{
		if(*areaIndex == 0)
		{
			*areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
		}
		(*areaIndex)--;			
		while(DeviceControlParaGet()->controlArea[*areaIndex] == FALSE)
		{			
			j++;
			if(*areaIndex == 0)
			{
				*areaIndex = AREA_DEVICE_TOTAL_NUMBER - 1;
			}
			(*areaIndex)--;	
			if(j >= AREA_DEVICE_TOTAL_NUMBER)   //�����������û��������־���˳�����״̬��
			{
				DeviceControlParaGet()->isClickStart = FALSE;
				return  CONT_ERR;
			}
			OSTimeDly(20);
		}
		
		while(AllTheControlParaGet(*areaIndex,i)->cDevice.placeNew.useID != 0)   // �õ���ǰ���ߵ��豸����
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
		*index = (i>0)?(i-1):0;	   //���Ͽ���������				
		
		while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == 0)
		{
			if(*index > 0)
				*index -= 1;
			else
			{
				//������һ������ѡ��
				AreaIndexAdministration(areaIndex,index);
				break;
			}
			OSTimeDly(5);
		}				
		
		OSTimeDly(5);
						
	}
	else if(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == 0)  //����ͺ��Ǹ��յ���˵�����豸����Ҫ��ѯ����ƣ�
	{
		while((AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == 0)
			|| (AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_CONTROL))
		{
			if(*index > 0)
				*index -= 1;
			else
			{
				//������һ������ѡ��
				AreaIndexAdministration(areaIndex,index);
				break;
			}
			OSTimeDly(5);
		}
	}
	return 0;
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
	deciceNum[areaIndex] = 0;
	for(i = 0 ;i < SING_LINK_DEVICE_TOTAL_NUMBER-1; i++)
	{   
		//�ж��ж��ٸ��豸��ѡ��
		if(AllTheControlParaGet(areaIndex,i)->cDevice.placeNew.useID != 0
			&&AllTheControlParaGet(areaIndex,i)->cDevice.place.type != DEVICE_NAME_CONTROL)
		{
			deciceNum[areaIndex]++;
			if(AllTheControlParaGet(areaIndex,i)->isSelect == TRUE)
			{
				selectNum[areaIndex]++;				
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
						
		}
		OSTimeDly(5);
	}
	if(stopNum[areaIndex] == deciceNum[areaIndex])
	{
		DeviceControlParaGet()->controlStopArea[areaIndex] = FALSE ;  //��ǰ���������ɣ�
	}
}
static u8 FirstStopControlIndexConfirm(u8 *areaIndex,u8 *index)
{
	u8 areaIndexCountFirst = 0;		
	while(DeviceControlParaGet()->controlStopArea[*areaIndex] == FALSE)  // ֻѡ�񱻱��ֹͣ������
	{
		areaIndexCountFirst++;
		(*areaIndex)++;			
		if(*areaIndex > AREA_DEVICE_TOTAL_NUMBER - 1)
		{
			*areaIndex = 0;
//				break;
		}
		if(areaIndexCountFirst >= AREA_DEVICE_TOTAL_NUMBER)
		{
//			while(AllTheControlParaGet(*areaIndex,0)->cDevice.placeNew.useID == 0)
//			{
//				if(*areaIndex  < (AREA_DEVICE_TOTAL_NUMBER - 1))
//				{
//					*areaIndex += 1;		
//				}
//				else
//				{
//					*areaIndex = 0;					
//				}					
//			}
//			*index = 0;
//			if(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_CONTROL
//				||AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_NONE)
//			{
//				(*index)++;
//				if(*index > SING_LINK_DEVICE_TOTAL_NUMBER - 1)
//				{
//					*index = 0;
//				}			
//				while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_NONE)
//				{
//					if(AllTheControlParaGet(*areaIndex,*index)->cDevice.placeNew.useID != 0 )
//					{
//						(*index)++;
//						if(*index > SING_LINK_DEVICE_TOTAL_NUMBER - 1)
//						{
//							*index = 0;
//						}	
//					}
//					else
//						break;
//				}
//			}				
//			DeviceControlParaGet()->controlIndexMemory[*areaIndex] = *index ;
			DeviceControlParaGet()->isClickStop = FALSE;
			return  CONT_ERR;
		}
		OSTimeDly(5);
	}
	//����������һ���豸��Ӵ�������ʼ
	if( AllTheControlParaGet(*areaIndex,(DeviceControlParaGet()->controlIndexMemory[*areaIndex]))->cDevice.placeNew.useID != 0
		&& (DeviceControlParaGet()->controlRestartStopFlag[(DEVICE_AREA_S - 1)] == FALSE))
	{
		*index = DeviceControlParaGet()->controlIndexMemory[*areaIndex];   //����һ�ε�����λ�ÿ�ʼ�顣
		while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_NONE
			||AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_CONTROL)   // ������豸�ͺ�Ϊ�գ�����������
		{
			(*index)++;
			if(AllTheControlParaGet(*areaIndex,*index)->cDevice.placeNew.useID != 0)  //�����һ����豸Ϊ�գ���������λ��ʼλ��
			{
				*index = 0;
				while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_CONTROL
					||AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_NONE)
				{
					(*index)++;
				}	
			}
		}
	}
	else
	{	if(DeviceControlParaGet()->controlRestartStopFlag[(DEVICE_AREA_S - 1)] == TRUE)	
		{
			(*index) = 0;
			DeviceControlParaGet()->controlRestartStopFlag[(DEVICE_AREA_S - 1)] = FALSE;
		}
		while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_CONTROL
			||AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_NONE)
		{
			(*index)++;
			if(*index > SING_LINK_DEVICE_TOTAL_NUMBER - 1)
			{
				*index = 0;
			}			
//			while(AllTheControlParaGet(*areaIndex,*index)->cDevice.place.type == DEVICE_NAME_NONE)
//			{
//				if(AllTheControlParaGet(*areaIndex,*index)->cDevice.placeNew.useID != 0 )
//				{
//					(*index)++;
//					if(*index > SING_LINK_DEVICE_TOTAL_NUMBER - 1)
//					{
//						*index = 0;
//					}	
//				}
//			}
		}	
	}
	return 0;
}
void DeviceStopAndInquire()
{
    u8 err = 0;
	
    INPUT_EVENT event;
	RTC_TIME overTimer;
    static u8 index = 0,areaIndex = 0; 
	   
    err = OS_MsgBoxReceive(gControlStopRe_Queue, &event, OS_NO_DELAY);
    if (err == OS_NO_ERR)
    {
		if(FirstStopControlIndexConfirm(&areaIndex,&index) == CONT_ERR)	  //���û����Ҫ���Ƶ�����ֱ�ӷ���
			return ;
    }
    if(DeviceSendCmd(areaIndex,index, SEND_TYPE_INQUIRE) == FALSE)
    {           
        return; 
    }
//	judgeControlStopArea(areaIndex,index);//�ж�ĳ��������豸�Ƿ�ȫ���رճɹ�������ǣ��򽫸�����Ŀ�����־λ��0
	continueControlJudge(areaIndex,index);   //  ��������������ߵ�ȱ����������������� 
	// ��ʱ��������Ҫ����	
    if(AllTheControlParaGet(areaIndex,index)->time)
    {        
		DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = TRUE;	// ��������ʱ�����߱�Ǻ�
		DelayShutDown(areaIndex, index, overTimer);
		if(AllTheControlParaGet(areaIndex,index)->time == 0
			&& DeviceControlParaGet()->controlStopDelayFlag[areaIndex] == FALSE)  //�����ʱ��⵽ʱ���Ϊ0�ˣ�������رա�
		{
			if(SingleDeviceStop(areaIndex,index) == FALSE)      // ����ֹͣ����   ������֮������Ҫ�����
			{				
				return ;
			}
		}
    }
	else
	{	  
		if(DeviceControlParaGet()->controlStopArea[areaIndex] == TRUE)   //����������û���յ�ֹͣ�������ж��Ƿ���Ҫֹͣ��
		{
			if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type != DEVICE_NAME_VICE_FLINE)   // ���Ǹ����߲Ŵ���
			{

				if(DeviceStopStateInquire(areaIndex,index) == TRUE)    // ����Ƿ���Ҫֹͣ
				{			
					if(DeviceControlParaGet()->controlStopArea[areaIndex] == TRUE)		
					{						
						if(SingleDeviceStop(areaIndex,index) == FALSE)      // ����ֹͣ����   ������֮������Ҫ�����
						{				
							return ;
						}
					}
				}
			}
			else
			{
				if(AllTheControlParaGet(areaIndex,index)->stateByte != FOODLINE_PARA_CLOSE
					&&AllTheControlParaGet(areaIndex,index)->isSelect == FALSE)    // ���������û��ѡ�����ǿ�״̬��ر�
				{				
					if(SingleDeviceStop(areaIndex,index) == FALSE)      // ����ֹͣ����   ������֮������Ҫ�����
					{				
						return ;
					}
				}			
			}
		}
	}
    if(  AllTheControlParaGet(areaIndex,index)->time == 0
		|| DeviceControlParaGet()->controlStopArea[areaIndex] == FALSE	)
	{
		if(index < SING_LINK_DEVICE_TOTAL_NUMBER - 1)
		{
			index += 1;	
			while(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == 0)
			{
				if(AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID != 0 )
					index++;
				else
					break;
				OSTimeDly(5);
			}
		}
		else
		{	
			index = 0;									 
		}    
	}	
	if(AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID != 0 )
		DeviceControlParaGet()->controlIndexMemory[areaIndex] = index ;
	
	if(AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID == 0     //���ѭ������һ���豸��������������������������
		|| DeviceControlParaGet()->controlStopDelayFlag[areaIndex] == TRUE )  //���ѭ������һ���豸��������������������������
	{
		u8 areaIndexCount = 0;
		while(1)
		{			
			areaIndexCount++;
			if(areaIndex  < (AREA_DEVICE_TOTAL_NUMBER - 1))
			{
				areaIndex += 1;		
			}
			else
			{
				areaIndex = 0;					
			}
			if(AllTheControlParaGet(areaIndex,0)->cDevice.placeNew.useID != 0
				&&DeviceControlParaGet()->controlStopArea[areaIndex] == TRUE)
			{
				break;
			}
			if(areaIndexCount >= AREA_DEVICE_TOTAL_NUMBER)
			{
//				while(AllTheControlParaGet(areaIndex,0)->cDevice.placeNew.useID == 0)
//				{
//					if(areaIndex  < (AREA_DEVICE_TOTAL_NUMBER - 1))
//					{
//						areaIndex += 1;		
//					}
//					else
//					{
//						areaIndex = 0;					
//					}					
//				}
//				index = 0;
//				if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_CONTROL
//					||AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_NONE)
//				{	
//					index++;
//					while(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_NONE)
//					{
//						if(AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID != 0 )
//							index++;
//						else
//							break;
//						OSTimeDly(5);
//					}
//					
//				}	
//				DeviceControlParaGet()->controlIndexMemory[areaIndex] = index ;				
				DeviceControlParaGet()->isClickStop = FALSE;
				return ;
			}			
			OSTimeDly(5);
		}	
		if(DeviceControlParaGet()->controlStopDelayFlag[areaIndex] == TRUE)
		{
			 index = DeviceControlParaGet()->controlIndexMemory[areaIndex] ;
		}
		else
		{
			index = 0;
			if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_CONTROL
				||AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_NONE)
			{	
				index++;
				while(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_NONE)
				{
					if(AllTheControlParaGet(areaIndex,index)->cDevice.placeNew.useID != 0 )
						index++;
					else
						break;
					OSTimeDly(5);
				}
				
			}
		}
	}
	DeviceControlParaGet()->controlIndexMemory[areaIndex] = index ;
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
				AllTheControlParaGet(areaIndex,index)->startStopTime = 0;
				DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = FALSE;	// ��������ʱ������ȡ�����				
			}
		}
		else
		{
			AllTheControlParaGet(areaIndex,index)->startStopTime = 0;
			DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = FALSE;	// ��������ʱ������ȡ�����
		}
	}	
	else
	{
		AllTheControlParaGet(areaIndex,index)->time = 0;
		AllTheControlParaGet(areaIndex,index)->startStopTime = 0;
		DeviceControlParaGet()->controlStopDelayFlag[areaIndex] = FALSE;	// ��������ʱ������ȡ�����		
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
				if(AllTheControlParaGet(j,i)->cDevice.place.type != DEVICE_NAME_CONTROL && 
					AllTheControlParaGet(j,i)->cDevice.place.type != DEVICE_NAME_NONE)
				{
					if(AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_FLINE
						|| AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_VICE_FLINE
						||AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
					  DeviceSendCmd(j, i, SEND_TYPE_CONTROL_STOP);
				}
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
				if(AllTheControlParaGet(j,i)->cDevice.place.type != DEVICE_NAME_CONTROL  
					&& AllTheControlParaGet(j,i)->cDevice.place.type != DEVICE_NAME_NONE)
				{
					if(DeviceSendCmd(j, i, SEND_TYPE_INQUIRE) == FALSE)
					{           
						return; 
					} 
					if(AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_FLINE
						|| AllTheControlParaGet(j,i)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)
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
	}	
    if(stopTimer == 0)
    {
       DeviceControlParaGet()->isHaveAlarm = FALSE; 
    }
}


