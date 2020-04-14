#ifndef MZSYS_WLAN4G_APP_HANDLER_H
#define MZSYS_WLAN4G_APP_HANDLER_H

#include "stm32f10x.h"
#include "Debug.h"


#define GET_WLAN4G_USIM_CCID    0xe2


void Wlan4G_Init(void);
void Wlan4G_ResetIO(u8 val);

// conn: 
//    0: unconnect,  1: connect,  2: disconnect
// err:
//    for unconnect:  1: ready to reset,  2: reset end,  3: too weak signal to connect to base station
//    for connect:  0: ok
//    for disconnect:  1: socket close,  2: pdp deact due to base station,  3: other error
void Wlan4G_Stat(u8 conn, u8 err);

// type:
//    1: no signal,  2: CREG error,  3: CGREG error (no matter ...),  4: +CME error,  5: connect error
void Wlan4G_Errinfo(u8 type, u8 *errmsg);


void Wlan4G_RecvMsg(u8 flag, u8 *buffer, u16 len, u32 srcid, u32 dstid);

#endif
