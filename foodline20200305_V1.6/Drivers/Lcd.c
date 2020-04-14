#include "System.h"


extern SMOKE_BAKE_RECORD Gbl_SmokeBake_St;
extern REALTIME_INFONODE Gbl_Rtinfo_Nod2;
extern DEVICE_CFG_PARAS Gbl_Device_Para;


const u8 Lcd_num_seqs_array[114] = {
    //  1   2   3   4   5   6   7   8   9  10   // 
        2,  4,  6, 14, 16, 18,  7,  9, 11, 13,
    // 11  12  13  14  15  16  17  18  19  20   //    
       15, 17, 19, 21, 23, 25,  5,  3,  1, 12,
    // 21  22  23  24  25  26  27  28  29  30   //   
       10,  8, 25, 23, 21, 19, 23, 21, 19, 17,
    // 31  32  33  34  35  36  37  38  39  40   //   
       11,  9,  7,  5,  3,  1, 31, 29, 27, 25,
    // 41  42  43  44  45  46  47  48  49  50   //   
       11,  9,  1,  3,  5,  7, 13, 15, 17, 19,
    // 51  52  53  54  55  56  57  58  59  60   //  
       21, 23, 25, 27,  1,  3,  5,  7,  9, 11,
    // 61  62  63  64  65  66  67  68  69  70   //  
       13, 15, 31, 29, 27, 25, 23, 21, 19, 17,
    // 71  72  73  74  75  76  77  78  79  80   //   
       15, 13, 11,  9,  7,  5,  3,  1, 15, 13,
    // 81  82  83  84  85  86  87  88  89  90   //   
       11,  9,  7,  5,  3,  1, 30, 28, 26, 24,
    // 91  92  93  94  95  96  97  98  99 100   //   
        7,  5,  3,  1, 23, 21, 19, 17, 15, 13,
    //101 102 103 104 105 106 107 108 109 110   //
        9, 11, 13, 15, 17, 19, 21, 23, 25, 27,
    //111 112 113 114 
       29, 18, 20, 22, 
};


void Lcd_Out(u8 data)
{
    if ((data&0x80) != 0)
        SPIDAT = 1;
    else
        SPIDAT = 0;
    
    SPIWR = 0;
    SPIWR = 0;
    SPIWR = 1;
}


void Comwrite(u8 com)
{
    u8 i;
    
    Lcd_Out(0x80);      // 100 format
    Lcd_Out(0x00); 
    Lcd_Out(0x00); 
    for (i=0;i<9;i++)   // 8 bits
    {
        Lcd_Out(com);
        com <<= 1;
    }
    
    SPICS2 = 1;
    SPICS1 = 1;
    SPICS0 = 0;
}


void Lcd_Init_Ctrl(void)
{
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;     //IC:5
    Comwrite(0x00);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x01);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x02);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x03);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x18);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x29);
    
    OSTimeDly(80);
    
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 1;     //IC:1
    Comwrite(0x00);
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x01);
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x02);
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x03);
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x18);
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
    SPICS2 = 1; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x29);
    
    OSTimeDly(1);
    
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 0;     //8
    Comwrite(0x00);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x01);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x02);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x03);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x18);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x29);
    
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;     //IC:3
    Comwrite(0x00);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x01);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x02);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x03);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x18);
    SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x29);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;
    
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 1;     //IC:2
    Comwrite(0x00);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x01);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x02);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x03);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x18);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 1;
    Comwrite(0x29);
    
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 0;     //6
    Comwrite(0x00);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x01);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x02);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x03);
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x18);
    
    SPICS2 = 0; SPICS1 = 1; SPICS0 = 0;
    Comwrite(0x29);
    
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 1;     //IC:4
    Comwrite(0x00);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x01);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x02);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x03);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x18);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 1;
    Comwrite(0x29);
    
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;     //IC:7
    Comwrite(0x00);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x01);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x02);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x03);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x18);
    SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;
    Comwrite(0x29);
}


void Lcd_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
    
    // S4 S3 S2 S1
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
#if USE_RTC_WITH_BATTERY == 0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
#else
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
#endif
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
#if USE_RTC_WITH_BATTERY == 0
    PWR_BackupAccessCmd(ENABLE);
    RCC_LSEConfig(RCC_LSE_OFF);
    BKP_TamperPinCmd(DISABLE);
    BKP_ITConfig(DISABLE);
#endif
    
    Lcd_Init_Ctrl();
}


void Lcd_Senddat(u8 addr, u8 data)
{
    u8 i;
    
    Lcd_Out(0x80);      // 101 format
    Lcd_Out(0x00);
    Lcd_Out(0X80);
    addr <<= 2;
    for (i=0;i<6;i++)   // 6 bits
    {
        Lcd_Out(addr);
        addr <<= 1;
    }
    for (i=0;i<4;i++)   // 4 bits
    {   
        Lcd_Out(data);
        data <<= 1;
    }
}


void Lcd_Datawrite(u8 addr, u8 data, u8 sector)
{
    switch (sector)
    {
    case 0:
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 1;
        break;
    case 1:
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
        SPICS2 = 0; SPICS1 = 1; SPICS0 = 1;
        break;
    case 2:
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;            
        SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
        break;
    case 3:
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
        SPICS2 = 0; SPICS1 = 0; SPICS0 = 1;
        break;
    case 4:
        SPICS2 = 1; SPICS1 = 0; SPICS0 = 1;
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
        break;
    case 5:
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
        SPICS2 = 0; SPICS1 = 1; SPICS0 = 0;
        break;
    case 6:
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
        SPICS2 = 0; SPICS1 = 0; SPICS0 = 0;
        break;
    case 7:
        SPICS2 = 1; SPICS1 = 1; SPICS0 = 0;
        SPICS2 = 1; SPICS1 = 0; SPICS0 = 0;
        break;
    }
    Lcd_Senddat(addr, data);
}


//  F   A             A
//  G   B          F     B
//  E   C             G
//      D          E     C
//                    D
void Lcd_WriteNum_1(u8 seqs, u8 num, u8 xorval, u8 sector)
{
    xorval <<= 4;
    switch (num)
    {
    case 0:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0xa0|xorval, sector);
        break;
    case 1:
        Lcd_Datawrite(seqs, 0x60, sector);
        Lcd_Datawrite(seqs-1, xorval, sector);
        break;
    case 2:
        Lcd_Datawrite(seqs, 0xd0, sector);
        Lcd_Datawrite(seqs-1, 0x60|xorval, sector);
        break;
    case 3:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0x40|xorval, sector);
        break;
    case 4:
        Lcd_Datawrite(seqs, 0x60, sector);
        Lcd_Datawrite(seqs-1, 0xc0|xorval, sector);
        break;
    case 5:
        Lcd_Datawrite(seqs, 0xb0, sector);
        Lcd_Datawrite(seqs-1, 0xc0|xorval, sector);
        break;
    case 6:
        Lcd_Datawrite(seqs, 0xb0, sector);
        Lcd_Datawrite(seqs-1, 0xe0|xorval, sector);
        break;
    case 7:
        Lcd_Datawrite(seqs, 0xe0, sector);
        Lcd_Datawrite(seqs-1, xorval, sector);
        break;
    case 8:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0xe0|xorval, sector);
        break;
    case 9:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0xc0|xorval, sector);
        break;
    case 10:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, xorval, sector);
        break;
    default:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, 0x40|xorval, sector);
        break;
    }
}


//  D                 A
//  C   E          F     B
//  B   G             G
//  A   F          E     C
//                    D
void Lcd_WriteNum_2(u8 seqs, u8 num, u8 xorval, u8 sector)
{
    xorval <<= 7;
    switch (num)
    {
    case 0:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs+1, 0x50|xorval, sector);
        break;
    case 1:
        Lcd_Datawrite(seqs, 0x60, sector);
        Lcd_Datawrite(seqs+1, xorval, sector);
        break;
    case 2:
        Lcd_Datawrite(seqs, 0xb0, sector);
        Lcd_Datawrite(seqs+1, 0x60|xorval, sector);
        break;
    case 3:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs+1, 0x20|xorval, sector);
        break;
    case 4:
        Lcd_Datawrite(seqs, 0x60, sector);
        Lcd_Datawrite(seqs+1, 0x30|xorval, sector);
        break;
    case 5:
        Lcd_Datawrite(seqs, 0xd0, sector);
        Lcd_Datawrite(seqs+1, 0x30|xorval, sector);
        break;
    case 6:
        Lcd_Datawrite(seqs, 0xd0, sector);
        Lcd_Datawrite(seqs+1, 0x70|xorval, sector);
        break;
    case 7:
        Lcd_Datawrite(seqs, 0x70, sector);
        Lcd_Datawrite(seqs+1, xorval, sector);
        break;
    case 8:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs+1, 0x70|xorval, sector);
        break;
    case 9:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs+1, 0x30|xorval, sector);
        break;
    case 10:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs+1, xorval, sector);
        break;
    default:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs+1, 0x20|xorval, sector);
        break;
    }
}


//  C   D             A
//  G   E          F     B
//  B   F             G
//      A          E     C
//                    D
void Lcd_WriteNum_3(u8 seqs, u8 num, u8 xorval, u8 sector)
{
    xorval <<= 4;
    switch (num)
    {
    case 0:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0xa0|xorval, sector);
        break;
    case 1:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, 0xa0|xorval, sector);
        break;
    case 2:
        Lcd_Datawrite(seqs, 0xd0, sector);
        Lcd_Datawrite(seqs-1, 0x60|xorval, sector);
        break;
    case 3:
        Lcd_Datawrite(seqs, 0x90, sector);
        Lcd_Datawrite(seqs-1, 0xe0|xorval, sector);
        break;
    case 4:
        Lcd_Datawrite(seqs, 0x20, sector);
        Lcd_Datawrite(seqs-1, 0xe0|xorval, sector);
        break;
    case 5:
        Lcd_Datawrite(seqs, 0xb0, sector);
        Lcd_Datawrite(seqs-1, 0xc0|xorval, sector);
        break;
    case 6:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0xc0|xorval, sector);
        break;
    case 7:
        Lcd_Datawrite(seqs, 0x10, sector);
        Lcd_Datawrite(seqs-1, 0xa0|xorval, sector);
        break;
    case 8:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0xe0|xorval, sector);
        break;
    case 9:
        Lcd_Datawrite(seqs, 0xb0, sector);
        Lcd_Datawrite(seqs-1, 0xe0|xorval, sector);
        break;
    case 10:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, xorval, sector);
        break;
    default:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, 0x40|xorval, sector);
        break;
    }
}


//      A             A
//  B   F          F     B
//  G   E             G
//  C   D          E     C
//                    D
void Lcd_WriteNum_4(u8 seqs, u8 num, u8 xorval, u8 sector)
{
    xorval <<= 7;
    switch (num)
    {
    case 0:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0x50|xorval, sector);
        break;
    case 1:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, 0x50|xorval, sector);
        break;
    case 2:
        Lcd_Datawrite(seqs, 0xb0, sector);
        Lcd_Datawrite(seqs-1, 0x60|xorval, sector);
        break;
    case 3:
        Lcd_Datawrite(seqs, 0x90, sector);
        Lcd_Datawrite(seqs-1, 0x70|xorval, sector);
        break;
    case 4:
        Lcd_Datawrite(seqs, 0x40, sector);
        Lcd_Datawrite(seqs-1, 0x70|xorval, sector);
        break;
    case 5:
        Lcd_Datawrite(seqs, 0xd0, sector);
        Lcd_Datawrite(seqs-1, 0x30|xorval, sector);
        break;
    case 6:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0x30|xorval, sector);
        break;
    case 7:
        Lcd_Datawrite(seqs, 0x80, sector);
        Lcd_Datawrite(seqs-1, 0x50|xorval, sector);
        break;
    case 8:
        Lcd_Datawrite(seqs, 0xf0, sector);
        Lcd_Datawrite(seqs-1, 0x70|xorval, sector);
        break;
    case 9:
        Lcd_Datawrite(seqs, 0xd0, sector);
        Lcd_Datawrite(seqs-1, 0x70|xorval, sector);
        break;
    case 10:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, xorval, sector);
        break;
    default:
        Lcd_Datawrite(seqs, 0, sector);
        Lcd_Datawrite(seqs-1, 0x40|xorval, sector);
        break;
    }
}


void Lcd_WriteNum(u8 index, u8 num, u8 xorval, u8 clrs, u8 bar)
{
    u8 num_seqs = index-1;
    
    if (clrs)
        num = 10;
    else if (bar)
        num = 11;
    
    switch (index)
    {
    case 1: case 2: case 3: case 22: case 21: case 20: case 4: case 5: case 6:
        // sector1-1
        Lcd_WriteNum_1(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_1);
        break;
    case 26: case 25: case 24: case 23: 
        // sector1-2
        Lcd_WriteNum_2(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_1);
        break;
    case 19: case 18: case 17: case 7: case 8: case 9: case 10: case 11: case 12:
    case 13: case 14: case 15: case 16:
        // sector2-1
        Lcd_WriteNum_1(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_2);
        break;
    case 36: case 35: case 34: case 33: case 32: case 31: 
        // sector3-3
        Lcd_WriteNum_3(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_3);
        break;
    case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: 
        // sector3-4
        Lcd_WriteNum_4(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_3);
        break;
    case 42: case 41: case 100: case 99: case 98: case 97: case 96: case 95: 
        case 40: case 39: case 38: case 37: 
        // sector4-3
        Lcd_WriteNum_3(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_4);
        break;
    case 43: case 44: case 45: case 46: 
        // sector4-4
        Lcd_WriteNum_4(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_4);
        break;
    case 94: case 93: case 92: case 91: 
        // sector5-3
        Lcd_WriteNum_3(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_5);
        break;
    case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: 
    case 109: case 110: case 111: 
        // sector5-4
        Lcd_WriteNum_4(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_5);
        break;
    case 90: case 89: case 88: case 87: case 86: case 85: case 84: case 83: case 82: 
    case 81: case 80: case 79: 
        // sector6-3
        Lcd_WriteNum_3(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_6);
        break;
    case 112: case 113: case 114: 
        // sector6-4
        Lcd_WriteNum_4(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_6);
        break;
    case 78: case 77: case 76: case 75: case 74: case 73: case 72: case 71: case 70: 
    case 69: case 68: case 67: case 66: case 65: case 64: case 63: 
        // sector7-3
        Lcd_WriteNum_3(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_7);
        break;
    case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62: 
        // sector8-3
        Lcd_WriteNum_3(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_8);
        break;
    case 30: case 29: case 28: case 27: 
        // sector8-4
        Lcd_WriteNum_4(Lcd_num_seqs_array[num_seqs], num, xorval, Lcd_Sector_8);
        break;
    default:
        break;
    }
}


// mode: 0x80, clear off
void Lcd_Display(u8 mode, u8 index, u8 frame)
{
    u16 num;
    u8 frac, num1, num2;
    u8 xors, xors2, clrs, bar;
    
    // cear only for smoke load and date
    frac = 0;
    clrs = mode & CLEAR_ALL_CHARS;
    bar = mode & SHOW_ONLY_BAR;
    mode &= 0x3f;
    
    if ((clrs == 0) && (bar == 0))
    {
        switch (mode)
        {
        case DRY_TEMP_PART_TEMP:
        case WET_TEMP_PHASE_LOSS:
        case DRY_TEMP_TARGET_OVERLOAD:
        case WET_TEMP_TARGET_VAL:
            if (Gbl_SmokeBake_St.func == 0)
            {
                if (mode == DRY_TEMP_PART_TEMP)
                    num = Gbl_SmokeBake_St.temp.dry_temp;
                else if (mode == WET_TEMP_PHASE_LOSS)
                    num = Gbl_SmokeBake_St.temp.wet_temp;
                else if (mode == DRY_TEMP_TARGET_OVERLOAD)
                    num = Gbl_SmokeBake_St.temp.dry_temp_target;
                else
                    num = num = Gbl_SmokeBake_St.temp.wet_temp_target;
                
                if (num == 0)
                    bar = SHOW_ONLY_BAR;
                else
                {
                    frac = num % 10;
                    num = num / 10;
                }
            }
            else if (Gbl_SmokeBake_St.func == 1)
            {
                if (mode == DRY_TEMP_PART_TEMP)
                    num = Gbl_Rtinfo_Nod2.dry;
                else if (mode == WET_TEMP_PHASE_LOSS)
                    num = Gbl_Rtinfo_Nod2.wet;
                else if (mode == DRY_TEMP_TARGET_OVERLOAD)
                    num = Gbl_Rtinfo_Nod2.dry_target;
                else
                    num = Gbl_Rtinfo_Nod2.wet_target;
                
                if (num == 0)
                    bar = SHOW_ONLY_BAR;
                else
                {
                    frac = num % 10;
                    num = num / 10;
                }
            }
            else if ((Gbl_SmokeBake_St.func >= 2) && (Gbl_SmokeBake_St.func <= 3))
            {
                // ID and thresh temp
                if (Gbl_SmokeBake_St.func == 2)
                    num = Gbl_Device_Para.device_id;
                else
                {
                    num = Gbl_Device_Para.bwind_temp;
                    frac = num % 10;
                    num = num / 10;
                }
            }
            else if (Gbl_SmokeBake_St.func == 4)
            {
                if (mode == DRY_TEMP_PART_TEMP)
                {
                    num = (SOFT_BUILD_VERSION & 0xff00)>>8;
                    num = num*10+((SOFT_BUILD_VERSION & 0xf0)>>4);
                    frac = SOFT_BUILD_VERSION & 0x0f;
                }
                else if (mode == WET_TEMP_PHASE_LOSS)
                {
                    num = (SOFT_BUILD_TIME & 0xf0000000)>>28;
                    num = num*10+((SOFT_BUILD_TIME & 0xf000000)>>24);
                }
                else if (mode == WET_TEMP_TARGET_VAL)
                {
                    num = (SOFT_BUILD_TIME & 0xf00000)>>20;
                    num = num*10+((SOFT_BUILD_TIME & 0xf0000)>>16);
                }
            }
            break;
        case BAKE_STAGE_TIME:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.innert;
            }
            else
            {
                num = Gbl_SmokeBake_St.bake_time.stage_time;
            }
            frac = num % 10;
            num = num / 10;
            break;
        case BAKE_TOTLE_TIME:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.totalt;
            }
            else
            {
                num = Gbl_SmokeBake_St.bake_time.total_time/10;
            }
            break;
        case DRY_BAKE_CURVE_VAL:
            num = Gbl_SmokeBake_St.curves.dry_temp[index];
            break;
        case WET_BAKE_CURVE_VAL:
            num = Gbl_SmokeBake_St.curves.wet_temp[index];
            frac = (num & 0x80)?1:0;
            num &= 0x7f;
            break;
        case BAKE_CURVE_STAGE_TIME:
            num = Gbl_SmokeBake_St.curves.temp_time[index];
            break;
        case BAKE_VOLTAGE_VALUE:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.voltage;
            }
            else
            {
                num = Gbl_SmokeBake_St.rt_voltage;
            }
            break;
        case BAKE_STAGE_SEQUENCE:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.bakeseq;
            }
            else
            {
                num = Gbl_SmokeBake_St.bake_seq;
            }
            break;
        case LCD_SYTEM_TIME_YEAR:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.year+17;
            }
            else
            {
                num = Gbl_SmokeBake_St.date.year-2000;
            }
            break;
        case LCD_SYTEM_TIME_MONTH:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.mon;
            }
            else
            {
                num = Gbl_SmokeBake_St.date.mon;
            }
            break;
        case LCD_SYTEM_TIME_DAY:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.day;
            }
            else
            {
                num = Gbl_SmokeBake_St.date.day;
            }
            break;
        case LCD_SYTEM_TIME_HOUR:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.hour;
            }
            else
            {
                num = Gbl_SmokeBake_St.date.hour;
            }
            break;
        case LCD_SYTEM_TIME_MIN:
            if (Gbl_SmokeBake_St.func == 1)
            {
                num = Gbl_Rtinfo_Nod2.min;
            }
            else
            {
                num = Gbl_SmokeBake_St.date.min;
            }
            break;
        case LCD_LOAD_SMOKE_VOLUME:
            num = Gbl_SmokeBake_St.smoke.loads[index];
            break;
        default:
            break;
        }
        
        switch (mode)
        {
        case DISPLAY_LOGO_FRAME:
        case SWITCH_UP_DOWN_ROOM:
            break;
        
        default: 
            num2 = num/100;
            num -= num2*100;
            num1 = num/10;
            num -= num1*10;
            break;
        }
    }
    
    switch (mode)
    {
    case DRY_TEMP_PART_TEMP:
        xors = Gbl_SmokeBake_St.part_temp;
        if ((Gbl_SmokeBake_St.func >= 2) && (Gbl_SmokeBake_St.func <= 3))
        {
            // ID and thresh temp
            if (Gbl_SmokeBake_St.func == 2)
            {
                Lcd_WriteNum(1, num1, xors, clrs, bar);     // part_temp
                Lcd_WriteNum(2, num, xors, clrs, bar);      // part_temp frame 
                Lcd_WriteNum(3, 0, 0, CLEAR_ALL_CHARS, bar);
            }
            else
            {
                Lcd_WriteNum(1, num1, xors, clrs, bar);     // part_temp
                Lcd_WriteNum(2, num, xors, clrs, bar);      // part_temp frame 
                Lcd_WriteNum(3, frac, 1, clrs, bar);
            }
        }
        else if (Gbl_SmokeBake_St.func == 4)
        {
            Lcd_WriteNum(1, num1, xors, clrs, bar);         // part_temp
            Lcd_WriteNum(2, num, xors, clrs, bar);          // part_temp frame 
            Lcd_WriteNum(3, frac, 1, clrs, bar);
        }
        else
        {
            Lcd_WriteNum(1, num1, xors, clrs, bar);         // part_temp
            Lcd_WriteNum(2, num, xors, clrs, bar);          // part_temp frame 
            Lcd_WriteNum(3, frac, clrs?0:1, clrs, bar);
        }
        break;
    case WET_TEMP_PHASE_LOSS:
        xors = Gbl_SmokeBake_St.phaseloss;
        if ((Gbl_SmokeBake_St.func >= 2) && (Gbl_SmokeBake_St.func <= 4))
        {
            if (Gbl_SmokeBake_St.func == 4)
            {
                Lcd_WriteNum(4, num1, xors, clrs, bar);         // phaseloss
                Lcd_WriteNum(5, num, xors, clrs, bar);          // phaseloss frame 
                Lcd_WriteNum(6, 0, 0, CLEAR_ALL_CHARS, bar);
            }
            else
            {
                // null
                Lcd_WriteNum(4, 0, xors, CLEAR_ALL_CHARS, bar); // phaseloss
                Lcd_WriteNum(5, 0, xors, CLEAR_ALL_CHARS, bar); // phaseloss frame 
                Lcd_WriteNum(6, 0, 0, CLEAR_ALL_CHARS, bar);
            }
        }
        else
        {
            Lcd_WriteNum(4, num1, xors, clrs, bar);             // phaseloss
            Lcd_WriteNum(5, num, xors, clrs, bar);              // phaseloss frame 
            Lcd_WriteNum(6, frac, clrs?0:1, clrs, bar);
        }
        break;
    case DRY_TEMP_TARGET_OVERLOAD:
        xors = Gbl_SmokeBake_St.overload;
        if ((Gbl_SmokeBake_St.func >= 2) && (Gbl_SmokeBake_St.func <= 4))
        {
            // F
            Lcd_Datawrite(8, 0x80, Lcd_Sector_1);
            Lcd_Datawrite(7, 0xe0|xors, Lcd_Sector_1);      // overload
            
            // F5-ID and F6-Blows and F8-soft version
            if (Gbl_SmokeBake_St.func == 2)
            {
                Lcd_WriteNum(21, 5, xors, clrs, bar);       // overload frame 
            }
            else if (Gbl_SmokeBake_St.func == 3)
            {
                Lcd_WriteNum(21, 6, xors, clrs, bar);       // overload frame 
            }
            else if (Gbl_SmokeBake_St.func == 4)
            {
                Lcd_WriteNum(21, 8, xors, clrs, bar);       // overload frame 
            }
            else if (Gbl_SmokeBake_St.func == 5)
            {
                Lcd_WriteNum(21, 9, xors, clrs, bar);       // overload frame 
            }
            Lcd_WriteNum(20, 0, 0, CLEAR_ALL_CHARS, bar);
        }
        else
        {
            Lcd_WriteNum(22, num1, xors, clrs, bar);        // overload
            Lcd_WriteNum(21, num, xors, clrs, bar);         // overload frame 
            Lcd_WriteNum(20, frac, clrs?0:1, clrs, bar);
        }
        break;
    case WET_TEMP_TARGET_VAL:
        if ((Gbl_SmokeBake_St.func >= 2) && (Gbl_SmokeBake_St.func <= 4))
        {
            if (Gbl_SmokeBake_St.func == 4)
            {
                Lcd_WriteNum(19, num1, 0, clrs, bar);
                Lcd_WriteNum(18, num, 0, clrs, bar);
                Lcd_WriteNum(17, 0, 0, CLEAR_ALL_CHARS, bar);
            }
            else
            {
                // null
                Lcd_WriteNum(19, 0, 0, CLEAR_ALL_CHARS, bar);
                Lcd_WriteNum(18, 0, 0, CLEAR_ALL_CHARS, bar);
                Lcd_WriteNum(17, 0, 0, CLEAR_ALL_CHARS, bar);
            }
        }
        else
        {
            Lcd_WriteNum(19, num1, 0, clrs, bar);
            Lcd_WriteNum(18, num, 0, clrs, bar);
            Lcd_WriteNum(17, frac, clrs?0:1, clrs, bar);
        }
        break;
    case BAKE_STAGE_TIME:
        Lcd_WriteNum(7, num1, 0, clrs, bar);
        Lcd_WriteNum(8, num, 0, clrs, bar);
        Lcd_WriteNum(9, frac, clrs?0:1, clrs, bar);
        break;
    case BAKE_TOTLE_TIME:
        Lcd_WriteNum(10, num2, 0, clrs, bar);
        Lcd_WriteNum(11, num1, 0, clrs, bar);
        Lcd_WriteNum(12, num, 0, clrs, bar);
        break;
    case DRY_BAKE_CURVE_VAL:
        index <<= 1;
        switch (index)
        {
        case 0: case 2:
            xors = Gbl_SmokeBake_St.rise_temp;
            if (index == 0)
            {
                if (xors == 2)
                    xors = 1;
                else
                    xors = 0;
            }
            else
            {
                if (xors != 1)
                    xors = 0;
            }
            Lcd_WriteNum(23+index, num1, xors, clrs, bar);
            Lcd_WriteNum(24+index, num, frame, clrs, bar);
            break;
        case 4: case 6: case 8: case 10: case 12: case 14: case 16: case 18: 
            Lcd_WriteNum(23+index, num1, frame, clrs, bar);
            Lcd_WriteNum(24+index, num, 0, clrs, bar);
            break;
        default:
            break;
        }
        break;
    case WET_BAKE_CURVE_VAL:
        index <<= 1;
        switch (index)
        {
        case 18: 
            Lcd_WriteNum(61-index, num, 0, clrs, bar);
            Lcd_WriteNum(62-index, num1, frame, clrs, bar);
            break;
        case 0: case 2: case 4: case 6: case 8: case 10: case 12: case 14: case 16: 
            Lcd_WriteNum(61-index, num, frac, clrs, bar);
            Lcd_WriteNum(62-index, num1, frame, clrs, bar);
            break;
        default:
            break;
        }
        break;
    case BAKE_CURVE_STAGE_TIME:
        index <<= 1;
        switch (index)
        {
        case 0: 
            xors = Gbl_SmokeBake_St.clean_wet;
            Lcd_WriteNum(63, num1, xors, clrs, bar);
            Lcd_WriteNum(64, num, frame, clrs, bar);
            break;
        case 2: 
            xors = Gbl_SmokeBake_St.help_fire;
            Lcd_WriteNum(65, num1, xors, clrs, bar);
            Lcd_WriteNum(66, num, frame, clrs, bar);
            break;
        case 4: case 6: case 8: case 10: case 12: case 14: case 16: case 18: case 20: 
        case 22: case 24: case 26: case 28: case 30: case 32: case 34: case 36: 
            Lcd_WriteNum(63+index, num1, frame, clrs, bar);
            Lcd_WriteNum(64+index, num, 0, clrs, bar);
            break;
        default:
            break;
        }
        break;
    case BAKE_VOLTAGE_VALUE:
        Lcd_WriteNum(112, num, 1, clrs, bar);
        xors = Gbl_SmokeBake_St.wind_speed;
        if (xors == 0)
        {
            Lcd_WriteNum(114, num2, 0, clrs, bar);
            Lcd_WriteNum(113, num1, 0, clrs, bar);
            Lcd_Datawrite(16, 0xc0, Lcd_Sector_6);
        }
        else if (xors == 1)
        {
            Lcd_WriteNum(114, num2, 1, clrs, bar);
            Lcd_WriteNum(113, num1, 0, clrs, bar);
            Lcd_Datawrite(16, 0xc0, Lcd_Sector_6);
        }
        else
        {
            Lcd_WriteNum(114, num2, 0, clrs, bar);
            Lcd_WriteNum(113, num1, 0, clrs, bar);
            Lcd_Datawrite(16, 0xe0, Lcd_Sector_6);
        }
        break;
    case BAKE_STAGE_SEQUENCE:
        Lcd_WriteNum(111, num, 0, clrs, bar);           // no BC
        break;
    case LCD_SYTEM_TIME_YEAR:
        xors = Gbl_SmokeBake_St.curveflash;
        xors2 = (xors & 0x08)?1:0;
        Lcd_WriteNum(110, num1, xors2, clrs, bar);
        Lcd_WriteNum(109, num, xors2, clrs, bar);
        break;
    case LCD_SYTEM_TIME_MONTH:
        xors = Gbl_SmokeBake_St.curveflash;
        xors2 = (xors & 0x04)?1:0;
        Lcd_WriteNum(108, num1, xors2, clrs, bar);
        Lcd_WriteNum(107, num, xors2, clrs, bar);
        break;
    case LCD_SYTEM_TIME_DAY:
        Lcd_WriteNum(106, num1, 0, clrs, bar);
        Lcd_WriteNum(105, num, 0, clrs, bar);
        break;
    case LCD_SYTEM_TIME_HOUR:
        xors = Gbl_SmokeBake_St.curveflash;
        xors2 = (xors & 0x02)?1:0;
        Lcd_WriteNum(104, num1, xors2, clrs, bar);
        Lcd_WriteNum(103, num, xors2, clrs, bar);
        break;
    case LCD_SYTEM_TIME_MIN:
        xors = Gbl_SmokeBake_St.curveflash;
        xors2 = (xors & 0x01)?1:0;
        Lcd_WriteNum(102, num1, xors2, clrs, bar);
        Lcd_WriteNum(101, num, xors2, clrs, bar);
        break;
    case LCD_LOAD_SMOKE_VOLUME:
        Lcd_WriteNum(13+index, num, 0, clrs, bar);
        break;
    case DISPLAY_LOGO_FRAME:
        xors = Gbl_SmokeBake_St.wind_speed;
        if (xors == 2)
        {
            Lcd_Datawrite(16, 0xe0, Lcd_Sector_6);
        }
        else
        {
            Lcd_Datawrite(16, 0xc0, Lcd_Sector_6);
        }
        break;
    case SWITCH_UP_DOWN_ROOM:
        xors = Gbl_SmokeBake_St.updown_room;
        if (xors)
        {
            Lcd_Datawrite(0, 0xc0, Lcd_Sector_1);
        }
        else
        {
            Lcd_Datawrite(0, 0x30, Lcd_Sector_1);
        }
        break;
    default:
        break;
    }
}


