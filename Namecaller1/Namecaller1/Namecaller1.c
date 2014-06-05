#include <avr/io.h>
#include "timer.h"
#include "scheduler.h"
#include "io.c"
#include "usart.h"

void LCD_build( unsigned char location, unsigned char *ptr)
{
	unsigned char i;
	if(location < 8){
		LCD_WriteCommand(0x40+(location*8));
		for(i=0;i<8;i++)
		LCD_WriteData(ptr[i]);
	}
}

char ship[] = {0x4, 0x1c, 0x12, 0x5, 0x5, 0x12, 0x1c, 0x4};
char wing1[] = {0x7,0x8,0x17,0x10,0xf,0x3,0x0,0x0};
char wing2[] = {0x1c,0x2,0x19,0x1,0x1e,0x18,0x0,0x0};
char block[] = {0x1f,0x0,0x15,0x0,0x1f,0xe,0x0,0x0};
char alien[] = {0x0,0x0,0x0,0xe,0x11,0x15,0x15,0x15};
char blast[] = {0x15, 0xa, 0xa, 0x11, 0xa, 0x11, 0xa, 0x15};
char fireworks1[] = {0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0};
char fireworks2[] = {0x0, 0x0, 0x4, 0xa, 0x4, 0x0, 0x0, 0x0};
char fireworks3[] = {0x0, 0x4, 0xa, 0x11, 0xa, 0x4, 0x0, 0x0};
char fireworks4[] = {0xa, 0x15, 0xa, 0x11, 0xa, 0x15, 0xa, 0x0};
char current_key;
char prev;
char start = 0;

char on_off = 1;
char reset = 0;
unsigned char send_flag = 0;

char original1[] = "             <  ";
char original2[] = "    <           ";

char obs1[] = "             <  ";
char obs2[] = "    <           ";
char obs3[] = "     <     <   <";
char obs4[] = "   <    <    <  ";
char obs5[] = "    <     <     ";
char obs6[] = "       <       <";
char obs7[] = "   <   <   <   <";
char obs8[] = " <   <   <   <  ";
char obs9[] = "                ";
char obs10[] = "                ";

char menu[] = "Start = 1, down = 2 up = 3";
char user_pos = 1;
char m1_p = 0;
char m2_p = 0;
char lives = 0x10;
unsigned short dodged = 0;

enum sm_2{wait_k,press};
int KEYS_SM(int state)
{
	switch(state)
	{
		case wait_k:
			if(~PINB & 0x0F)
			{
				state = press;
			}				
			break;
		case press:
			//PORTB = 0x00;
			state = wait_k;
			break;
		default:
			state = wait_k;
			break;		
	}
	switch(state)
	{
		case wait_k:
			break;
		case press:
			current_key = (~PINB & 0x0F);
			break;
		default:
			break;
	}
	return state;
}

enum sm_1 { Init, Init2, wait, up, down,toggle,restart};
int USER_SM(int state)
{
	static char pos;
	switch(state)
	{
		case Init:
			LCD_DisplayString(1,menu);
			lives = (0x0F<<1);
			state = Init2;
			dodged = 0;
				LCD_build( 0, ship);
				LCD_build( 1, wing1);
				LCD_build( 2, wing2);
				LCD_build( 3, block);
				LCD_build( 4, alien);
				LCD_build( 5, blast);
				strncpy(obs1, original1, sizeof(obs1));
				strncpy(obs2, original2, sizeof(obs2));
			break;
		case Init2:
			if(current_key == 1)
			{
				lives = 0x0F;
				start = 1;
				PORTA = lives;
				state = wait;
			}
			break;
		case wait:
			if((~PINB & 0x0F) != 0){
				if((~PINB & 0x0F) == 0x01)
					if(reset)
						state = restart;
					else
						state = toggle;
				else if(!on_off || reset)
					return;
				else if((~PINB & 0x0F) == 0x02)
					state = down;
				else if((~PINB & 0x0F) == 0x04)
					state = up;
			}
			
			if((~PINB & 0x0F) == 0x02)
			state = down;
			break;
		case up:
			state = wait;
			break;
		case down:
			state = wait;
			break;
		case toggle:
			state = wait;
			break;
		case restart:
			state = wait;
			break;
		default:
			pos = 1;
			state = Init;
			break;
	}
	switch(state)
	{
		case Init:
			break;
		case Init2:
		
			break;
		case wait:
			PORTA = lives;
			break;
		case up:
			LCD_Cursor(1);
			user_pos = 1;
			LCD_WriteData(0x08);
			break;
		case down:
			LCD_Cursor(17);
			user_pos = 17;
			LCD_WriteData(0x08);
			break;
		case toggle:
			on_off = on_off == 1 ? 0: 1;
			break;
		case restart:
			reset = m1_p = m2_p = 0;
			LCD_DisplayString(1,obs1);
			LCD_DisplayString(17,obs2);
			LCD_Cursor(1);
			LCD_WriteData(0x08);
			on_off = 1;
			lives = 0x0F;
			dodged = 0;
			LCD_build( 0, ship);
			LCD_build( 1, wing1);
			LCD_build( 2, wing2);
			LCD_build( 3, block);
			LCD_build( 4, alien);
			LCD_build( 5, blast);
			strncpy(obs1, original1, sizeof(obs1));
			strncpy(obs2, original2, sizeof(obs2));
			break;
		default:
			break;
	}
	prev = current_key;
	return state;
}

int SCROLL_SM()
{
	if(!on_off || reset || !start)
		return 0;
	if(m1_p >= 16)
		m1_p = m2_p = 0;
	for(char i = 0; i < 16; ++i)
	{
		LCD_Cursor(i+1);
		LCD_WriteData(obs1[(i + m1_p)% 16]);
		LCD_Cursor(i+17);
		LCD_WriteData(obs2[(i+m2_p) % 16]);
		LCD_Cursor(user_pos);
		LCD_WriteData(0x08);
		if(dodged < 20 && dodged >= 10)
		{
			LCD_Cursor(32);//wing1
			LCD_WriteData(0x09);//wing1
		}
		else if(dodged < 30 && dodged >= 20)
		{
			LCD_Cursor(32);//block
			LCD_WriteData(0x0B);//block
			LCD_Cursor(31);//wing1
			LCD_WriteData(0x09);//wing1
			LCD_Cursor(16);//alien
			LCD_WriteData(0x0C);//alien
		}
		else if(dodged == 30)
		{
			LCD_Cursor(31);//block
			LCD_WriteData(0x0B);//block
			LCD_Cursor(30);//wing1
			LCD_WriteData(0x09);//wing1
			LCD_Cursor(15);//alien
			LCD_WriteData(0x0C);//alien
			LCD_Cursor(32);//wing2
			LCD_WriteData(0x0A);//wing2
		}
		else if(dodged == 31)// blast1
		{
			LCD_build( 1, blast);
			LCD_build( 2, wing2);
			LCD_build( 3, block);
			LCD_build( 4, alien);
			LCD_Cursor(31);//block
			LCD_WriteData(0x0B);//block
			LCD_Cursor(30);//wing1
			LCD_WriteData(0x09);//wing1
			LCD_Cursor(15);//alien
			LCD_WriteData(0x0C);//alien
			LCD_Cursor(32);//wing2
			LCD_WriteData(0x0A);//wing2
		}
		else if(dodged == 32)
		{
			LCD_build( 1, wing1);
			LCD_build( 2, blast);
			LCD_build( 3, block);
			LCD_build( 4, alien);
			LCD_Cursor(31);//block
			LCD_WriteData(0x0B);//block
			LCD_Cursor(30);//wing1
			LCD_WriteData(0x09);//wing1
			LCD_Cursor(15);//alien
			LCD_WriteData(0x0C);//alien
			LCD_Cursor(32);//wing2
			LCD_WriteData(0x0A);//wing2
		}
		else if(dodged == 33)
		{
			LCD_build( 1, blast);
			LCD_build( 2, wing2);
			LCD_build( 3, block);
			LCD_build( 4, blast);
			LCD_Cursor(31);//block
			LCD_WriteData(0x0B);//block
			LCD_Cursor(30);//wing1
			LCD_WriteData(0x09);//wing1
			LCD_Cursor(15);//alien
			LCD_WriteData(0x0C);//alien
			LCD_Cursor(32);//wing2
			LCD_WriteData(0x0A);//wing2
		}
		else if(dodged < 38 && dodged >= 34)
		{
			LCD_build( 1, blast);
			LCD_build( 2, blast);
			LCD_build( 3, blast);
			LCD_build( 4, blast);
			LCD_Cursor(31);//block
			LCD_WriteData(0x0B);//block
			LCD_Cursor(30);//wing1
			LCD_WriteData(0x09);//wing1
			LCD_Cursor(15);//alien
			LCD_WriteData(0x0C);//alien
			LCD_Cursor(32);//wing2
			LCD_WriteData(0x0A);//wing2
		}
		else if(dodged == 38)
		{
			LCD_build( 5, fireworks1);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
		else if(dodged == 39)
		{
			LCD_build( 5, fireworks2);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
		else if(dodged == 40)
		{
			LCD_build( 5, fireworks3);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
		else if(dodged == 41)
		{
			LCD_build( 5, fireworks4);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
		else if(dodged == 42)
		{
			LCD_build( 5, fireworks1);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
		else if(dodged == 43)
		{
			LCD_build( 5, fireworks2);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
		else if(dodged == 44)
		{
			LCD_build( 5, fireworks3);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
		else if(dodged == 45)
		{
			LCD_build( 5, fireworks4);
			for(unsigned char p = 5; p < 10; p++)
			{
				LCD_Cursor(p);
				LCD_WriteData(0x0D);
				LCD_Cursor(p+16);
				LCD_WriteData(0x0D);
			}
		}
	}
	LCD_Cursor(user_pos);	
	if((obs1[m1_p] == '<' && user_pos == 1) || (obs2[m2_p] == '<' && user_pos == 17))
	{
		if( lives == 0)
		{
			lives = 'G';
			LCD_DisplayString(1,"You lose!1:Again  2, 3:Menu");
			reset = 1;
			on_off = 0;				
		}
		else
		{
			lives = (lives >> 1);
			PORTA = lives;	
		}	
	}
	if((obs1[m1_p] == '<' && user_pos == 17) || (obs2[m2_p] == '<' && user_pos == 1) || dodged >30)
	{
		dodged++;
		if(dodged <= 10)
		{
			strncpy(obs1, obs5, sizeof(obs1));
			strncpy(obs2, obs6, sizeof(obs2));
		}
		else if(dodged <= 20)
		{
			strncpy(obs1, obs3, sizeof(obs1));
			strncpy(obs2, obs4, sizeof(obs2));
		}
		else if(dodged <= 30)
		{
			strncpy(obs1, obs7, sizeof(obs1));
			strncpy(obs2, obs8, sizeof(obs2));
		}
		else if(dodged < 50)
		{
			lives = 'W';
			strncpy(obs1, obs9, sizeof(obs1));
			strncpy(obs2, obs10, sizeof(obs2));
			
		}
		else if(dodged >= 50)
		{
			LCD_DisplayString(1,"VICTORY!!!!!!  1:Again 2,3:Menu");
			reset = 1;
			on_off = 0;
			dodged = 0;
		}
	}
	++m1_p; ++m2_p;
	return 0;
}
int main()
{
	DDRB = 0x00; PORTB = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0xFF; PORTA = 0x00;
	unsigned long int tick1_calc = 5;
	unsigned long int tick2_calc = 10;
	unsigned long int tick3_calc = 50;
	
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(tick1_calc,tick2_calc);
	tmpGCD = findGCD(tick3_calc,tmpGCD);
	unsigned long int GCD = tmpGCD;
	
	static task task1,task2,task3;
	task * tasks[] = {&task1,&task2, &task3};
	const short numTasks = 3;

	unsigned long int tick1_period = tick1_calc / GCD;
	unsigned long int tick2_period = tick2_calc / GCD;
	unsigned long int tick3_period = tick3_calc / GCD;

	task1.state = 10;
	task1.period = tick1_period;
	task1.elapsedTime = tick1_period;
	task1.TickFct = &USER_SM;

	task2.state = 10;
	task2.period = tick2_period;
	task2.elapsedTime = tick2_period;
	task2.TickFct = &KEYS_SM;
	
	task3.state = 10;
	task3.period = tick3_period;
	task3.elapsedTime = tick3_period;
	task3.TickFct = &SCROLL_SM;

	TimerSet(GCD);
	TimerOn();
	TimerFlag = 0;
	LCD_init();
	LCD_DisplayString(1,obs1);
	LCD_DisplayString(17,obs2);
	LCD_Cursor(1);
	unsigned long int i;
	
	initUSART0();
	
	while(1)
	{
		if( USART_IsSendReady0() ) {
			send_flag = 1;
		}
		if( send_flag == 1)
		{
			USART_Send0(lives);
			send_flag = 0;
		}
		USART_Send0(lives);
		for(i = 0; i < numTasks; ++i)
		{
			if(tasks[i]->elapsedTime == tasks[i]->period)
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}