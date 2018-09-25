

#include "config.h"

//200K
void I2C_SoftWare_Init200(void)
{
    /* GPIO Configuration */
    /*Configure I2C SCL pin */
    GPIO_Init(I2C_PORT, I2C_SCL, GPIO_MODE_OUT_OD_LOW_FAST);
    /*Configure I2C SDA pin */
    GPIO_Init(I2C_PORT, I2C_SDA, GPIO_MODE_OUT_OD_LOW_FAST);
}
u8 I2C_SoftWare_ReInit200(void)
{
    I2C_SoftWare_Init200();

    return 1;
}


void I2C_delay200(void)
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
}


u8 I2C_Start200(void)
{
    SDA_H;
    SCL_H;
    I2C_delay200();
    if (!SDA_read)
        return 0;
    SDA_L;
    I2C_delay200();
    if (SDA_read)
        return 0;
    SCL_L;
    I2C_delay200();
    return 1;
}

void I2C_Stop200(void)
{
    SCL_L;
    I2C_delay200();
    SDA_L;
    I2C_delay200();
    SCL_H;
    I2C_delay200();
    SDA_H;
    I2C_delay200();
}


void I2C_Ack200(void)
{
    SCL_L;
    I2C_delay200();
    SDA_L;
    I2C_delay200();
    SCL_H;
    I2C_delay200();
    SCL_L;
    I2C_delay200();
}

void I2C_NoAck200(void)
{
    SCL_L;
    I2C_delay200();
    SDA_H;
    I2C_delay200();
    SCL_H;
    I2C_delay200();
    SCL_L;
    I2C_delay200();
}

u8 I2C_WaitAck200(void)
{
    SCL_L;
    I2C_delay200();
    SDA_H;
    I2C_delay200();
    SCL_H;

    I2C_delay200();
    if (SDA_read)
    {
        SCL_L;
        I2C_delay200();
        return 0;
    }

    SCL_L;
    I2C_delay200();

    return 1;
}

void I2C_SendByte200(u8 byte)
{
    u8 i = 8;
    while (i--)
    {
        SCL_L;
        I2C_delay200();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        I2C_delay200();
        SCL_H;
        I2C_delay200();
    }
    SCL_L;
}

u8 I2C_ReceiveByte200(void)
{
    u8 i = 8;
    u8 byte = 0;

    SDA_H;
    while (i--)
    {
        byte <<= 1;
        SCL_L;
        I2C_delay200();
        SCL_H;
        I2C_delay200();
        if (SDA_read)
	{
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}

u8 I2C_SoftWare_Write200(u8 WriteAddr, u8* pBuffer, u16 NumByteToWrite)
{
    //Send Start
    u16 i;
    if (!I2C_Start200())
        return I2C_SoftWare_ReInit200();
    //Send Slave Write Address
    I2C_SendByte200(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck200())
    {
        I2C_Stop200();
        return 1;
    }
    //Send Data
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte200(pBuffer[i]);
        if (!I2C_WaitAck200())
	   {
            I2C_Stop200();
            return I2C_SoftWare_ReInit200();
        }
    }
    //Send Stop
    I2C_Stop200();

    return 0;
}

u8 I2C_SoftWare_Read200(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead)
{
    u16 i;
    //Send Start
    if (!I2C_Start200())
        return I2C_SoftWare_ReInit200();
    //Send Slave Write Address
    I2C_SendByte200(WriteAddr | I2C_DIRECTION_TRANSMITTER);
    if (!I2C_WaitAck200())
    {
        I2C_Stop200();
        return 1;
    }
    //Send Read Command
    for (i = 0; i < NumByteToWrite; i++)
    {
        I2C_SendByte200(pBuffer_CMD[i]);
        if (!I2C_WaitAck200())
       {
            I2C_Stop200();
            return I2C_SoftWare_ReInit200();
        }
    }
    //Send Stop
    I2C_Stop200();
    //Send Start
    I2C_Start200();
    //Send Slave Read Address
    I2C_SendByte200(ReadAddr | I2C_DIRECTION_RECEIVER);
    //Wait Ack
    I2C_WaitAck200();
    //Revice Data
    while (NumByteToRead)
    {
        *pBuffer = I2C_ReceiveByte200();
        if (NumByteToRead == 1)
            I2C_NoAck200();
        else
            I2C_Ack200();
        pBuffer++;
        NumByteToRead--;
    }
    //Send Stop
    I2C_Stop200();

    return 0;
}