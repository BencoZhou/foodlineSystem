/*****************************************************************
 * File: Page2PathConfig.c
 * Date: 2019/11/21 14:30
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page13LinkConfig.h"
#include "ParaSettingLib.h"
static u8 gP13Table, areaIndex ;  //areaIndex区域号或数组一维序号，  gP13Table
//static u8 gCountdown;
static u8 gPage13Seq; // 序号，累加

extern AllTheControlPara gAllTheControlPara[AREA_DEVICE_TOTAL_NUMBER][SING_LINK_DEVICE_TOTAL_NUMBER];

void Page13LinkConfigTurn(u8 table);


void Page13LinkConfigInit(void)
{
	ScrInfGet()->currentTable = gP13Table;
    Page13LinkConfigTurn(gP13Table);
    DisplayCommNumSend(PAGE13_LINK_CURRENT_PAGE, gP13Table+1);    // 当前页面号
    DisplayCommNumSend(PAGE13_LINK_TOTAL_PAGES, PAGE13_DEVICE_PAGES);    //总数
    ParaRefreshGet()->count = 1;
}





void Page13LinkConfigTurn(u8 table)
{
	switch(table)
	{
	case 0:
 		DisplayCommIconSend(PAGE13_LINK_LEFT_BUTTON,  ICO_HIDE);
 		DisplayCommIconSend(PAGE13_LINK_RIGHT_BUTTON, ICO_SHOW);
 		break;
 	case 4:
 		DisplayCommIconSend(PAGE13_LINK_LEFT_BUTTON,  ICO_SHOW);
 		DisplayCommIconSend(PAGE13_LINK_RIGHT_BUTTON, ICO_HIDE);
 		break;
 	default:
 		DisplayCommIconSend(PAGE13_LINK_LEFT_BUTTON,  ICO_SHOW);
 		DisplayCommIconSend(PAGE13_LINK_RIGHT_BUTTON, ICO_SHOW);
		break;
	}
    DisplayCommNumSend(PAGE13_LINK_CURRENT_PAGE, gP13Table+1);
}

void Page13LinkConfigInsert(u8 areaIndex,u8 index)
{
	u8 i, num;
    num = PAGE13_DEVICE_PER*PAGE13_DEVICE_PAGES;
	if(index >= num)
		return;
	for(i = num-1; i > index; i--)
	{
        gAllTheControlPara[areaIndex][i].cDevice.id = gAllTheControlPara[areaIndex][i-1].cDevice.id;
        gAllTheControlPara[areaIndex][i].prevDevice.id = gAllTheControlPara[areaIndex][i-1].prevDevice.id;
	}
    gAllTheControlPara[areaIndex][index].cDevice.placeNew.useID= 0;
    gAllTheControlPara[areaIndex][index].prevDevice.placeNew.useID= 0;
}

void Page13LinkConfigDelete(u8 areaIndex,u8 index)
{
	u8 i, num;
    num = PAGE13_DEVICE_PER*PAGE13_DEVICE_PAGES;
	if(index >= num)
		return;
	for(i = index; i < num-1; i++)
	{
        gAllTheControlPara[areaIndex][i].cDevice.id = gAllTheControlPara[areaIndex][i+1].cDevice.id;
        gAllTheControlPara[areaIndex][i].prevDevice.id = gAllTheControlPara[areaIndex][i+1].prevDevice.id;
	}
    gAllTheControlPara[areaIndex][num-1].cDevice.placeNew.useID = 0;
    gAllTheControlPara[areaIndex][num-1].prevDevice.placeNew.useID = 0;
}
/*
功能：录入控件数据，区域为数组一维，行数为数组二维
		

*/
void Page13LinkConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
	u32 tempIndex, data;
    u16 tempAddr;
//	u8 areaIndex;
	u8 i;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    tempIndex = (addr&0x00F0)>>4;    //行标号
    tempAddr = addr&0xFF0F;      		// 列标号
//    gIndex = PathParameterGet()->index;   //   路由组
	
	//   先得到区域这个数据
	if(addr == PAGE13_LINK_HEARD_AREA)
	{
		if(data > 0)
		{
			areaIndex = (data - 1 );
		}
		else
		{
			areaIndex = 0;
		}
        if(LinkParameterGet()->index != data)
        {
            LinkParameterGet()->index = data;
        }		
//		for(i = 0 ;i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)  // 选好区域后对应数组的二维数据的区域ID都为数组一维号。
//		{
//			gAllTheControlPara[areaIndex][i].cDevice.place.area = data;
//		}
    }
	
	
	// 数据录入
    if(tempIndex < PAGE13_DEVICE_PER && gP13Table < PAGE13_DEVICE_PAGES)
    {
        if(tempAddr == PAGE13_PREV_LINK_TYPE1)   //  与绞龙同一个料塔的前组三通，用来得到料塔是否空料信息，判断是否停止打料。
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.type = data;
        }		
        if(tempAddr == PAGE13_PREV_LINK_AREA1)   
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.area = data;
        }	
        if(tempAddr == PAGE13_PREV_LINK_MAIN_LINE1) 
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.mainLine = data;
        }	
        if(tempAddr == PAGE13_PREV_LINK_VICE_LINE1)  
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.viceLine = data;
        }	
		
        if(tempAddr == PAGE13_LINK_TYPE1) 
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].cDevice.place.type = data;
        }
        if(tempAddr == PAGE13_LINK_AREA1) 
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].cDevice.place.area = data;
        }		
        if(tempAddr == PAGE13_LINK_MAIN_LINE1)
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].cDevice.place.mainLine = data;
        }
        if(tempAddr == PAGE13_LINK_VICE_LINE1)
        {
            gAllTheControlPara[areaIndex][tempIndex+gP13Table*PAGE13_DEVICE_PER].cDevice.place.viceLine = data;
        }
        if(tempAddr == PAGE13_LINK_INSERT1)
        {
		    Page13LinkConfigInsert(areaIndex,tempIndex+gP13Table*PAGE13_DEVICE_PER);
        }
        if(tempAddr == PAGE13_LINK_DELETE1)
        {
		    Page13LinkConfigDelete(areaIndex,tempIndex+gP13Table*PAGE13_DEVICE_PER);
        }
    
    }
	// 翻页设置
	if(addr == PAGE13_LINK_LEFT_BUTTON && gP13Table > 0)// 左
	{
		gP13Table--;   // 页面好
		Page13LinkConfigTurn(gP13Table);
	}
	else if(addr == PAGE13_LINK_RIGHT_BUTTON && gP13Table < 4)// 右
	{
		gP13Table++;
		Page13LinkConfigTurn(gP13Table);
	}
    else if(addr == PAGE13_LINK_LEFT_BUTTON || addr == PAGE13_LINK_RIGHT_BUTTON)
    {
		Page13LinkConfigTurn(gP13Table);
    }

    if(addr == PAGE13_LINK_SAVE)
    {
        ParaLinkParaSave();   //保存至flash里面
    }

    ParaRefreshGet()->count = 2;   //显示界面刷新两次
    Page13LinkConfigRefresh();
}

void Page13LinkConfigRefresh(void)
{
	char str[12];
	u32 tempID, i;
	u8 data;
    if(ParaRefreshGet()->count == 0)
    {
        return;
    }
    else
    {
        ParaRefreshGet()->count--;
    }
    
    data = LinkParameterGet()->index;
	if(data > 0)
	{
		for(i = 0; i < 8; i++)
		{
			DisplayCommIconSend(PAGE13_LINK_TYPE1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].cDevice.place.type);
			DisplayCommIconSend(PAGE13_LINK_AREA1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].cDevice.place.area);
			DisplayCommNumSend(PAGE13_LINK_MAIN_LINE1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].cDevice.place.mainLine);
			DisplayCommNumSend(PAGE13_LINK_VICE_LINE1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].cDevice.place.viceLine);

			DisplayCommIconSend(PAGE13_PREV_LINK_TYPE1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.type);
			DisplayCommIconSend(PAGE13_PREV_LINK_AREA1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.area);
			DisplayCommNumSend(PAGE13_PREV_LINK_MAIN_LINE1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.mainLine);
			DisplayCommNumSend(PAGE13_PREV_LINK_VICE_LINE1+i*0x10, gAllTheControlPara[areaIndex][i+gP13Table*PAGE13_DEVICE_PER].prevDevice.place.viceLine);

			OSTimeDly(3);
		}
	}
	else
	{
		for(i = 0; i < 8; i++)
		{
			DisplayCommIconSend(PAGE13_LINK_TYPE1+i*0x10, 0);
			DisplayCommIconSend(PAGE13_LINK_AREA1+i*0x10, 0);
			DisplayCommNumSend(PAGE13_LINK_MAIN_LINE1+i*0x10, 0);
			DisplayCommNumSend(PAGE13_LINK_VICE_LINE1+i*0x10, 0);

			DisplayCommIconSend(PAGE13_PREV_LINK_TYPE1+i*0x10, 0);
			DisplayCommIconSend(PAGE13_PREV_LINK_AREA1+i*0x10, 0);
			DisplayCommNumSend(PAGE13_PREV_LINK_MAIN_LINE1+i*0x10, 0);
			DisplayCommNumSend(PAGE13_PREV_LINK_VICE_LINE1+i*0x10,0);

			OSTimeDly(3);
		}	
	}
    DisplayCommIconSend(PAGE13_LINK_HEARD_AREA, data);

}



