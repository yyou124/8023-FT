

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
  //设置预分配器，并产生一个强制更新事件
  TIM2_PrescalerConfig(0x0004, TIM2_PSCRELOADMODE_UPDATE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  //TIM2_Cmd(ENABLE);
}

void signal_capture_cc3(void)
{
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);   //输入浮动，没有外部中断

    TIM1_DeInit();                                         //tim1 定时器寄存器全部复位
    TIM1_TimeBaseInit(0x00, TIM1_COUNTERMODE_UP, 40000, 0);//  10ms  100hz      设置时间分频  向上计数    40000代表自动重装载计数值 0~40001溢出
    //设置预分配器，并产生一个强制更新事件
    TIM1_PrescalerConfig(0x000f, TIM1_PSCRELOADMODE_IMMEDIATE);
    TIM1_ICInit(TIM1_CHANNEL_3, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV1, 0x00);   //设置通道 设置输入捕获上升触发  捕获输入引脚  输入捕捉预定标器    滤波
    TIM1_ClearFlag(TIM1_FLAG_CC3);                         //指定要清除的标志
    TIM1_ITConfig( TIM1_IT_CC3 , ENABLE);                  //去掉是查询方式，否则是中断方式
    TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE);                //使能向上计数 功能
    TIM1_CCxCmd(TIM1_CHANNEL_3, ENABLE);                   //使能输入捕获通道3
    TIM1_Cmd(ENABLE);                                      //使能定时器
}


void signal_capture_cc2(void)
{
    GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);   //输入浮动，没有外部中断

    TIM1_DeInit();                                         //tim1 定时器寄存器全部复位
    TIM1_TimeBaseInit(0x00, TIM1_COUNTERMODE_UP, 40000, 0);//  10ms  100hz      设置时间分频  向上计数    40000代表自动重装载计数值 0~40001溢出
    //设置预分配器，并产生一个强制更新事件
    TIM1_PrescalerConfig(0x000f, TIM1_PSCRELOADMODE_IMMEDIATE);
    TIM1_ICInit(TIM1_CHANNEL_2, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV1, 0x00);   //设置通道 设置输入捕获上升触发  捕获输入引脚  输入捕捉预定标器    滤波
    TIM1_ClearFlag(TIM1_FLAG_CC2);                         //指定要清除的标志
    TIM1_ITConfig( TIM1_IT_CC2 , ENABLE);                  //去掉是查询方式，否则是中断方式
    TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE);                //使能向上计数 功能
    TIM1_CCxCmd(TIM1_CHANNEL_2, ENABLE);                   //使能输入捕获通道2
    TIM1_Cmd(ENABLE);                                      //使能定时器
}
void signal_capture_cc1(void)
{
    GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);   //输入浮动，没有外部中断

    TIM1_DeInit();                                         //tim1 定时器寄存器全部复位
    TIM1_TimeBaseInit(0x0f, TIM1_COUNTERMODE_UP, 10000, 0);//  10ms  100hz      设置时间分频  向上计数    20000代表自动重装载计数值 0~20001溢出
    //设置预分配器，并产生一个强制更新事件
    TIM1_PrescalerConfig(0x000f, TIM1_PSCRELOADMODE_IMMEDIATE);
    TIM1_ICInit(TIM1_CHANNEL_1, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV1, 0x00);   //设置通道 设置输入捕获上升触发  捕获输入引脚  输入捕捉预定标器    滤波
    TIM1_ClearFlag(TIM1_FLAG_CC1);                         //指定要清除的标志
    TIM1_ITConfig(TIM1_IT_CC1, ENABLE);                  //去掉是查询方式，否则是中断方式
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);                //使能向上计数 功能
    TIM1_CCxCmd(TIM1_CHANNEL_1, ENABLE);                   //使能输入捕获通道3
    TIM1_Cmd(ENABLE);                                      //使能定时器
}