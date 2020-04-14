#ifndef MZSYS_IAP_HANDLER_H
#define MZSYS_IAP_HANDLER_H


#include "stm32f10x.h"
#include "Debug.h"


#define ASK_IAPVERSON   0xf0
#define ASK_IAPDATA     0xf1
#define ASK_IAPINFO     0xf7
#define ASK_CHIPGUID    0xf8

// 0xf2 and 0xf3 used for wireless module
// 0xff used for upload task stack
// 0xfb and 0xfc and 0xfd used for debug
// 0xfa used for outer sensor

#define CONFIG_LOCAL_DEVICE_PARA    0xf5
#define READ_LOCAL_DEVICE_PARA      0xf4
#define CONFIG_LOCAL_DEVICE_ID      0xf6
#define RESET_LOCAL_DEVICE          0xf9
#define ADJUST_NETWORK_TIME         0xfe



/*******************************************************************/
/* 0x08000000 :  bootloader                                        */
/* 0x080027FF :     10K                                            */
/*******************************************************************/
/* 0x08002800 :  Running App                                       */
/* 0x080207FF :    120K                                            */
/*******************************************************************/
/* 0x08020800 :   Iap App                                          */
/* 0x0803E7FF :    120K                                            */
/*******************************************************************/
/* 0x0803E800 :  Reserved                                          */
/* 0x0807E7FF :    256K                                            */
/*******************************************************************/
/* 0x0807E800 :  Iap infomation                                    */
/* 0x0807EFFF :     2K  (32 byte each block, last byte is crc8)    */
/*******************************************************************/
/* 0x0807F000 :    Reserved for configuration                      */
/* 0x0807F7FF :     2K                                             */
/*******************************************************************/
/* 0x0807F800 :    Mac Addr and other local infomation             */
/* 0x0807FFFF :     2K  (32 byte each block, last byte is crc8)    */
/*******************************************************************/


// for Mac Addr and other local infomation
/*******************************************************************/
/* 0-3:   device id                                                */
/* 4-7:   platform id                                              */
/* 8-11:  server ip                                                */
/* 12-13: server port                                              */
/* 14-30: other configuration                                      */
/* 31:    crc8                                                     */
/*******************************************************************/



void ParaFlash_Init(u32 rom_flashsize);

// loopasktime: unit s, for ask new version, 120 -> 60000 s
// interreqms: unit ms, for data request interval time, 300 -> 30000 ms
// mmver: main version,  vver: vice version
void AirIap_Init(u8 enablertc, void *pfunc4g, void *pfuncwl, u16 loopasktime, u16 interreqms, 
    u8 reqtaskprio, u8 dattaskprio, u8 mver, u8 vver, u8 reqstack, u8 datstack, u32 *guid);

void Sys_Soft_Reset(void);
void RcvIapData(u8 cmd, u16 seq, u8 *pbuf, u16 len, u32 saddr, u32 daddr, u32 srcid, u8 frmsrc);

u8 WriteLocalConfig(u8 *buffer);
u8 ReadLocalConfig(u8 *retbuf);
u8 ClearLocalConfig(void);

#endif
