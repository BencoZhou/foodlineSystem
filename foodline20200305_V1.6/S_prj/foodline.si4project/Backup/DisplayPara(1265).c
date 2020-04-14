/*****************************************************************
 * File: DisplayPara.c
 * Date: 2018/03/25 15:04
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:DisplayParaList
 *
 ******************************************************************/
#include "DisplayPara.h"
#include "ParaHistory.h"
#include "Page1flinePathConfig.h"
#include "Page2PathConfig.h"
#include "Page3FlineDevice.h"
#include "Page5Revise.h"
#include "Page6ReadPathConfig.h"
#include "Page7ControlFline.h"
#include "Page8SwitchValve.h"
#include "Page9SwitchValveConfig.h"


void DisplayParaInit(void)
{
    DisplayCommJumpToPic(4);
}

void DisplayPageJump(u16 page, u8 item)
{		
	switch(page)
	{
		case 0x01:
			Page1FlinePathConfigInit();
			break;
        case 0x02:
            Page2PathConfigInit();
            break;
        case 0x03:
            Page3FlineDeviceInit();
            break;
        case 0x05:
            Page5ReviseInit();
            break;
        case 0x06:
            Page6PathConfigInit();
            break;
        case 0x07:
            Page7ControlFlineInit();
            break;
        case 0x08:
            Page8SwitchValveInit();
            break;
        case 0x09:
            Page9SwitchValveConfigInit();
            break;
		default:
			break;
	}
}

void DisplayPageDealProcess(u16 page, u8 reg, u16 addr, u8 *pbuf, u8 len)
{
	switch(page)
	{
		case 0x01:
			Page1FlinePathConfigProcess(reg, addr, pbuf, len);
			break;
        case 0x02:
            Page2PathConfigProcess(reg, addr, pbuf, len);
            break;
        case 0x03:
            Page3FlineDeviceProcess(reg, addr, pbuf, len);
            break;
        case 0x05:
            Page5ReviseProcess(reg, addr, pbuf, len);
            break;
        case 0x06:
            Page6PathConfigProcess(reg, addr, pbuf, len);
            break;
        case 0x07:
            Page7ControlFlineProcess(reg, addr, pbuf, len);
            break;
        case 0x08:
            Page8SwitchValveProcess(reg, addr, pbuf, len);
            break;
        case 0x09:
            Page9SwitchValveConfigProcess(reg, addr, pbuf, len);
            break;
		default:
			break;
	}
}

bool DisplayLastPageJumpAfterDeal(void)
{
	static bool afterDealState;
	switch(ScrInfGet()->lastPage)
	{
		default:
			afterDealState = TRUE;
			break;
	}
	return afterDealState;
}

bool DisplayCurrentPageJumpBeforeDeal(void)
{
	static bool beforeDealState;
	switch(ScrInfGet()->currentPage)
	{
		default:
			beforeDealState = TRUE;
			break;
	}
	return beforeDealState;
}

void DisplayParaDealProcess(u8 reg, u16 displayAddr, u8 *pbuf, u8 len)
{
	u16 keyAddr;
	if(displayAddr == 0)
	{
		keyAddr = pbuf[0]<<8 | pbuf[1];
		if((PAGE_SIGN & keyAddr) == PAGE_SIGN)// 区分同页面跳转但参数不同
		{
			ScrInfGet()->currentPage = keyAddr & 0xFF;
			ScrInfGet()->lastItem = (keyAddr >> 8) & 0x0F;
		}
		if(ScrInfGet()->lastPage != ScrInfGet()->currentPage)
		{
			// 页面跳转前、后的任务处理
			if(DisplayLastPageJumpAfterDeal() && DisplayCurrentPageJumpBeforeDeal())
			{
    			DisplayCommJumpToPic(ScrInfGet()->currentPage);
				ScrInfGet()->lastPage = ScrInfGet()->currentPage;
				DisplayPageJump(ScrInfGet()->currentPage, ScrInfGet()->lastItem);
			}
			else
			{
				ScrInfGet()->currentPage = ScrInfGet()->lastPage;
			}
		}
		else
		{
			DisplayPageDealProcess(ScrInfGet()->currentPage, reg, displayAddr, pbuf, len);
		}
	}
	else if(displayAddr > 0)
	{		
		DisplayPageDealProcess(ScrInfGet()->currentPage, reg, displayAddr, pbuf, len);
	}	
}

