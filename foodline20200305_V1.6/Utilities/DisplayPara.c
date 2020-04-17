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
#include "Page10TowersOut.h"
#include "Page11TowersOutConfig.h"
#include "Page12S1S2Control.h"
#include "Page13LinkConfig.h"
#include "Page14W1Control.h"
#include "Page15W2Control.h"
#include "Page17C1Control.h"
#include "Page18C2Control.h"
#include "Page19N1Control.h"
#include "Page20N2Control.h"
#include "Page16E1E2Control.h"


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
        case 0x0A:
            Page10TowersOutInit();
            break;
        case 0x0B:
            Page11TowersOutConfigInit();
            break;
        case 0x0C:
            Page12S1S2ControlInit();
            break;
        case 0x0D:
            Page13LinkConfigInit();
            break;     
        case 0x0E:
            Page14W1ControlInit();
            break; 
        case 0x0F:
            Page15W2ControlInit();
            break; 		
        case 0x10:
            Page16E1E2ControlInit();
            break; 			
		case 0x11:
            Page17C1ControlInit();
            break; 
        case 0x12:
            Page18C2ControlInit();
            break; 	
        case 0x13:
            Page19N1ControlInit();
            break; 
        case 0x14:
            Page20N2ControlInit();
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
        case 0x0A:
            Page10TowersOutProcess(reg, addr, pbuf, len);
            break;
        case 0x0B:
            Page11TowersOutConfigProcess(reg, addr, pbuf, len);
            break;
        case 0x0C:
            Page12S1S2ControlProcess(reg, addr, pbuf, len);
            break;
        case 0x0D:
            Page13LinkConfigProcess(reg, addr, pbuf, len);
            break;  
        case 0x0E:
            Page14W1ControlProcess(reg, addr, pbuf, len);
            break;  
        case 0x0F:
            Page15W2ControlProcess(reg, addr, pbuf, len);
            break;  
        case 0x10:
            Page16E1E2ControlProcess(reg, addr, pbuf, len);
            break;  
        case 0x11:
            Page17C1ControlProcess(reg, addr, pbuf, len);
            break;  
        case 0x12:
            Page18C2ControlProcess(reg, addr, pbuf, len);
            break;
        case 0x13:
            Page19N1ControlProcess(reg, addr, pbuf, len);
            break;
        case 0x14:
            Page20N2ControlProcess(reg, addr, pbuf, len);
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
		if((PAGE_SIGN & keyAddr) == PAGE_SIGN)// ����ͬҳ����ת��������ͬ
		{
			ScrInfGet()->currentPage = keyAddr & 0xFF;
			ScrInfGet()->lastItem = (keyAddr >> 8) & 0x0F;
		}
		if(ScrInfGet()->lastPage != ScrInfGet()->currentPage)
		{
			// ҳ����תǰ�����������
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
