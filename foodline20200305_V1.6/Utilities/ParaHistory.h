/*****************************************************************
 * File: ParaHistory.h
 * Date: 2018/11/21 10:01
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PARAHISTORY_H_
#define _PARAHISTORY_H_

#include "System.h"


#define Sensor_Enable_TEMPHUMI      0x0001	// 			00000001
#define Sensor_Enable_BH1750LUX     0x0002	// 			00000010
#define Sensor_Enable_AIRPRESS      0x0004	// 			00000100
#define Sensor_Enable_MICROPRES     0x0008	// 			00001000
#define Sensor_Enable_CO2           0x0010	// 			00010000
#define Sensor_Enable_NH3           0x0020	// 			00100000
#define Sensor_Enable_WINRAIN       0x0040	// 			01000000
#define Sensor_Enable_TEMP1         0x0080	// 			10000000
#define Sensor_Enable_TEMP2         0x0100	// 00000001	00000000
#define Sensor_Enable_TEMP3         0x0200	// 00000010	00000000
#define Sensor_Enable_TEMP4         0x0400	// 00000100	00000000

#define SENSOR_ENABLE_MASK_VERSION				0x10

#define HISTORY_CURVE_SAVE_NUMBER						(24)


#define SENSOR_STAR_VARS_LEN							(32)
#define NEW_SENSOR_STAR_VARS_LEN						(28)
#define OUTER_SENSOR_LEN								(24)
#define NEW_OUTER_SENSOR_LEN							(20)



typedef struct
{
	// 32 bytes
    u32 senscfg;
    
    short temp;         //*10,  -40-123.8, 0x7fff invalid
    short humi;         //*10,  0-100, 0x7fff invalid
    
    u32 bh1750_lux;     //*10,  1-65535, 0xffffffff invalid
    
    u32 result_up;      //air pressure,  300-110000, 0xffffffff invalid
    u16 msii_value;     //*10,  micro pressure,  0-1250, 0xffff invalid
    
    u16 co2_value;      //0-5000,  0xffff invalid
    u16 nh3_value;      //*10,  0-1000,  0xffff invalid
    
    u16 ventile;        // parasite for ventilation
    
    short other_temp[4];    //*10, -55-125, 0x7fff invalid, ds18b20
}SensorStatVars;

typedef struct
{
	// 28 bytes
	u32 senscfg;
	
	short temp; 		//*10,	-40-123.8, 0x7fff invalid
	short humi; 		//*10,	0-100, 0x7fff invalid
	
	u32 bh1750_lux; 	//*10,	1-65535, 0xffffffff invalid
	u16 msii_value; 	//*10,	micro pressure,  0-1250, 0xffff invalid
	
	u16 co2_value;		//0-5000,  0xffff invalid
	u16 nh3_value;		//*10,	0-1000,  0xffff invalid
	
	u16 ventile;		// parasite for ventilation
	
	short other_temp[4];	//*10, -55-125, 0x7fff invalid, ds18b20
}NewSensorStatVars;

typedef struct
{
	// 24 bytes
    u32 devid;
    
    short temp;
    short humi;
    
    u32 bh1750_lux;
    
    u32 result_up;
    
    u16 sdirect;        //wind dir, 0-360,  0xffff invalid
    u16 swind;          //*10, 0-600,  0xffff invalid
    u16 srain;          //*10, rain volume,  0xffff invalid
    
    u16 reserv;
}OuterSensor;

typedef struct
{
	// 20 bytes
	u32 devid;
	
	short temp;
	short humi;
	
	u32 bh1750_lux;
	
	u16 sdirect;		//wind dir, 0-360,	0xffff invalid
	u16 swind;			//*10, 0-600,  0xffff invalid
	u16 srain;			//*10, rain volume,  0xffff invalid
	
	u16 reserv;
}NewOuterSensor;


typedef union
{
    u16 val;
    struct {
        u16 year:7;
        u16 mon:4;
        u16 day:5;
    } t;
} VersionTime;

typedef struct
{
    VersionTime date;
    u8 hour;
    u8 min;
} LogTimeHdr;


typedef struct
{
	u8 mask;						// 掩码
	SensorStatVars gSensorStatVars[HISTORY_CURVE_SAVE_NUMBER];
	NewSensorStatVars gNewSensorStatVars[HISTORY_CURVE_SAVE_NUMBER];
	bool isHaveSensorStatVar[HISTORY_CURVE_SAVE_NUMBER];
	bool isInnerValid;
	OuterSensor gOuterSensor[HISTORY_CURVE_SAVE_NUMBER];
	NewOuterSensor gNewOuterSensor[HISTORY_CURVE_SAVE_NUMBER];
	bool isHaveOuterSensor[HISTORY_CURVE_SAVE_NUMBER];
	bool isOuterValid;
	short innerTemp[HISTORY_CURVE_SAVE_NUMBER];
	short outerTemp[HISTORY_CURVE_SAVE_NUMBER];
	short innerHumi[HISTORY_CURVE_SAVE_NUMBER];
	short outerHumi[HISTORY_CURVE_SAVE_NUMBER];
    u32 innerbh1750_lux[HISTORY_CURVE_SAVE_NUMBER];   //*10,  1-65535, 0xffffffff invalid
    u32 outerbh1750_lux[HISTORY_CURVE_SAVE_NUMBER];   //*10,  1-65535, 0xffffffff invalid
    u16 ventile[HISTORY_CURVE_SAVE_NUMBER];
	u16 co2_value[HISTORY_CURVE_SAVE_NUMBER];
	u16 nh3_value[HISTORY_CURVE_SAVE_NUMBER];	
	bool isDoneFirstWr;				// 是否接收过一天的完整数据 24条	
	u8   dataSendCnt;				// 从第0条开始
	u8   dataRcCnt;
	u8   hour[HISTORY_CURVE_SAVE_NUMBER];
	bool isError[HISTORY_CURVE_SAVE_NUMBER];
}HistoryPara;

void ParaHistoryInit(void);
void ParaHistoryFirstWr(u8 count);
void ParaHistoryHourWr(void);
void ParaHistoryDataProcess(bool isNeedInit);
HistoryPara *ParaHistoryGet(void);

#endif // _PARAHISTORY_H_
