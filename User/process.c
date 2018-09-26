/************************ (C) COPYRIGHT  ChipNorth ****************************/
/*
 *  Author  : youdeqiang
 *  Date    : 2017-07-11
 *  Library : 2.1.0
 *  MCU     : STM8S207R8T6
 */
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/*******************************************************************************
* Function Name  : PowerCtrl
* Description    :
* Input          : VddTest, VddTest
* Output         : None
* Return         : None
*******************************************************************************/
void PowerCtrl(u8 VddTest, u8 status)
{
  GPIO_Init(GPIOE, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_SLOW);  //VDD_CTL1
  if(VddTest == _5_0V) //VddTest = 5V;
  {
    VDD_CTL1_H;
  }
  else if(VddTest == _3_3V) //VddTest = 3.3V;
  {
    VDD_CTL1_L;
  }
  else
  {
    return;
  }
  if(status == OS1)
  {
    BL1551Ctrl(VddTest,_X_XV,VddTest);
    _74CTRL(_74_PULL);
  }
  else if(status == OS2)
  {
    BL1551Ctrl(VddTest,_0_0V,_X_XV);
    _74CTRL(_74_SINK);
  }
  else if(status == NORL)
  {
    BL1551Ctrl(VddTest,VddTest,_0_0V);
    _74CTRL(_74_HRS);
  }
  else{
    return;
  }
}
/*******************************************************************************
* Function Name  : BL1551Ctrl
* Description    :
* Input          : VddTest, DutVDD, DutVSS
* Output         : None
* Return         : None
*******************************************************************************/
void BL1551Ctrl(u8 VddTest, u8 DutVDD, u8 DutVSS)
{

  //Init Pins VddTest = 5V; DutVDD = z; DutVSS = z
  GPIO_Init(GPIOE, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_SLOW);  //VDD_CTL1
  GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_SLOW);  //VDD_CTL2
  GPIO_Init(GPIOE, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW);  //VDD_CTL3
  GPIO_Init(GPIOG, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);  //VDD_CTL4
  //VddTest ctrl
  if(VddTest == _5_0V) //VddTest = 5V;
  {
    VDD_CTL1_H;
  }
  else if(VddTest == _3_3V) //VddTest = 3.3V;
  {
    VDD_CTL1_L;
  }
  else
  {
    return;
  }

  if((DutVDD == _X_XV) && (DutVSS == _X_XV))//DutVDD = z; DutVSS = z
  {
    VDD_CTL2_H;
    VDD_CTL3_H;
    VDD_CTL4_L;
  }
  else if((DutVDD == VddTest) && (DutVSS == _0_0V))//DutVDD = VddTest; DutVSS = 0
  {
    VDD_CTL2_H;
    VDD_CTL3_L;
    VDD_CTL4_H;
  }
  else if((DutVDD == _X_XV) && (DutVSS == VddTest))//DutVDD = z; DutVSS = VddTest
  {
    VDD_CTL2_L;
    VDD_CTL3_L;
    VDD_CTL4_L;
  }
  else if((DutVDD == _0_0V) && (DutVSS == _X_XV))//DutVDD = 0; DutVSS = z
  {
    VDD_CTL2_L;
    VDD_CTL3_H;
    VDD_CTL4_H;
  }
  else
  {
    return;
  }
}

/*******************************************************************************
* Function Name  : _74CTRL
* Description    :
* Input          : _74STAT
* Output         : None
* Return         : None
*******************************************************************************/
void _74CTRL(u8 _74STAT)
{
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW);  //_74_CTRL
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW);  //OE_N

  if(_74STAT == _74_PULL) //Pull current
  {
    OE_N_L;
    _74_CTRL_L;
  }
  else if(_74STAT == _74_SINK)  //Sink current
  {
    OE_N_L;
    _74_CTRL_H;
  }
  else if(_74STAT == _74_HRS) //High resistance state
  {
    OE_N_H;
  }
  else
  {
    return;
  }

}
/*******************************************************************************
* Function Name  : Cmd_Flag
* Description    : Decode UART input parameters
* Input          : Uart Rx data
* Output         : None
* Return         : Flag
*******************************************************************************/
u16 Cmd_Flag(u8 *DataBuff)
{
  u16 CMD_Flag = 0;
  //Command Judgment
  u8 mode_sel = 0;
  if(*DataBuff == BU_READ_ADDRESS) mode_sel = 1;
  else if(*DataBuff == BU_WRITE_ADDRESS) mode_sel = 2;
  else if((*DataBuff != BU_READ_ADDRESS)&&(*DataBuff != BU_WRITE_ADDRESS) \
    &&(*DataBuff != 0x00))  CMD_Flag = 0xfffc;
  DataBuff++;
  //Read Mode

  if(mode_sel == 1)
  {
    if(*DataBuff == 0xEC) CMD_Flag = FLAG_READ_CMD;//Read CMD
    else if(*DataBuff == 0xE8) CMD_Flag = FLAG_READ_DATA;//Read Data
    else CMD_Flag = 0xfffb;
  }
  //Write Mode
  if(mode_sel == 2) CMD_Flag = FLAG_WRITE;

  return CMD_Flag;
}


/*******************************************************************************
* Function Name  : case_5A_check
* Description    : Decode UART input parameters
* Input          : AdcData, AdcData
* Output         : None
* Return         : case_flag: 0- test failed, 1- test success, 2- data error, 3- case error
*******************************************************************************/
u8 case_check(u16 *AdcData, u8 case_flag,u8 case_state)
{
  u8 i;
  u16 com[4];// COM0, COM1, COM2 COM3
  u16 seg_even[18], seg_odd[18];
  u16 voltage_range[12];
  //ADC测试值分组
  com[0] = AdcData[5];
  com[1] = AdcData[6];
  com[2] = AdcData[7];
  com[3] = AdcData[8];
  //SEG奇数组
  seg_odd[0] = AdcData[0];
  seg_odd[1] = AdcData[2];
  seg_odd[2] = AdcData[4];
  seg_odd[3] = AdcData[10];
  seg_odd[4] = AdcData[12];
  seg_odd[5] = AdcData[14];
  for(i=8; i<20; i++)
    seg_odd[i-2] = AdcData[2*i+1];
  //SEG偶数组
  seg_even[0] = AdcData[1];
  seg_even[1] = AdcData[3];
  seg_even[2] = AdcData[9];
  seg_even[3] = AdcData[11];
  seg_even[4] = AdcData[13];
  seg_even[5] = AdcData[15];
  for(i=8; i<20; i++)
    seg_even[i-2] = AdcData[2*i];

  //配置需要校验的内容
  if(case_state ==0x5A)
  {
    if(case_flag == CASE_1)
    {
      voltage_range[0] = VOLTAGE_ADC_2_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_2_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_0_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_0_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_2_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_2_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_2_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_2_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_3_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_3_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_1_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_1_MIN;//seg_odd 电压最小值
      if(com[1]>voltage_range[2] || com[1]<voltage_range[3]) return 2;
    }
    else if(case_flag == CASE_2)
    {
      voltage_range[0] = VOLTAGE_ADC_1_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_1_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_1_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_1_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_3_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_3_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_1_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_1_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_2_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_2_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_0_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_0_MIN;//seg_odd 电压最小值
      if(com[2]>voltage_range[4] || com[2]<voltage_range[5]) return 2;
    }
    else if(case_flag == CASE_3)
    {
      voltage_range[0] = VOLTAGE_ADC_0_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_0_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_2_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_2_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_2_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_2_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_2_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_2_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_1_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_1_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_3_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_3_MIN;//seg_odd 电压最小值
      if(com[0]>voltage_range[0] || com[0]<voltage_range[1]) return 2;
    }
    else if(case_flag == CASE_4)
    {
      voltage_range[0] = VOLTAGE_ADC_2_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_2_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_2_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_2_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_2_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_2_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_0_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_0_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_3_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_3_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_1_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_1_MIN;//seg_odd 电压最小值
      if(com[3]>voltage_range[6] || com[3]<voltage_range[7]) return 2;
    }
    else if(case_flag == CASE_5)
    {
      voltage_range[0] = VOLTAGE_ADC_3_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_3_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_1_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_1_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_1_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_1_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_1_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_1_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_2_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_2_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_0_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_0_MIN;//seg_odd 电压最小值
      if(com[0]>voltage_range[0] || com[0]<voltage_range[1]) return 2;
    }
    else if(case_flag == CASE_6)
    {
      voltage_range[0] = VOLTAGE_ADC_2_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_2_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_2_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_2_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_0_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_0_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_2_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_2_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_1_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_1_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_3_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_3_MIN;//seg_odd 电压最小值
      if(com[2]>voltage_range[4] || com[2]<voltage_range[5]) return 2;
    }
    else if(case_flag == CASE_7)
    {
      voltage_range[0] = VOLTAGE_ADC_1_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_1_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_1_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_1_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_1_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_1_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_3_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_3_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_0_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_0_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_2_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_2_MIN;//seg_odd 电压最小值
      if(com[3]>voltage_range[6] || com[3]<voltage_range[7]) return 2;
    }
    else if(case_flag == CASE_8)
    {
      voltage_range[0] = VOLTAGE_ADC_1_MAX;//com0 电压最大值
      voltage_range[1] = VOLTAGE_ADC_1_MIN;//com0 电压最小值
      voltage_range[2] = VOLTAGE_ADC_3_MAX;//com1 电压最大值
      voltage_range[3] = VOLTAGE_ADC_3_MIN;//com1 电压最小值
      voltage_range[4] = VOLTAGE_ADC_1_MAX;//com2 电压最大值
      voltage_range[5] = VOLTAGE_ADC_1_MIN;//com2 电压最小值
      voltage_range[6] = VOLTAGE_ADC_1_MAX;//com3 电压最大值
      voltage_range[7] = VOLTAGE_ADC_1_MIN;//com3 电压最小值
      voltage_range[8] = VOLTAGE_ADC_0_MAX;//seg_even 电压最大值
      voltage_range[9] = VOLTAGE_ADC_0_MIN;//seg_even 电压最小值
      voltage_range[10] = VOLTAGE_ADC_2_MAX;//seg_odd 电压最大值
      voltage_range[11] = VOLTAGE_ADC_2_MIN;//seg_odd 电压最小值
      if(com[1]>voltage_range[2] || com[1]<voltage_range[3]) return 2;
    }
  }
  else if(case_state == 0xA5)
  {

  }
  else//CASE不对
    {
      return 3;
    }
  //执行判断
  while(1)
  {
    //判断COM值
    if(com[0]>voltage_range[0] || com[0]<voltage_range[1]) return 0;
    if(com[1]>voltage_range[2] || com[1]<voltage_range[3]) return 0;
    if(com[2]>voltage_range[4] || com[2]<voltage_range[5]) return 0;
    if(com[3]>voltage_range[6] || com[3]<voltage_range[7]) return 0;
    //判断SEG值
    for(i=0; i<18; i++)
    {
      if(seg_even[0]>voltage_range[8] || seg_even[0]<voltage_range[9]) return 0;
      if(seg_odd[0]>voltage_range[10] || seg_odd[0]<voltage_range[11]) return 0;

    }
    return 1;
  }

}

/*******************************************************************************
* Function Name  : i2c_speed_switch
* Description    : Test IIC read and write speed
* Input          : i2c_speed, pBuer_CMD, pBufferWirte etc..
* Output         : None
* Return         : diff_flag: 1- TEST SUCCESS 0- TEST FAILED
*******************************************************************************/
u8 i2c_speed_switch(u16 i2c_speed,u8* pBuer_CMD, u8* pBufferWirte, u16 NumByteToWrite,\
  u8* pBufferRead, u16 NumByteToRead)
{
  //Input: i2c speed:
  //Output: diff Flag(Is Write data are the same as READ data?)
  u8 diff_flag, i;

  if(i2c_speed == 100)
  {
    I2C_SoftWare_Write(BU_WRITE_ADDRESS,pBufferWirte,NumByteToWrite);
    Delay_MS(500);
    I2C_SoftWare_Read(BU_WRITE_ADDRESS,pBuer_CMD,0x02,BU_READ_ADDRESS,pBufferRead,NumByteToRead);
    Delay_MS(500);
  }
  else if(i2c_speed ==200)
  {
    I2C_SoftWare_Write200(BU_WRITE_ADDRESS,pBufferWirte,NumByteToWrite);
    Delay_MS(500);
    I2C_SoftWare_Read200(BU_WRITE_ADDRESS,pBuer_CMD,0x02,BU_READ_ADDRESS,pBufferRead,NumByteToRead);
    Delay_MS(500);
  }
  else if(i2c_speed == 300)
  {
    I2C_SoftWare_Write300(BU_WRITE_ADDRESS,pBufferWirte,NumByteToWrite);
    Delay_MS(500);
    I2C_SoftWare_Read300(BU_WRITE_ADDRESS,pBuer_CMD,0x02,BU_READ_ADDRESS,pBufferRead,NumByteToRead);
    Delay_MS(500);
  }
  else if(i2c_speed == 400)
  {
    I2C_SoftWare_Write400(BU_WRITE_ADDRESS,pBufferWirte,NumByteToWrite);
    Delay_nUS(50);
    I2C_SoftWare_Read400(BU_WRITE_ADDRESS,pBuer_CMD,0x02,BU_READ_ADDRESS,pBufferRead,NumByteToRead);
  }
  else if(i2c_speed == 500)
  {
    I2C_SoftWare_Write500(BU_WRITE_ADDRESS,pBufferWirte,NumByteToWrite);
    Delay_MS(500);
    I2C_SoftWare_Read500(BU_WRITE_ADDRESS,pBuer_CMD,0x02,BU_READ_ADDRESS,pBufferRead,NumByteToRead);
  }
  //else if(i2c_speed == 600)
  //Write data compared with Read data
  diff_flag = 1;
  for (i = 0; i < 9; i++)
  {
    if (pBufferWirte[i + 2] != pBufferRead[i])
    {
      diff_flag = 0;
      break;
    }
  }
  return diff_flag;
}

/*******************************************************************************
* Function Name  : Key_init
* Description    : Key initialization
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Key_init(void)
{
  GPIO_Init(GPIOA,GPIO_PIN_6,GPIO_MODE_IN_PU_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_FALL_ONLY);
}

/*******************************************************************************
* Function Name  : Key_Scan
* Description    : Detect if the button is pressed
* Input          : None
* Output         : None
* Return         : ButtomStatus: 0- No pressed  1- Pressed
*******************************************************************************/
u8 Key_Scan(void)
{
    u8 ButtomStatus;
    ButtomStatus = 0;
    if(!GPIO_ReadInputPin(GPIOA,GPIO_PIN_6))
    {
        Delay_Time(1);
        if(!GPIO_ReadInputPin(GPIOC,GPIO_PIN_6))
        {
          while(!GPIO_ReadInputPin(GPIOD,GPIO_PIN_6));
          ButtomStatus = 1;
        }
    }
    return ButtomStatus;
}

/*******************************************************************************
* Function Name  : Delay_Time
* Description    : Delay a period of time
* Input          : nCount
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_Time(u16 nCount)
{
  /* Decrement nCount value */
    u16 Timer;
    while(nCount--)
    {
      Timer = 1600;
      while(Timer--);
    }
}

