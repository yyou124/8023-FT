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
u8 Channel3_num;   //��׽����
//STM32
u8 ADC_START;      //stm32 ADC�ɼ�ʹ��
/* Private function prototypes -----------------------------------------------*/

u8 i2c_speed_test(u16 i2c_speed);
/* Main function  ------------------------------------------------------------*/
void main(void)
{
  u8 flag,i,j,stm32_sel,case_state;
  u8 sample_num;//�ظ���������
  u8 test_case;//������case
  u8 iic_speed[2];
  float freq[3];
  //Initialize MCU mode
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  Uart_Driver_Init();
  Delay_MS(200);
  I2C_SoftWare_Init();  //Software IIC
  Key_init();
  Delay_MS(200);

  //STM32 U2-U5 ʹ���ź�PG0~PG3 LED�ź� PG6
  GPIO_Init(GPIOG, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOG, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_SLOW);
  enableInterrupts();

  //WI-FIģ���ʼ��
  //UART3_ITConfig(UART3_IT_RXNE_OR, DISABLE);//��ʼ�����̹رս����жϣ�����Ӱ��
  //Init_ESP8266_tuochuang();//��ʼ��Ϊ͸��ģʽ
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
    if (flag == 0x01)//I2C�ٶȲ��� 400K
    {
      printf("I2C speed test start.\r\n");
      //ֱ�Ӵ�400K��ʼ����
      iic_speed[0] = i2c_speed_switch(400, READ_DATA, CMD_ALLON,countof(CMD_ALLON),Tx1_Buffer,DATA_RAM+1);

      if(iic_speed[0])//I2C ����ͨ��
      {
        printf("pass\r\n");
        //ˢ�����Դ� ����SEGƵ�ʲ���
         signal_capture_cc2();  //TIM1 CC2��׽ʹ��
        I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
        I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLON, countof(CMD_ALLON));
        printf("Frequence test start.\r\n");
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
      if(Channel3_num == 3) //����ADC�ɼ�,����enable�ź�
      {
        //��Ƶ��ƽ��ֵ����һ����Ҫ
        freq[0] = freq[1];
        freq[0] += freq[2];
        freq[0] = freq[0]/2;
        printf("Average LCD Frequence: %0.2fHz\r\n",freq[0]);
        if(freq[0]<FREQ_MAX && freq[0]>FREQ_MIN)//Ƶ�ʲ���ͨ��
        {
          printf("pass\r\n");
          //ˢ�����Դ�, ����case 1 2 3
          signal_capture_cc3();  //TIM1 CC3��׽ʹ��
          TIM1_ITConfig(TIM1_IT_CC2, DISABLE);
          TIM1_CCxCmd(TIM1_CHANNEL_2, DISABLE);
          I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
          I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALL5A, countof(CMD_ALL5A));
          printf("ADC test start\r\n");
          flag = 0x03;
          test_case = CASE_1;
          ADC_START = 1;
        }
        else //����δͨ��
        {
          printf("NOT pass\r\n");
          test_case = CASE_0;
          flag = 0x0F;
        }
      }
      else if(isCaptureOver && (fre !=0))//���Խ������,3��
      {
        freq[Channel3_num] = (1000000/(float)fre)/4;
        printf("Frequence: %.02fHz\r\n",freq[Channel3_num]);
        isCaptureOver = 0;
        fre = 0;
        Channel3_num++;
        Delay_MS(50);
      }
    }
    else if (flag == 0x03)//�ȴ�ADC���ý��� ����ADC����
    {
      if(ADC_START == 2)
      {
        Delay_MS(500);
        for(j=0;j<4;j++)
        {
          I2C_SoftWare_ReadOnly(0xA1+stm32_sel, &Tx1_Buffer[0],23);
          Delay_MS(200);
          for(i=1;i<11;i++)//��������
          {
            ADC_ConvertedValue[i-1+stm32_sel] = Tx1_Buffer[2*i-1];//��ø�8λ
            ADC_ConvertedValue[i-1+stm32_sel] = (ADC_ConvertedValue[i-1]<<8) + Tx1_Buffer[2*i];//�õ���8λ
          }
          stm32_sel += 10;
        }
        flag = 0x04;
         GPIOG->ODR = 0xF0; //PG0-PG3�õ͵�ƽ
         stm32_sel = 0;
      }
    }
    else if (flag == 0x04)//ADC���ݴ���
    {
      //�Ȳ���case 1 2 3
      case_state = case_check(&ADC_ConvertedValue[0],test_case,0x5A);
      for(i=0;i<COM_SEG_NUM;i++)
      {
        ADC_ConvertedValue[i] = 0;
      }
      switch(case_state)
      {
        case 0://�ܽŴ���
        {
          flag = 0x0f;
          break;
        }
        case 1://������ȷ
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
            //COM0�����ز���
            printf("test case 4");
           signal_capture_cc1();  //TIM1 CC1��׽ʹ��
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
            //COM2�½��ز���
            printf("test case 7");
            signal_capture_cc3();  //TIM1 CC3��׽ʹ��
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
        case 2://�������� ���²���
        {
          case_state = 0;
          Delay_MS(sample_num*3);
          flag = 0x03;
          sample_num ++;
          if(sample_num >50)
            flag = 0x0f;
          GPIOG->ODR = 0xF0; //PG0-PG3�õ͵�ƽ
          ADC_START = 1;
          break;
        }
        case 3://�������ݴ���
        {
          flag = 0x0f;
          break;
        }
      }
    }
    else if (flag == 0x0F)//������
    {
      if(test_case == CASE_8)            //����ͨ��
        printf("LCD test pass!\r\n");
      else if(iic_speed[0]== 0)//I2C����δ��
        printf("LCD test I2C NOT pass!\r\n");
      else if(test_case == CASE_0)       //Ƶ�ʲ���δ��
        printf("LCD test FREQ NOT pass!\r\n");
      else if((case_state == 0)||(sample_num>15))         //�ܽŵ�ѹ����δ��
        printf("LCD test PINS NOT pass!\r\n");
      else if(case_state == 3)           //������������
        printf("Input case data ERROR\r\n");
      flag = 0x10;
      TIM1_Cmd(DISABLE);
    }
    else if(flag == 0x10)//�ȴ���������
    {
      if(Rst_adc == 1)//��������
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
