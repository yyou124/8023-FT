/************************ (C) COPYRIGHT  ChipNorth ****************************/
/*
 *  Author  : youdeqiang
 *  Date    : 2017-07-11
 *  Library : 2.1.0
 *  MCU     : STM8S207R8T6
 */
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __PROCESS_H__
#define __PROCESS_H__
/* Private function prototypes -----------------------------------------------*/
//FOR POWER CTRL
#define OS1                1
#define OS2                2
#define NORL               3
//ENB-->H  B to A1
//ENB-->L  B to A2
#define VDD_CTL1_H         GPIOE->ODR |= GPIO_PIN_3
#define VDD_CTL1_L         GPIOE->ODR &= (~GPIO_PIN_3)
#define VDD_CTL2_H         GPIOE->ODR |= GPIO_PIN_5
#define VDD_CTL2_L         GPIOE->ODR &= (~GPIO_PIN_5)
#define VDD_CTL3_H         GPIOE->ODR |= GPIO_PIN_6
#define VDD_CTL3_L         GPIOE->ODR &= (~GPIO_PIN_6)
#define VDD_CTL4_H         GPIOG->ODR |= GPIO_PIN_1
#define VDD_CTL4_L         GPIOG->ODR &= (~GPIO_PIN_1)
#define _5_0V              50
#define _3_3V              33
#define _0_0V              0
#define _X_XV              0xFF

//74HC245 CTRL
//DIR-->l B to A
//DIR-->H A to B
#define OE_N_H             GPIOC->ODR |= GPIO_PIN_7
#define OE_N_L             GPIOC->ODR |= GPIO_PIN_7
#define _74_CTRL_H         GPIOC->ODR |= GPIO_PIN_6
#define _74_CTRL_L         GPIOC->ODR |= GPIO_PIN_6
#define _74_PULL            1
#define _74_SINK            2
#define _74_HRS             3

void PowerCtrl(u8 VddTest, u8 status);
void BL1551Ctrl(u8 VddTest, u8 DutVDD, u8 DutVSS);
void _74CTRL(u8 _74STAT);
u16 Cmd_Flag(u8 *DataBuff);
u8 i2c_speed_switch(u16 i2c_speed,u8* pBuer_CMD, u8* pBufferWirte, u16 NumByteToWrite,\
  u8* pBufferRead, u16 NumByteToRead);

u8 case_check(u16 *AdcData, u8 case_flag,u8 case_state);
void Key_init(void);
u8 Key_Scan(void);
void Delay_Time(u16 nCount);
#endif /* __PROCESS_H__ */