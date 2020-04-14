#ifndef MZSYS_SENSORSTAT_HANDLER_H
#define MZSYS_SENSORSTAT_HANDLER_H


typedef struct
{
    short temp;         //*10,  -40-123.8, 0x7fff invalid
    short humi;         //*10,  0-100, 0x7fff invalid
    
    u32 bh1750_lux;     //*10,  1-65535, 0xffffffff invalid
    
    u32 result_up;      //air pressure,  300-110000, 0xffffffff invalid
    short msii_value;   //*10,  micro pressure,  0-1250, 0x7fff invalid
    
    u16 co2_value;      //0-5000,  0xffff invalid
    u16 nh3_value;      //*10,  0-1000,  0xffff invalid
    
    u16 sdirect;        //wind dir, 0-360,  0xffff invalid
    u16 swind;          //*10, 0-600,  0xffff invalid
    u16 srain;          //*10, rain volume,  0xffff invalid
    
    short air_temp;     //*10, -55-125, 0x7fff invalid
    short air_temp1;    //*10
    short air_temp2;    //*10
    short water_temp;   //*10
} SensorStat;

#endif
