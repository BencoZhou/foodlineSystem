#ifndef MZSYS_OS_TASK_PRIO_HEADER_H
#define MZSYS_OS_TASK_PRIO_HEADER_H

#include "ucos_ii.h"
#include "Debug.h"


typedef void (*TASK_FUNC)(void *);

typedef struct
{
    void *QMBMemAddr;       // mem box address
    void *QMBMemFreeList;   // next idle blcok address
    u16 QMBMemBlkSize;      // mem block size
    u8 QMBMemBlks;          // mem number
    u8 QMBMemNFree;         // idle mem number
} SYS_MSG_BOX_MEM;          // mailbox mem control block

typedef struct
{
    OS_EVENT *Queue;        // msg queue
    OS_EVENT *Sem;          // count semphore
    SYS_MSG_BOX_MEM *Mem;
    const u8 *Name;
} OS_MSG_BOX;               // mem control block

typedef OS_EVENT    *OS_QueueHandle;
typedef OS_MSG_BOX  *OS_MsgBoxHandle;
typedef OS_EVENT    *OS_SemaphoreHandle;
typedef OS_EVENT    *OS_MutexHandler;


//os config
#define OS_MINIMAL_STACK_SIZE   128
#define OS_HIGHEST_PRIORITIES   6
#define OS_LOWEST_PRIORITIES    OS_LOWEST_PRIO
#define OS_TICK_RATE_HZ         OS_TICKS_PER_SEC
#define OS_TICK_RATE_MS         (1000/OS_TICKS_PER_SEC)
#define OS_MAX_DELAY            0
#define OS_NO_DELAY             0xffff
#define OS_RETURN_OK            OS_ERR_NONE
#define OS_Ms2Tick(M)           (M/(1000/OS_TICKS_PER_SEC))


typedef enum
{    
    SYS_TASK_PRIORITY = OS_HIGHEST_PRIORITIES,
    SYS_MUTEX_MAX_PRIORITY,
    
//    AIRIAPREQ_TASK_PRIORITY,
//    AIRIAPDAT_TASK_PRIORITY,
//    PARA_REFRESH_TASK_PRIORITY,
//    // LCD
//    DISPLAYRECV_TASK_PRIORITY,
//    DISPLAYSEND_TASK_PRIORITY,
//    
//    // app
//    WLAN4GPKT_TASK_PRIORITY,
//    WLAN4GLNK_TASK_PRIORITY,
//    WLAN4GSND_TASK_PRIORITY,
//    WIRELESSRCV_TASK_PRIORITY,
//    WIRELESSSND_TASK_PRIORITY,
//    WIRELESSRTRAN_TASK_PRIORITY,
//    
//    DISPATCHER_TASK_PRIORITY,
    
    
    // app
    WLAN4GPKT_TASK_PRIORITY,
    WLAN4GLNK_TASK_PRIORITY,
    WLAN4GSND_TASK_PRIORITY,
    WIRELESSRCV_TASK_PRIORITY,
    WIRELESSSND_TASK_PRIORITY,
    WIRELESSRTRAN_TASK_PRIORITY,
    
    DISPATCHER_TASK_PRIORITY,
    
    AIRIAPREQ_TASK_PRIORITY,
    AIRIAPDAT_TASK_PRIORITY,
    PARA_REFRESH_TASK_PRIORITY,
    
    // LCD
    DISPLAYRECV_TASK_PRIORITY,
    DISPLAYSEND_TASK_PRIORITY,
} TASK_PRIORITY;

#endif
