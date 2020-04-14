#ifndef MZSYS_OS_WRAP_H
#define MZSYS_OS_WRAP_H

#include "stm32f10x.h"
#include "ucos_ii.h"
#include "Debug.h"
#include "Os_TaskPrio.h"


typedef struct
{
    u8 Type;        // source type
    u8 Subtype;     // message type
    u16 Num;
    union
    {
        u8 b[4];
        u16 w[2];
        u32 dw;
    } Info;
} INPUT_EVENT;


#define BIGMSG_BLOCK_SIZ        128

typedef struct
{
    u8 Type;        // source type
    u8 Subtype;     // message type
    u16 Len;
    union
    {
        u8 b[4];
        u16 w[2];
        u32 dw;
    } Info;
    u8 buffer[BIGMSG_BLOCK_SIZ];
} BIGMSG_EVENT;



#define HEAP_SIZE_BYTE  (24*1024)

struct SysHighMem
{
    u32 dDummy;                 // for align
    u16 Heap[HEAP_SIZE_BYTE/2]; // mem
};


#define MZOS_TASK_RECORD_NUM    20
typedef struct
{
    const u8 *Name;
    void *Stk;
    u16 StkSize;
    u16 StkLeft;
    u8 Priority;
    u8 Overstep;    // 0: init  1: running
} MZSYS_TASK_RECORD;




//**************************** public function ****************************************
u8 IntToStr(u8 *str, u32 val);
u8 IpToStr(u8 *str, u32 ip);
u32 StrToInt(u8 *str);
int FloatToInt(float f);
void OS_MemSet(void *Dst, u8 C, u16 Byte);
void OS_MemCpy(void *Dst, const void *Src, u16 Byte);
void OS_MemCpy2(void *Dst, const void *Src, u16 Byte);
bool CompareBuf(u8 *Buf1, u8 *Buf2, u16 Len);

void DisplayBuf(const u8 *Buf, u16 Len, u8 RawLen);

u8 docrc8(u8 crc, u8 *p, u16 len);



//**************************** os ****************************************
void OS_WrapInit(u16 heapsize);
void OS_StartRun(void);


u8 OS_TaskCreate(TASK_FUNC TaskFunc, const u8 *TaskName, u16 StackSizeByte, void *pParam, u8 Priority, void **pTaskHandle);
void OS_TaskStkCheck(void);
u8 OS_TaskDelete(void *TaskHandle);
u8 OS_TaskSuspend(u8 Prio);
u8 OS_TaskResume(u8 Prio);
void OS_TaskDelay (u16 Ticks);
void OS_TaskDelayMs(u16 Ms);


OS_MsgBoxHandle OS_MsgBoxCreate(const u8 *Name, u16 ItemSize, u8 ItemNum);
u8 OS_MsgBoxSend(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks, bool IfPostFront);
u8 OS_MsgBoxReceive(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks);
u8 OS_MsgBoxSend_Ex(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks, bool IfPostFront);
u8 OS_MsgBoxReceive_Ex(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks);


OS_SemaphoreHandle OS_SemaphoreCreate(u8 Cnt);
u8 OS_SemaphoreTake(OS_SemaphoreHandle Sem, u16 WaitTicks);
u8 OS_SemaphoreGive(OS_SemaphoreHandle Sem);

OS_MutexHandler OS_MutexCreate(void);
u8 OS_MutexTake(OS_MutexHandler Mutex, u16 WaitTicks);
u8 OS_MutexGive(OS_MutexHandler Mutex);

u32 OS_GetCurrentTick(void);
u32 OS_GetCurrentSysMs(void);


//**************************** sys heap ****************************************
void DebugHeap(void);
void QS_HeapInit(void);
u16 QS_MonitorFragment(void);

void *QS_Mallco(u16 Size);
bool QS_Free(void *Ptr);


//**************************** sys macro define ****************************************
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
#define OS_IntEnter()       {CPU_SR cpu_sr;CPU_CRITICAL_ENTER();OSIntNesting++;CPU_CRITICAL_EXIT();}
#else
#define OS_IntEnter()       {CPU_CRITICAL_ENTER();OSIntNesting++;CPU_CRITICAL_EXIT();}
#endif

#define OS_IntExit()        OSIntExit()

#define OS_EnterCritical    OS_ENTER_CRITICAL
#define OS_ExitCritical     OS_EXIT_CRITICAL

#define OS_SchedLock        OSSchedLock
#define OS_SchedUnlock      OSSchedUnlock

#define OS_DebugHeap        DebugHeap
#define OS_HeapMonitor      QS_MonitorFragment

#define OS_Mallco           QS_Mallco
#define OS_Free             QS_Free

#endif
