#include <avr/io.h>
#include "timer.h"
#include "scheduler.h"
#include "io.c"
#include "usart.h"

void transmit_data(unsigned char row, unsigned char column) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTA = 0x08;
		// set SER = next bit of data to be sent.
		PORTA |= ((row >> i) & 0x01);
		PORTB = ((column >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTA |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTA |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTA = 0x00;
	PORTB = 0x00;
}

unsigned short x;
unsigned char lives;
unsigned char start = 0;
const unsigned char welcome[] = "You're ugly         HAH HAH";
const unsigned char hah[] = "HAH HAH";
const unsigned char dont[]= "SHOOT HIM        DON'T EVEN TRY";
const unsigned char see[] = "Can't get me :p";
const unsigned char aw[] = "Aw Do I see some tears?";
const unsigned char die[] = "DIE DIE";
const unsigned char your[] = "YOUR SHIELDS ARE DOWN";
const unsigned char ill[] = "YOU'RE STILL    UGLY";
const unsigned char lowpow[] = "Systems low power?! NO &@#^(!";
const unsigned char done[] = "^%@#*!@!&(%*#($&# *(#&$*&!@#^@";
//unsigned char mean_flag = 0;
//unsigned char sad_flag = 0;

void e_normal(){
	transmit_data(0x40, 0x18);//eyebrows
	transmit_data(0x20, 0xBD);//eyes
}

void e_mean(){
	transmit_data(0x80, 0x7E);
	transmit_data(0x40, 0xBD);
	transmit_data(0x20, 0x99);
}
void e_meanup(){
	transmit_data(0x80, 0xBD);
	transmit_data(0x40, 0x99);
}

void e_sad1(){
	transmit_data(0x80, 0xDB);
	transmit_data(0x40, 0xBD);
	transmit_data(0x20, 0x5A);
}	
void e_sad2(){
	transmit_data(0x80, 0xDB);
	transmit_data(0x40, 0xBD);
	transmit_data(0x20, 0x5A);
	transmit_data(0x10, 0xFB);
}

void m_closed(){
	transmit_data(0x02, 0x81);//mouthclosed
}

void m_smallo(){
	transmit_data(0x04, 0xC3);//mouth oup
	transmit_data(0x02, 0xDB);//mouth omid
	transmit_data(0x01, 0xC3);//mouth odown
}

void m_largeo(){
	transmit_data(0x04, 0x81);//mouth oup
	transmit_data(0x02, 0xBD);//mouth omid
	transmit_data(0x01, 0x81);//mouth odown
}

void m_largeoup(){
	transmit_data(0x08, 0x81);//mouth oup
	transmit_data(0x04, 0xBD);//mouth omid
	transmit_data(0x02, 0x81);//mouth odown
}

void m_largeo2(){
	transmit_data(0x08, 0x81);//mouth oup
	transmit_data(0x04, 0xBD);//mouth omid
	transmit_data(0x02, 0xBD);//mouth omid
	transmit_data(0x01, 0x81);//mouth odown
}

void m_closed2(){
	transmit_data(0x02, 0xC3);//mouth mid
}

void break1()
{
	transmit_data(0xAA, 0x55);
	transmit_data(0x55, 0xAA);
}

void break2()
{
	transmit_data(0x55, 0x55);
	transmit_data(0xAA, 0xAA);
}

void m_tongue1(){
	transmit_data(0x04, 0x81);
	transmit_data(0x02, 0xD7);
	transmit_data(0x01, 0xC7);
}
void m_tongue2(){
	transmit_data(0x04, 0x81);
	transmit_data(0x02, 0xEB);
	transmit_data(0x01, 0xE3);
}

/*enum sm_2{normal, mean, sad} e_state;
int Eyes_SM(int state)
{
	switch(e_state)
	{
		case -1:
		e_state = normal;
		break;
		case normal:
			if( mean_flag == 0 && sad_flag == 0)
			{
				e_state = normal;
			}
			else if( mean_flag != 0)
			{
				e_state = mean;
			}				
			else if(sad_flag != 0)
			{
				e_state = sad;
			}
			break;
		case mean:
			if( mean_flag != 0)
			{
				e_state = sad;
			}
			else
			{
				e_state = normal;
			}
			break;
		case sad:
			if( sad_flag == 1)
			{
				e_state = sad;
			}
			else
			{
				e_state = normal;
			}
			break;
		default:
			e_state = normal;
			break;
	}
	switch(e_state)
	{
		case normal:
			e_normal();
			break;
		case mean:
			e_mean();
			break;
		case sad:
			e_sad();
			break;
		default:
			break;
	}
	return e_state;
}
*/
enum sm_1 { Init, Init2, begin, life3, life2, life1, life0, gameover, win};
int Mouth_SM(int state)
{
	static char pos;
	switch(state)
	{
		case Init:
			 LCD_DisplayString(1, welcome);
			state = Init2;
			break;
		case Init2:
			if(lives == (0x0F <<1))
			{
				state = Init2;
				x++;
			}
			else if( lives <= 0x0F)
			{
				x = 0;
				LCD_DisplayString(1, dont);
				state = begin;
			}
			break;
		case begin:
			if(lives == 0x0F)
			{
				state = begin;
				x++;
			}
			else if ( lives <= (0x0F>>1) )
			{
				state = life3;
				LCD_DisplayString(1, see);
				x = 0;
			}
			else if (lives == 'W')
			{
				state = win;
				LCD_DisplayString(1, ill);
				x=0;
			}
			break;
		case life3:
			if(lives == (0x0F>>1))
			{
				state = life3;
				x++;
			}
			else if ( lives <= (0x0F>>2) )
			{
				state = life2;
				LCD_DisplayString(1, aw);
				x = 0;
			}
			else if (lives == 'W')
			{
				state = win;
				LCD_DisplayString(1, ill);
				x=0;
			}
			break;
		case life2:
		if(lives == (0x0F>>2))
		{
			state = life2;
			x++;
		}
		else if ( lives <= (0x0F>>3) )
		{
			state = life1;
			LCD_DisplayString(1, die);
			x = 0;
		}
		else if (lives == 'W')
		{
			state = win;
			LCD_DisplayString(1, ill);
			x=0;
		}
		break;
		case life1:
		if(lives == (0x0F>>3))
		{
			state = life1;
			x++;
		}
		else if ( lives <= (0x0F>>4) )
		{
			state = life0;
			LCD_DisplayString(1, your);
			x = 0;
		}
		else if (lives == 'W')
		{
			state = win;
			LCD_DisplayString(1, ill);
			x=0;
		}
		break;
		case life0:
		if(lives <= (0x0F>>4))
		{
			state = life0;
			x++;
		}
		else if ( lives == 'G' )
		{
			state = gameover;
			LCD_DisplayString(1, hah);
			x = 0;
		}
		else if (lives == 'W')
		{
			state = win;
			LCD_DisplayString(1, ill);
			x=0;
		}
		break;
		case gameover:
			if( lives == 'G')
			{
				state = gameover;
				x++;
			}
			else if(lives == (0x0F<<1))
			{
				state = Init;
				x=0;
			}
			else
			{
				state = begin;
				LCD_DisplayString(1, dont);
				x = 0;
			}		
			break;
		case win:
			if( lives == 'W')
			{
				state = win;
				x++;
			}
			else if(lives == (0x0F<<1))
			{
				state = Init;
				x=0;
			}
			else
			{
				state = begin;
				LCD_DisplayString(1, dont);
				x = 0;
			}
			break;
		default:
			state = Init;
			break;
	}
	switch(state)
	{
		case Init:
			lives = 0x10;
			x = 0;
			break;
		case Init2: //You're ugly         HAH HAH
			if( x  < 100){
				e_normal();
				m_smallo();
			}
			else if ( x < 200 ){
				e_normal();
				m_largeo();
			}
			else if ( x < 400){
				e_normal();
				m_largeo();
			}
			else if ( x < 600){
				e_normal();
				m_closed2();
			}
			else if ( x < 800){
				e_normal();
				m_closed();
			}
			else if (x < 1400){
				e_normal();
				m_closed();
			}
			else if ( x < 1600 ){
				e_mean();
				m_largeo();
			}
			else if ( x < 1800){
				e_mean();
				m_closed();
			}
			else if ( x < 2000){
				e_mean();
				m_largeo();
			}
			else if ( x < 10000){
				e_normal();
				m_closed();
			}
			else{
				x = 0;
			}
			break;
		case begin:
			if( x  < 100){
				e_mean();
				m_closed();//shit
			}
			else if ( x < 200 ){
				e_mean();
				m_closed2();
			}
			else if ( x < 400){
				e_mean();
				m_smallo();
			}
			else if ( x < 600){
				e_mean();
				m_closed2(); //shi
			}
			else if ( x < 800){
				e_mean();
				m_closed2();//f
			}
			else if (x < 1000){
				e_mean();
				m_smallo();
			}
			else if ( x < 1200 ){
				e_mean();
				m_closed2();//f
			}
			else if ( x < 1400){
				e_mean();
				m_smallo();//d
			}
			else if ( x < 1600){
				e_mean();
				m_closed();//d
			}
			else if ( x < 1800){
				e_mean();
				m_closed2();//e
			}
			else if ( x < 2000){
				e_mean();
				m_smallo();
			}
			else if ( x < 2200){
				e_mean();
				m_closed2();
			}
			else if ( x < 2400){
				e_mean();
				m_smallo();//try
			}
			else if ( x < 2600){
				e_mean();
				m_closed2();
			}
			else if ( x >= 2600){
				e_mean();
				m_closed();
			}
			break;
		case life3://Can't get me :p
			if ( x < 200 ){
				e_mean();
				m_largeo();
			}
			else if ( x < 400){
				e_mean();
				m_closed();
			}
			else if ( x < 600){
				e_mean();
				m_smallo();
			}
			else if ( x < 800){
				e_normal();
				m_closed2();
			}
			else if ( x < 1000){// :p
				e_normal();
				m_tongue1();
			}
			else if ( x < 1200){
				e_normal();
				m_tongue2();
			}
			else if( x>= 1200)
			{
				e_normal();
				x = 801;
			}
			break;
		case life2://"Aw Do I see some tears?";
			if ( x < 200 ){
				e_sad1();
				m_largeo();
			}
			else if ( x < 400){
				e_sad1();
				m_smallo();
			}
			else if ( x < 600){
				e_sad1();
				m_largeo();
			}
			else if ( x < 800){
				e_sad1();
				m_closed2();
			}
			else if ( x < 1000){
				e_sad1();
				m_largeo();
			}
			else if( x < 1200)
			{
				e_sad1();
				m_closed2();
			}
			else if( x < 1400)
			{
				e_sad1();
				m_closed();
			}
			else if( x < 1600)
			{
				e_sad2();
				m_closed();
			}
			else if( x >= 1600)
			{
				x=1200;
			}
		break;
		case life1://DIE DIE
			if ( x < 200 ){
				e_mean();
				m_largeo();
			}
			else if ( x < 400){
				e_mean();
				m_closed();
			}
			else if ( x < 600){
				e_mean();
				m_largeo();
			}
			else if ( x >= 600){
				e_mean();
				m_closed();
			}
		break;
		case life0://"YOUR SHIELDS ARE DOWN"
			if ( x < 200 ){
				e_mean();
				m_largeo();
			}
			else if ( x < 400){
				e_mean();
				m_closed2();
			}
			else if ( x < 600){
				e_mean();
				m_largeo();
			}
			else if ( x < 800){
				e_mean();
				m_closed();
			}
			else if ( x < 1000){
				e_mean();
				m_largeo();
			}
			else if ( x >= 1000){
				e_mean();
				m_closed();
			}
		break;
		case gameover://hahahahahahaha
			if ( x < 200 ){
				e_mean();
				m_largeo();
			}
			else if ( x < 400 ){
				e_meanup();
				m_largeoup();
			}
			else if ( x < 600){
				e_mean();
				m_largeo();
			}
			else if ( x < 800 ){
				e_meanup();
				m_largeoup();
			}
			else if( x>= 800)
			{
				x = 0;
			}
		break;
		case win://"YOU'RE STILL UGLY";//"Systems low power?! NO &@#^(!"
			if( x  < 500){
				e_normal();
				m_smallo();
			}
			else if ( x < 1000 ){
				e_normal();
				m_largeo();
			}
			else if ( x < 1500){
				e_normal();
				m_closed2();
			}
			else if ( x < 2000){
				e_normal();
				m_largeo();
			}
			else if ( x < 2500){
				e_normal();
				m_closed2();
			}
			else if ( x < 3000){//ugly
				e_normal();
				m_closed();
			}
			else if ( x < 3500 ){
				e_sad1();
				m_closed2();
			}
			else if ( x < 4000){
				e_sad1();
				m_largeo();
			}
			else if ( x < 4500){
				e_sad1();
				m_smallo();
			}
			else if ( x < 5000){
				e_sad1();
				m_largeo();
			}
			else if ( x < 5500){
				e_sad1();
				m_smallo();
			}
			else if ( x < 6000){
				e_sad1();
				m_closed();
			}
			else if ( x < 6500){
				e_sad2();
				m_largeo2();
			}
			else if ( x < 7300){//break
				break1();
			}
			else if ( x < 7600){
				break2();
			}
			else if ( x < 7900){
				break1();
			}
			else if ( x < 8200){
				break2();
			}
			else if( x>= 8200)
			{
				x = 6500;
			}
			if( x ==  3000)
			{
				LCD_DisplayString(1, lowpow);
			}
			if( x ==  8199)
			{
				LCD_DisplayString(1, done);
			}
		break;
		
		default:
			break;
	}
	
	return state;
}

int main()
{
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long int tick1_calc = 1;
	unsigned long int tick2_calc = 1;
	unsigned long int tick3_calc = 1;
	
	unsigned long int tmpGCD = 1;
	/*tmpGCD = findGCD(tick1_calc,tick2_calc);
	tmpGCD = findGCD(tick3_calc,tmpGCD);
	unsigned long int GCD = tmpGCD;*/
	unsigned long int GCD = 1;
	
	static task task1/*,task2,task3*/;
	task * tasks[] = {&task1/*, &task2, &task3*/};
	const short numTasks = 1;

	unsigned long int tick1_period = tick1_calc / GCD;
	/*unsigned long int tick2_period = tick2_calc / GCD;
	unsigned long int tick3_period = tick3_calc / GCD;*/

	task1.state = 10;
	task1.period = tick1_period;
	task1.elapsedTime = tick1_period;
	task1.TickFct = &Mouth_SM;

	/*task2.state = 10;
	task2.period = tick2_period;
	task2.elapsedTime = tick2_period;
	task2.TickFct = &Eyes_SM;
	
	task3.state = 10;
	task3.period = tick3_period;
	task3.elapsedTime = tick3_period;
	task3.TickFct = &SCROLL_SM;*/

	TimerSet(GCD);
	TimerOn();
	TimerFlag = 0;
	LCD_init();
	unsigned long int i;
	
	initUSART0();
	
	while(1)
	{
		if( USART_HasReceived0() ) {
			lives = USART_Receive0();
			USART_Flush0();
		}
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