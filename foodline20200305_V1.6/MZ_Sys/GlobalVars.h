#ifndef MZSYS_GLOBAL_VARS_HEADER_H
#define MZSYS_GLOBAL_VARS_HEADER_H


// dispatcher
OS_MsgBoxHandle gDispatchHandler_Queue;
OS_MsgBoxHandle gControl_Handle_Queue;
// wireless
OS_MsgBoxHandle gWirelessSend_Queue;
OS_MsgBoxHandle gWirelessMsg_Queue;
OS_MsgBoxHandle gWirelessNotify_Queue;
u8 gWirelessMsgBody[WIRELESS_RCVBODY_COUNT][128];
u8 gWirelessMsgSend[WIRELESS_MSGSND_COUNT][128];
u8 gWirelessMsgBodyLen[WIRELESS_RCVBODY_COUNT];
u8 gWirelessMsgSendLen[WIRELESS_MSGSND_COUNT];
// display
OS_MsgBoxHandle gDisplaySend_Queue;
OS_MsgBoxHandle gDisplayMsg_Queue;
OS_MsgBoxHandle gDisplayNotify_Queue;
u8 gDisplayMsgBody[DISPLAY_RCVBODY_COUNT][128];
u8 gDisplayMsgSend[DISPLAY_MSGSND_COUNT][128];
u8 gDisplayMsgBodyLen[DISPLAY_RCVBODY_COUNT];
u8 gDisplayMsgSendLen[DISPLAY_MSGSND_COUNT];


// 4G
//OS_MsgBoxHandle gWlan4GRcv_Queue;
//u8 gWlan4GRcvBody[WLAN4G_RCVBODY_COUNT][WLAN4G_MAX_BUFFER_SIZ];

//OS_MsgBoxHandle gWlan4GSnd_Queue;
//u8 gWlan4GSndBody[WLAN4G_SNDBODY_COUNT][WLAN4G_MAX_BUFFER_SIZ];

//u16 gWlan4GRcvBodyLen[WLAN4G_RCVBODY_COUNT];
//u16 gWlan4GSndBodyLen[WLAN4G_SNDBODY_COUNT];
//u8 gWlan4GSndBodySk[WLAN4G_SNDBODY_COUNT];

//OS_MsgBoxHandle gWlan4GMsg_Queue;



// id
u32 gSrcwareId;
u32 gCtrlPlatformId;
//µç»ú¿ØÖÆ
MOTOR_INFO MotorInfo;

CONTROL_ZERO_INFO m_ControlZero;
PWM_INFO PwmInfo;
CONFIG_RECORD Config_Info;
#endif
