/************************ (C) COPYRIGHT  ChipNorth ****************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __I2C_DRIVER_HARD_H
#define __I2C_DRIVER_HARD_H

/* Includes ------------------------------------------------------------------*/
//#include "stm8s.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed             100000
#define I2C_SLAVE_ADDRESS	   0xA0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void I2C_Driver_Init(void);
void I2C_Send(u8 Address,u8 *DataBuff,u16 DataLen);
void I2C_OFF(void);
void I2C_Read(u8 Address,u8 *DataBuff,u16 NumByteToRead);
void I2C_ReadCMD(u8 Address,u8 *DataBuff,u16 NumByteToRead);


#endif /* __I2C_DRIVER_H */

/************************ (C) COPYRIGHT  ChipNorth ****************************/