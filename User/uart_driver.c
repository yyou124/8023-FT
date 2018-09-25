/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* ********************************************
UART2  configured as follow:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Receive and transmit enabled
 -  Receive interrupt
  - UART2 Clock disabled
*********************************************/
void Uart_Driver_Init(void)
{
    UART2_DeInit();
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2, ENABLE);
    UART2_Init((u32)115200, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, \
    UART2_PARITY_NO,  UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
    UART2_ITConfig(UART2_IT_RXNE_OR,ENABLE); //UART2 RX Regist No Empty Interrup
    UART2_Cmd(ENABLE);  //Enable UART2
}

void UART2_SendByte(u8 data)
{
    FlagStatus fs;
    UART2_SendData8((u8)data);
    do{
        fs = UART2_GetFlagStatus(UART2_FLAG_TXE);
    }while(fs == RESET );
}

void UART2_SendString(u8* Data,u16 len)
{
  u16 i=0;
  for(;i<len;i++)
    UART2_SendByte(Data[i]);
}

u8 UART2_ReceiveByte(void)
{
  u8 USART3_RX_BUF;
  //Read Data Register Not Empty flag
  while (UART2_GetFlagStatus(UART2_FLAG_RXNE) == RESET);
  USART3_RX_BUF=UART2_ReceiveData8();
  return  USART3_RX_BUF;
}

int fputc(int ch, FILE *f)
{
  /* Send Printf content to uart */
  UART2_SendByte(ch);
  return (ch);
}
/******************* (C) COPYRIGHT ChipNorth *****END OF FILE****/