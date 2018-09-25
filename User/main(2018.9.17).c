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
u8 Channel3_num;   //捕捉次数
//STM32
u8 ADC_START;      //stm32 ADC采集使能
/* Private function prototypes -----------------------------------------------*/

u8 i2c_speed_test(u16 i2c_speed);
/* Main function  ------------------------------------------------------------*/
void main(void)
{
  u8 flag,i,j,stm32_sel,case_state;
  u8 sample_num;//重复采样次数
  u8 test_case;//待测试case
  u8 iic_speed[2];
  float freq[3];
  //Initialize MCU mode
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  Uart_Driver_Init();
  Delay_MS(200);
  I2C_SoftWare_Init();  //Software IIC
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
  ADC_START = 0;
  Rst_adc = 0;
  stm32_sel = 0;
  sample_num = 0;
  //Initialize LCD
  printf("LCD Test Start.\r\n");
  I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
  while (1)
  {
    if (flag == 0x01)//I2C速度测试 400K
    {
      printf("I2C speed test start.\r\n");
      //直接从400K开始测试
      iic_speed[0] = i2c_speed_switch(400, READ_DATA, CMD_ALLON,countof(CMD_ALLON),Tx1_Buffer,DATA_RAM+1);

      if(iic_speed[0])//I2C 测试通过
      {
        printf("pass\r\n");
        //刷入新显存 用于SEG频率测试
         signal_capture_cc2();  //TIM1 CC2捕捉使能
        I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
        I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLON, countof(CMD_ALLON));
        printf("Frequence test start.\r\n");
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
      if(Channel3_num == 3) //进行ADC采集,发送enable信号
      {
        //求频率平均值，第一个不要
        freq[0] = freq[1];
        freq[0] += freq[2];
        freq[0] = freq[0]/2;
        printf("Average LCD Frequence: %0.2fHz\r\n",freq[0]);
        if(freq[0]<FREQ_MAX && freq[0]>FREQ_MIN)//频率测试通过
        {
          printf("pass\r\n");
          //刷入新显存, 测试case 1 2 3
          signal_capture_cc3();  //TIM1 CC3捕捉使能
          TIM1_ITConfig(TIM1_IT_CC2, DISABLE);
          TIM1_CCxCmd(TIM1_CHANNEL_2, DISABLE);
          I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
          I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALL5A, countof(CMD_ALL5A));
          printf("ADC test start\r\n");
          flag = 0x03;
          test_case = CASE_1;
          ADC_START = 1;
        }
        else //测试未通过
        {
          printf("NOT pass\r\n");
          test_case = CASE_0;
          flag = 0x0F;
        }
      }
      else if(isCaptureOver && (fre !=0))//测试结果处理,3次
      {
        freq[Channel3_num] = (1000000/(float)fre)/4;
        printf("Frequence: %.02fHz\r\n",freq[Channel3_num]);
        isCaptureOver = 0;
        fre = 0;
        Channel3_num++;
        Delay_MS(50);
      }
    }
    else if (flag == 0x03)//等待ADC采用结束 接收ADC数据
    {
      if(ADC_START == 2)
      {
        Delay_MS(500);
        for(j=0;j<4;j++)
        {
          I2C_SoftWare_ReadOnly(0xA1+stm32_sel, &Tx1_Buffer[0],23);
          Delay_MS(200);
          for(i=1;i<11;i++)//处理数据
          {
            ADC_ConvertedValue[i-1+stm32_sel] = Tx1_Buffer[2*i-1];//获得高8位
            ADC_ConvertedValue[i-1+stm32_sel] = (ADC_ConvertedValue[i-1]<<8) + Tx1_Buffer[2*i];//得到低8位
          }
          stm32_sel += 10;
        }
        flag = 0x04;
         GPIOG->ODR = 0xF0; //PG0-PG3置低电平
         stm32_sel = 0;
      }
    }
    else if (flag == 0x04)//ADC数据处理
    {
      //先测试case 1 2 3
      case_state = case_check(&ADC_ConvertedValue[0],test_case,0x5A);
      for(i=0;i<COM_SEG_NUM;i++)
      {
        ADC_ConvertedValue[i] = 0;
      }
      switch(case_state)
      {
        case 0://管脚错误
        {
          flag = 0x0f;
          break;
        }
        case 1://采样正确
        {
          flag = 0x03;
          if(test_case & CASE_1)
          {
            printf("test case 2");
            test_case = CASE_2;
          }
          else if(test_case & CASE_2)
          {
            printf("test case 3");
            test_case = CASE_3;
          }
          else if(test_case & CASE_3)
          {
            //COM0上升沿采样
            printf("test case 4");
           signal_capture_cc1();  //TIM1 CC1捕捉使能
            TIM1_ITConfig(TIM1_IT_CC3, DISABLE);
            TIM1_CCxCmd(TIM1_CHANNEL_3, DISABLE);
            test_case = CASE_4;
          }
          else if(test_case & CASE_4)
          {
            printf("test case 5");
            test_case = CASE_5;
          }
          else if(test_case & CASE_5)
          {
            printf("test case 6");
            test_case = CASE_6;
          }
          else if(test_case & CASE_6)
          {
            //COM2下降沿采样
            printf("test case 7");
            signal_capture_cc3();  //TIM1 CC3捕捉使能
            TIM1_ITConfig(TIM1_IT_CC1, DISABLE);
            TIM1_CCxCmd(TIM1_CHANNEL_1, DISABLE);
            TIM1_ICInit(TIM1_CHANNEL_3, TIM1_ICPOLARITY_FALLING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV1, 0x00);
            test_case = CASE_7;
          }
          else if(test_case & CASE_7)
          {
            printf("test case 8\r\n");
            test_case = CASE_8;
          }
          else if(test_case & CASE_8) flag = 0x0f;
          case_state = 0;
          sample_num = 0;

          ADC_START = 1;
          break;
        }
        case 2://采样错误 重新采样
        {
          case_state = 0;
          Delay_MS(sample_num*3);
          flag = 0x03;
          sample_num ++;
          if(sample_num >50)
            flag = 0x0f;
          GPIOG->ODR = 0xF0; //PG0-PG3置低电平
          ADC_START = 1;
          break;
        }
        case 3://输入数据错误
        {
          flag = 0x0f;
          break;
        }
      }
    }
    else if (flag == 0x0F)//结果输出
    {
      if(test_case == CASE_8)            //测试通过
        printf("LCD test pass!\r\n");
      else if(iic_speed[0]== 0)//I2C测试未过
        printf("LCD test I2C NOT pass!\r\n");
      else if(test_case == CASE_0)       //频率测试未过
        printf("LCD test FREQ NOT pass!\r\n");
      else if((case_state == 0)||(sample_num>15))         //管脚电压测试未过
        printf("LCD test PINS NOT pass!\r\n");
      else if(case_state == 3)           //输入数据有误
        printf("Input case data ERROR\r\n");
      flag = 0x10;
      TIM1_Cmd(DISABLE);
    }
    else if(flag == 0x10)//等待重新启动
    {
      if(Rst_adc == 1)//重新启动
      {
        iic_speed[0] = 0;
        case_state = 0;
        Channel3_num = 0;
        flag = 0x01;
        ADC_START = 0;
        Rst_adc = 0;
        stm32_sel = 0;
        sample_num = 0;
        printf("------------------------------------------------------------\r\n");
        printf("LCD Test Restart.\r\n");
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
