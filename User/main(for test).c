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
CASE 1 2 3 ��COM2�����ش���
CASE 4 5 6 ��COM0�����ش���
CASE 7 9   ��COM2�½��ش���

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
//WI-FI��ر���
u8 Ready_TuoChuang=0;
u8 Ready_Ap=0;
u16 Tim1_num=0;//��TIM1��ÿ���жϼ�1
//ADC���ݻ���
u8 Rst_adc; //��������ADC�ɼ�
u8 rsing_falling_n; //ADC������/�½��ز����л�
u16 ADC_ConvertedValue[COM_SEG_NUM];
//Timer1 -> Channel3 ��ر���
u8 isCaptureOver;  //��׽������־
u16 fre;           //Ƶ��
u8 tim2_flag, tim2_count;
u8 Channel3_num;   //��׽����
//STM32
u8 ADC_START;      //stm32 ADC�ɼ�ʹ��
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
  //STM32 U2-U5 ʹ���ź�PG0~PG3 LED�ź� PG6
  GPIO_Init(GPIOG, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_SLOW);

  GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST);
  enableInterrupts();//���ж�ʹ��
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
    if (flag == 0x01)//I2C�ٶȲ��� 400K
    {
      printf("I2C speed test start.\r\n");
      GPIOC->ODR |= 0x80; //PC7 --> Hihg  .... test
      iic_speed = i2c_speed_switch(400, READ_DATA, CMD_ALLON,countof(CMD_ALLON),Tx1_Buffer,DATA_RAM+1);
      if(iic_speed)//I2C ����ͨ��
      {
        printf("pass\r\n");
        signal_capture_cc2();  //ʹ��TIM1 CC2��׽ ....Ƶ�ʲ���
        flag = 0x02;
      }
      else          //����δͨ��
      {
        printf("NOT pass\r\n");
        flag = 0x0F;
      }
    }
    else if(flag == 0x02)//SEGƵ�ʲ���
    {
      if(Channel3_num == 1) //����ADC�ɼ�,����enable�ź�
      {
        if(freq[0]<FREQ_MAX_US && freq[0]>FREQ_MIN_US)//Ƶ�ʲ���ͨ��
        {
          printf("Frequence: %uus\r\n",freq[0]);
          Timer2_Init(freq[0]>>1);//���ö�ʱ��2���������ADCʹ������
          printf("pass\r\n");
          //ˢ�����Դ�,
          signal_capture_cc3();  //TIM1 CC3��׽ʹ��
          TIM1_ITConfig(TIM1_IT_CC2, DISABLE);
          TIM1_CCxCmd(TIM1_CHANNEL_2, DISABLE);
          I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
          I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALL5A, countof(CMD_ALL5A));
          printf("ADC test start\r\n");
          flag = 0x03;
          ADC_START = 1;
        }
        else //����δͨ��
        {
          printf("NOT pass\r\n");
          flag = 0x0F;
        }
      }
      else if(isCaptureOver && (fre !=0))//���Խ������,1��
      {
        freq[Channel3_num] = fre>>1;
        isCaptureOver = 0;
        fre = 0;
        Channel3_num++;
      }
    }
    else if(flag == 0x03)//lineģʽ����
    {
      if(tim2_flag)
        GPIOB->ODR |= 0x01; //PB0 --> High
      else
        GPIOB->ODR &= ~0x01; //PB0 --> Low
      if(ADC_START == 2)//ADC���ݻش����ȶ�
      {
        tim2_count = 0;
        ADC_START = 3;
        flag = 0x04;
      }
    }
    else if(flag == 0x04)//Frameģʽ����
    {
      //�л�ΪFrameģʽ
      I2C_SoftWare_Write(BU_WRITE_ADDRESS,LINE_TO_FRAME,countof(LINE_TO_FRAME));
      Timer2_Init(freq[0]);//���ö�ʱ��2���������ADCʹ������
      if(tim2_flag)
        GPIOB->ODR |= 0x01; //PB0 --> High
      else
        GPIOB->ODR &= ~0x01; //PB0 --> Low
      if(ADC_START == 4)//ADC���ݻش����ȶ�
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
