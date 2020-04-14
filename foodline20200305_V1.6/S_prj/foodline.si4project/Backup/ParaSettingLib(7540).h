/*****************************************************************
 * File: ParaSettingLib.h
 * Date: 2018/03/28 14:53
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PARASETTINGLIB_H_
#define _PARASETTINGLIB_H_

#include "System.h"
                                
#define ICO_SHOW								(1)
#define ICO_HIDE								(0)


#define BigtoLittle16(A)   (( ((u16)(A) & 0xff00) >> 8)|	\
                                (( (u16)(A) & 0x00ff) << 8))  


typedef enum
{
	SYSTEM_TIME_SATE_YEAR = 1,
	SYSTEM_TIME_SATE_MON,
	SYSTEM_TIME_SATE_DAY,
	SYSTEM_TIME_SATE_HOUR,
	SYSTEM_TIME_SATE_MIN,
	SYSTEM_TIME_SATE_MAX,
}SYSTEM_TIME_SATE;

typedef struct
{
	u8 year;
	u8 mon;
	u8 day;
	u8 hour;
	u8 min;
	SYSTEM_TIME_SATE state;
}SystemTime;

typedef struct
{
	u16 currentPage;	// 表示在哪一页
	u16 lastPage;		// 上次在哪一页
	u8  lastItem;		// 上一页的第几级	从1开始1~8
	u8  currentTable;	// 当前页的第几个table 从0开始
	u8  piggeryIndexhome;	// 首页进的第几个猪舍
	u16 paraLastPage;		// 跳转到策略显示界面前的界面记录
}ScrInf;        // 当前页面显示信息

void ParaSettingLibInit(void);
ScrInf *ScrInfGet(void);
//////////////////////////////////////////////////////////////
// send
//////////////////////////////////////////////////////////////
#define SEND_SIZE						(87)

typedef struct
{
    u8 len;
    u8 buffer[SEND_SIZE];
    u8 cmd;
}WirlessPara;

void ParaSettingSendData(u32 srcaddr, u32 dstaddr, u8 len);
WirlessPara *WirlessParaGet(void);
/////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// path
//////////////////////////////////////////////////////////////
#define PATH_NUMBERS                    (50)    // 多少条路径
#define PATH_PER_INDEX                  (40)    // 一条路径最多40个设备

typedef struct
{
    u8  type[PATH_NUMBERS][PATH_PER_INDEX];
    u8  area[PATH_NUMBERS][PATH_PER_INDEX];
    u8  mainLine[PATH_NUMBERS][PATH_PER_INDEX];
    u8  viceLine[PATH_NUMBERS][PATH_PER_INDEX];
    u32 id[PATH_NUMBERS][PATH_PER_INDEX];   // 路径统计
    u8  how[PATH_NUMBERS];                  // 每条路径的设备个数
    u8  index;                              // 第几条路径，0表示第1条
}PathPara;

typedef union
{
    u32 id;
    struct
    {
        u32 viceLine:   7;
        u32 mainLine:   3;
        u32 area:       3;
        u32 type:       3;
        u32 device:     16;         // 固定为0x3200
    }place;
    struct
    {
        u32 useID:      16;         // 路由协议中使用的ID
        u32 device:     16;         // 固定为0x3200
    }placeNew;
}DevicePara;

typedef struct
{
    DevicePara dPara[PATH_NUMBERS][PATH_PER_INDEX];
    u8  how[PATH_NUMBERS];                  // 每条路径的设备个数
    u8  index;                              // 第几条路径，0表示第1条
}PathParameter;

PathPara *PathParaGet(void);
PathParameter *PathParameterGet(void);

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// foodline device config 料线驱动配置参数
//////////////////////////////////////////////////////////////

#define DEVICE_CMD_PARA_CONFIG                          (0x39)

typedef union
{
    u8 buf;
    struct
    {
        u8 openAlarm:       1;
        u8 regulatingAlarm: 1;
        u8 currentAlarm:    1;
        u8 openphaseAlarm:  1;
        u8 rev:             4;
    }alarm;
}AlarmShield;

typedef union
{
    u8 buf;
    struct
    {
        u8 numberOne:       1;
        u8 numberTwo:       1;
        u8 numberThree:     1;  // [1,0] = [使能，失能]
        u8 rev:             4;
        u8 conType:         1;  // [1,0]=[无线，有线]
    }device;
}MatLevDevice;

typedef struct
{
    AlarmShield shield;     // 告警开关屏蔽位 Bit0:开盖告警 Bit1:调节轮告警 Bit2:过流告警 Bit3:缺相告警 [1,0] = [使能，失能]
    u8  stateTime;          // 开关状态滤波时间
    u8  motorMintime;       // 电机启停最小间隔
    u16 overCurMax;         // 过流判断最大阈值
    u16 missPhaseMin;       // 缺相判断最小值
    u8  curAlarmFilTime;    // 电流告警滤波时间
    u16 curCalib;           // 电流校准系数
    u8  motorOnFilTime;     // 电机开启过滤时间
    MatLevDevice  maleDevice;   // 料位器
    u8  commSum;            // 料位器通信状态计数 3~20
}FoodlineDevicePara;

FoodlineDevicePara *FoodlineDeviceParaGet(void);

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// MassStorage
//////////////////////////////////////////////////////////////

void ParaMassStorageInit(void);
void ParaPathParaSave(void);
void ParaPathParaRead(void);
//////////////////////////////////////////////////////////////

#endif // _PARASETTINGLIB_H_















