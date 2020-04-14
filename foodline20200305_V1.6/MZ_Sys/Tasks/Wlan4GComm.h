#ifndef MZSYS_WIRELESS_4G_COMM_HANDLER_H
#define MZSYS_WIRELESS_4G_COMM_HANDLER_H

// suggest: packet task priority > recv task priority > send task priority
// NOTE: for Quectel EC20


#include "stm32f10x.h"
#include "Debug.h"
#include "PeripheralIO.h"
#include "Uart.h"


// ******************************4G service******************************
// add to interrupt service routine
void Wlan4GIntService(void);
// ******************************end 4G service**************************



#define MIDWARE_DATA_MASK_BIT   0x0F
#define MIDWARE_CTRL_MASK_BIT   0x30

#define MIDWARE_DATA_MESSAGE    0
#define MIDWARE_DATA_CONTROL    1       // reserved
#define MIDWARE_DATA_UPDATE     2       // reserved
#define MIDWARE_DATA_DESTROY    4
#define MIDWARE_DATA_HBEAT      5
#define MIDWARE_DATA_DEBUG      6
#define MIDWARE_DATA_WIRELESS   7

#define MIDWARE_CTRL_COMPRESS   0x10
#define MIDWARE_CTRL_CRYPTO     0x20


typedef struct
{
    u16 len;        // high, low, include frame data and frame header (12 bytes)
    u8 flags;       // 3-0: 0: data, message
                    //      1: control message
                    //      2: update
                    //      3: reserved
                    //      4: destroy
                    //      5: heart beat
                    //      6: debug
                    //      7: for wireless at each device
                    // 4:   compressed
                    // 5:   crypt
                    // 6:   wire mode
                    // 7:   enable crc8
    u8 crc8;        // inlcude data and header exclude crc8
    u32 dstid;      // high -> low
    u32 srcid;      // high -> low
} Wlan4GFrame;



void Wlan4GComm_Init(USART_TypeDef *uart);
void Wlan4G_SetServer(u8 *ip, u16 port);
void Wlan4G_SendData(u8 *msg, u16 len, u32 srcid, u32 dstid, u8 flag, u8 encrc);
void CreateW4GTask(u8 lnkstack, u8 lnkprio, u8 rcvstack, u8 rcvprio, u8 sndstack, u8 sndprio);

#endif
