#ifndef MZSYS_WIRELESS_COMM_HANDLER_H
#define MZSYS_WIRELESS_COMM_HANDLER_H

#include "stm32f10x.h"
#include "Debug.h"
#include "Uart.h"


#define UART_CONFIG_PARA_COMMAND    1

// for para command
#define UART_CONFIG_DEVICE_PANID    1
#define UART_CONFIG_DEVICE_DEVID    2
#define UART_CONFIG_CONTROL_ID      3

#define UART_CONFIG_STATIC_ROUTE    4
#define UART_CONFIG_NET2NET_ROUTE   5

#define UART_CONFIG_RF_CHANNEL      6

#define UART_CONFIG_ROUND_RPTTIME   7

#define UART_CONFIG_NEIGHBOR_LQI    8
// end for para command

#define UART_CONFIG_UPDATE_COMMAND  2



#define ROUTE_TYPE_COMM_NETNODE         0
#define ROUTE_TYPE_STATIC_NETNODE       1
#define ROUTE_TYPE_NET2NET              2



// len [ fc seq  devid  ****  crc ]
//  -    -   -   ----    --    -
// [ len fc seq  devid  **** ] crc


#define UART_FRMTYPE_DATA       0
#define UART_FRMTYPE_CMD        1
#define UART_FRMTYPE_ACK        2
#define UART_FRMTYPE_DEBUG      3

typedef union
{
    u8 val;
    struct {
        u8 type:2;       //0: data, 1: command, 2: ack, 3: debug
        u8 ttl:2;        //for rf side
        u8 srcaddr:1;
        u8 reserv:2;
        u8 needack:1;
    } ctrl;
} UartFctrl;


#define FRAME_DISABLE_CRC       0x80
#define FRAME_CLOSE_RETRANSMIT  0x40
#define FRAME_CTRL_MASK         0x03
#define FRAME_NEED_ACK          0x00
#define FRAME_NEED_NO_ACK       0x01
#define FRAME_JUST_ACK          0x02
#define FRAME_JUST_DEBUG        0x03

// ************************* MAX User Data Length ***********************
#define MAX_USER_DATA_LENS      80
// ************************* MAX User Data Length ***********************

// frame description for terminal device or platform
// frame tag (1) | len (1) | control (1) | dest addr (4) | src addr (4) | user data (n) | crc (1) | frame tag (1)
//    tag len ctrl dest src  user data  crc tag
//     7e   *   *   *    *      ***      *  7e
// 1. for len, inlcude control+dest addr+src addr+data+crc
// 2. for crc, crc8, include len+control+dest addr+src addr+data
// 3. for control
//    7: disable crc8
//    6: close retransmit, no matter what, no retransmit
//    5-3: reserved, 0
//    2  : control bit (1: control frame and now reserved, 0: data frame),
//    1-0: frame type (0: frame need ack  1: frame need not ack  2: ack frame  3: debug frame)
// 3. for user data, n <= 84
//    when frame need ack, data[0] = command for app, data[1-2] = sequence for data[0]
//        NOTE: sequence in data[1-2] MUST be exist
//    when frame need not ack, data[0] = command for app
//        NOTE: NO sequence, data[1-2] is content
//    when frame is ack frame, data[0] = command for ack, data[1-2] = seqence for ack
//        NOTE: sequence in data[1-2] MUST be exist
//    when frame is debug, content is applied according to debug
// 4. for frame tag, now is 0x7e, maybe 0x7f better


// if device used as middleware to forward data, then use4g = 1, otherwise = 0, 
// and MUST process data Carfully, because NOT all data need to forward
// retranscnt, queue length for retransmit of wireless, 
// for middleware, greater will be good, for common, 8 is ok
void WirelessComm_Init(USART_TypeDef *uart, u8 use4g, u8 retranscnt);

// type 0: frame need ack  1: frame need not ack  2: ack frame  3: debug frame
// srcaddr: gLocalDeviceId for terminal
// dstaddr: gCtrlPlatformId for terminal
// msg: user data, exclude sequence and command
// 
// return:
// 0: sent, 1: no local retrans buffer if message need to ack, 2: user message length > max length
u8 WirelessApp_SendData(u8 cmd, u8 type, u8 *msg, u8 len, u32 srcaddr, u32 dstaddr, u32 dstid, u16 ackseq);

// ctrl: 
// 0: data channel,  1: command channel,  2: wireless channel
void WirelessComm_SendData(BIGMSG_EVENT *evt, u8 ctrl, u8 len, u32 srcid, u32 dstid, u8 ttl);

void CreateWrelessTask(u8 rcvstack, u8 rcvprio, u8 sndstack, u8 sndprio, u8 retransstack, u8 retransprio);



// ******************************Comm init******************************
void WirelessIntService(void);
// ******************************end Comm init**************************

#endif
