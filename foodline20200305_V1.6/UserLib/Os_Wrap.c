#include <stddef.h>
#include "stm32f10x.h"
#include "Os_Wrap.h"


#if OS_WRAP_DBG
#define OS_Wrap_Debug   Debug
#else
#define OS_Wrap_Debug(x,y...)
#endif


#if OS_HEAP_DBG
#define Heap_Debug  Debug
#else
#define Heap_Debug(x,y...)
#endif


#define HEAP_UNIT_REWRITE_ID    0xa55a

#define HEAP_MINI_UNIT      4   // must be times of 4
#define HEAP_UNIT_OFFSET    2   // log2(HEAP_MINI_UNIT)

#define UNIT_MAX_NUM        (HEAP_SIZE_BYTE/HEAP_MINI_UNIT)

// mem block index
#define UnitIdxMask         0x7fff
#define GetUnitIdx(x)       (HeapRecord[x].UnitIdx & UnitIdxMask)
#define SetUnitIdx(x,Index) {HeapRecord[x].UnitIdx = ((HeapRecord[x].UnitIdx&(~UnitIdxMask))|((Index)&UnitIdxMask));}
#define GetUnitSize(x)      (GetUnitIdx(GetNext((x)))-GetUnitIdx((x)))


#define UsedMask            0x8000
#define UNUSE               0x0000
#define USED                0x8000
#define GetUsedFlag(x)      (HeapRecord[x].UnitIdx & (UsedMask))    // o is idle
#define SetUsedFlag(x,v)    {HeapRecord[x].UnitIdx = ((HeapRecord[x].UnitIdx&(~UsedMask))|(v?USED:UNUSE));}


#define GetNext(x)          (HeapRecord[x].Next)
#define SetNext(x,v)        {HeapRecord[x].Next = (v);}
#define GetPrev(x)          (HeapRecord[x].Prev)
#define SetPrev(x,v)        {HeapRecord[x].Prev = (v);}

#define CleanRecord(x)      {HeapRecord[x].Prev=HeapRecord[x].Next=HeapRecord[x].UnitIdx=0;}

#define SetRecdName(x,p)
#define SetRecdLines(x,s)
#define GetRecdName(x)
#define GetRecdLines(x)


#define Unit2Byte(U)        ((U)<<HEAP_UNIT_OFFSET)
#define Byte2Unit(B)        ((B)?((((B)-1)>>HEAP_UNIT_OFFSET)+1):0)

typedef struct 
{
    u8 Prev;
    u8 Next;
    u16 UnitIdx;
} HEAP_RECORD;

#define RECORD_START    0
#define RECORD_END      0xff
#define MAX_RECORD_NUM  256

static HEAP_RECORD HeapRecord[MAX_RECORD_NUM];


#define MSG_BOX_IDLE    0   // null
#define MSG_BOX_BUSY    1   // no null no full
#define MSG_BOX_FULL    2   // full


extern struct SysHighMem HMem;
extern u16 SysHMemLeftLen;

extern u8 MZOsTaskTotalNum;
extern MZSYS_TASK_RECORD MZQsTaskRecord[];


void OS_WrapInit(u16 heapsize)
{
    u16 i;
    
    SysHMemLeftLen = heapsize;
    MZOsTaskTotalNum = 0;
    
    for (i=0;i<MZOS_TASK_RECORD_NUM;i++)
    {
        MZQsTaskRecord[i].StkSize = 0;
        MZQsTaskRecord[i].StkLeft = 0;
        MZQsTaskRecord[i].Overstep = 0;
    }
    
    OSInit();
    QS_HeapInit();
}


void OS_StartRun(void)
{
    OSTimeSet(0);
    OSStart();
}


u8 OS_TaskCreate(TASK_FUNC TaskFunc, const u8 *TaskName, u16 StackSizeByte, void *pParam, u8 Priority, void **pTaskHandle)
{
    u8 error;
    void *ptr = OS_Mallco(StackSizeByte+4);
    
    OS_MemSet(ptr, 0x5a, StackSizeByte);
    
    error = OSTaskCreate(TaskFunc, pParam, (OS_STK *)(((((u32)ptr)+StackSizeByte+4)&0xfffffff8)-4), Priority);
    
    if (OS_ERR_NONE != error)
    {
        OS_Wrap_Debug("!!!Create Task error %d\n\r", error);
        OS_SchedLock();
        while (1);
    }
    
    OSTaskNameSet(Priority, (void *)TaskName, &error);
    
    MZQsTaskRecord[MZOsTaskTotalNum].Name = TaskName;
    MZQsTaskRecord[MZOsTaskTotalNum].Priority = Priority;
    MZQsTaskRecord[MZOsTaskTotalNum].Stk = ptr;
    MZQsTaskRecord[MZOsTaskTotalNum].StkSize = StackSizeByte;
    MZQsTaskRecord[MZOsTaskTotalNum++].StkLeft = StackSizeByte;
    Heap_Debug("Create Task %s @ STK 0x%x-0x%x\n\r", TaskName, (u32)ptr, (u32)ptr+StackSizeByte);
    
    if (pTaskHandle != NULL)
        *pTaskHandle = (void *)((u32)Priority);
    
    return error;
}


void OS_TaskStkCheck(void)
{
    u8 *p;
    u16 i, j;
    
    Heap_Debug("----------------------Task Record----------------------\n\r");
    for (i=0;i<MZOsTaskTotalNum;i++)
    {
        // *(u32 *)MZQsTaskRecord[i].Stk != 0x5a5a5a5a -> overflow
        // MZQsTaskRecord[i].Name, (u32)MZQsTaskRecord[i].Stk
        
        j = 0;
        p = (u8 *)MZQsTaskRecord[i].Stk;
        while (j < MZQsTaskRecord[i].StkSize)
        {
            if (*p != 0x5a)
                break;
            
            j ++;
            p ++;
        }
        
        if (j < MZQsTaskRecord[i].StkSize)
        {
            MZQsTaskRecord[i].StkLeft = j;
            Heap_Debug("Task %s, Stk 0x%x, Remain %d\n\r", MZQsTaskRecord[i].Name, MZQsTaskRecord[i].Stk, j);
        }
        else
        {
            MZQsTaskRecord[i].StkLeft = 0;
        }
    }
    Heap_Debug("----------------------Task Record----------------------\n\r");
}


u8 OS_TaskDelete(void *TaskHandle)
{
    if (TaskHandle == NULL)
        return OS_ERR_TASK_NOT_EXIST;
    return (OSTaskDel((u32)TaskHandle));
}


void OS_TaskDelay(u16 Ticks)
{
    OSTimeDly(Ticks);
}


void OS_TaskDelayMs(u16 Ms)
{
    OSTimeDly(Ms/OS_TICK_RATE_MS);
}


u8 OS_TaskSuspend(u8 Prio)
{
    return OSTaskSuspend(Prio);
}


u8 OS_TaskResume(u8 Prio)
{
    return OSTaskResume(Prio);
}


SYS_MSG_BOX_MEM *QMBMemCreat(u16 blksize, u8 nblks)
{
    SYS_MSG_BOX_MEM *pmem;
    void *addr;
    u8   *pblk;
    void **plink;
    u16  ablksize;
    u32  i;
    if (blksize == 0 || nblks == 0)
    {
        OS_Wrap_Debug("QMBMemCreat parameter error!blksize and nblks must >0\r\n");
        return NULL;
    }
    
    // ensure that start address can 4 bytes align for each mail
    ablksize = (((blksize-1)>>2)+1)<<2;
    addr = QS_Mallco(ablksize*nblks);
    if (addr == NULL)
    {
        OS_Wrap_Debug("QMBMemCreat can't get mem from heap to creat SYS_MSG_BOX_MEM\r\n");
        return NULL;
    }
    pmem = QS_Mallco(sizeof(SYS_MSG_BOX_MEM));
    if (pmem == NULL)
    {
        OS_Wrap_Debug("QMBMemCreat can't get mem from heap to creat SYS_MSG_BOX_MEM\r\n");
        QS_Free(addr);
        return NULL;
    }
    
    plink = (void **)addr;
    pblk = (u8 *)((u32)addr + ablksize);
    for (i=0;i<(nblks-1);i++)
    {
       *plink = (void *)pblk;
        plink = (void **)pblk;
        pblk = (u8 *)((u32)pblk + ablksize);
    }
    *plink = NULL;
    
    pmem->QMBMemAddr     = addr;            
    pmem->QMBMemFreeList = addr;            
    pmem->QMBMemNFree    = nblks;                     
    pmem->QMBMemBlks     = nblks;
    pmem->QMBMemBlkSize  = blksize; 
    return pmem;
}


void *QMBMemGet(SYS_MSG_BOX_MEM *pQMBMem)
{
    void *pblk;
    OS_CPU_SR cpu_sr = 0;
    if (pQMBMem == NULL) return NULL;
    OS_ENTER_CRITICAL();
    if (pQMBMem->QMBMemNFree > 0)
    {
        pblk = pQMBMem->QMBMemFreeList;
        pQMBMem->QMBMemFreeList = *(void **)pblk;
        pQMBMem->QMBMemNFree --;
        OS_EXIT_CRITICAL();
        return pblk;
    }
    else
    {
        OS_EXIT_CRITICAL();
        OS_Wrap_Debug("QMBMemGet can't get get free block\r\n");
        return NULL;
    }
}


u8 QMBMemPut(SYS_MSG_BOX_MEM *pQMBMem, void *pblk)
{
    OS_CPU_SR cpu_sr = 0;
    if (pQMBMem == NULL || pblk == NULL)
        return NULL;
    
    OS_ENTER_CRITICAL();
    if (pQMBMem->QMBMemNFree >= pQMBMem->QMBMemBlks)
    {
        OS_EXIT_CRITICAL();
        OS_Wrap_Debug("QMBMemPut can't put pblk,the pblk maybe illegal\r\n");
        return 0;
    }
    *(void **)pblk = pQMBMem->QMBMemFreeList;
    pQMBMem->QMBMemFreeList = pblk;
    pQMBMem->QMBMemNFree ++;
    OS_EXIT_CRITICAL();
    return 1;
}


OS_MsgBoxHandle OS_MsgBoxCreate(const u8 *Name, u16 ItemSize, u8 ItemNum)
{
    OS_MSG_BOX *pMsgBox;
    void *ptr;
    if (ItemNum == 0 || ItemSize == 0 || Name == NULL)
    {
        OS_Wrap_Debug("OS_MsgBoxCreate parameter error!blksize and nblks must >0\r\n");
        return NULL;
    }
    if ((pMsgBox = (OS_MSG_BOX *)QS_Mallco(sizeof(OS_MSG_BOX))) == NULL)
    {
        OS_Wrap_Debug("OS_MsgBoxCreate can't get mem from heap to creat OS_MSG_BOX\r\n");
        return NULL;
    }
    if ((pMsgBox->Mem = QMBMemCreat(ItemSize, ItemNum)) == NULL)
    {
        OS_Wrap_Debug("OS_MsgBoxCreate failed because QMBMemCreat failed\r\n");
        QS_Free(pMsgBox);
        return NULL;
    }
    if ((ptr = QS_Mallco(ItemNum*4)) == NULL)
    {
        OS_Wrap_Debug("OS_MsgBoxCreate can't get mem from heap to creat OSMsg\r\n");
        QS_Free(pMsgBox);
        return NULL;
    }
    if ((pMsgBox->Queue = OSQCreate(ptr, ItemNum)) == (OS_EVENT *)0)
    {
        OS_Wrap_Debug("OS_MsgBoxCreate failed because OSQCreate failed\r\n");
        QS_Free(pMsgBox);
        QS_Free(ptr);
        return NULL;
    }
    if ((pMsgBox->Sem = OSSemCreate(ItemNum)) == (OS_EVENT *)0)
    {
        OS_Wrap_Debug("OS_MsgBoxCreate failed because OSSemCreate failed\r\n");
        QS_Free(pMsgBox);
        QS_Free(ptr);
        return NULL;
    }
    pMsgBox->Name = Name;
    return pMsgBox;
}


u8 OS_MsgBoxSend(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks, bool IfPostFront)
{
    void *pTempMsg;
    u8 error;
    if (WaitTicks == OS_NO_DELAY)
    {
        if (OSSemAccept(pMsgBox->Sem) == NULL)
        {
            OS_Wrap_Debug("OS_MsgBoxSend failed when WaitTicks==OS_NO_DELAY\r\n");
            return OS_ERR_MBOX_FULL;
        }
        else
        {
            pTempMsg = QMBMemGet(pMsgBox->Mem);
            if (pTempMsg == NULL)
                return OS_ERR_MBOX_FULL;
            
            OS_MemCpy(pTempMsg, Msg, pMsgBox->Mem->QMBMemBlkSize);
            if (IfPostFront == FALSE)
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_NONE);
            else
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_FRONT);
            
            return OS_NO_ERR;
        }
    }
    else
    {
        if (OSIntNesting > 0)
        {
            OS_Wrap_Debug("in ISR OS_MsgBoxSend's WaitTicks must be OS_NO_DELAY\r\n");
            return OS_ERR_MBOX_FULL;
        }
        OSSemPend(pMsgBox->Sem, WaitTicks, &error);
        if (error != OS_ERR_NONE)
        {
            return OS_ERR_MBOX_FULL;
        }
        else
        {
            pTempMsg = QMBMemGet(pMsgBox->Mem);
            if (pTempMsg == NULL)
                return OS_ERR_MBOX_FULL;
            
            OS_MemCpy(pTempMsg, Msg, pMsgBox->Mem->QMBMemBlkSize);
            if (IfPostFront == FALSE)
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_NONE);
            else
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_FRONT);
            
            return OS_NO_ERR;
        }
    }
}


u8 OS_MsgBoxSend_Ex(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks, bool IfPostFront)
{
    void *pTempMsg;
    u16 len;
    u8 error;
    if (WaitTicks == OS_NO_DELAY)
    {
        if (OSSemAccept(pMsgBox->Sem) == NULL)
        {
            OS_Wrap_Debug("OS_MsgBoxSend_Ex failed when WaitTicks==OS_NO_DELAY\r\n");
            return OS_ERR_MBOX_FULL;
        }
        else
        {
            pTempMsg = QMBMemGet(pMsgBox->Mem);
            if (pTempMsg == NULL)
                return OS_ERR_MBOX_FULL;
            
            len = ((BIGMSG_EVENT*)Msg)->Len+8;
            if (len > pMsgBox->Mem->QMBMemBlkSize)
                len = pMsgBox->Mem->QMBMemBlkSize;
            
            OS_MemCpy(pTempMsg, Msg, len);
            
            if (IfPostFront == FALSE)
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_NONE);
            else
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_FRONT);
            
            return OS_NO_ERR;
        }
    }
    else
    {
        if (OSIntNesting > 0)
        {
            OS_Wrap_Debug("in ISR OS_MsgBoxSend_Ex's WaitTicks must be OS_NO_DELAY\r\n");
            return OS_ERR_MBOX_FULL;
        }
        OSSemPend(pMsgBox->Sem, WaitTicks, &error);
        if (error != OS_ERR_NONE)
        {
            return OS_ERR_MBOX_FULL;
        }
        else
        {
            pTempMsg = QMBMemGet(pMsgBox->Mem);
            if (pTempMsg == NULL)
                return OS_ERR_MBOX_FULL;
            
            len = ((BIGMSG_EVENT*)Msg)->Len+8;
            if (len > pMsgBox->Mem->QMBMemBlkSize)
                len = pMsgBox->Mem->QMBMemBlkSize;
            
            OS_MemCpy(pTempMsg, Msg, len);
            
            if (IfPostFront == FALSE)
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_NONE);
            else
                OSQPostOpt(pMsgBox->Queue, pTempMsg, OS_POST_OPT_FRONT);
            
            return OS_NO_ERR;
        }
    }
}


u8 OS_MsgBoxReceive(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks)
{
    void *pTempMsg;
    u8 error;
    if (WaitTicks == OS_NO_DELAY)
    {
        pTempMsg = OSQAccept(pMsgBox->Queue, &error);
        if (pTempMsg == NULL)
        {
            return OS_TIMEOUT;
        }
        else
        {
            OS_MemCpy(Msg, pTempMsg, pMsgBox->Mem->QMBMemBlkSize);
            QMBMemPut(pMsgBox->Mem, pTempMsg);
            OSSemPost(pMsgBox->Sem);
            
            return OS_NO_ERR;
        }
    }
    else
    {
        pTempMsg = OSQPend(pMsgBox->Queue, WaitTicks, &error);
        if (pTempMsg == NULL)
        {
            return OS_TIMEOUT;
        }
        else
        {
            OS_MemCpy(Msg, pTempMsg, pMsgBox->Mem->QMBMemBlkSize);
            QMBMemPut(pMsgBox->Mem, pTempMsg);
            OSSemPost(pMsgBox->Sem);
            
            return OS_NO_ERR;
        }
    }
}


u8 OS_MsgBoxReceive_Ex(OS_MsgBoxHandle pMsgBox, void *Msg, u16 WaitTicks)
{
    void *pTempMsg;
    u16 len;
    u8 error;
    if (WaitTicks == OS_NO_DELAY)
    {
        pTempMsg = OSQAccept(pMsgBox->Queue, &error);
        if (pTempMsg == NULL)
        {
            return OS_TIMEOUT;
        }
        else
        {
            len = ((BIGMSG_EVENT*)pTempMsg)->Len+8;
            if (len > pMsgBox->Mem->QMBMemBlkSize)
                len = pMsgBox->Mem->QMBMemBlkSize;
            
            OS_MemCpy(Msg, pTempMsg, len);
            
            QMBMemPut(pMsgBox->Mem, pTempMsg);
            OSSemPost(pMsgBox->Sem);
            
            return OS_NO_ERR;
        }
    }
    else
    {
        pTempMsg = OSQPend(pMsgBox->Queue, WaitTicks, &error);
        if (pTempMsg == NULL)
        {
            return OS_TIMEOUT;
        }
        else
        {
            len = ((BIGMSG_EVENT*)pTempMsg)->Len+8;
            if (len > pMsgBox->Mem->QMBMemBlkSize)
                len = pMsgBox->Mem->QMBMemBlkSize;
            
            OS_MemCpy(Msg, pTempMsg, len);
            
            QMBMemPut(pMsgBox->Mem, pTempMsg);
            OSSemPost(pMsgBox->Sem);
            
            return OS_NO_ERR;
        }
    }
}


OS_SemaphoreHandle OS_SemaphoreCreate(u8 Cnt)
{
    if (Cnt == 0)
        return OSMboxCreate(NULL);
    else
        return OSMboxCreate((void*)1);
}


// WaitTicks :
//   OS_NO_DELAY : no wait
//   OS_MAX_DELAY: wait for ever
//   1-0xfffe    : tick number
u8 OS_SemaphoreTake(OS_SemaphoreHandle Sem, u16 WaitTicks)
{
    u8 error;
    if (WaitTicks == OS_NO_DELAY)
    {
        if (OSMboxAccept(Sem) != NULL)
            return 0;
        else
        {
            OS_Wrap_Debug("SemaphoreTake error:%d\n\r", error);
            return 1;
        }
    }
    else if (WaitTicks == OS_MAX_DELAY)
    {
        if (OSMboxPend(Sem, 0, &error) != NULL)
            return 0;
        else
        {
            OS_Wrap_Debug("SemaphoreTake error:%d\n\r", error);
            return 1;
        }
    }
    else
    {
        if (OSMboxPend(Sem, WaitTicks, &error) != NULL)
            return 0;
        else
        {
            OS_Wrap_Debug("SemaphoreTake error:%d\n\r", error);
            return 1;
        }
    }
}


u8 OS_SemaphoreGive(OS_SemaphoreHandle Sem)
{
    return OSMboxPost(Sem, (void*)1);
}


OS_MutexHandler OS_MutexCreate(void)
{
    u8 error;
    static u8 Prio = SYS_MUTEX_MAX_PRIORITY;

    OS_MutexHandler pEvent = OSMutexCreate(Prio++, &error);
    if (error) OS_Wrap_Debug("Mutex Create %d\n\r", error);
    
    return pEvent;
}


u8 OS_MutexTake(OS_MutexHandler Mutex, u16 WaitTicks)
{
    u8 error;
    
    OSMutexPend(Mutex,WaitTicks, &error);
    if (error) OS_Wrap_Debug("MutexTake error:%d\n\r", error);
    return error;
}


u8 OS_MutexGive(OS_MutexHandler Mutex)
{
    return OSMutexPost(Mutex);
}


u32 OS_GetCurrentTick(void)
{
    return OSTimeGet();
}


u32 OS_GetCurrentSysMs(void)
{
    return (OSTimeGet()*OS_TICK_RATE_MS);
}


// **********************************heap**************************************

void DebugHeap(void)
{
    u16 Index;
    CPU_SR cpu_sr;
    
    Heap_Debug("--------------Heap Record--0x%08x----R:0x%08x-0x%08x--------\n\r", 
            (u32)HMem.Heap, (u32)HeapRecord, (u32)HeapRecord+MAX_RECORD_NUM*sizeof(HEAP_RECORD));
    OS_EnterCritical();
    for (Index=RECORD_START;;Index=GetNext(Index))
    {
        Heap_Debug("%3d:Used %d,Unit 0x%04x-0x%04x, Addr 0x%08x-0x%08x, Size %5dBytes, Prev %3d,Next %3d", 
            Index, GetUsedFlag(Index)?1:0,
            GetUnitIdx(Index), GetUnitIdx(GetNext(Index)), 
            (u32)HMem.Heap+Unit2Byte(GetUnitIdx(Index)), (u32)HMem.Heap+Unit2Byte(GetUnitIdx(GetNext(Index))), 
            Unit2Byte(GetUnitSize(Index)), 
            GetPrev(Index),GetNext(Index));
        Heap_Debug("\n\r");
        
        if (GetNext(Index) == RECORD_END) break;
    }
    OS_ExitCritical();
    Heap_Debug("--------------Heap Record End--0x%08x---------\n\r", (u32)HMem.Heap+HEAP_SIZE_BYTE);
}


void QS_HeapInit(void)
{
    u16 Index;
    CPU_SR cpu_sr;
    
    if ((Unit2Byte(1) != HEAP_MINI_UNIT) || (Byte2Unit(HEAP_MINI_UNIT) != 1))
    {
        Heap_Debug("HEAP_MINI_UNIT & HEAP_UNIT_OFFSET Marco Define Error!!!\n\r");
        while(1);
    }
    
    OS_EnterCritical();
    for (Index=0;Index<(sizeof(HeapRecord)/sizeof(HEAP_RECORD));Index++)
    {
        CleanRecord(Index);
    }
    
    // head
    SetPrev(RECORD_START, RECORD_END);
    SetNext(RECORD_START, RECORD_END);
    SetUsedFlag(RECORD_START, UNUSE);
    SetUnitIdx(RECORD_START, 0);
    
    // tail
    SetPrev(RECORD_END, RECORD_START);
    SetNext(RECORD_END, RECORD_START);
    SetUsedFlag(RECORD_END, USED);
    SetUnitIdx(RECORD_END, UNIT_MAX_NUM);
    
    HMem.dDummy = HEAP_UNIT_REWRITE_ID;
    HMem.dDummy <<= 16;
    
    DebugHeap();
    OS_ExitCritical();
}


static bool InsertRecord(u16 Index, u16 NeedUnit)
{    
    u16 i;
    if (NeedUnit < GetUnitSize(Index))
    {
        for (i=(RECORD_START+1);i<RECORD_END;i++)
        {
            if (GetUnitIdx(i) == 0)
            {
                SetPrev(i, Index);
                SetNext(i, GetNext(Index));
                SetUnitIdx(i, GetUnitIdx(Index)+NeedUnit);
                SetUsedFlag(i, UNUSE);
                
                SetPrev(GetNext(Index), i);
                
                SetUsedFlag(Index, USED);
                SetNext(Index, i);
                return TRUE;
            }
        }
        
        if (i == RECORD_END)
        {
            Heap_Debug("!!!Record Num is not enough!!!\n\r");
        }
        return FALSE;
    }
    else
    {
        SetUsedFlag(Index, USED);
        return TRUE;
    }
}


static u16 DeleteRecord(u16 UnitIdx)
{
    u16 Index, Size = 0;
    u16 i;
    
    for (Index=RECORD_START;;Index=GetNext(Index))
    {
        if (GetUnitIdx(Index) == UnitIdx)
        {
            Size = Unit2Byte(GetUnitSize(Index));
            SetUsedFlag(Index, UNUSE);
            
            i = GetPrev(Index);
            if (GetUsedFlag(i) == UNUSE)
            {
                SetNext(i, GetNext(Index));
                SetPrev(GetNext(Index), i);
                CleanRecord(Index);
                Index = i;
            }
            
            i = GetNext(Index);
            if (GetUsedFlag(i) == UNUSE)
            {
                SetNext(Index, GetNext(i));
                SetPrev(GetNext(i), Index);
                CleanRecord(i);
            }
            
            return Size;
        }
        
        if (GetNext(Index) == RECORD_END) break;
    }
    
    return 0;
}


void *QS_Mallco(u16 Size)
{
    u16 UnitNum, Index;
    u16 *Mem = NULL;
    CPU_SR cpu_sr;
    
    if (Size == 0) return NULL;
    
    UnitNum = Byte2Unit(Size);
    
    Size %= HEAP_MINI_UNIT;
    if (Size == 0 || (HEAP_MINI_UNIT-Size) < 2) UnitNum++;
    
    OS_EnterCritical();
    for (Index=RECORD_START;;Index=GetNext(Index))
    {
        if (GetUsedFlag(Index) == UNUSE)
        {
            if (GetUnitSize(Index) >= UnitNum)
            {
                if (InsertRecord(Index, UnitNum) == TRUE)
                {
                    Mem = (void *)((u32)HMem.Heap+Unit2Byte(GetUnitIdx(Index)));
                    Mem[(Unit2Byte(UnitNum)>>1)-1] = HEAP_UNIT_REWRITE_ID;
                    Heap_Debug("##Mallco Size %d, Addr 0x%x,Unit 0x%x\n\r", Unit2Byte(UnitNum), (u32)Mem, GetUnitIdx(Index));
                    OS_ExitCritical();
                    return (void *)Mem;
                }
                else 
                    break;
            }
        }
        
        if (GetNext(Index) == RECORD_END) 
            break;
    }
    OS_ExitCritical();
    
    DebugHeap();
    Heap_Debug("!!!No Get Heap!!!\n\r");
    return NULL;
}



bool QS_Free(void *Ptr)
{
    u16 *Mem = Ptr;
    u16 Size;
    CPU_SR cpu_sr;
    
    if (Ptr)
    {
        OS_EnterCritical();
        
        Size = DeleteRecord(Byte2Unit((u32)Ptr-(u32)HMem.Heap));
        
        if (Size == 0)
        {
            Heap_Debug("##Free Error!!!Can not find the match memory!!!\n\r");
            while(1);
        }
        
        Heap_Debug("##Free   Size %d, Addr 0x%x,Unit 0x%x\n\n\r", Size, (u32)Ptr, Byte2Unit((u32)Ptr-(u32)HMem.Heap));
        
        if (Mem[(Size>>1)-1] != HEAP_UNIT_REWRITE_ID)
        {
            Heap_Debug("Memory end be rewrited!!!%x %d\n\r", Mem[(Size>>1)-1], Size);
            while(1);
        }
        
        Mem --;
        if (Mem[0] != HEAP_UNIT_REWRITE_ID)
        {
            Heap_Debug("Memory header be rewrited!!!%x\n\r", Mem[0]);
            while(1);
        }
        
        OS_ExitCritical();
        return TRUE;
    }
    else
        return FALSE;
}



// fragindices = (float)MaxSize/(float)TotalSize
// fragindices -> 0, fragments more
u16 QS_MonitorFragment(void)
{
    u16 Index, Size, MaxSize = 0, TotalSize = 0;
    
    for (Index=RECORD_START;;Index=GetNext(Index))
    {
        if (GetUsedFlag(Index) == UNUSE)
        {
            Size = Unit2Byte(GetUnitSize(Index));
            TotalSize += Size;
            if (Size > MaxSize)
                MaxSize = Size;
        }
        if (GetNext(Index) == RECORD_END) break;
    }
    
    Heap_Debug("********** Heap Monitor **********\n\r");
    Heap_Debug(" TotalFreeMem=%d Byte\n\r", TotalSize);
    Heap_Debug(" MaxFreeMem=%d Byte\n\r", MaxSize);
    Heap_Debug(" Fragindices=%.2f\n\r", (float)MaxSize/(float)TotalSize);
    Heap_Debug("**********************************\n\r");
    
    return TotalSize;
}


bool QS_HeapArrange(void)
{
    return TRUE;
}


// these functions are all re-enter function


u8 IntToStr(u8 *str, u32 val)
{
    u8 buffer[12];
    u8 i = 0, v, j;
    
    if (val == 0)
    {
        str[0] = 0x30;
        str[1] = 0;
        return 1;
    }
    
    while (val)
    {
        v = val % 10;
        buffer[i++] = v+0x30;
        val /= 10;
    }
    
    for (j=0;j<i;j++)
    {
        str[j] = buffer[i-1-j];
    }
    str[i] = 0;
    
    return i;
}


u8 IpToStr(u8 *str, u32 ip)
{
    u8 i, v, k, j = 0;
    
    for (i=0;i<4;i++)
    {
        v = ip & 0xFF;
        k = IntToStr(str+j, v);
        j += k;
        if (i < 3)
        {
            str[j++] = 0x2e;
        }
        ip >>= 8;
    }
    
    return j;
}


u32 StrToInt(u8 *str)
{
    u32 i = 0, sum = 0;
    
    while (str[i])
    {
        sum = sum*10+(str[i]-'0');
        i ++;
    }
    
    return (sum);
}


int FloatToInt(float f)
{
    bool minus=(f<0.0?TRUE:FALSE);
    int a;
    
    if (minus) f *= -1;
    a = (int)f;
    if ((f-a) >= 0.5) ++a;
    if (minus) a *= -1;
    return a;
}


void OS_MemSet(void *Dst, u8 C, u16 Byte)
{
    if (!Byte) return;

    for (Byte-=1;Byte;Byte--)
    {
        ((u8 *)Dst)[Byte] = C;
    }
    ((u8 *)Dst)[0] = C;
}


void OS_MemCpy(void *Dst, const void *Src, u16 Byte)
{
    if (!Byte) return;
    
    for (Byte-=1;Byte;Byte--)
    {
        ((u8 *)Dst)[Byte] = ((u8 *)Src)[Byte];
    }
    ((u8 *)Dst)[0] = ((u8 *)Src)[0];
}


void OS_MemCpy2(void *Dst, const void *Src, u16 Byte)
{
    u16 i = 0;
    if (!Byte) return;
    
    for (i=0;i<Byte;i++)
    {
        ((u8 *)Dst)[i] = ((u8 *)Src)[i];
    }
}


bool CompareBuf(u8 *Buf1, u8 *Buf2, u16 Len)
{
    u16 i;

    for (i=0;i<Len;i++)
    {
        if (Buf1[i] != Buf2[i]) return FALSE;
    }

    return TRUE;
}


void DisplayBuf(const u8 *Buf, u16 Len, u8 RawLen)
{
#if MZ_PRINT_DEBUG
    int i;
    
    for (i=0;i<Len;i++)
    {
        Debug("0x%02x ", Buf[i]);
        if ((i % RawLen) == (RawLen-1))
            Debug("\n\r");
    }
    Debug("\n\r");
#endif
}


static u32 MakeHash33(u8 *pData, u32 Len)
{
    u32 hash = 0;
    u32 i = 0;
    
    for (;i<Len;i++)
    {
        hash += pData[i];
        hash += (hash<<5);
    }
    
    return hash;
}


// eg. 0x1234 -> 0x3412
u16 Rev16(u16 Data)
{
    return __REV16(Data);
}


// eg. 0x12345678 -> 0x78563412
u32 Rev32(u32 Data)
{
    return __REV(Data);
}


u32 GetHwID(void)
{
    static u32 Id = 0;

    if (Id) return Id;
    else
    {
        Id = MakeHash33((u8 *)0x1FFFF7E8, 96);
        return Id;
    }
}


const u8 crc8table[256] = 
{
    0,   94,  188, 226, 97,  63,  221, 131, 194, 156, 126, 32,  163, 253, 31,  65, 
    157, 195, 33,  127, 252, 162, 64,  30,  95,  1,   227, 189, 62,  96,  130, 220, 
    35,  125, 159, 193, 66,  28,  254, 160, 225, 191, 93,  3,   128, 222, 60,  98, 
    190, 224, 2,   92,  223, 129, 99,  61,  124, 34,  192, 158, 29,  67,  161, 255, 
    70,  24,  250, 164, 39,  121, 155, 197, 132, 218, 56,  102, 229, 187, 89,  7, 
    219, 133, 103, 57,  186, 228, 6,   88,  25,  71,  165, 251, 120, 38,  196, 154, 
    101, 59,  217, 135, 4,   90,  184, 230, 167, 249, 27,  69,  198, 152, 122, 36, 
    248, 166, 68,  26,  153, 199, 37,  123, 58,  100, 134, 216, 91,  5,   231, 185, 
    140, 210, 48,  110, 237, 179, 81,  15,  78,  16,  242, 172, 47,  113, 147, 205, 
    17,  79,  173, 243, 112, 46,  204, 146, 211, 141, 111, 49,  178, 236, 14,  80, 
    175, 241, 19,  77,  206, 144, 114, 44,  109, 51,  209, 143, 12,  82,  176, 238, 
    50,  108, 142, 208, 83,  13,  239, 177, 240, 174, 76,  18,  145, 207, 45,  115, 
    202, 148, 118, 40,  171, 245, 23,  73,  8,   86,  180, 234, 105, 55,  213, 139, 
    87,  9,   235, 181, 54,  104, 138, 212, 149, 203, 41,  119, 244, 170, 72,  22, 
    233, 183, 85,  11,  136, 214, 52,  106, 43,  117, 151, 201, 74,  20,  246, 168, 
    116, 42,  200, 150, 21,  75,  169, 247, 182, 232, 10,  84,  215, 137, 107, 53 
};


u8 docrc8(u8 crc, u8 *p, u16 len)
{
    for (;len>0;len--)
        crc = crc8table[crc^(*p++)];
    
    return crc;
}
