#include "System.h"


void I2C_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = I2C_SCL|I2C_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    I2C_SCL_H;
    I2C_SDA_H;
}


void I2C_SDA_OUT()
{
    GPIO_InitTypeDef GPIO_InitStructure;    
    
    GPIO_InitStructure.GPIO_Pin = I2C_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void I2C_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;    
    
    GPIO_InitStructure.GPIO_Pin = I2C_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void I2C_Delay(u8 us)
{
    u16 i = 72*us;
    while (i) 
    {
        i --; 
    }
}


void I2C_Start(void)
{
    I2C_SDA_OUT();
    
    I2C_SDA_H;
    I2C_SCL_H;
    I2C_Delay(5);
    I2C_SDA_L;
    I2C_Delay(6);
    I2C_SCL_L;
}


void I2C_Stop(void)
{
    I2C_SDA_OUT();
    
    I2C_SCL_L;
    I2C_SDA_L;
    I2C_SCL_H;
    I2C_Delay(6);
    I2C_SDA_H;
    I2C_Delay(6);
}


void I2C_Ack(void)
{
    I2C_SCL_L;
    I2C_SDA_OUT();
    I2C_SDA_L;
    I2C_Delay(2);
    I2C_SCL_H;
    I2C_Delay(5);
    I2C_SCL_L;
}


void I2C_NAck(void)
{
    I2C_SCL_L;
    I2C_SDA_OUT();
    I2C_SDA_H;
    I2C_Delay(2);
    I2C_SCL_H;
    I2C_Delay(5);
    I2C_SCL_L;
}


// 1: fail
// 0: success
u8 I2C_Wait_Ack(void)
{
    u8 tempTime = 0;
    
    I2C_SDA_IN();
    
    I2C_SDA_H;
    I2C_Delay(1);
    I2C_SCL_H;
    I2C_Delay(1);
    
    while (GPIO_ReadInputDataBit(GPIO_I2C, I2C_SDA))
    {
        tempTime ++;
        if (tempTime > 250)
        {
            I2C_Stop();
            return 1;
        }
    }
    
    I2C_SCL_L;
    return 0;
}


void I2C_Send_Byte(u8 txd)
{
    u8 i = 0;
    
    I2C_SDA_OUT();
    I2C_SCL_L;  // pull down clk to start data
    
    for (i=0;i<8;i++)
    {
        if ((txd & 0x80) > 0)   //0x80  1000 0000
            I2C_SDA_H;
        else
            I2C_SDA_L;
        
        txd <<= 1;
        I2C_SCL_H;
        I2C_Delay(2);
        I2C_SCL_L;
        I2C_Delay(2);
    }
}


u8 I2C_Read_Byte(u8 ack)
{
    u8 i = 0, receive = 0;
    
    I2C_SDA_IN();
    for (i=0;i<8;i++)
    {
        I2C_SCL_L;
        I2C_Delay(2);
        I2C_SCL_H;
        receive<<=1;
        if (GPIO_ReadInputDataBit(GPIO_I2C, I2C_SDA))
            receive ++;
        I2C_Delay(1);    
    }
    
    if (ack == 0)
        I2C_NAck();
    else
        I2C_Ack();
    
    return receive;
}


u8 AT24Cxx_ReadOneByte(u16 addr)
{
    u8 temp = 0;
    
    I2C_Start();
    
    if (EE_TYPE > AT24C16)
    {
        I2C_Send_Byte(0xA0);
        I2C_Wait_Ack();
        I2C_Send_Byte(addr>>8);             // data addr high
    }
    else
    {
       I2C_Send_Byte(0xA0+((addr/256)<<1)); // dev addr and data addr high
    }

    I2C_Wait_Ack();
    I2C_Send_Byte(addr%256);    // data addr low
    I2C_Wait_Ack();

    I2C_Start();
    I2C_Send_Byte(0xA1);
    I2C_Wait_Ack();

    temp = I2C_Read_Byte(0);    // 0: NACK
    I2C_NAck();
    I2C_Stop();
    
    return temp;    
}


void AT24Cxx_WriteOneByte(u16 addr, u8 dt)
{
    I2C_Start();
    
    if (EE_TYPE > AT24C16)
    {
        I2C_Send_Byte(0xA0);
        I2C_Wait_Ack();
        I2C_Send_Byte(addr>>8);
    }
    else
    {
       I2C_Send_Byte(0xA0+((addr/256)<<1)); // dev addr + data addr
    }
    
    I2C_Wait_Ack();
    I2C_Send_Byte(addr%256);
    I2C_Wait_Ack();
    
    I2C_Send_Byte(dt);
    I2C_Wait_Ack();
    I2C_Stop();
    
    OSTimeDly(2);
}


void AT24Cxx_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead)
{
    while (NumToRead)
    {
        *pBuffer++ = AT24Cxx_ReadOneByte(ReadAddr++);
        NumToRead --;
    }
}


void AT24Cxx_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
    while (NumToWrite--)
    {
        AT24Cxx_WriteOneByte(WriteAddr, *pBuffer);
        WriteAddr ++;
        pBuffer ++;
    }
}


u8 E2rom_Read(u32 addr, u8 *data, u16 len)
{
    u8 val;
    
    // last byte must be crc
    if ((len < 2) || ((addr+len) > AT24CXX_BASE_CONFIG_SIZE))
        return FLASH_OP_ERR_ADDR;
    
    AT24Cxx_Read(addr, data, len);
    val = docrc8(0, data, len-1);
    if (val == *((u8*)(data+len-1)))
        return FLASH_OP_SUCCESS;
    
    return FLASH_OP_ERR_CHK;
}


u8 E2rom_Write(u32 addr, u8 *data, u16 len)
{
    u8 val;
    
    // last byte must be crc
    if ((len < 2) || ((addr+len) > AT24CXX_BASE_CONFIG_SIZE))
        return FLASH_OP_ERR_ADDR;
    
    val = docrc8(0, data, len-1);
    *((u8*)(data+len-1)) = val;
    
    AT24Cxx_Write(addr, data, len);
    return FLASH_OP_SUCCESS;
}


void AT24CXX_Init(void)
{
    I2C_Init();
}

