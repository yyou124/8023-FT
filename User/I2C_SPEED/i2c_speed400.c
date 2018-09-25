

#include "config.h"

//200K


u8 I2C_Start400(void)
{
    SDA_H;
    SCL_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    if (!SDA_read)
        return 0;
    SDA_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    if (SDA_read)
        return 0;
    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    return 1;
}

void I2C_Stop400(void)
{
    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SDA_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SCL_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SDA_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
}


void I2C_Ack400(void)
{
    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SDA_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SCL_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
}

void I2C_NoAck400(void)
{
    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SDA_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SCL_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
}

u8 I2C_WaitAck400(void)
{
    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SDA_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    SCL_H;

    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    if (SDA_read)
    {
        SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
        return 0;
    }

    SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();

    return 1;
}

void I2C_SendByte400(u8 byte)
{
    u8 i = 8;
    while (i--)
    {
        SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
        SCL_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    }
    SCL_L;
}

u8 I2C_ReceiveByte400(void)
{
    u8 i = 8;
    u8 byte = 0;

    SDA_H;
    while (i--)
    {
        byte <<= 1;
        SCL_L;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
        SCL_H;
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
    nop();
        if (SDA_read)
	{
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}

u8 I2C_SoftWare_Write400(u8 WriteAddr, u8* pBuffer, u16 NumByteToWrite)
{
    //Send Start
    u16 i;
    if (!I2C_Start400())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte400(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck400())
    {
        I2C_Stop400();
        return 1;
    }
    //Send Data
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte400(pBuffer[i]);
        if (!I2C_WaitAck400())
	   {
            I2C_Stop400();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop400();

    return 0;
}

u8 I2C_SoftWare_Read400(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead)
{
    u16 i;
    //Send Start
    if (!I2C_Start400())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte400(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck400())
    {
        I2C_Stop400();
        return 1;
    }
    //Send Read Command
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte400(pBuffer_CMD[i]);
        if (!I2C_WaitAck400())
       {
            I2C_Stop400();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop400();
    //Send Start
    I2C_Start400();
    //Send Slave Read Address
    I2C_SendByte400(ReadAddr | I2C_DIRECTION_RECEIVER);
    //Wait Ack
    I2C_WaitAck400();
    //Revice Data
    while (NumByteToRead)
    {
        *pBuffer = I2C_ReceiveByte400();
        if (NumByteToRead == 1)
            I2C_NoAck400();
        else
            I2C_Ack400();
        pBuffer++;
        NumByteToRead--;
    }
    //Send Stop
    I2C_Stop400();

    return 0;
}