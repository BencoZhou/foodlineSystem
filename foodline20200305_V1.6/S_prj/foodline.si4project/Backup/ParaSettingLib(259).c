/*****************************************************************
 * File: ParaSettingLib.c
 * Date: 2018/03/28 14:53
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "ParaSettingLib.h"
static ScrInf gScrInf;
static WirlessPara gWirlessPara;

void ParaSettingLibInit(void)
{
	memset(&gScrInf, 0, sizeof(ScrInf));
    gScrInf.lastPage = 4;
	gScrInf.currentPage = 4;	// 初始界面为第1页
	ParaMassStorageInit();
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

//PathPara *PathParaGet(void)
//{
//    return &gPathPara;
//}

PathParameter *PathParameterGet(void)
{
    return &gPathParameter;
}

DevicePara *RcDeviceParaGet(void)
{
    return &gRcDevicePara;
}

//////////////////////////////////////////////////////////////
// foodline device config 料线驱动配置参数
//////////////////////////////////////////////////////////////

static FoodlineDevicePara gFoodlineDevicePara;

FoodlineDevicePara *FoodlineDeviceParaGet(void)
{
    return &gFoodlineDevicePara;
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
#define VERSON_HISTORY						            (0x07)
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
//        memset(&gPathPara, 0, sizeof(PathPara));
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


