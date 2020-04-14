/*****************************************************************
 * File: ParaSettingLib.c
 * Date: 2018/03/28 14:53
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "ParaSettingLib.h"


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
static DevicePara gRcDevicePara;
static ReadPath gReadPath;
static S1S2ControlPara gS1S2ControlPara[S1S2DEVICE_TOTAL_NUMBER];

void S1S2ParaInit(void)
{
    u8 i, index;
    memset(&gS1S2ControlPara[0], 0, S1S2DEVICE_TOTAL_NUMBER*sizeof(S1S2ControlPara));
    for(i = 0; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
        gS1S2ControlPara[i].cDevice.placeNew.device = FOODLINE_STATE;
    }
    index = 0;
    gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_TOWERS_OUT;
    gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
    gS1S2ControlPara[index].cDevice.place.mainLine = 1;
    gS1S2ControlPara[index].cDevice.place.viceLine = 10;
    index += 1;
    gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_TOWERS_OUT;
    gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
    gS1S2ControlPara[index].cDevice.place.mainLine = 1;
	gS1S2ControlPara[index].cDevice.place.viceLine = 20;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_FLINE;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 1;
	gS1S2ControlPara[index].cDevice.place.viceLine = 0;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_MAIN_VICE;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 1;
	gS1S2ControlPara[index].cDevice.place.viceLine = 20;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_FLINE;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 2;
	gS1S2ControlPara[index].cDevice.place.viceLine = 0;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_IN_TOWERS;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 2;
	gS1S2ControlPara[index].cDevice.place.viceLine = 10;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_IN_TOWERS;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 2;
	gS1S2ControlPara[index].cDevice.place.viceLine = 20;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_IN_TOWERS;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 2;
	gS1S2ControlPara[index].cDevice.place.viceLine = 30;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_IN_TOWERS;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 2;
	gS1S2ControlPara[index].cDevice.place.viceLine = 40;
    index += 1;
	gS1S2ControlPara[index].cDevice.place.type = DEVICE_NAME_IN_TOWERS;
	gS1S2ControlPara[index].cDevice.place.area = DEVICE_AREA_S;
	gS1S2ControlPara[index].cDevice.place.mainLine = 2;
	gS1S2ControlPara[index].cDevice.place.viceLine = 50;
}

S1S2ControlPara *S1S2ControlParaGet(u8 index)
{
    return &gS1S2ControlPara[index];
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

PathParameter *PathParameterGet(void)
{
    return &gPathParameter;
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
	MASS_STORAGE_PARA_SAVE_PIGGERY_PARA = 1,
	MASS_STORAGE_PARA_SAVE_PARAPATH     = 3,
}MASS_STORAGE_PARA_SAVE;

#define SECTOR_SIZE										(4*1024)
#define VERSON_HISTORY						            (0x0A)
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
        IWDG_Feed();
        ParaPathParaSave();
	}
    else
    {
    	ParaPathParaRead();
    }
    memset(&gReadPath, 0, sizeof(ReadPath));
    
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

