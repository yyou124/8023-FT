/************************ (C) COPYRIGHT  ChipNorth ****************************/
/*
 *  Author  : youdeqiang
 *  Date    : 2017-07-11
 *  Library : 2.1.0
 *  MCU     : STM8S207R8T6
 */
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __I2C_DRIVER_SOFT_H__
#define __I2C_DRIVER_SOFT_H__
/* Includes ------------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
//Software I2C Ports define
//PE1 ->  SCL
//PE2 ->  SDA
#define I2C_PORT      GPIOE
#define I2C_SCL       GPIO_PIN_1
#define I2C_SDA       GPIO_PIN_2


#define SCL_H         I2C_PORT->ODR |= I2C_SCL
#define SCL_L         I2C_PORT->ODR &= (~I2C_SCL)

#define SDA_H         I2C_PORT->ODR |= I2C_SDA
#define SDA_L         I2C_PORT->ODR &= (~I2C_SDA)

#define SCL_read      I2C_PORT->IDR & I2C_SCL
#define SDA_read      I2C_PORT->IDR & I2C_SDA

/* Private function prototypes -----------------------------------------------*/



void I2C_delay(void);
u8 I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
u8 I2C_WaitAck(void);
void I2C_SendByte(u8 byte);
u8 I2C_ReceiveByte(void);
extern void I2C_SoftWare_Init(void);
u8 I2C_SoftWare_Write(u8 DeviceAddr, u8* pBuffer, u16 NumByteToWrite);
u8 I2C_SoftWare_Read(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);

u8 I2C_SoftWare_ReadOnly(u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);
extern u8 I2C_SoftWare_ReInit(void);

#endif /* __I2C_DRIVER_SOFT_H__ */