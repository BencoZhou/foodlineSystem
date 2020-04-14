#ifndef MZSYS_AT24CXX_EEROM_HANDLER_H
#define MZSYS_AT24CXX_EEROM_HANDLER_H


#include "stm32f10x.h"
#include "ucos_ii.h"
#include "Debug.h"
#include "PeripheralIO.h"


#define AT24C01     127
#define AT24C02     255
#define AT24C04     511
#define AT24C08     1023
#define AT24C16     2047
#define AT24C32     4095
#define AT24C64     8191
#define AT24C128    16383
#define AT24C256    32767

#define EE_TYPE     AT24C128



#define I2C_SCL     GPIO_Pin_14     // PB
#define I2C_SDA     GPIO_Pin_15     // PB
#define GPIO_I2C    GPIOB

#define I2C_SCL_H   GPIO_SetBits(GPIO_I2C,I2C_SCL)
#define I2C_SCL_L   GPIO_ResetBits(GPIO_I2C,I2C_SCL)

#define I2C_SDA_H   GPIO_SetBits(GPIO_I2C,I2C_SDA)
#define I2C_SDA_L   GPIO_ResetBits(GPIO_I2C,I2C_SDA)


#define AT24CXX_PAGE_SIZE               0x200       // 512, self define

#if E2ROM_SPACE_EXTEND_1024

#define AT24CXX_BASE_CONFIG_SIZE        (128*1024)
#define AT24CXX_BASE_CONFIG_ADDRESS     0

#define AT24CXX_CFG_CURVES_ADDRESS      AT24CXX_BASE_CONFIG_ADDRESS
#define AT24CXX_RT_INFO_ADDRESS         (AT24CXX_BASE_CONFIG_ADDRESS+AT24CXX_PAGE_SIZE)
#define AT24CXX_RT_INFO_SECTORS         255

#else

#define AT24CXX_BASE_CONFIG_SIZE        (32*1024)
#define AT24CXX_BASE_CONFIG_ADDRESS     0

#define AT24CXX_CFG_CURVES_ADDRESS      AT24CXX_BASE_CONFIG_ADDRESS
#define AT24CXX_RT_INFO_ADDRESS         (AT24CXX_BASE_CONFIG_ADDRESS+AT24CXX_PAGE_SIZE)
#define AT24CXX_RT_INFO_SECTORS         63

#endif

void AT24CXX_Init(void);

u8 AT24Cxx_ReadOneByte(u16 addr);
void AT24Cxx_WriteOneByte(u16 addr, u8 dt);
void AT24Cxx_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead);
void AT24Cxx_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite);
u8 E2rom_Read(u32 addr, u8 *data, u16 len);
u8 E2rom_Write(u32 addr, u8 *data, u16 len);

#endif
