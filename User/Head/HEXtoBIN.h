/************************ (C) COPYRIGHT  ChipNorth ****************************/
/*
 *  Author  : youdeqiang
 *  Date    : 2018-03-30
 *  Library : 2.1.0
 *  MCU     : STM8S207R8T6
 */
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __HEX_TO_BIN_H__
#define __HEX_TO_BIN_H__


/* Private function prototypes -----------------------------------------------*/
void decToBin(int num, char *buffer);
void reOrder(char *buffer);
void supplement_zero(char *buffer);
void printf_binary_byte(int num);


#endif /* __HEX_TO_BIN_H__ */