#include "System.h"
#include <stddef.h>
#include "Os_Wrap.h"
#include "Os_TaskPrio.h"
#include "WirelessComm.h"
#include "WirelessApp.h"
#include "Wlan4GComm.h"
#include "Wlan4GApp.h"
#include "AirIap.h"
#include "Dispatcher.h"
#include "Uart.h"
#include "Rtc.h"
#include "wdg.h"
#include "led.h"
#include "flash.h"
#include "FlineCtrl.h"
#include "StateMachine.h"
#include "FoodlineControl.h"

extern OS_MsgBoxHandle gDisplaySend_Queue;
extern OS_MsgBoxHandle gDisplayMsg_Queue;
extern OS_MsgBoxHandle gDisplayNotify_Queue;

void *DisplayCommRecv_Task_Handle = NULL;
void *DisplayCommSend_Task_Handle = NULL;
void *ParaRefresh_Task_Handle = NULL;
void *FoodlineControl_Task_Handle = NULL;

// ***********************************************************
// define for system heap memory and task stack check
// heap memory size HEAP_SIZE_BYTE in Os_Wrap.h
// IF need modify, go there and EDIT it

struct SysHighMem HMem;
u16 SysHMemLeftLen;

MZSYS_TASK_RECORD MZQsTaskRecord[MZOS_TASK_RECORD_NUM];
u8 MZOsTaskTotalNum;

// end define for system heap memory and task stack check
// ***********************************************************

u32 Stm32DeviceID[3];

u32 gNineSensorId   = 0x09000003;


//****************************************** Bsae Config ******************************************************//
// 31-24: type, 23-10: pigfarm, 9-0: piggery
u32 gUploadDeviceId = 0x08000A01;   // support 16383 pig farms and 1023 piggery (or middleware) each pigfarm
                                    // ONLY for 4G or platform channel
                                    // Here, indicate pigfarm 2 and NO.1 middleware
u32 gAlarmCtrlId = 0x0D000001;
u32 gOuterSensId = 0x0C000001;

//u16 gSensorConfig = Sensor_Enable_TEMPHUMI|Sensor_Enable_BH1750LUX|Sensor_Enable_CO2|
//                    Sensor_Enable_NH3|Sensor_Enable_TEMP1|
//                    Sensor_Enable_TEMP2|Sensor_Enable_TEMP3|Sensor_Enable_TEMP4;
u8 gSensorCo2Range = 5;

u32 gLocalDeviceId = 0x32000001;    //
u32 gCtrlPlatformId = 0x03000010;   // 0x10~0x17
u32 gCtrlServerIp = 0x91F41777;     // 0x81C04E78-120.78.192.129   0x91F41777-119.23.244.145  0x784D4F78-120.79.77.120
u16 gCtrlServerPort = 10063;
//****************************************** End Bsae Config **************************************************//


u8 gLocalInfoFromRom = 0;
static LocalConfigPara gLocalConfigPara;


u32 LocalDeviceIdGet(void)
{
	return gLocalDeviceId;
}

void LocalDeviceIdSet(u32 id)
{
    u8 buffer[32];
    
    ReadLocalConfig(buffer);
	gLocalDeviceId = id;
    buffer[0] = (u8)((gLocalDeviceId>>24) & 0xff);
    buffer[1] = (u8)((gLocalDeviceId>>16) & 0xff);
    buffer[2] = (u8)((gLocalDeviceId>>8) & 0xff);
    buffer[3] = (u8)(gLocalDeviceId & 0xff);
    
    WriteLocalConfig(buffer);
	Wireless_ResetIO(0);
	OSTimeDly(100);
	Wireless_ResetIO(1);
}

LocalConfigPara *LocalConfigParaGet(void)
{
    return &gLocalConfigPara;
}

void Parameter_Init(void)
{
    u8 buffer[32];
    u8 err;
    
    err = ReadLocalConfig(buffer);
    if (err == FLASH_OP_SUCCESS)
    {
        gLocalDeviceId = ((u32)(buffer[0])<<24)|((u32)(buffer[1])<<16)|((u32)(buffer[2])<<8)|(u32)(buffer[3]);
        gCtrlPlatformId = ((u32)(buffer[4])<<24)|((u32)(buffer[5])<<16)|((u32)(buffer[6])<<8)|(u32)(buffer[7]);
        
        // here config from 32 bytes
        gCtrlServerIp = ((u32)(buffer[8])<<24)|((u32)(buffer[9])<<16)|((u32)(buffer[10])<<8)|(u32)(buffer[11]);
        gCtrlServerPort = ((u16)(buffer[12])<<8)|(u16)(buffer[13]);
        
        gLocalInfoFromRom = 1;
        gLocalConfigPara.isUse4G = buffer[14];

    }
    else if (err == FLASH_OP_ERR_ALLF)
    {
        buffer[0] = (u8)((gLocalDeviceId>>24) & 0xff);
        buffer[1] = (u8)((gLocalDeviceId>>16) & 0xff);
        buffer[2] = (u8)((gLocalDeviceId>>8) & 0xff);
        buffer[3] = (u8)(gLocalDeviceId & 0xff);
        buffer[4] = (u8)((gCtrlPlatformId>>24) & 0xff);
        buffer[5] = (u8)((gCtrlPlatformId>>16) & 0xff);
        buffer[6] = (u8)((gCtrlPlatformId>>8) & 0xff);
        buffer[7] = (u8)(gCtrlPlatformId & 0xff);
        
        buffer[8] = (u8)((gCtrlServerIp>>24) & 0xff);
        buffer[9] = (u8)((gCtrlServerIp>>16) & 0xff);
        buffer[10] = (u8)((gCtrlServerIp>>8) & 0xff);
        buffer[11] = (u8)(gCtrlServerIp & 0xff);
        
        buffer[12] = (u8)((gCtrlServerPort>>8) & 0xff);
        buffer[13] = (u8)(gCtrlServerPort & 0xff);

        buffer[14] = 0;
        
        WriteLocalConfig(buffer);
    }
}
void GlobalVars_Init(void)
{	
    // display
    gDisplaySend_Queue = OS_MsgBoxCreate("DsndEvt", sizeof(INPUT_EVENT), DISPLAY_MSGSND_COUNT);
    if (gDisplaySend_Queue == NULL)
        SYSTEMINIT_Debug("gDisplaySend_Queue NULL\n\r");
    
    gDisplayMsg_Queue = OS_MsgBoxCreate("DmsgEvt", sizeof(INPUT_EVENT), DISPLAY_RCVBODY_COUNT);
    if (gDisplayMsg_Queue == NULL)
        SYSTEMINIT_Debug("gDisplayMsg_Queue NULL\n\r");
    
    gDisplayNotify_Queue = OS_MsgBoxCreate("DectEvt", sizeof(INPUT_EVENT), 12);
    if (gDisplayNotify_Queue == NULL)
        SYSTEMINIT_Debug("gDisplayNotify_Queue NULL\n\r");
}


void MZ_SYS_Init(void)
{
    u8 svrips[20];
    
    OS_CPU_SysTickInit();
    CPU_IntSrcPrioSet(CPU_INT_PENDSV, 15);
    CPU_IntSrcPrioSet(CPU_INT_SYSTICK, 15);
    
    Stm32DeviceID[0] = *(__IO uint32_t*)(0x1FFFF7E8);
    Stm32DeviceID[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    Stm32DeviceID[2] = *(__IO uint32_t*)(0x1FFFF7F0);
    
    IWDG_Init(6, 1625);
    // LED_Init();
    
    // init hardware
    RTC_SetUp();
    
    // Must call before Parameter_Init
    ParaFlash_Init(FLASH_SIZE);
    
    // paras
    Parameter_Init();
    GlobalVars_Init();
    // mass storage
    SPI_Flash_Init();
    
    WirelessApp_Init(0);
    OSTimeDly(600);
    DisplayComm_Init();
	ParaSettingLibInit();
    StateMachineInit();
    
    IpToStr(svrips, gCtrlServerIp);
    Wlan4G_SetServer(svrips, gCtrlServerPort);
    Wlan4G_Init();
    
    // AirIap_Init(0, 1, WirelessApp_SendData, 60000, 500);
    AirIap_Init(1, Wlan4G_SendData, WirelessApp_SendData, 60000, 500, AIRIAPREQ_TASK_PRIORITY, AIRIAPDAT_TASK_PRIORITY, 
        (SOFT_BUILD_VERSION>>8) & 0xFF, SOFT_BUILD_VERSION & 0xFF, 4, 5, Stm32DeviceID);
    // OS_TaskSuspend can be called if needed
    OS_TaskCreate(DisplayCommRecv_Task, "DisplayR", OS_MINIMAL_STACK_SIZE*(4+1), NULL, DISPLAYRECV_TASK_PRIORITY, &DisplayCommRecv_Task_Handle);
    OS_TaskCreate(DisplayCommSend_Task, "DisplayS", OS_MINIMAL_STACK_SIZE*(4+1), NULL, DISPLAYSEND_TASK_PRIORITY, &DisplayCommSend_Task_Handle);

	OS_TaskCreate(DisplayParaRefresh_Task, "ParaRefresh", OS_MINIMAL_STACK_SIZE*(4+1), NULL, PARA_REFRESH_TASK_PRIORITY, &ParaRefresh_Task_Handle);
	OS_TaskCreate(ControlSend_Task, "ControlSend", OS_MINIMAL_STACK_SIZE*(4+1), NULL, FOODLINE_CONTROL_PRIORITY, &FoodlineControl_Task_Handle);
    
    OS_TaskDelay(100);
    OS_TaskStkCheck();
    
    // *********************** switch for all tasks ****************************
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    #if 0
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
    #endif
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    
    #if MZ_PRINT_DEBUG
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    #endif
    
    // *********************** end switch for all tasks ************************
}

