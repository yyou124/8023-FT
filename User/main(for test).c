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
//Timer1 -> Channel3 相关变量
u8 isCaptureOver;  //捕捉结束标志
u16 fre;           //频率
u8 tim2_flag, tim2_count;
u8 Channel3_num;   //捕捉次数
//STM32
u8 ADC_START;      //stm32 ADC采集使能
/* Private function prototypes -----------------------------------------------*/

u8 i2c_speed_test(u16 i2c_speed);
/* Main function  ------------------------------------------------------------*/
void main(void)
{
  u8 flag;
  u8 iic_speed;
  u16 freq[5];
  //Initialize MCU mode
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  Uart_Driver_Init();
  Delay_MS(200);
  I2C_SoftWare_Init();  //Software IIC
  //Key_init();
  Delay_MS(200);
  //STM32 U2-U5 使能信号PG0~PG3 LED信号 PG6
  GPIO_Init(GPIOG, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_SLOW);

  GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST);
  enableInterrupts();//总中断使能
  Channel3_num = 0;
  flag = 0x01;
  ADC_START = 0;
  Rst_adc = 0;
  tim2_count = 0;

  //Initialize LCD
  printf("LCD Test Start.\r\n");
  I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
  while (1)
  {
    if (flag == 0x01)//I2C速度测试 400K
    {
      printf("I2C speed test start.\r\n");
      GPIOC->ODR |= 0x80; //PC7 --> Hihg  .... test
      iic_speed = i2c_speed_switch(400, READ_DATA, CMD_ALLON,countof(CMD_ALLON),Tx1_Buffer,DATA_RAM+1);
      if(iic_speed)//I2C 测试通过
      {
        printf("pass\r\n");
        signal_capture_cc2();  //使能TIM1 CC2捕捉 ....频率测试
        flag = 0x02;
      }
      else          //测试未通过
      {
        printf("NOT pass\r\n");
        flag = 0x0F;
      }
    }
    else if(flag == 0x02)//SEG频率测试
    {
      if(Channel3_num == 1) //进行ADC采集,发送enable信号
      {
        if(freq[0]<FREQ_MAX_US && freq[0]>FREQ_MIN_US)//频率测试通过
        {
          printf("Frequence: %uus\r\n",freq[0]);
          Timer2_Init(freq[0]>>1);//配置定时器2，用于输出ADC使能脉冲
          printf("pass\r\n");
          //刷入新显存,
          signal_capture_cc3();  //TIM1 CC3捕捉使能
          TIM1_ITConfig(TIM1_IT_CC2, DISABLE);
          TIM1_CCxCmd(TIM1_CHANNEL_2, DISABLE);
          I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
          I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALL5A, countof(CMD_ALL5A));
          printf("ADC test start\r\n");
          flag = 0x03;
          ADC_START = 1;
        }
        else //测试未通过
        {
          printf("NOT pass\r\n");
          flag = 0x0F;
        }
      }
      else if(isCaptureOver && (fre !=0))//测试结果处理,1次
      {
        freq[Channel3_num] = fre>>1;
        isCaptureOver = 0;
        fre = 0;
        Channel3_num++;
      }
    }
    else if(flag == 0x03)//line模式测试
    {
      if(tim2_flag)
        GPIOB->ODR |= 0x01; //PB0 --> High
      else
        GPIOB->ODR &= ~0x01; //PB0 --> Low
      if(ADC_START == 2)//ADC数据回传，比对
      {
        tim2_count = 0;
        ADC_START = 3;
        flag = 0x04;
      }
    }
    else if(flag == 0x04)//Frame模式测试
    {
      //切换为Frame模式
      I2C_SoftWare_Write(BU_WRITE_ADDRESS,LINE_TO_FRAME,countof(LINE_TO_FRAME));
      Timer2_Init(freq[0]);//配置定时器2，用于输出ADC使能脉冲
      if(tim2_flag)
        GPIOB->ODR |= 0x01; //PB0 --> High
      else
        GPIOB->ODR &= ~0x01; //PB0 --> Low
      if(ADC_START == 4)//ADC数据回传，比对
      {
        tim2_count = 0;
        GPIOC->ODR &= ~0x80; //PC7 --> Low ... test
        flag = 0x05;
      }
    }

  } //Loop finish

}
/* Private functions ---------------------------------------------------------*/


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
