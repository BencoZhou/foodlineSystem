#ifndef MZSYS_CONTROL_CONFIG_HANDLER_H
#define MZSYS_CONTROL_CONFIG_HANDLER_H



// piggery config
typedef struct
{
    u8  piggery_type;    // 0~31 猪舍类型		0空栏不能修改ID
    u8  floor_no;
    u8  piggery_no;
    u8  reserv;
    
    u16 piggery_length;
    u16 piggery_width;
    u16 piggery_height;
    u16 piggery_top;
    
    u16 pigs_number;
    u16 pigs_weight;
    u16 bars_number;
    u16 pigs_days;
    
    u32 ninesensor1;
    u32 ninesensor2;
    u32 localid;
} PiggeryConfig;


typedef struct
{
    u32 initsecs;		// 入栏当天的日期
    u8 initdayage;		// 入栏当天的日龄
    u8 reserv;
    u16 day;            // 预计饲养时间
} BasicConfig;


// policy
// -------------target for control---------------
typedef struct
{
    u8 dayage;
    u8 reserv[2];
    // interval for check(min), 1-99
    u8 interval;		// min
    u16 target;			// *10
    u16 heat;			// *10
    u16 highwarning;	// *10
    u16 lowwarning;		// *10
} TempPolicy;


typedef struct
{
    // 0-160
    u8 dayage;
    // 0-99%
    u8 target;
    // start humi value, 0-100%
    u8 startval;
    // interval for check(min), 1-99
    u8 addinterval;
    // time to continuing add humi(s), 5-600
    u16 addtime;
    // 0-99%
    u8 highwarning;
    // 0-99%
    u8 lowwarning;
} HumiPolicy;


typedef struct
{
    // 0-160
    u8 dayage;
    // interval (min), 1-99
    u8 interval;
    u8 reserv[2];
    // 0-5000
    u16 startval;
    // 0-5000
    u16 endval;
    
    // continuing time (s), reach time then wind return original level, 5-600
    u16 keeps;
    // 0-5000
    u16 highwarning;
} Co2Policy;


typedef struct
{
    // 0-160
    u8 dayage;
    // interval for check(min), 1-99
    u8 interval;
    // 0-60000
    u16 target;
    u16 highwarning;
    u16 lowwarning;
} ShinePolicy;


typedef struct
{
    // 0-25.0
    u8 tempdiff;
    // 0-99%
    u8 stopwet;
    // 0-23
    u8 start;
    u8 end;
} WetShutterPolicy;


// -------------device 1 for control---------------
typedef struct
{
    // 0-25.0
    u8 tempdiff;
    u8 volPercent;
	u16 fanPeriod;		// 风机周期，与循环轮替周期一直，3个数据联动
    // only display
    u16 volume;
    // only display, no point
    u16 addpercent;
} WindPolicy;


typedef struct
{
    u8 ctrlst;      // close, open, run with time setting, loop with time setting, adjust percent
                    // fixed fan:  0: close, 1: open, 2: always run in loop, 3-6: cycle queues for reserved                    
    u8 type;
					// 1: 2 phase fixed, 2: 3 phase fixed, 3: adjust, 4: mix fan
					// 1: 0x27		, 2: 0x28			3:0x2B	 4: 0x35
	u8 percent;     // adjust fan: percent, 0-100
    u8 reserv;
    // s, 0-300
    u16 open;
    u16 close;
} FanPolicy;


typedef struct
{
    u8 percent[4];
} WindowPolicy;


typedef struct
{
    u8 percent[4];
} ShutterPolicy;



// -------------device 3 for control---------------
typedef struct
{
    // open, close, period(s)
    // 0: close, 1: open, 2: period
    u8 ctrlst[4];
    // s, 0-300
    u16 open;
    u16 close;
} WaterPolicy;


// -------------device 4 for control---------------
typedef struct
{
    // 0-99%
    u8 wetdiff;
    u8 reserv;
    // valid period, 0-23
    u8 start;
    u8 end;
    // open, close
    u8 sprayst[4];
} SprayPolicy;


// -------------device 5 for control---------------
typedef struct
{
    // 0-25.0
    u8 tempdiff;
    // min, 0-99
    u8 interval;
    // s, 0-999
    u16 keep;
    // percent
    u8 heatst[4];
} HeatPolicy;


// -------------device 6 for control---------------
typedef struct
{
    // 0-60000
    u16 shinediff;
    u8 reserv[2];
    // percent
    u8 shine[4];
} LightPolicy;


// -------------device 7 for control---------------
typedef struct
{
    // 0-25.0
    u8 tempdiff;
    // min, 0-99
    u8 interval;
    // s, 600
    u16 keep;
    u8 fans[4];
} MixFanPolicy;


// -------------device 8 for control---------------
typedef struct
{
    // 0 - 2: vice foodline, 3: main foodline
    // 0-2359
    u16 starttime[4];
    u8 valid[4];
    u8 delay;
    u8 reserv[3];
} FoodlinePolicy;


// -------------device 9 for control---------------
typedef struct
{
    // 0-2359
    u16 starttime[4];
    u8 valid[4];
} ScrapePolicy;


// -------------device configuration---------------
typedef struct
{
    u32 fanid;
    u16 power;
    u16 current;            //  *10
    // 1: 2 phase fixed, 2: 3 phase fixed, 3: adjust, 4: mix fan
    // 1: 0x27		, 2: 0x28			3:0x2B	 4: 0x35
    u8 type;
    u8 valid;
    u16 volume;
} FansConf;


typedef struct
{
    u32 id;
    // foodline : 1: main  2: vice
    // scrape: reservd
    // window: 1, push window, 2: slide window
    // others: reservd
    u8 prop;
    u8 valid;
    u8 reserv[2];
} CommDevConf;

typedef struct {
    u16 year:7;
    u16 mon:4;
    u16 day:5;
    u8 hour;
    u8 min;
    u8 sec;
    u8 floord;
    u8 piggery;
    u8 number;          // environment control device no.
                        // for web, 255, for app, 254
} PolicyVersion;

typedef struct
{
    u16 totlength;      // length include this struct
    u8 crc8;            // crc8 = struct + content
    u8 reserv;
    
    PolicyVersion ver;
    
    PiggeryConfig config;
    BasicConfig basic;
    
    // device
    u8 fansid_count;
    
    // common device id
    u8 windowid_count;
    u8 shutterid_count;
    
    u8 waterid_count;
    u8 sprayid_count;
    u8 heatid_count;
    u8 lightid_count;
    
    u8 foodlineid_count;
    u8 scrapesid_count;
    
    u8 reserv2;
    
    // common policy
    // TempPolicy
    u8 temp_count;
    // HumiPolicy
    u8 humi_count;
    // Co2Policy
    u8 co2_count;
    // WetShutterPolicy
    u8 wetshutter_count;
    
    // ShinePolicy
    u8 shine_count;
    // WindPolicy
    u8 wind_count;
    
    // FanPolicy
    u8 fan_pol_count;	// = wind_count*fans_per_pol
    u8 fans_per_pol;
    // WindowPolicy
    u8 window_count;
    // ShutterPolicy
    u8 shutter_count;
    
    // WaterPolicy
    u8 water_count;
    // SprayPolicy
    u8 spray_count;
    // HeatPolicy
    u8 heat_count;
    // LightPolicy
    u8 light_count;
    // MixFanPolicy
    u8 mixfan_count;
    // FoodlinePolicy
    u8 foodline_count;
    // ScrapePolicy
    u8 scrape_count;
    
    u8 reserv3[3];
    
    // offset
    u16 devs_id_offset;     // common device id
    u16 temp_pol_offset;    // common policy
    u16 humi_pol_offset;
    u16 co2_pol_offset;
    u16 wetshutter_pol_offset;
    u16 shine_pol_offset;
    u16 wind_pol_offset;
    
    u16 fans_pol_offset;
    u16 window_pol_offset;
    u16 shuttle_pol_offset;
    u16 water_pol_offset;
    u16 spray_pol_offset;
    u16 heat_pol_offset;
    u16 light_pol_offset;
    u16 mixfan_pol_offset;
    u16 foodline_pol_offset;
    u16 scrape_pol_offset;
} PolicyStats;
/******************************************Fault Error**************************************************/

// for device
#define DevError_Comm_Abort     0x01
#define DevError_Current_Low    0x02

// for N sensor
#define DevError_NSensor_TEMPHUMI       0x01
#define DevError_NSensor_BH1750LUX      0x02
#define DevError_NSensor_CO2            0x04
#define DevError_NSensor_NH3            0x08
#define DevError_NSensor_MICROPRES      0x10
#define DevError_NSensor_TEMPS          0x20
#define DevError_NSensor_AIRPRESS       0x40

// for weather site

/******************************************End Fault Error**********************************************/

#endif
