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

#define FOODLINE_STATE                          (0x3200)
#define DEVICE_CMD_PARA_CONFIG                          (0x39)
#define DEVICE_CMD_PATH_CONFIG                          (0x32)
#define DEVICE_CMD_READ_PATH                            (0x33)
#define DEVICE_CMD_FLINE_SET                            (0x36)
#define DEVICE_CMD_FLINE_READ                           (0x38)
                                
#define ICO_SHOW								(1)
#define ICO_HIDE								(0)


#define BigtoLittle16(A)   (( ((u16)(A) & 0xff00) >> 8)|	\
                                (( (u16)(A) & 0x00ff) << 8))  

typedef union
{
    u8 buf;
    struct
    {
        u8 b0:          1;
        u8 b1:          1;
        u8 b2:          1;
        u8 b3:          1;
        u8 b4:          1;
        u8 b5:          1;
        u8 b6:          1;
        u8 b7:          1;
    }b;
}BitOperation;


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

void ParaSettingSendData(u32 srcaddr, u32 dstaddr);
WirlessPara *WirlessParaGet(void);
/////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// path
//////////////////////////////////////////////////////////////
#define PATH_NUMBERS                    (50)    // 多少条路径
#define PATH_PER_INDEX                  (40)    // 一条路径最多40个设备

typedef enum
{
    DEVICE_NAME_NONE = 0,
    DEVICE_NAME_FLINE,              // 料线驱动
    DEVICE_NAME_IN_TOWERS,          // 料仓落料
    DEVICE_NAME_TOWERS_OUT,         // 料塔输料
    DEVICE_NAME_MAIN_VICE,          // 主副闸
    DEVICE_NAME_CONTROL,            // 主机
    DEVICE_NAME_RELAY,              // 信号中继
    DEVICE_NAME_MAX,
}DEVICE_NAME;

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
    struct
    {
        u8 rc0:         8;
        u8 rc1:         8;
        u8 rc2:         8;
        u8 rc3:         8;
    }placeRc;
}DevicePara;

typedef struct
{
    u8 next;    // 0xAA表示等待应答,未被置0则认为通信失败
    u8 path;    // 错误的路径
    u32 id;     // 错误的ID
}PathAlarmPara;

typedef struct
{
    DevicePara dPara[PATH_NUMBERS][PATH_PER_INDEX];
    u8  how[PATH_NUMBERS];                  // 每条路径的设备个数
    u8  index;                              // 第几条路径，0表示第1条
    PathAlarmPara  alarm;
}PathParameter;

typedef struct
{
    DevicePara rcPara[PATH_PER_INDEX];
    u8 path;
    DevicePara readDevice;
}ReadPath;

//PathPara *PathParaGet(void);
PathParameter *PathParameterGet(void);
DevicePara *RcDeviceParaGet(void);
ReadPath *ReadPathGet(void);
u8 PathTrav(u32 id);


//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// foodline device config 料线驱动配置参数
//////////////////////////////////////////////////////////////


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
#define FOODLINEDEVICE_RC_DATA_LEN                         (13)

typedef struct
{
    u8 control;
    u8 sensorSelect;
    u16 overtime;
    u8 rcAlarm; // 命令结果:  1 设置成功。       2 设置失败。3 手动状态设置失败。              4  料位器打料料位器通讯失败通讯失败。                  5 单口双蛟龙不能反转。
    u16 currentA;
    u16 currentB;
    u16 currentC;
    u8 state;
    BitOperation sensorState;
    BitOperation sensorComm;
    BitOperation alarm;
    u8 manualAuto;
}FoodlineControl;
#define FOODLINECONTROL_RC_DATA_LEN                         (11)


typedef struct
{
    BitOperation alarm;     // 告警开关屏蔽位 Bit0：阀开异常 Bit1：阀关异常 [1,0]=[使能，失能]
    u8 stateTime;           // 开关状态滤波时间 单位0.1S默认0.5S 0.1~5s
    u16 openOvertime;        // 阀开超时时间 单位S 默认200 0~255s
}SwitchValvePara;
#define SWITCHVALVEPARA_RC_DATA_LEN                         (4)

typedef struct
{
    u8 control;
    u8 rcAlarm; // 命令结果:  1 设置成功。       2 设置失败。3 手动状态设置失败。              4  料位器打料料位器通讯失败通讯失败。                  5 单口双蛟龙不能反转。
    BitOperation onoff;
    u8 tcock;
    BitOperation alarm;
    u8 manualAuto;
}SwitchValveControl;
#define SVALVECONTROL_RC_DATA_LEN                           (4)

FoodlineDevicePara *FoodlineDeviceParaGet(void);
FoodlineControl *FoodlineControlGet(void);
SwitchValveControl *SwitchValveControlGet(void);
SwitchValvePara *SwitchValveParaGet(void);

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// MassStorage
//////////////////////////////////////////////////////////////

void ParaMassStorageInit(void);
void ParaPathParaSave(void);
void ParaPathParaRead(void);
//////////////////////////////////////////////////////////////
// 刷新屏幕使用参数
//////////////////////////////////////////////////////////////
typedef struct
{
    u8 count;           // 刷新次数
}ParaRefresh;
ParaRefresh *ParaRefreshGet(void);
//////////////////////////////////////////////////////////////

#endif // _PARASETTINGLIB_H_















