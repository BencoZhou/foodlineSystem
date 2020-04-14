#include <stddef.h>
#include "Os_Wrap.h"
#include "Os_TaskPrio.h"
#include "wdg.h"
#include "ParaSettingLib.h"
#include "ParaHistory.h"
#include "FlineCtrl.h"


#if DISPATCHER_DBG
#define DISPATCHER_Debug    Debug
#else
#define DISPATCHER_Debug(x,y...)
#endif


OS_MsgBoxHandle gDispatchHandler_Queue;


void MZ_SYS_Init(void);

extern OS_MsgBoxHandle gDispatchHandler_Queue;

extern u32 gLocalDeviceId;
extern u32 gCtrlPlatformId;
static u32 gCmdTimeSent;
extern u32 gNineSensorId;
extern MZSYS_TASK_RECORD MZQsTaskRecord[];


void Dispatcher_Init(void)
{
    // dispatcher
    gDispatchHandler_Queue = OS_MsgBoxCreate("DptEvt", sizeof(INPUT_EVENT), 12);
    if (gDispatchHandler_Queue == NULL)
        DISPATCHER_Debug("gDispatchHandler_Queue NULL\n\r");
}


void Dispatcher_Task(void *Task_Parameters)
{
    INPUT_EVENT event;
    u8 err = err;
    
    // init server
    Dispatcher_Init();
    
    MZ_SYS_Init();
#if 1
    {
        u16 leftsize = QS_MonitorFragment();
        leftsize = leftsize;
    }
#endif
    while (1)
    {
        IWDG_Feed();
        gCmdTimeSent++;
        
        err = OS_MsgBoxReceive(gDispatchHandler_Queue, &event, 20);
        
        OS_TaskStkCheck();

//		if(gCmdTimeSent % 20 == 0)
//		{
//            u16 temp, i;
//            
//			ParaHistoryDataProcess(FALSE);
//            for (i=0;i<MZOS_TASK_RECORD_NUM;i++)
//            {
//                temp = MZQsTaskRecord[i].StkLeft;
//                temp = temp;
//            }
//		}

    }
}

