/************************ (C) COPYRIGHT  ChipNorth ****************************/
/*
 *  Author  : youdeqiang
 *  Date    : 2018-03-30
 *  Library : 2.1.0
 *  MCU     : STM8S207R8T6
 */
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __CONFIG_H__
#define __CONFIG_H__

//------------------------------------------------------------------------------

#define countof(a) (sizeof(a) / sizeof(*(a)))
//IIC
#define I2C_DIRECTION_TRANSMITTER       ((u8)0x00)
#define I2C_DIRECTION_RECEIVER          ((u8)0x01)

#define BU_WRITE_ADDRESS  0x7C
#define BU_READ_ADDRESS   0x7D

#define FLAG_WRITE 0xffff
#define FLAG_READ_CMD  0xfffe
#define FLAG_READ_DATA  0xfffd
//LDC
#define COM_SEG_NUM             40
//LCD频率范围  HZ
#define FREQ_MIN                65
#define FREQ_MAX                90
//LCD频率范围  us
#define FREQ_MIN_US             1000000/(FREQ_MAX*8)
#define FREQ_MAX_US             1000000/(FREQ_MIN*8)


#define ADC_ACCUARCY            4096    //ADC的精度
#define VOLTAGE_DEVIATION       0.08    //允许电压偏差  8%
#define MAX_ADC_VOLTAGE         3.3     //ADC的最大转换电压
#define MIN_ADC_VOLTAGE         0.0     //ADC的最小转换电压


#define VOLTAGE_0               0.0
#define VOLTAGE_1               1.1
#define VOLTAGE_2               2.2
#define VOLTAGE_3               3.3

#define VOLTAGE_ADC_0_MIN       0
#define VOLTAGE_ADC_0_MAX       ((unsigned int)(ADC_ACCUARCY*VOLTAGE_DEVIATION))
#define VOLTAGE_ADC_1_MIN       ((unsigned int)(ADC_ACCUARCY*(1-VOLTAGE_DEVIATION)*VOLTAGE_1/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_1_MAX       ((unsigned int)(ADC_ACCUARCY*(1+VOLTAGE_DEVIATION)*VOLTAGE_1/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_2_MIN       ((unsigned int)(ADC_ACCUARCY*(1-VOLTAGE_DEVIATION)*VOLTAGE_2/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_2_MAX       ((unsigned int)(ADC_ACCUARCY*(1+VOLTAGE_DEVIATION)*VOLTAGE_2/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_3_MIN       ((unsigned int)(ADC_ACCUARCY*(1-VOLTAGE_DEVIATION)*VOLTAGE_3/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_3_MAX       ADC_ACCUARCY

#define VOLTAGE_ADC_0           ((unsigned int)(ADC_ACCUARCY*VOLTAGE_0/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_1           ((unsigned int)(ADC_ACCUARCY*VOLTAGE_1/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_2           ((unsigned int)(ADC_ACCUARCY*VOLTAGE_2/MAX_ADC_VOLTAGE))
#define VOLTAGE_ADC_3           ((unsigned int)(ADC_ACCUARCY*VOLTAGE_3/MAX_ADC_VOLTAGE))

/*
CASE 1 2 3 由COM2上升沿触发
CASE 4 5 6 由COM0上升沿触发
CASE 7 9   由COM2下降沿触发

COM0 -> TIM1 CC1
COM2 -> TIM1 CC3
SEG7 -> TIM1 CC2
*/

#define CASE_0              0X00
#define CASE_1              0X01
#define CASE_2              0X02
#define CASE_3              0X04
#define CASE_4              0X08
#define CASE_5              0X10
#define CASE_6              0X20
#define CASE_7              0X40
#define CASE_8              0X80


#define NOP   nop()

#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#include "stm8s.h"

//Driver
#include "i2c_driver_hard.h"
#include "I2C_driver_soft.h"
#include "uart_driver.h"
#include "timer_driver.h"
//print
#include "HEXtoBIN.h"
//ctrl
#include "process.h"
#include "delay.h"
//CMD&Data
//#include "lcd_cmd.h"
// 200K
u8 I2C_SoftWare_Write200(u8 DeviceAddr, u8* pBuffer, u16 NumByteToWrite);
u8 I2C_SoftWare_Read200(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);
// 300K
u8 I2C_SoftWare_Write300(u8 DeviceAddr, u8* pBuffer, u16 NumByteToWrite);
u8 I2C_SoftWare_Read300(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);
// 400K
u8 I2C_SoftWare_Write400(u8 DeviceAddr, u8* pBuffer, u16 NumByteToWrite);
u8 I2C_SoftWare_Read400(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);
// 500K
u8 I2C_SoftWare_Write500(u8 DeviceAddr, u8* pBuffer, u16 NumByteToWrite);
u8 I2C_SoftWare_Read500(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);
// 600K
u8 I2C_SoftWare_Write600(u8 DeviceAddr, u8* pBuffer, u16 NumByteToWrite);
u8 I2C_SoftWare_Read600(u8 WriteAddr, u8* pBuffer_CMD, u16 NumByteToWrite,\
    u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);
#endif