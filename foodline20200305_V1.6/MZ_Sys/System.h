#ifndef  MZSYS_SYSTEM_HEADER_H
#define  MZSYS_SYSTEM_HEADER_H



#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Os_Wrap.h"
#include "Os_TaskPrio.h"

#include "MsgTypes.h"
#include "Uart.h"
#include "Rtc.h"
#include "Flash.h"
#include "Time.h"
#include "wdg.h"
#include "Delay.h"
#include "led.h"

#include "Dispatcher.h"
#include "Tasks/CtrlPolicy.h"
#include "Tasks/WirelessComm.h"
#include "Tasks/WirelessApp.h"
#include "Tasks/Wlan4GComm.h"
#include "Tasks/Wlan4GApp.h"
#include "Tasks/AirIap.h"

#include "ParaSettingLib.h"
#include "DisplayProtocol.h"
#include "Display.h"
#include "CtrlConfig.h"
#include "spiFlash.h"


#ifdef MZSYS_DEBUG
#define SYSTEMINIT_Debug    Debug
#else
#define SYSTEMINIT_Debug(x,y...)
#endif


#define WIRELESS_RCVBODY_COUNT                          (16)
#define WIRELESS_MSGSND_COUNT                           (16)
#define WIRELESS_BUF_LEN                                (128)

typedef struct
{
    u8 isUse4G;         // 0:ÎÞÏß£»1:4G
    u8 reserve[17];
}LocalConfigPara;

LocalConfigPara *LocalConfigParaGet(void);


#endif
