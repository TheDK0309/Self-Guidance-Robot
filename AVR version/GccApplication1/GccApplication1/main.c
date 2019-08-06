#define F_CPU 8000000UL		
#include <avr/io.h>		
#include <stdio.h>		
#include <util/delay.h>		
#include <avr/interrupt.h>
#include <stdlib.h>
//enalbe PWM pins of L298N
#define ENA PD5
#define ENB PD6
//motors pins
#define forward1 PB2
#define forward2 PB3
#define backward1 PB4
#define backward2 PB5
//servo motor pin
#define servo PB1
//ultrasonic pins
#define trig PD7
#define echo PB0

int maximum = 15;       //maximum distance allowed
int i;
int x=0;
unsigned long time_count; //time
int distance;    //distance
int distance_left,distance_right; //distance to the left/right
int TimerOverflow = 0;

void distance_mes();  //distance measurement
void go_forward();//go forward
void go_left();//go to left
void go_right();//go to right
void go_backward();//go backward
void reset_motor(); //reset the DC motor
void turn_servo_right();//turn the ultrasonic sensor to the right
void turn_servo_left();//turn the ultrasonic sensor to the left

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	// Increment Timer Overflow count
}

int main(void)
{
	DDRB = 0xff;           // Configure PORTB as output
	DDRD |= (1<<trig);
	
	TCNT1 = 0;		//Set timer1 count zero 
	ICR1 = 2499;		//Set TOP count for timer1 in ICR1 register 
	
	TCCR0A |= (1 << COM0A1);
	// set none-inverting mode
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	// set fast PWM Mode
	TCCR0B |= (1 << CS01);
	// set prescaler to 8 and starts PWM
	//both motor run at 50% speed.
	OCR0A = 127;
	OCR0B = 127;
	//set output PWM Pins
	DDRD|=(1<<ENA)| (1<<ENB);
			
	/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	
	OCR1A = 175;	//Set servo at 0° position
	PORTB &= ~ ((1<<forward1)|(1<<forward2)|(1<<backward1)|(1<<backward2))|(1<<echo); //turn off all DC motors at start
	
	_delay_ms(1000);
	
	sei();			//Enable global interrupt
	TIMSK1 = (1 << TOIE0);	// Enable Timer1 overflow interrupts 
	TCCR1A = 0;		// Set all bit to zero Normal operation */
	
	while(1)
	{
		distance=0;
		distance_mes();
		if(distance>maximum||distance==0) //reset the distance if it exceeds the limit,or at the start
		{
			distance_mes(); //measure the distance
			if(distance>maximum||distance==0) //if distance > limit or at the start, go forward
			{
				go_forward();
			}
		}
		else //if distance<=limit(detected obstacle)
		{
			reset_motor();
			turn_servo_left(); //turn the sensor to the left
			
			distance_left=distance; //measure left distance
			
			turn_servo_right(); //turn the sensor to the right
			distance_right=distance; //measure right distance
			if(distance_right<10&&distance_left<10){ //if both distance to the left and right <backward10, go backward
				go_backward();
			}
			else
			{
				if(distance_right>distance_left) //if left distance<right distance, go to left
				{
					go_left();
					_delay_ms(1000); //1s
				}
				if(distance_right<distance_left) //if right distance<left distance, go to right
				{
					go_right();
					_delay_ms(1000); //1s
				}
			}
		}
	}
	return 0;
}

void distance_mes(){
	PORTD &= ~(1<<trig);
	_delay_us(2);
	PORTD |= (1<<trig);
	_delay_us(10);
	PORTD &= ~(1<<trig);
	
	TCNT1 = 0;	// Clear Timer counter 
	TCCR1B = 0x41;	// Capture on rising edge, No prescaler
	TIFR1 = 1<<ICF1;	// Clear ICP flag (Input Capture flag) 
	TIFR1 = 1<<TOV1;	// Clear Timer Overflow flag 
	
	while ((TIFR1 & (1 << ICF1)) == 0);// Wait for rising edge 
	TCNT1 = 0;	// Clear Timer counter 
	TCCR1B = 0x01;	// Capture on falling edge, No prescaler 
	TIFR1 = 1<<ICF1;	// Clear ICP flag (Input Capture flag) 
	TIFR1 = 1<<TOV1;	// Clear Timer Overflow flag 
	TimerOverflow = 0;// Clear Timer overflow count 
	
	while ((TIFR1 & (1 << ICF1)) == 0);// Wait for falling edge 
	time_count = ICR1 + (65535 * TimerOverflow);	// Take count 
	// 8MHz Timer freq, sound speed =343 m/s 
	distance = (int)time_count / 466.47;
}
void go_forward(){
	PORTB |=(1<<forward1)|(1<<forward2);
	PORTB &=~((1<<backward1) |(1<<backward2));
}
void go_left(){
	reset_motor();
	PORTB |=(1<<backward1);//backward pin of left motor on, makes left motor turns clockwise, right motor stop
	_delay_ms(1000);
	PORTB &=~(1<<backward1); //turn off motor
}
void go_right(){
	reset_motor();
	PORTB |=(1<<backward2);//backward pin of right motor on, makes left motor turns clockwise, left motor stop
	_delay_ms(1000);
	PORTB &=~(1<<backward2); //turn off motor
}
void go_backward(){
	reset_motor();
	for(i=0;i<20;i++)//turn on both backward pins
	{
		PORTB |= (1<<backward1) |(1<<backward2);
	}
	//after going backward for 20,turn both pins down
	PORTB &=~((1<<backward1) |(1<<backward2));
}
void reset_motor(){
	PORTB &= ~ ((1<<forward1)|(1<<forward2)|(1<<backward1)|1<<(backward2));
}
void turn_servo_right(){
	OCR1A = 65;	//Set servo at -90/180° position
	_delay_ms(1000);
	distance_mes(); //measure the distance
	OCR1A = 175;
}
void turn_servo_left(){
	OCR1A = 300;	//Set servo at 90° position
	_delay_ms(1000);
	distance_mes(); //measure the distance
	OCR1A = 175;
}
