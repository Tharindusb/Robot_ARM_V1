
#include <avr/io.h>		
#include <stdio.h>		
#include <util/delay.h>		
#include <stdlib.h>

#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)

int angle;
int duration;


////////////////////////////USART COMMUNICATION///////////////////////////////////////////////

void USART_Init(unsigned long BAUDRATE)				
{ 
	UCSRB |= (1 << RXEN) | (1 << TXEN);				
	UCSRC |= (1 << URSEL)| (1 << UCSZ0) | (1 << UCSZ1);	
	UBRRL = BAUD_PRESCALE;							
	UBRRH = (BAUD_PRESCALE >> 8);					
}

unsigned char USART_RxChar(void)									/* Data receiving function */
{
	while ((UCSRA & (1 << RXC)) == 0);					
	return(UDR);									
}

void USART_TxChar(char data)						/* Data transmitting function */
{
	UDR = data;										
	while (!(UCSRA & (1<<UDRE)));
}					

void USART_SendString(char *str)					/* Send string of USART data function */ 
{
	int i=0;																	
	while (str[i]!=0)
	{
		USART_TxChar(str[i]);						/* Send each char of string till the NULL */
		i++;
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(void)
{   
    
    DDRD |= (1<<PD3);	
	
	DDRD |= 0xff;/*  output */ 
	TCNT1 = 0;	/* Set timer1 count zero */
    TCNT2 = 0;	/* Set timer2 count zero */
	 /* Set TOP count for timer1 in ICR1 register */
	USART_Init(9600);
	

	/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	  TCCR1A = 0;    // disable all PWM on Timer1 whilst we set it up
	  TCCR1B=0;
	  ICR1 = 3749;
	  TCCR1A = (1<<WGM11);
	  TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	  DDRD  |= (1<<PD5)|(1<<PD4);
	  TCCR1A|= (1<<COM1A1); 
	  TCCR1A|= (1<<COM1B1); 
	  ////////////////////////////////////////////////////////////////
	  
	  
	  
	  /* setup PB3 pin    */
	  TCCR0 = 0;    
	  //OCR0 = 155;
	  TCCR0  = (1<<WGM00)|(1<<WGM01)|(1<<CS01)|(1<<CS00);
	  DDRB  |= (1<<PB3);
	  TCCR0 |=(1 << COM01);
	  ///////////////////////////////////////////////
	  
	  /* setup Pd7 pin    */
	  TCCR2 = 0;
	  //OCR2 = 155;
	  //TCCR2 = (1 << WGM21)|(1 << WGM20) | (1 << CS22) | (1 << CS21)| (1 << CS20); //1024
	  TCCR2  = (1<<WGM21)|(1<<WGM20)|(1<<COM21)|(1<<CS22); //64
	  DDRD  |=(1 << PD7);
	  TCCR2 |=(1 << COM21);
	  
	
	
	///////MOTORS INITIAL POSITIONS////////////////////
	angle = 178; //Gripper//
    
     OCR1A = 290;//shoulder// INITIALIZE THE MOTOR AT 50 ANGEL 
	_delay_ms(1000);
	OCR0  = 150; //elbow //INITIALIZE THE MOTOR AT 0 ANGEL 
	_delay_ms(1000);
	OCR1B = 68; //waist //INITIALIZE THE MOTOR AT 0 ANGEL 
	_delay_ms(1000);
	OCR2  =  68; //wrist roll //INITIALIZE THE MOTOR AT 0 ANGEL 
	_delay_ms(1000);
	
	////////////////////////////////////////////////////

  
	
	while(1)
	{  
	   
	 /////////////////GRIPPER///////////////////////////
	  int count1 = 0;
	   while((USART_RxChar() == '1') && (angle <= 173) && (++count1 < 20))
		{    
		    
			duration = -5*(angle) + 1610 ;
			PORTD  |= (1<<3);
			
			duration /= 2 ;
			for(int i = 0; i < duration; i++)
			{
                _delay_us(1);
            }
			PORTD  &= ~(1<<3);
			angle = angle + 5 ;
			_delay_ms(20);
			
		}
		
		int count2 = 0;
		while((USART_RxChar() == '2') && (angle >= 20) && (++count2 < 20))
		{
			
			duration = -5*(angle) + 1610 ;
			PORTD  |= (1<<3);
			
			duration /= 2 ;
			for(int i = 0; i < duration; i++)
			{
                _delay_us(1);
            }
			PORTD  &= ~(1<<3);
			angle = angle - 5 ;
			_delay_ms(20);
		}
	  /////////////////////////////////////////////////////
	
	 
	  /////////////////WRIST ROLL///////////////////////////
		while((USART_RxChar() == '3') && (OCR2 >= 68))
		{
			OCR2 = OCR2 - 4 ; 
			_delay_ms(80);
			char str[16];
            USART_SendString(itoa(OCR1A,str,10));
			
		}

		while((USART_RxChar() == '4') && (OCR2 <= 298))
		{
			OCR2 = OCR2 + 4 ;
			_delay_ms(80);
			char str[16];
            USART_SendString(itoa(OCR1A,str,10));
			
		}
	 ///////////////////////////////////////////////////////
	
	
	 
	  /////////////////ELBOW/////////////////////////////////
		while((USART_RxChar() == '5') && (OCR0 >= 68))
		{
			OCR0 = OCR0 - 2 ; 
			_delay_ms(50);
			char str[16];
            USART_SendString(itoa(OCR0,str,10));
			
		}

		while((USART_RxChar() == '6') && (OCR0<= 298))
		{
			OCR0 = OCR0 + 2 ;
			_delay_ms(50);
			char str[16];
            USART_SendString(itoa(OCR0,str,10));
			
		}
	 
	 ///////////////////////////////////////////////////////
	 
	  
	  /////////////////SHOULDER///////////////////////////
		while((USART_RxChar() == '7') && (OCR1A >= 68))
		{
			OCR1A = OCR1A - 2 ; 
			_delay_ms(50);
			char str[16];
            USART_SendString(itoa(OCR1A,str,10));
			
		}

		while((USART_RxChar() == '8') && (OCR1A <= 298))
		{
			OCR1A = OCR1A + 2 ;
			_delay_ms(50);
			char str[16];
            USART_SendString(itoa(OCR1A,str,10));
			
		}
	 ///////////////////////////////////////////////////////
	 
	 
	 /////////////////WAIST/////////////////////////////////
		

		while((USART_RxChar() == '9') && (OCR1B >= 68))
		{
			OCR1B = OCR1B - 1 ;
			_delay_ms(80);
			char str[16];
            USART_SendString(itoa(OCR1B,str,10));
			
		}
		while((USART_RxChar() == '0') && (OCR1B <= 298))
		{
			OCR1B = OCR1B + 1 ; 
			_delay_ms(80);
			char str[16];
            USART_SendString(itoa(OCR1B,str,10));
			
		}
	 
	 ///////////////////////////////////////////////////////
		
	}
}
