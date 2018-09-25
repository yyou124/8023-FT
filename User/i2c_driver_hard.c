

/* Includes ------------------------------------------------------------------*/
#include "config.h"


/*******************************************************************************
* Function Name  : I2C_DRIVER_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Driver_Init(void)
{
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C_Speed, I2C_SLAVE_ADDRESS, I2C_DUTYCYCLE_2,I2C_ACK_CURR, \
            I2C_ADDMODE_7BIT, CLK_GetClockFreq()/1000000);
  /* I2C Peripheral Enable */
  I2C_Cmd(ENABLE);
}
/*******************************************************************************
* Function Name  : I2C_Send
* Description    : Hardware I2C send
* Input          : Slave address,
                    The head pointer of the data to be sent;
                    The length of the data to be sent;
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Send(u8 Address,u8 *DataBuff,u16 DataLen)
{
  /* While the bus is busy */
  while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
    ;
  /* Send START condition */
  I2C_GenerateSTART(ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    ;
  /* Send SLAVE address for write */
  I2C_Send7bitAddress(Address, I2C_DIRECTION_TX);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    ;
  //Send Data
  while(DataLen--)
  {
    //while( !I2C_GetFlagStatus(I2C_FLAG_TXEMPTY));//Wait register empty
    I2C_SendData(*DataBuff);
    /* Point to the next byte to be written */
    DataBuff++;
    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING))
     ;
  }
  /* Send STOP condition */
  I2C_GenerateSTOP(ENABLE);
}
/*******************************************************************************
* Function Name  : I2C_OFF
* Description    : I2C modle off
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/

void I2C_OFF(void)
{
  u16 time;
  //Wait for the bus idle, timeout to wait no longer
  time = 500;
  while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY) || --time);
  //I2C modle off
  I2C_Cmd(DISABLE);
  //I2C clock off
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
}
/*******************************************************************************
* Function Name  : I2C_Read
* Description    : Hardware I2C Read
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void I2C_Read(u8 Address,u8 *DataBuff,u16 NumByteToRead)
{
  /* While the bus is busy */
  while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
    ;
  /* Generate start & wait event detection */
  I2C_GenerateSTART(ENABLE);
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    ;
   // /* Send slave Address in write direction & wait detection event */
   // I2C_Send7bitAddress(BU_WRITE_ADDRESS, I2C_DIRECTION_RX);
   // /* Test on EV6 and clear it */
   // while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
   //   ;
   // /* Send command: DDRAM address P5 = 1 */
   //  I2C_SendData(0xEC);
   // /* Test on EV8 and clear it */
   // while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
   //   ;
   // /* Send command: DDRAM address P[4:0] = 0_1100 */
   // I2C_SendData(0x8C);
   // /* Test on EV8 and clear it */
   // while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
   //   ;
   // /* Generate start again & wait event detection */
   // I2C_GenerateSTART(ENABLE);
   // /* Test on EV5 and clear it */
   // while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
   //   ;
  /* Send slave Address in write direction & wait detection event */
  I2C_Send7bitAddress(Address, I2C_DIRECTION_RX);
  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    ;
  /* While there is data to be read */
  while(NumByteToRead)
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C_ACK_NONE);
      /* Send STOP Condition */
      I2C_GenerateSTOP(ENABLE);
    }
    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      /* Read a byte from the EEPROM */
      *DataBuff = I2C_ReceiveData();
      /* Point to the next location where the byte read will be saved */
      DataBuff++;
      /* Decrement the read bytes counter */
      NumByteToRead--;
    }
  }
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C_ACK_CURR);
}

/*******************************************************************************
* Function Name  : I2C_Read
* Description    : Hardware I2C Read
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void I2C_ReadCMD(u8 Address,u8 *DataBuff,u16 NumByteToRead)
{
  /* While the bus is busy */
  while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
    ;
  /* Generate start & wait event detection */
  I2C_GenerateSTART(ENABLE);
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    ;
   /* Send slave Address in write direction & wait detection event */
   I2C_Send7bitAddress(BU_WRITE_ADDRESS, I2C_DIRECTION_RX);
   /* Test on EV6 and clear it */
   while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
     ;
   /* Send command: DDRAM address P5 = 1 */
    I2C_SendData(0xEC);
   /* Test on EV8 and clear it */
   while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
     ;
   /* Send command: DDRAM address P[4:0] = 0_1100 */
   I2C_SendData(0x8C);
   /* Test on EV8 and clear it */
   while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
     ;
   /* Generate start again & wait event detection */
   I2C_GenerateSTART(ENABLE);
   /* Test on EV5 and clear it */
   while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
     ;
  /* Send slave Address in write direction & wait detection event */
  I2C_Send7bitAddress(Address, I2C_DIRECTION_RX);
  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    ;
  /* While there is data to be read */
  while(NumByteToRead)
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C_ACK_NONE);
      /* Send STOP Condition */
      I2C_GenerateSTOP(ENABLE);
    }
    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      /* Read a byte from the EEPROM */
      *DataBuff = I2C_ReceiveData();
      /* Point to the next location where the byte read will be saved */
      DataBuff++;
      /* Decrement the read bytes counter */
      NumByteToRead--;
    }
  }
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C_ACK_CURR);
}