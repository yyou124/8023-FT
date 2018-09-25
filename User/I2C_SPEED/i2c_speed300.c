

#include "config.h"

//300K
void I2C_delay300(void)
{
    nop();
}


u8 I2C_Start300(void)
{
    SDA_H;
    SCL_H;
    I2C_delay300();
    if (!SDA_read)
        return 0;
    SDA_L;
    I2C_delay300();
    if (SDA_read)
        return 0;
    SCL_L;
    I2C_delay300();
    return 1;
}

void I2C_Stop300(void)
{
    SCL_L;
    I2C_delay300();
    SDA_L;
    I2C_delay300();
    SCL_H;
    I2C_delay300();
    SDA_H;
    I2C_delay300();
}


void I2C_Ack300(void)
{
    SCL_L;
    I2C_delay300();
    SDA_L;
    I2C_delay300();
    SCL_H;
    I2C_delay300();
    SCL_L;
    I2C_delay300();
}

void I2C_NoAck300(void)
{
    SCL_L;
    I2C_delay300();
    SDA_H;
    I2C_delay300();
    SCL_H;
    I2C_delay300();
    SCL_L;
    I2C_delay300();
}

u8 I2C_WaitAck300(void)
{
    SCL_L;
    I2C_delay300();
    SDA_H;
    I2C_delay300();
    SCL_H;

    I2C_delay300();
    if (SDA_read)
    {
        SCL_L;
        I2C_delay300();
        return 0;
    }

    SCL_L;
    I2C_delay300();

    return 1;
}

void I2C_SendByte300(u8 byte)
{
    u8 i = 8;
    while (i--)
    {
        SCL_L;
        I2C_delay300();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        I2C_delay300();
        SCL_H;
        I2C_delay300();
    }
    SCL_L;
}

u8 I2C_ReceiveByte300(void)
{
    u8 i = 8;
    u8 byte = 0;

    SDA_H;
    while (i--)
    {
        byte <<= 1;
        SCL_L;
        I2C_delay300();
        SCL_H;
        I2C_delay300();
        if (SDA_read)
	{
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}

u8 I2C_SoftWare_Write300(u8 WriteAddr, u8* pBuffer, u16 NumByteToWrite)
{
    //Send Start
    u16 i;
    if (!I2C_Start300())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte300(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck300())
    {
        I2C_Stop300();
        return 1;
    }
    //Send Data
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte300(pBuffer[i]);
        if (!I2C_WaitAck300())
	   {
            I2C_Stop300();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop300();

    return 0;
}

u8 I2C_SoftWare_Read300(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead)
{
    u16 i;
    //Send Start
    if (!I2C_Start300())
        return I2C_SoftWare_ReInit();
    //Send Slave Write Address
    I2C_SendByte300(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck300())
    {
        I2C_Stop300();
        return 1;
    }
    //Send Read Command
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte300(pBuffer_CMD[i]);
        if (!I2C_WaitAck300())
       {
            I2C_Stop300();
            return I2C_SoftWare_ReInit();
        }
    }
    //Send Stop
    I2C_Stop300();
    //Send Start
    I2C_Start300();
    //Send Slave Read Address
    I2C_SendByte300(ReadAddr | I2C_DIRECTION_RECEIVER);
    //Wait Ack
    I2C_WaitAck300();
    //Revice Data
    while (NumByteToRead)
    {
        *pBuffer = I2C_ReceiveByte300();
        if (NumByteToRead == 1)
            I2C_NoAck300();
        else
            I2C_Ack300();
        pBuffer++;
        NumByteToRead--;
    }
    //Send Stop
    I2C_Stop300();

    return 0;
}