#include "config.h"

void Delay(u16 j)
{
	u8 i;
	for(;j>0;j--)
	 for(i=0;i<10;i++);
}

void Delay_MS(u16 time)
{
  u16 i;
  while(time--)
  for(i=900;i>0;i--)
		Delay_US();
}
void Delay_US(void)
{
	nop();
	nop();
	nop();
	nop();
}
void Delay_nUS(u16 time)
{
	u16 temp = time;
	if(time == 10)	temp = 8;
	if(time == 20)	temp = 20;
	if(time == 30)	temp = 32;
	if(time == 40)	temp = 42;
	if(time == 50)	temp = 54;
	while(temp--)
	{
		nop();
		nop();
		nop();
		nop();
	}
}