

#include "config.h"
//16MHz * 1/16 = 1MHz
//10000us = 10ms = 0x2710
void Timer1_Init(void)//10ms
{
  TIM1_TimeBaseInit(0x000f, TIM1_COUNTERMODE_UP, 0x2710, 0x00);
  TIM1_SetCounter(0x2710);
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  TIM1_Cmd(ENABLE);
}

void Timer2_Init(u16 count)
{
  TIM2_TimeBaseInit(0x0000, count);
  //����Ԥ��������������һ��ǿ�Ƹ����¼�
  TIM2_PrescalerConfig(0x0004, TIM2_PSCRELOADMODE_UPDATE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  //TIM2_Cmd(ENABLE);
}

void signal_capture_cc3(void)
{
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);   //���븡����û���ⲿ�ж�

    TIM1_DeInit();                                         //tim1 ��ʱ���Ĵ���ȫ����λ
    TIM1_TimeBaseInit(0x00, TIM1_COUNTERMODE_UP, 40000, 0);//  10ms  100hz      ����ʱ���Ƶ  ���ϼ���    40000�����Զ���װ�ؼ���ֵ 0~40001���
    //����Ԥ��������������һ��ǿ�Ƹ����¼�
    TIM1_PrescalerConfig(0x000f, TIM1_PSCRELOADMODE_IMMEDIATE);
    TIM1_ICInit(TIM1_CHANNEL_3, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV1, 0x00);   //����ͨ�� �������벶����������  ������������  ���벶׽Ԥ������    �˲�
    TIM1_ClearFlag(TIM1_FLAG_CC3);                         //ָ��Ҫ����ı�־
    TIM1_ITConfig( TIM1_IT_CC3 , ENABLE);                  //ȥ���ǲ�ѯ��ʽ���������жϷ�ʽ
    TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE);                //ʹ�����ϼ��� ����
    TIM1_CCxCmd(TIM1_CHANNEL_3, ENABLE);                   //ʹ�����벶��ͨ��3
    TIM1_Cmd(ENABLE);                                      //ʹ�ܶ�ʱ��
}


void signal_capture_cc2(void)
{
    GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);   //���븡����û���ⲿ�ж�

    TIM1_DeInit();                                         //tim1 ��ʱ���Ĵ���ȫ����λ
    TIM1_TimeBaseInit(0x00, TIM1_COUNTERMODE_UP, 40000, 0);//  10ms  100hz      ����ʱ���Ƶ  ���ϼ���    40000�����Զ���װ�ؼ���ֵ 0~40001���
    //����Ԥ��������������һ��ǿ�Ƹ����¼�
    TIM1_PrescalerConfig(0x000f, TIM1_PSCRELOADMODE_IMMEDIATE);
    TIM1_ICInit(TIM1_CHANNEL_2, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV1, 0x00);   //����ͨ�� �������벶����������  ������������  ���벶׽Ԥ������    �˲�
    TIM1_ClearFlag(TIM1_FLAG_CC2);                         //ָ��Ҫ����ı�־
    TIM1_ITConfig( TIM1_IT_CC2 , ENABLE);                  //ȥ���ǲ�ѯ��ʽ���������жϷ�ʽ
    TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE);                //ʹ�����ϼ��� ����
    TIM1_CCxCmd(TIM1_CHANNEL_2, ENABLE);                   //ʹ�����벶��ͨ��2
    TIM1_Cmd(ENABLE);                                      //ʹ�ܶ�ʱ��
}
void signal_capture_cc1(void)
{
    GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);   //���븡����û���ⲿ�ж�

    TIM1_DeInit();                                         //tim1 ��ʱ���Ĵ���ȫ����λ
    TIM1_TimeBaseInit(0x0f, TIM1_COUNTERMODE_UP, 10000, 0);//  10ms  100hz      ����ʱ���Ƶ  ���ϼ���    20000�����Զ���װ�ؼ���ֵ 0~20001���
    //����Ԥ��������������һ��ǿ�Ƹ����¼�
    TIM1_PrescalerConfig(0x000f, TIM1_PSCRELOADMODE_IMMEDIATE);
    TIM1_ICInit(TIM1_CHANNEL_1, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV1, 0x00);   //����ͨ�� �������벶����������  ������������  ���벶׽Ԥ������    �˲�
    TIM1_ClearFlag(TIM1_FLAG_CC1);                         //ָ��Ҫ����ı�־
    TIM1_ITConfig(TIM1_IT_CC1, ENABLE);                  //ȥ���ǲ�ѯ��ʽ���������жϷ�ʽ
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);                //ʹ�����ϼ��� ����
    TIM1_CCxCmd(TIM1_CHANNEL_1, ENABLE);                   //ʹ�����벶��ͨ��3
    TIM1_Cmd(ENABLE);                                      //ʹ�ܶ�ʱ��
}