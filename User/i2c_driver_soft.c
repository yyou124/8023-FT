/*
 *  Author  : youdeqiang
 *  Date    : 2017-07-11
 *  Library : 2.1.0
 *  MCU     : STM8S207R8T6
 */
#include "config.h"



void I2C_SoftWare_Init(void)
{
    /* GPIO Configuration */
    /*Configure I2C SCL pin */
    GPIO_Init(I2C_PORT, I2C_SCL, GPIO_MODE_OUT_OD_LOW_FAST);
    /*Configure I2C SDA pin */
    GPIO_Init(I2C_PORT, I2C_SDA, GPIO_MODE_OUT_OD_LOW_FAST);
}

void I2C_delay(void)
{
    u8 i=0;
    for(;i<1;i++)
    {
    nop();
    nop();
    nop();
    nop();
    nop();

    nop();
    nop();
    nop();
    nop();
    nop();

    nop();
    nop();
    nop();
    nop();
    nop();

    nop();
    nop();
    nop();
    nop();
    nop();
        nop();
    nop();
    nop();
    nop();
    nop();

    nop();
    nop();
    nop();
    nop();
    nop();

    nop();
    nop();
    nop();
    nop();
    nop();

    }
}

u8 I2C_Start(void)
{
    SDA_H;
    SCL_H;
    I2C_delay();
    if (!SDA_read)
        return 0;
    SDA_L;
    I2C_delay();
    if (SDA_read)
        return 0;
    SCL_L;
    I2C_delay();
    return 1;
}

void I2C_Stop(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}


void I2C_Ack(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

void I2C_NoAck(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

u8 I2C_WaitAck(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;

    I2C_delay();
    if (SDA_read)
    {
        SCL_L;
        I2C_delay();
        return 0;
    }

    SCL_L;
    I2C_delay();

    return 1;
}

void I2C_SendByte(u8 byte)
{
    u8 i = 8;
    while (i--)
    {
        SCL_L;
        I2C_delay();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

u8 I2C_ReceiveByte(void)
{
    u8 i = 8;
    u8 byte = 0;

    SDA_H;
    while (i--)
    {
        byte <<= 1;
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if (SDA_read)
	{
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}

u8 I2C_SoftWare_Write(u8 WriteAddr, u8* pBuffer, u16 NumByteToWrite)
{
    //Send Start
    u16 i;
    if (!I2C_Start())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck())
    {
        I2C_Stop();
        return 1;
    }
    //Send Data
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte(pBuffer[i]);
        if (!I2C_WaitAck())
	   {
            I2C_Stop();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop();

    return 0;
}

u8 I2C_SoftWare_Read(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead)
{
    u16 i;
    //Send Start
    if (!I2C_Start())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck())
    {
        I2C_Stop();
        return 1;
    }
    //Send Read Command
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte(pBuffer_CMD[i]);
        if (!I2C_WaitAck())
       {
            I2C_Stop();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop();
    //Send Start
    I2C_Start();
    //Send Slave Read Address
    I2C_SendByte(ReadAddr | I2C_DIRECTION_RECEIVER);
    //Wait Ack
    I2C_WaitAck();
    //Revice Data
    while (NumByteToRead)
    {
        *pBuffer = I2C_ReceiveByte();
        if (NumByteToRead == 1)
            I2C_NoAck();
        else
            I2C_Ack();
        pBuffer++;
        NumByteToRead--;
    }
    //Send Stop
    I2C_Stop();

    return 0;
}

u8 I2C_SoftWare_ReadOnly(u8 ReadAddr, u8* pBuffer, u16 NumByteToRead)
{
    //Send Start
    I2C_Start();
    //Send Slave Read Address
    I2C_SendByte(ReadAddr | I2C_DIRECTION_RECEIVER);
    //Wait Ack
    I2C_WaitAck();
    //Revice Data
    while (NumByteToRead)
    {
        *pBuffer = I2C_ReceiveByte();
        if (NumByteToRead == 1)
            I2C_NoAck();
        else
            I2C_Ack();
        pBuffer++;
        NumByteToRead--;
    }
    //Send Stop
    I2C_Stop();

    return 0;
}

u8 I2C_SoftWare_ReInit(void)
{
    I2C_SoftWare_Init();

    return 1;
}
