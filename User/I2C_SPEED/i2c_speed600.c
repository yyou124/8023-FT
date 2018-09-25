

#include "config.h"

//600K


u8 I2C_Start600(void)
{
    SDA_H;
    SCL_H;
    nop();
    if (!SDA_read)
        return 0;
    SDA_L;
    nop();
    if (SDA_read)
        return 0;
    SCL_L;
    nop();
    return 1;
}

void I2C_Stop600(void)
{
    SCL_L;
    nop();
    SDA_L;
    nop();
    SCL_H;
    nop();
    SDA_H;
    nop();
}


void I2C_Ack600(void)
{
    SCL_L;
    nop();
    SDA_L;
    nop();
    SCL_H;
    nop();
    SCL_L;
    nop();
}

void I2C_NoAck600(void)
{
    SCL_L;
    nop();
    SDA_H;
    nop();
    SCL_H;
    nop();
    SCL_L;
    nop();
}

u8 I2C_WaitAck600(void)
{
    SCL_L;
    nop();
    SDA_H;
    nop();
    SCL_H;

    nop();
    if (SDA_read)
    {
        SCL_L;
        nop();

        return 0;
    }

    SCL_L;
    nop();

    return 1;
}

void I2C_SendByte600(u8 byte)
{
    u8 i = 8;
    while (i--)
    {
        SCL_L;
        nop();

        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        nop();

        SCL_H;
        nop();

    }
    SCL_L;
}

u8 I2C_ReceiveByte600(void)
{
    u8 i = 8;
    u8 byte = 0;

    SDA_H;
    while (i--)
    {
        byte <<= 1;
        SCL_L;
        nop();

        SCL_H;
        nop();

        if (SDA_read)
        {
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}

u8 I2C_SoftWare_Write600(u8 WriteAddr, u8* pBuffer, u16 NumByteToWrite)
{
    //Send Start
    u16 i;
    if (!I2C_Start600())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte600(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck600())
    {
        I2C_Stop600();
        return 1;
    }
    //Send Data
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte600(pBuffer[i]);
        if (!I2C_WaitAck600())
	   {
            I2C_Stop600();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop600();

    return 0;
}

u8 I2C_SoftWare_Read600(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead)
{
    u16 i;
    //Send Start
    if (!I2C_Start600())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte600(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck600())
    {
        I2C_Stop600();
        return 1;
    }
    //Send Read Command
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte600(pBuffer_CMD[i]);
        if (!I2C_WaitAck600())
       {
            I2C_Stop600();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop600();
    //Send Start
    I2C_Start600();
    //Send Slave Read Address
    I2C_SendByte600(ReadAddr | I2C_DIRECTION_RECEIVER);
    //Wait Ack
    I2C_WaitAck600();
    //Revice Data
    while (NumByteToRead)
    {
        *pBuffer = I2C_ReceiveByte600();
        if (NumByteToRead == 1)
            I2C_NoAck600();
        else
            I2C_Ack600();
        pBuffer++;
        NumByteToRead--;
    }
    //Send Stop
    I2C_Stop600();

    return 0;
}