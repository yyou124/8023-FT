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


u16 Cmd_Flag(u8 *DataBuff);
u8 i2c_speed_switch(u16 i2c_speed,u8* pBuer_CMD, u8* pBufferWirte, u16 NumByteToWrite,\
  u8* pBufferRead, u16 NumByteToRead);

u8 case_check(u16 *AdcData, u8 case_flag,u8 case_state);
void Key_init(void);
u8 Key_Scan(void);
void Delay_Time(u16 nCount);
#endif /* __PROCESS_H__ */