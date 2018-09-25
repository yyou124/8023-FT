/************************ (C) COPYRIGHT  ChipNorth ****************************/
/*
 *  Author  : youdeqiang
 *  Date    : 2017-07-11
 *  Library : 2.1.0
 *  MCU     : STM8S207R8T6
 */
/* Includes ------------------------------------------------------------------*/
#include "config.h"
void decToBin(int num, char *buffer)
{

    if(num>0)
    {
        decToBin(num/2,buffer+1);
        *buffer = (char)(num%2+48);
    }
}


void reOrder(char *buffer)
{
    int i;
    char temp[8]={0};
    for(i=0;i<4;i++)
    {
        temp[7-i]=*(buffer+i);
        *(buffer+i)=*(buffer+7-i);
        *(buffer+7-i)=temp[7-i];

    }

}

void supplement_zero(char *buffer)
{
    int i;
    int len = (int)strlen(buffer);
    //add '0' to buffer's blank data
    for(i=len;i<8;i++)
    {
        *(buffer+i) = '0';
    }
}

void printf_binary_byte(int num)
{
    //int i;
    char buffer[8]={0};
    decToBin(num,buffer);
    supplement_zero(buffer);
    reOrder(buffer);
    printf("%.8s",buffer);
}