#ifndef MZSYS_WIRELESS_APP_HANDLER_H
#define MZSYS_WIRELESS_APP_HANDLER_H

#include "stm32f10x.h"
#include "Debug.h"


#define FRAME_FLAG_MSG_NEED_ACK     0
#define FRAME_FLAG_MSG_NO_ACK       1
#define FRAME_FLAG_PEER_MSG_ACK     2
#define FRAME_FLAG_PEER_DEBUG       3
#define FRAME_FLAG_RETRANS_TIMEOUT  4



void WirelessApp_Init(u8 use4g);
void Wireless_ResetIO(u8 val);

void WirelessDebug(u8 *pbuf);

// flag:
// 0: peer message need ack, 1: peer message without ack, 2: peer ack message, 3: peer debug message, 
// 4: local message timeout and info come from local send message
void WirelessApp_RcvMsg(u8 cmd, u16 seq, u8 *msg, u16 len, u32 saddr, u32 daddr, u32 srcid, u8 flag);

// forward message to 4G, if device has other function, can do actions by yourself
// flag: 0: data, message
//       1: control message
//       2: update
//       3: reserved
//       4: destroy
//       5: heart beat
//       6: debug
//       7: for wireless at each device
// ttl:  0-3, indicate left route hops from source device, can use it to decide process for forward
// srcid: here source device MAYBE = 0, if 0, indicate sender omit source id
void WirelessApp_ForwMsg(u8 *msg, u16 len, u32 saddr, u32 daddr, u8 flag, u8 ttl);

#endif
