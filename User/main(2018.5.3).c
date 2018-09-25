/**
  ******************************************************************************
  * @file    Project/main.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    05-July-2017
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 ChipNorth</center></h2>
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "lcd_cmd.h"
#include "config.h"
/* Private defines -----------------------------------------------------------*/

/*
TXD PD5
RXD PD6
*/
/*
CASE 1 2 3 由COM2上升沿触发
CASE 4 5 6 由COM0上升沿触发
CASE 7 9   由COM2下降沿触发

COM0 -> TIM1 CC1
COM2 -> TIM1 CC3
SEG7 -> TIM1 CC2
*/

#define BufferSize1             (UART_RX_NUM - 1)
#define RxBuffer1               (RxBuffer + 1)
#define DATA_RAM          0x10
/* Private variables ---------------------------------------------------------*/
extern u8 RxBuffer[RxBufferSize];
extern u8 UART_RX_NUM;
volatile u16 CMD_Flag = 0x0001;
u8 Tx1_Buffer[64]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//WI-FI相关变量
u8 Ready_TuoChuang=0;
u8 Ready_Ap=0;
u16 Tim1_num=0;//中TIM1中每次中断加1
//ADC数据汇总
u8 Rst_adc; //重新启动ADC采集
u8 rsing_falling_n; //ADC上升沿/下降沿采样切换
u16 ADC_ConvertedValue[COM_SEG_NUM];
float ADC_ConvertedValueLocal[COM_SEG_NUM];
//Timer1 -> Channel3 相关变量
u8 isCaptureOver;  //捕捉结束标志
u16 fre;           //频率
u8 Channel3_num;   //捕捉次数
//STM32
u8 ADC_START;      //stm32 ADC采集使能
/* Private function prototypes -----------------------------------------------*/

u8 i2c_speed_test(u16 i2c_speed);
/* Main function  ------------------------------------------------------------*/
void main(void)
{
  u8 flag,i;
  u16 speeded;
  float freq[10];
  //Initialize MCU mode
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  Uart_Driver_Init();
  Delay_MS(200);
  //I2C_Driver_Init(); //Hardware IIC
  I2C_SoftWare_Init();  //Software IIC
  signal_capture_Init();  //捕捉使能
  Key_init();
  Delay_MS(200);
  //STM32 U2-U5 使能信号PG0~PG3 LED信号 PG6
  GPIO_Init(GPIOG, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_SLOW);
  enableInterrupts();

  //WI-FI模块初始化
  //UART3_ITConfig(UART3_IT_RXNE_OR, DISABLE);//初始化过程关闭接收中断，否则影响
  //Init_ESP8266_tuochuang();//初始化为透传模式
  //UART3_ITConfig(UART3_IT_RXNE_OR, ENABLE);
  Channel3_num = 0;
  flag = 0x01;
  speeded = 300;//直接从400K开始测试
  ADC_START = 0;
  Rst_adc = 0;
  //rsing_falling_n = 1;
  //Initialize LCD
  printf("IIC Speed Test Start.\r\n");
  Delay_MS(500);
  I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
  //I2C_Send(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
  //I2C_Send(BU_WRITE_ADDRESS, CMD_ALLON, countof(CMD_ALLON));
  //I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLON, countof(CMD_ALLON));
  while (1)
  {
    if (flag == 0x01)//I2C速度测试 100K-500K
    {
      //直接从400K开始测试
      speeded += 100;
      flag = i2c_speed_test(speeded);

    }
    else if(flag == 0x0E)//输出测试结果
    {
      printf("The fastest I2C speed of this chip is %dK.\r\n", speeded-100);
      //刷入新显存
      I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
      I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLON, countof(CMD_ALLON));
      flag = 0x0F;
    }
    else if(flag == 0x0F)//SEG频率测试
    {
      if((Channel3_num == 3) || (Rst_adc == 1))//进行ADC采集,发送enable信号
      {
        if(Channel3_num>0)//求频率平均值，第一个不要
        {
          freq[0] = 0;
          for(i=1;i<3;i++) freq[0] += freq[i];
          printf("Average LCD Frequence: %0.2fHz\r\n",freq[0]/2);
        }
        Delay_MS(500);
        printf("ADC start\r\n");

        // if(rsing_falling_n == 1)
        //   printf("Sample rsing\r\n");
        // else if (rsing_falling_n == 0)
        //   printf("Sample falling\r\n");
        //刷入新显存, 用于ADC测试
        I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
        //I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLOFF, countof(CMD_ALLOFF));
        I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALL5A, countof(CMD_ALL5A));
        flag = 0x10;
        ADC_START = 1;
        Channel3_num = 0;
        Rst_adc = 0;
        //GPIO_Write(GPIOG, 0xFF);
      }
      else if(isCaptureOver && (fre !=0))//输出测试结果,3次
      {
        freq[Channel3_num] = (1000000/(float)fre)/4;
        printf("Frequence: %.02fHz\r\n",freq[Channel3_num]);
        isCaptureOver = 0;
        fre = 0;
        Channel3_num++;
        Delay_MS(200);
      }
    }
    else if (flag == 0x10)//接收ADC数据
    {
      if(ADC_START == 2)
      {
        Delay_MS(500);
        printf("ADC processing.\r\n");
        I2C_SoftWare_ReadOnly(0xA1, &Tx1_Buffer[0],23); //读取U2 ADC采样值
        Delay_MS(500);
        for(i=1;i<11;i++)//处理数据
        {
         ADC_ConvertedValue[i-1] = Tx1_Buffer[2*i-1];//获得高8位
         ADC_ConvertedValue[i-1] = (ADC_ConvertedValue[i-1]<<8) + Tx1_Buffer[2*i];//得到低8位
        }
        printf("ADC Channel 1-10 data ready!\r\n");
        I2C_SoftWare_ReadOnly(0xB1, &Tx1_Buffer[0],23); //读取U3 ADC采样值
        Delay_MS(500);
        for(i=1;i<11;i++)//处理数据
        {
         ADC_ConvertedValue[i-1+10] = Tx1_Buffer[2*i-1];//获得高8位
         ADC_ConvertedValue[i-1+10] = (ADC_ConvertedValue[i-1+10]<<8) + Tx1_Buffer[2*i];//得到低8位
        }
        printf("ADC Channel 11-20 data ready!\r\n");
        I2C_SoftWare_ReadOnly(0xC1, &Tx1_Buffer[0],23); //读取U4 ADC采样值
        Delay_MS(500);
        for(i=1;i<11;i++)//处理数据
        {
         ADC_ConvertedValue[i-1+20] = Tx1_Buffer[2*i-1];//获得高8位
         ADC_ConvertedValue[i-1+20] = (ADC_ConvertedValue[i-1+20]<<8) + Tx1_Buffer[2*i];//得到低8位
        }
        printf("ADC Channel 21-30 data ready!\r\n");
        I2C_SoftWare_ReadOnly(0xD1, &Tx1_Buffer[0],23); //读取U5 ADC采样值
        Delay_MS(500);
        for(i=1;i<11;i++)//处理数据
        {
         ADC_ConvertedValue[i-1+30] = Tx1_Buffer[2*i-1];//获得高8位
         ADC_ConvertedValue[i-1+30] = (ADC_ConvertedValue[i-1+30]<<8) + Tx1_Buffer[2*i];//得到低8位
        }
        printf("ADC Channel 31-40 data ready!\r\n");
        // TEST code start
        // for(i=0;i<21;i++)
        // printf("%02X ",Tx1_Buffer[i]);
        // printf("\r\n");
        // for(i=30;i<40;i++)
        // printf("%04X ",ADC_ConvertedValue[i]);
        // printf("\r\n");
        //TEST code finish
        flag = 0x11;
      }

    }
    else if (flag == 0x11)//ADC数据处理
    {

      for(i=0;i<COM_SEG_NUM;i++)
      {
        ADC_ConvertedValueLocal[i] =(float) ADC_ConvertedValue[i]/4096*3.3;
      }
      //串口打印ADC采样值
      //U2
      printf("U2 ADC DATA, SampleTime: 54us\r\n");
      printf("SEG31 %.02fV    SEG32 %.02fV\r\n",ADC_ConvertedValueLocal[0],ADC_ConvertedValueLocal[1]);
      printf("SEG33 %.02fV    SEG34 %.02fV\r\n",ADC_ConvertedValueLocal[2],ADC_ConvertedValueLocal[3]);
      printf("SEG35 %.02fV    COM0  %.02fV\r\n",ADC_ConvertedValueLocal[4],ADC_ConvertedValueLocal[5]);
      printf("COM1  %.02fV    COM2  %.02fV\r\n",ADC_ConvertedValueLocal[6],ADC_ConvertedValueLocal[7]);
      printf("COM3  %.02fV    SEG0  %.02fV\r\n",ADC_ConvertedValueLocal[8],ADC_ConvertedValueLocal[9]);
      printf("\r\n");
      //U3
      printf("U3 ADC DATA, SampleTime: 54us\r\n");
      printf("SEG1  %.02fV    SEG2  %.02fV\r\n",ADC_ConvertedValueLocal[10],ADC_ConvertedValueLocal[11]);
      printf("SEG3  %.02fV    SEG4  %.02fV\r\n",ADC_ConvertedValueLocal[12],ADC_ConvertedValueLocal[13]);
      printf("SEG5  %.02fV    SEG6  %.02fV\r\n",ADC_ConvertedValueLocal[14],ADC_ConvertedValueLocal[15]);
      printf("SEG30 %.02fV    SEG29 %.02fV\r\n",ADC_ConvertedValueLocal[16],ADC_ConvertedValueLocal[17]);
      printf("SEG28 %.02fV    SEG27 %.02fV\r\n",ADC_ConvertedValueLocal[18],ADC_ConvertedValueLocal[19]);
      printf("\r\n");
      //U4
      printf("U4 ADC DATA, SampleTime: 54us\r\n");
      printf("SEG26 %.02fV    SEG25 %.02fV\r\n",ADC_ConvertedValueLocal[20],ADC_ConvertedValueLocal[21]);
      printf("SEG24 %.02fV    SEG23 %.02fV\r\n",ADC_ConvertedValueLocal[22],ADC_ConvertedValueLocal[23]);
      printf("SEG22 %.02fV    SEG21 %.02fV\r\n",ADC_ConvertedValueLocal[24],ADC_ConvertedValueLocal[25]);
      printf("SEG20 %.02fV    SEG19 %.02fV\r\n",ADC_ConvertedValueLocal[26],ADC_ConvertedValueLocal[27]);
      printf("SEG18 %.02fV    SEG17 %.02fV\r\n",ADC_ConvertedValueLocal[28],ADC_ConvertedValueLocal[29]);
      printf("\r\n");
      //U5
      printf("U5 ADC DATA, SampleTime: 54us\r\n");
      printf("SEG16 %.02fV    SEG15 %.02fV\r\n",ADC_ConvertedValueLocal[30],ADC_ConvertedValueLocal[31]);
      printf("SEG14 %.02fV    SEG13 %.02fV\r\n",ADC_ConvertedValueLocal[32],ADC_ConvertedValueLocal[33]);
      printf("SEG12 %.02fV    SEG11 %.02fV\r\n",ADC_ConvertedValueLocal[34],ADC_ConvertedValueLocal[35]);
      printf("SEG10 %.02fV    SEG9  %.02fV\r\n",ADC_ConvertedValueLocal[36],ADC_ConvertedValueLocal[37]);
      printf("SEG8  %.02fV    SEG7  %.02fV\r\n",ADC_ConvertedValueLocal[38],ADC_ConvertedValueLocal[39]);
      //ADC采集结束
      for(i=0;i<40;i++)
      {
        ADC_ConvertedValueLocal[i] = 0;
        ADC_ConvertedValue[i] = 0;
        Tx1_Buffer[i] = 0;
      }
      GPIOG->ODR = 0xF0; //PG0-PG3置低电平
      ADC_START = 0;
      flag = 0x12;
    }
    else if (flag == 0x12)//等待重新启动ADC
    {
      if(Rst_adc == 1)
      {
         flag = 0x0F;//重新启动ADC
         ADC_START = 1;
      }
      // I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLON, countof(CMD_ALLON));
      // Delay_MS(800);
      // I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLAA, countof(CMD_ALLAA));
      // Delay_MS(800);
      // I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALL55, countof(CMD_ALL55));
      // Delay_MS(800);
    }
  } //Loop finish

}
/* Private functions ---------------------------------------------------------*/
//Software Delay Function




u8 i2c_speed_test(u16 i2c_speed)
{
  u8 diff_flag, Flag;

  printf("\r\nI2C Speed: %dK",i2c_speed);
  diff_flag = i2c_speed_switch(i2c_speed, READ_DATA, CMD_ALLON,countof(CMD_ALLON),\
  Tx1_Buffer,DATA_RAM+1);

  if(diff_flag)
  {
    printf("\r\nTest OK !!\r\n");

    Flag = 0x01;
  }
  else
  {
    printf("\r\n Test Failed!!\r\n");
    Flag = 0x0E;
  }
  return Flag;
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
