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
float ADC_ConvertedValueLocal[COM_SEG_NUM];
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
  u8 flag,i;
  u16 speeded;
  float freq[10];
  //Initialize MCU mode
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  Uart_Driver_Init();
  Delay_MS(200);
  //I2C_Driver_Init(); //Hardware IIC
  I2C_SoftWare_Init();  //Software IIC
  signal_capture_Init();  //��׽ʹ��
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
  speeded = 300;//ֱ�Ӵ�400K��ʼ����
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
    if (flag == 0x01)//I2C�ٶȲ��� 100K-500K
    {
      //ֱ�Ӵ�400K��ʼ����
      speeded += 100;
      flag = i2c_speed_test(speeded);

    }
    else if(flag == 0x0E)//������Խ��
    {
      printf("The fastest I2C speed of this chip is %dK.\r\n", speeded-100);
      //ˢ�����Դ�
      I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
      I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLON, countof(CMD_ALLON));
      flag = 0x0F;
    }
    else if(flag == 0x0F)//SEGƵ�ʲ���
    {
      if((Channel3_num == 3) || (Rst_adc == 1))//����ADC�ɼ�,����enable�ź�
      {
        if(Channel3_num>0)//��Ƶ��ƽ��ֵ����һ����Ҫ
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
        //ˢ�����Դ�, ����ADC����
        I2C_SoftWare_Write(BU_WRITE_ADDRESS,CMD_INIT,countof(CMD_INIT));
        //I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALLOFF, countof(CMD_ALLOFF));
        I2C_SoftWare_Write(BU_WRITE_ADDRESS, CMD_ALL5A, countof(CMD_ALL5A));
        flag = 0x10;
        ADC_START = 1;
        Channel3_num = 0;
        Rst_adc = 0;
        //GPIO_Write(GPIOG, 0xFF);
      }
      else if(isCaptureOver && (fre !=0))//������Խ��,3��
      {
        freq[Channel3_num] = (1000000/(float)fre)/4;
        printf("Frequence: %.02fHz\r\n",freq[Channel3_num]);
        isCaptureOver = 0;
        fre = 0;
        Channel3_num++;
        Delay_MS(200);
      }
    }
    else if (flag == 0x10)//����ADC����
    {
      if(ADC_START == 2)
      {
        Delay_MS(500);
        printf("ADC processing.\r\n");
        I2C_SoftWare_ReadOnly(0xA1, &Tx1_Buffer[0],23); //��ȡU2 ADC����ֵ
        Delay_MS(500);
        for(i=1;i<11;i++)//��������
        {
         ADC_ConvertedValue[i-1] = Tx1_Buffer[2*i-1];//��ø�8λ
         ADC_ConvertedValue[i-1] = (ADC_ConvertedValue[i-1]<<8) + Tx1_Buffer[2*i];//�õ���8λ
        }
        printf("ADC Channel 1-10 data ready!\r\n");
        I2C_SoftWare_ReadOnly(0xB1, &Tx1_Buffer[0],23); //��ȡU3 ADC����ֵ
        Delay_MS(500);
        for(i=1;i<11;i++)//��������
        {
         ADC_ConvertedValue[i-1+10] = Tx1_Buffer[2*i-1];//��ø�8λ
         ADC_ConvertedValue[i-1+10] = (ADC_ConvertedValue[i-1+10]<<8) + Tx1_Buffer[2*i];//�õ���8λ
        }
        printf("ADC Channel 11-20 data ready!\r\n");
        I2C_SoftWare_ReadOnly(0xC1, &Tx1_Buffer[0],23); //��ȡU4 ADC����ֵ
        Delay_MS(500);
        for(i=1;i<11;i++)//��������
        {
         ADC_ConvertedValue[i-1+20] = Tx1_Buffer[2*i-1];//��ø�8λ
         ADC_ConvertedValue[i-1+20] = (ADC_ConvertedValue[i-1+20]<<8) + Tx1_Buffer[2*i];//�õ���8λ
        }
        printf("ADC Channel 21-30 data ready!\r\n");
        I2C_SoftWare_ReadOnly(0xD1, &Tx1_Buffer[0],23); //��ȡU5 ADC����ֵ
        Delay_MS(500);
        for(i=1;i<11;i++)//��������
        {
         ADC_ConvertedValue[i-1+30] = Tx1_Buffer[2*i-1];//��ø�8λ
         ADC_ConvertedValue[i-1+30] = (ADC_ConvertedValue[i-1+30]<<8) + Tx1_Buffer[2*i];//�õ���8λ
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
    else if (flag == 0x11)//ADC���ݴ���
    {

      for(i=0;i<COM_SEG_NUM;i++)
      {
        ADC_ConvertedValueLocal[i] =(float) ADC_ConvertedValue[i]/4096*3.3;
      }
      //���ڴ�ӡADC����ֵ
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
      //ADC�ɼ�����
      for(i=0;i<40;i++)
      {
        ADC_ConvertedValueLocal[i] = 0;
        ADC_ConvertedValue[i] = 0;
        Tx1_Buffer[i] = 0;
      }
      GPIOG->ODR = 0xF0; //PG0-PG3�õ͵�ƽ
      ADC_START = 0;
      flag = 0x12;
    }
    else if (flag == 0x12)//�ȴ���������ADC
    {
      if(Rst_adc == 1)
      {
         flag = 0x0F;//��������ADC
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
