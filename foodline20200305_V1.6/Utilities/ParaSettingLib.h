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


typedef enum
{
	MANUAL_STOP_GEARS = 0,
	MANUAL_START_GEARS,
	AUTO_GEARS,
}CONTROL_GEARS;

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
	DEVICE_NAME_VICE_FLINE,			// 副料线驱动
    DEVICE_NAME_MAX,
}DEVICE_NAME;

typedef enum
{
    DEVICE_AREA_NONE = 0,
    DEVICE_AREA_C,
    DEVICE_AREA_E,
    DEVICE_AREA_N,
    DEVICE_AREA_S,
    DEVICE_AREA_W,
    DEVICE_AREA_MAX,
}DEVICE_AREA;
typedef enum
{
    DEVICE_STATE_CLOSE = 1,
    DEVICE_STATE_OPEN,
    DEVICE_STATE_AREA,
}DEVICE_C_STATE;
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
    //DevicePara dPara[LINK_AREA_NUMBERS][LINK_DEVICE_NUMBERS];
   // u8  how[LINK_AREA_NUMBERS];                  // 每个区域的设备个数
    u8  index;                              // 第几个区域，0表示第1条
}LinkParameter;

typedef struct
{
    DevicePara rcPara[PATH_PER_INDEX];
    u8 path;
    DevicePara readDevice;
}ReadPath;

typedef enum
{
    SWITCH_VALVE_PARA_FREE = 0,
    SWITCH_VALVE_PARA_OPEN,
    SWITCH_VALVE_PARA_CLOSE,
    SWITCH_VALVE_PARA_OPENING,
    SWITCH_VALVE_PARA_CLOSING,
    SWITCH_VALVE_PARA_OPENALARM,
    SWITCH_VALVE_PARA_CLOSEALARM,
}SWITCH_VALVE_PARA;             // 测试被修改过，使用时需要改回

typedef enum
{
    FOODLINE_PARA_CLOSE = 0,
    FOODLINE_PARA_OPEN,
    FOODLINE_PARA_JUSTSTART,    // 刚启动
    FOODLINE_PARA_OVERTIME,
}FOODLINE_PARA;
typedef enum
{
    S1_FOOD_LINE_TIME = 0,
    S2_FOOD_LINE_TIME,
    C1_FOOD_LINE_TIME,    // 刚启动
    C2_FOOD_LINE_TIME,
	E1_FOOD_LINE_TIME,
	E2_FOOD_LINE_TIME,
	W1_FOOD_LINE_TIME,
	W2_FOOD_LINE_TIME,
	N1_FOOD_LINE_TIME,
	N2_FOOD_LINE_TIME
}FOOD_LINE_TIME_INDEX;

typedef enum
{
    VICE_FOODLINE_PARA_CLOSE = 0,
    VICE_FOODLINE_PARA_OPEN,
    VICE_FOODLINE_PARA_JUSTSTART,    // 刚启动
    VICE_FOODLINE_PARA_OVERTIME,
}VICE_FOODLINE_PARA;

typedef enum
{
    TOWERSOUT_PARA_SOTP = 0,
    TOWERSOUT_PARA_FOREWARD,
    TOWERSOUT_PARA_REVERSAL,
    TOWERSOUT_PARA_FOREWARDING,
    TOWERSOUT_PARA_REVERSALING,
    TOWERSOUT_PARA_FOREWARD_OVERTIME,
    TOWERSOUT_PARA_REVERSAL_OVERTIME,
}TOWERSOUT_PARA;
typedef enum
{
    TOWERSOUT_CONTROL_SOTP = 0,
    TOWERSOUT_CONTROL_FOREWARD,
    TOWERSOUT_CONTROL_REVERSAL,
}TOWERSOUT_CONTROL;

typedef struct
{
    DevicePara cDevice;
	DevicePara prevDevice;   //关联设备的ID
    bool isSelect ;
    bool isCommAlarm;     //通讯故障告警标志位
	bool isMyself;		//是否属于本机控制区域	
    u8   cAlarm;
    u8   cState;        // 图标位置
    u16  time;          // 多少s后停止
	u32  startStopTime;  // 开始停止时间
    u8   prevGetAlarm;      // 关联设备发出的警告
    u8   commTimes;     // 发送次数
    u8   stateByte;     // 设备发来的状态字节
    u8   alarmByte;     // 设备发来的报警字节
    u8   RcControlAlarm;  // 命令结果:  1 设置成功。   2 设置失败。3 手动状态设置失败。  4  料位器打料料位器通讯失败通讯失败。     5 单口双蛟龙不能反转。
    BitOperation onoff;   //    三通的开关状态
    u8 manualAuto;   // 手自动状态
	u8 rotationDirection;		//蛟龙电机的正反转的控制标志
}AllTheControlPara; // 
#define SING_LINK_DEVICE_TOTAL_NUMBER             (40)    // 单条线路共40个设备
#define AREA_DEVICE_TOTAL_NUMBER             (5)    // 单条线路共40个设备

//PathPara *PathParaGet(void);
PathParameter *PathParameterGet(void);
LinkParameter *LinkParameterGet(void);
DevicePara *RcDeviceParaGet(void);
ReadPath *ReadPathGet(void);
u8 PathTrav(u32 id);
u8 IndexTrav(u8 path, u32 id);
AllTheControlPara *AllTheControlParaGet(u8 area ,u8 index);
DevicePara *PageXIdGet(void);
void PageXIdSet(DevicePara index);


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


typedef struct
{
    u8  control;
    u16 overtime;
    u8  rcAlarm; // 命令结果:  1 设置成功。       2 设置失败。3 手动状态设置失败。              4  料位器打料料位器通讯失败通讯失败。                  5 单口双蛟龙不能反转。
    u16 currentA;
    u16 currentB;
    u16 currentC;
    u8  motorState;
    BitOperation alarm;
    u8 manualAuto;
}TowersOutControl;
#define TOWERS_OUT_RC_DATA_LEN                              (9)  

typedef struct
{
    BitOperation shield;     // 告警开关屏蔽位 Bit0:开盖告警 Bit1:调节轮告警 Bit2:过流告警 Bit3:缺相告警 [1,0] = [使能，失能]
    u8  stateTime;          // 开关状态滤波时间
    u8  motorMintime;       // 电机启停最小间隔
    u16 overCurMax;         // 过流判断最大阈值
    u16 missPhaseMin;       // 缺相判断最小值
    u8  curAlarmFilTime;    // 电流告警滤波时间
    u16 curCalib;           // 电流校准系数
    u8  motorOnFilTime;     // 电机开启过滤时间
    u8  feedFilTime;        // 下料判断滤波时间
    u8  motorState;         // 变频双绞龙状态
}TowersOutDevicePara;
#define TOWERS_OUT_DEVICE_RC_DATA_LEN                              (13)


FoodlineDevicePara *FoodlineDeviceParaGet(void);
FoodlineControl *FoodlineControlGet(void);
SwitchValveControl *SwitchValveControlGet(void);
SwitchValvePara *SwitchValveParaGet(void);
TowersOutControl *TowersOutControlGet(void);
TowersOutDevicePara *TowersOutDeviceParaGet(void);
void ControlPareState(u8 deciceArea, u8 i);
u16 *FoodLineTimeGet(u8 index);

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// MassStorage
//////////////////////////////////////////////////////////////

void ParaMassStorageInit(void);
void ParaPathParaSave(void);
void ParaPathParaRead(void);
void ParaLinkParaSave(void);
void ParaLinkParaRead(void);
void ParaDelayParaSave(void);
void ParaDelayParaRead(void);
//////////////////////////////////////////////////////////////
// 刷新屏幕使用参数
//////////////////////////////////////////////////////////////
typedef struct
{
    u8 count;           // 刷新次数
}ParaRefresh;
ParaRefresh *ParaRefreshGet(void);

//////////////////////////////////////////////////////////////
// 功能码0x36 0x38
//////////////////////////////////////////////////////////////
void ParaAnswer(u32 id);
//////////////////////////////////////////////////////////////

#endif // _PARASETTINGLIB_H_















