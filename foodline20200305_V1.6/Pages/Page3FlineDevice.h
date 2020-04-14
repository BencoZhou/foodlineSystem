/*****************************************************************
 * File: Page3FlineDevice.h
 * Date: 2019/11/25 16:32
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE3FLINEDEVICE_H_
#define _PAGE3FLINEDEVICE_H_

#include "System.h"

#define PAGE3_OPENALARM_SHOW                        (0x0300)
#define PAGE3_REGULATINGALARM_SHOW                  (0x0301)
#define PAGE3_CURRENTALARM_SHOW                     (0x0302)
#define PAGE3_OPENPHASEALARM_SHOW                   (0x0303)
#define PAGE3_OPENALARM_BUTTON                      (0x0304)
#define PAGE3_REGULATINGALARM_BUTTON                (0x0305)
#define PAGE3_CURRENTALARM_BUTTON                   (0x0306)
#define PAGE3_OPENPHASEALARM_BUTTON                 (0x0307)
#define PAGE3_STATETIME                             (0x0308)
#define PAGE3_MOTORMINTIME                          (0x0309)
#define PAGE3_OVERCURMAX                            (0x030A)
#define PAGE3_MISSPHASEMIN                          (0x030B)
#define PAGE3_CURALARMFILTIME                       (0x030C)
#define PAGE3_CURCALIB                              (0x030D)
#define PAGE3_MOTORONFILTIME                        (0x030E)
#define PAGE3_CONTYPE_SHOW                          (0x0310)
#define PAGE3_NUMBERTHREE_SHOW                      (0x0311)
#define PAGE3_NUMBERTWO_SHOW                        (0x0312)
#define PAGE3_NUMBERONE_SHOW                        (0x0313)
#define PAGE3_CONTYPE_BUTTON                        (0x0314)
#define PAGE3_NUMBERTHREE_BUTTON                    (0x0315)
#define PAGE3_NUMBERTWO_BUTTON                      (0x0316)
#define PAGE3_NUMBERONE_BUTTON                      (0x0317)
#define PAGE3_COMMSUM                               (0x0318)

#define PAGE3_TYPE1                                 (0x0320)	// 
#define PAGE3_AREA1                                 (0x0321)
#define PAGE3_MAIN_LINE1                            (0x0322)
#define PAGE3_VICE_LINE1                            (0x0323)
#define PAGE3_ID1                                   (0x0330)	// id 

#define PAGE3_INQUIRY                               (0x0324)
#define PAGE3_SEND                                  (0x0325)


void Page3FlineDeviceInit(void);
void Page3FlineDeviceProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page3FlineDeviceRefresh(void);

#endif // _PAGE3FLINEDEVICE_H_

