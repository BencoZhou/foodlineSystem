#ifndef MZSYS_MESSAGE_DISPATCH_HANDLER_H
#define MZSYS_MESSAGE_DISPATCH_HANDLER_H


#include "stm32f10x.h"
#include "Debug.h"


typedef enum
{
    Input_Paras = 0,        // only for pass parameter
    Input_Threads,          // threads
    
    Input_ExtiKey,          // external key
    Input_Rtc,              // real colock
    Input_Timer,            // stm32 timer
    
    Input_PeripCtrl,        // ctrl peripheral device
    Input_PeripChk,         // check peripheral device
    
    Input_FlashCtrl,        // flash, e2rom, etc.
    
    Input_Error,            // error
    
    SysEvt_MaxNum,
} INPUT_SRC_TYPE;


typedef enum
{
    Threads_Control = 0,
    
    Rtc_Time_Second,            // real colock
    Rtc_Time_Setting,
    
    Perip_Wireless_Comm,
    Perip_Display_Comm,
    Perip_Foodline_control,
    Perip_Wlan4G_Recv,
    Perip_Wlan4G_Send,
    Perip_Wlan4G_Msg,
    
    Flash_Save_Info,
    Flash_Upgrade_Code,
} INPUT_MSG_EVT;


//typedef struct
//{
//    INPUT_SRC_TYPE Type;    // source type
//    INPUT_MSG_EVT Subtype;  // message type
//    u16 Num;
//    union
//    {
//        u8 b[4];
//        u16 w[2];
//        u32 dw;
//    } Info;
//} INPUT_EVENT;


//#define BIGMSG_BLOCK_SIZ        128

//typedef struct
//{
//    INPUT_SRC_TYPE Type;    // source type
//    INPUT_MSG_EVT Subtype;  // message type
//    u16 Len;
//    union
//    {
//        u8 b[4];
//        u16 w[2];
//        u32 dw;
//    } Info;
//    u8 buffer[BIGMSG_BLOCK_SIZ];
//} BIGMSG_EVENT;


#endif

