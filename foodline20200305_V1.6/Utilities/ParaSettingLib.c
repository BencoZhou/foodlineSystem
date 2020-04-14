/*****************************************************************
 * File: ParaSettingLib.c
 * Date: 2018/03/28 14:53
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "ParaSettingLib.h"

static u16 FoodLineTime[10];


void S1S2ParaInit(void);


static ScrInf gScrInf;
static WirlessPara gWirlessPara;



void ParaSettingLibInit(void)
{
	memset(&gScrInf, 0, sizeof(ScrInf));
    gScrInf.lastPage = 4;
	gScrInf.currentPage = 4;	// 初始界面为第1页
	ParaMassStorageInit();
    S1S2ParaInit();
}

ScrInf *ScrInfGet(void)
{
	return &gScrInf;
}	 	

//////////////////////////////////////////////////////////////
// send
//////////////////////////////////////////////////////////////
void ParaSettingSendData(u32 srcaddr, u32 dstaddr)
{
    if(gWirlessPara.len >= SEND_SIZE)
        return;
    gWirlessPara.buffer[5] |= 0x80;
    
	WirelessApp_SendData(gWirlessPara.cmd, FRAME_NEED_NO_ACK, gWirlessPara.buffer, gWirlessPara.len, srcaddr, dstaddr, dstaddr, 0);
}

WirlessPara *WirlessParaGet(void)
{
    return &gWirlessPara;
}

//////////////////////////////////////////////////////////////
// path
//////////////////////////////////////////////////////////////
//static PathPara gPathPara;
static PathParameter gPathParameter;
static LinkParameter gLinkParameter;   //建立链表缓存器
static DevicePara gRcDevicePara;
static ReadPath gReadPath;
AllTheControlPara gAllTheControlPara[AREA_DEVICE_TOTAL_NUMBER][SING_LINK_DEVICE_TOTAL_NUMBER];

void S1S2ParaInit(void)
{
//    u8 i, index;

//    for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
//    {
//        gAllTheControlPara[0][i].cDevice.placeNew.device = FOODLINE_STATE;
//    }
//    index = 0;

}

void ControlPareState(u8 deciceArea , u8 i)
{
	if(AllTheControlParaGet((deciceArea - 1),i)->cDevice.place.type == DEVICE_NAME_FLINE
		||AllTheControlParaGet((deciceArea - 1),i)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)
	{
		if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == FOODLINE_PARA_CLOSE)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_CLOSE;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == FOODLINE_PARA_OPEN)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == FOODLINE_PARA_JUSTSTART)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_CLOSE;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == FOODLINE_PARA_OVERTIME)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_AREA;
		
		if(AllTheControlParaGet((deciceArea - 1),i)->isCommAlarm )         //
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 5;          // 通讯故障
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x01)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 1;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x02)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 2;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x04)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 3;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x08)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 4;
		else if(AllTheControlParaGet((deciceArea - 1),i)->manualAuto != AUTO_GEARS)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 6;                 
		else 
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 0;
	}
	else if(AllTheControlParaGet((deciceArea - 1),i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS 
		|| AllTheControlParaGet((deciceArea - 1),i)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
	{
		if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == SWITCH_VALVE_PARA_FREE)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_CLOSE;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == SWITCH_VALVE_PARA_OPEN)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == SWITCH_VALVE_PARA_CLOSE)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_CLOSE;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == SWITCH_VALVE_PARA_OPENING)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == SWITCH_VALVE_PARA_CLOSING)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == SWITCH_VALVE_PARA_OPENALARM)
		{
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_AREA;
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 1;   // 阀开异常
		}
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == SWITCH_VALVE_PARA_CLOSEALARM)
		{
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_AREA;
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 2;    // 阀关异常
		}
		
		if(AllTheControlParaGet((deciceArea - 1),i)->isCommAlarm )         //
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 3;          // 通讯故障            
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x01)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 1;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x02)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 2;
		else if(AllTheControlParaGet((deciceArea - 1),i)->manualAuto != AUTO_GEARS)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 4;                 
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte != SWITCH_VALVE_PARA_OPENALARM&&
				AllTheControlParaGet((deciceArea - 1),i)->stateByte != SWITCH_VALVE_PARA_CLOSEALARM )
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 0;
	}
	else if(AllTheControlParaGet((deciceArea - 1),i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
	{
		if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == TOWERSOUT_PARA_SOTP)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_CLOSE;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == TOWERSOUT_PARA_FOREWARD)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == TOWERSOUT_PARA_REVERSAL)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == TOWERSOUT_PARA_FOREWARDING)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == TOWERSOUT_PARA_REVERSALING)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_OPEN;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == TOWERSOUT_PARA_FOREWARD_OVERTIME)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_AREA;
		else if(AllTheControlParaGet((deciceArea - 1),i)->stateByte == TOWERSOUT_PARA_REVERSAL_OVERTIME)
			AllTheControlParaGet((deciceArea - 1),i)->cState = DEVICE_STATE_AREA;

		if(AllTheControlParaGet((deciceArea - 1),i)->isCommAlarm )         //
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 7;          // 通讯故障            
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x01)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 1;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x02)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 2;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x04)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 3;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x08)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 4;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x10)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 5;
		else if(AllTheControlParaGet((deciceArea - 1),i)->alarmByte & 0x20)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 6;
		else if(AllTheControlParaGet((deciceArea - 1),i)->manualAuto != AUTO_GEARS)
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 8;            
		else 
			AllTheControlParaGet((deciceArea - 1),i)->cAlarm = 0;
	}
}
u16 *FoodLineTimeGet(u8 index)
{
	return &FoodLineTime[index];
}
AllTheControlPara *AllTheControlParaGet(u8 area ,u8 index)
{
    return &gAllTheControlPara[area][index];
}

u8 PathTrav(u32 id)
{
    u8 path, i;
    for(path = 0; path < PATH_NUMBERS; path++)
    {
        for(i = 0; i < PATH_PER_INDEX; i++)
        {
            if(gPathParameter.dPara[path][i].placeNew.useID != 0)
            {
                if(id == gPathParameter.dPara[path][i].id)
                    return path;
            }
            else
            {
                break;
            }
        }
    }
    return PATH_NUMBERS;
}
u8 IndexTrav(u8 path, u32 id)
{
    u8  i;
    if(path >= PATH_NUMBERS)
    {
        return PATH_PER_INDEX;
    }
    for(i = 0; i < PATH_PER_INDEX; i++)
    {
        if(gPathParameter.dPara[path][i].placeNew.useID != 0)
        {
            if(id == gPathParameter.dPara[path][i].id)
                return (i+1);
        }
        else
        {
            break;
        }
    }   
    return PATH_PER_INDEX;
}

PathParameter *PathParameterGet(void)
{
    return &gPathParameter;
}
LinkParameter *LinkParameterGet(void)
{
    return &gLinkParameter;
}
DevicePara *RcDeviceParaGet(void)
{
    return &gRcDevicePara;
}

ReadPath *ReadPathGet(void)
{
    return &gReadPath;
}

//////////////////////////////////////////////////////////////
// foodline device config 料线驱动配置参数
//////////////////////////////////////////////////////////////

static FoodlineDevicePara gFoodlineDevicePara;
static FoodlineControl gFoodlineControl;
static SwitchValveControl gSwitchValveControl;
static SwitchValvePara gSwitchValvePara;
static TowersOutControl gTowersOutControl;
static TowersOutDevicePara gTowersOutPara;
static DevicePara PageXId;

FoodlineDevicePara *FoodlineDeviceParaGet(void)
{
    return &gFoodlineDevicePara;
}

FoodlineControl *FoodlineControlGet(void)
{
    return &gFoodlineControl;
}

SwitchValveControl *SwitchValveControlGet(void)
{
    return &gSwitchValveControl;
}

SwitchValvePara *SwitchValveParaGet(void)
{
    return &gSwitchValvePara;
}

TowersOutControl *TowersOutControlGet(void)
{
    return &gTowersOutControl;
}

TowersOutDevicePara *TowersOutDeviceParaGet(void)
{
    return &gTowersOutPara;
}

//设置查询页面的ID号
void PageXIdSet(DevicePara index)
{
      PageXId = index;
}
//设置查询页面的ID号
DevicePara *PageXIdGet(void)
{
     return &PageXId;
}


//////////////////////////////////////////////////////////////
// MassStorage
//////////////////////////////////////////////////////////////
typedef struct 
{
	u16	version;
 	u16 magicNumber;
}ParaMassStorageHead;

typedef struct
{
    u8  scrsav_light;
    u16 scrsav_keep;
}AdminConfig;   // 本机管理，统一配置，所有猪舍都是一致的

typedef enum			// 存储位置安排
{
	MASS_STORAGE_PARA_SAVE_VERSION		= 0,
	MASS_STORAGE_PARA_SAVE_LINK 		= 1,
	MASS_STORAGE_PARA_SAVE_DELAY 		= 2,
	MASS_STORAGE_PARA_SAVE_PARAPATH     = 3,
}MASS_STORAGE_PARA_SAVE;

#define SECTOR_SIZE										(4*1024)
#define VERSON_HISTORY						            (0x0B)
#define HISTORY_MAGIC_NUMBER				            (0xA5)

static ParaMassStorageHead gHead;
//static AdminConfig gAdminConfig;

static void ParaMassStorageSaveHead(void)
{
	SPI_Flash_Write((u8*)&gHead, SECTOR_SIZE*MASS_STORAGE_PARA_SAVE_VERSION, sizeof(ParaMassStorageHead));
}

static void ParaMassStorageReadHead(void)
{
	SPI_Flash_Read((u8*)(&gHead), SECTOR_SIZE*MASS_STORAGE_PARA_SAVE_VERSION , sizeof(ParaMassStorageHead));
}

void ParaMassStorageInit(void)
{
	u8 i, j;
	memset(&gHead, 0, sizeof(ParaMassStorageHead));
	ParaMassStorageReadHead();
	if((gHead.version != VERSON_HISTORY)||(gHead.magicNumber != HISTORY_MAGIC_NUMBER))
	{
		gHead.version = VERSON_HISTORY;
		gHead.magicNumber = HISTORY_MAGIC_NUMBER;
		ParaMassStorageSaveHead();
        memset(&gPathParameter, 0, sizeof(PathParameter));
        for(i = 0; i < PATH_NUMBERS; i++)
        {
            for(j = 0; j < PATH_PER_INDEX; j++)
                gPathParameter.dPara[i][j].place.device = FOODLINE_STATE;
        }
        for(i = 0; i < AREA_DEVICE_TOTAL_NUMBER; i++)
        {
            for(j = 0; j < SING_LINK_DEVICE_TOTAL_NUMBER; j++)
                gAllTheControlPara[i][j].cDevice.place.device = FOODLINE_STATE;
        } 		
        for(i = 0; i < 10; i++)
        {
            FoodLineTime[i] = 0;
        } 			
        IWDG_Feed();
        ParaPathParaSave();
		ParaLinkParaSave();
		ParaDelayParaSave();
	}
    else
    {
    	ParaPathParaRead();
		ParaLinkParaRead();
		ParaDelayParaRead();
    }
    memset(&gReadPath, 0, sizeof(ReadPath));
    
}


void ParaDelayParaSave(void)
{
	SPI_Flash_Write((u8*)&FoodLineTime,
		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_DELAY,
		sizeof(FoodLineTime));
}
void ParaDelayParaRead(void)
{
	SPI_Flash_Read((u8*)&FoodLineTime,
		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_DELAY,
		sizeof(FoodLineTime));
}

void ParaLinkParaSave(void)
{
	SPI_Flash_Write((u8*)&gAllTheControlPara,
		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_LINK,
		sizeof(gAllTheControlPara));
}
void ParaLinkParaRead(void)
{
	SPI_Flash_Read((u8*)&gAllTheControlPara,
		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_LINK,
		sizeof(gAllTheControlPara));
}
void ParaPathParaSave(void)
{
	SPI_Flash_Write((u8*)&gPathParameter,
		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_PARAPATH,
		sizeof(PathParameter));
}

void ParaPathParaRead(void)
{
	SPI_Flash_Read((u8*)&gPathParameter,
		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_PARAPATH,
		sizeof(PathParameter));
}

//void CloseDelayParaSave(void)
//{
//	SPI_Flash_Write((u8*)&gPathParameter,
//		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_PARAPATH,
//		sizeof(PathParameter));
//}

//void CloseDelayParaRead(void)
//{
//	SPI_Flash_Read((u8*)&gPathParameter,
//		SECTOR_SIZE * MASS_STORAGE_PARA_SAVE_PARAPATH,
//		sizeof(PathParameter));
//}

//////////////////////////////////////////////////////////////
// 刷新屏幕使用参数
//////////////////////////////////////////////////////////////

static ParaRefresh gParaRefresh;
ParaRefresh *ParaRefreshGet(void)
{
    return &gParaRefresh;
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// 功能码0x36 0x38
//////////////////////////////////////////////////////////////


void ParaAnswer(u32 id)
{
    u8 index = 0;
    WirlessParaGet()->buffer[index++] = 3;
    
    WirlessParaGet()->cmd = 0x31;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(LocalDeviceIdGet(), id);
}

