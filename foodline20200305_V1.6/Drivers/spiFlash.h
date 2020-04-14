#ifndef _SPIFLASH_H_
#define _SPIFLASH_H_

    
#include "System.h" 
 
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
//#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 

#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128 0XEF17 
#define sector  0x1000
#define dbsector 0x2000
extern u16 SPI_FLASH_TYPE;			

#define	SPI_FLASH_CS PAout(4) 				 

//extern u8 SPI_FLASH_BUF[4096];

#define feeweith
#define FLASH_ID 0XEF14
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 




void SPI_Flash_Init(void);
       
void SPI_FLASH_Write_SR(u8 sr);  	
void SPI_FLASH_Write_Enable(void); 
void SPI_FLASH_Write_Disable(void);
void SPI_Flash_Erase_Chip(void);    	 
void SPI_Flash_Erase_Sector(u32 Dst_Addr);
void SPI_Flash_Wait_Busy(void);          
void SPI_Flash_PowerDown(void);        
void SPI_Flash_WAKEUP(void);			



void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);// nocheck erase


#endif // _SPIFLASH_H_














