/* To Run Application */

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "phoneInter.h"

#define CENTER_BUTTON    PE7
#define UP PC4
#define DOWN PC2

#define LEDS_OFF         PORTF &= ~_BV(PINF0) & ~_BV(PINF1) & ~_BV(PINF2)
#define LEDS_INIT        DDRF |= _BV(PINF0) | _BV(PINF1) | _BV(PINF2)
#define BLUE_ON          PORTF |= _BV(PINF0)
#define GREEN_ON         PORTF |= _BV(PINF1)
#define RED_ON           PORTF |= _BV(PINF2)

#define BUZZER_INIT      DDRD |= _BV(PIND1)
#define BUZZER_ON        PORTD |= _BV(PIND1)
#define BUZZER_OFF       PORTD &= ~_BV(PIND1)

char userNumber[14] = "+440000000000";
volatile uint8_t sensor, armed;

void init(void)
{
	// 8MHz clock with no prescaling
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
	
	// Inits all outputs
	LEDS_INIT;
	BUZZER_INIT;
	
	// Disables JTag
	MCUCR |=(1<<JTD);
	MCUCR |=(1<<JTD); 

	// IR Sensor and its interrupt
	DDRE &= ~_BV(CENTER_BUTTON);
	PORTE |= _BV(CENTER_BUTTON);
	EICRA |= _BV(ISC00) | _BV(ISC01);
	EIMSK |= _BV(INT0);
	
	// Up down and center button
	DDRE &= ~_BV(CENTER_BUTTON);
	PORTE |= _BV(CENTER_BUTTON);
	DDRC &= ~_BV(UP);
	DDRC &= ~_BV(DOWN);
	PORTC |= _BV(UP);
	PORTC |= _BV(DOWN);
}

// Sensor firing
ISR(INT0_vect)
{
	sensor++;
}

// Used to send a text message to user
void sendMessage(char *message)
{
	char* data = "AT+CMGS=\"             \"\r";
	
	int8_t cnt;
	for(cnt = 0; cnt < 13; cnt++)
	{
		data[cnt + 9] = userNumber[cnt];
	}
	sendStr(data); // First command
	
	_delay_ms(100); // Gives GSM module time to process
	
	char messageData[102];
	for(cnt = 0; (cnt < 100) && (message[cnt] != '\0'); cnt++)
	{
		messageData[cnt] = message[cnt];
	}
	
	messageData[cnt] = 26;
	messageData[cnt+1] = '\0';
	sendStr(messageData); // Message contents
}

volatile char toSend = 0;

// Called on message receiving
void gotMessage(char *data)
{
	static uint8_t state = 0;
	
	// state->0 means waiting to get message
	// state->1 means need to check number
	// state->2 means arm or disarm
	
	if((state==0) && (strContain("+CMTI:", data)))
	{
		toSend = 1;
		state = 1;
	}
	if((state == 1) && (toSend == 2) && (strContain(&userNumber, data)))
	{
		state = 2;
	}
	if((state == 2) && strContain("DISARM", data))
	{
		state = 0;
		armed = 0;
		toSend = 3;
	}
	else if((state == 2) && strContain("ARM", data))
	{
		state = 0;
		armed = 1;
		toSend = 3;
	}
}

// Used for entering number initially
uint8_t buttonLoop(uint8_t pos)
{
	// Center Button
	if(!(PINE & _BV(CENTER_BUTTON)))
	{
		return 1;
	}
	
	// Up Button
	if(!(PINC & _BV(UP)))
	{
		if(userNumber[pos] > '0')
		{
			userNumber[pos]--;
		}
		return 2;
	}
	
	// Down Button
	if(!(PINC & _BV(DOWN)))
	{
		if(userNumber[pos] < '9')
		{
			userNumber[pos]++;
		}
		return 2;
	}
	
	return 0;
}

// Called to enable user to enter mobile no
void setupNumber(void)
{
	uint8_t pos = 1;
	showClock(userNumber, pos);

	while(pos < 13)
	{
		uint8_t result = buttonLoop(pos);
		if(result == 1)
		{
			pos++;
			showClock(&userNumber, pos);
		}
		if(result == 2)
		{
			showClock(&userNumber, pos);
		}
		
		_delay_ms(100);
	}
	
	clear_screen();
}

// Holds setup and main loop
void main(void)
{		
	init();
	
	init_lcd();
	usart_Init(&gotMessage);
	
	static uint8_t isAlarm = 0;
	
	// Enables global interrupts and sync baud rate with GSM module
	sei();	
	sync();
	
	// Clear all sms messages stored on sim
	_delay_ms(100);
	sendStr("AT+CMGDA=\"DEL ALL\"\r");
	_delay_ms(100);
	
	// User enters their number
	setupNumber();
	
	BLUE_ON;
	sendMessage("SECSYS: System Started");
	display_string("SECSYS: SYSTEM Started\n");
	
	while(1)
	{
		// Return all messages stored on sim
		if(toSend == 1)
		{
			toSend = 2;
			sendStr("AT+CMGL=\"ALL\"\r");
		}
		
		// Used to indicate state of device
		if(toSend == 4)
		{
			toSend = 5;
			sensor = 0;
			isAlarm = 0;
		
			if(armed)
			{
				// Armed
				LEDS_OFF;
				GREEN_ON;
				
				_delay_ms(2500);
				sendMessage("SECSYS: Device armed");
				display_string("SECSYS: Device armed\n");
				_delay_ms(2500);
			}
			else
			{
				// Not armed
				LEDS_OFF;
				BLUE_ON;
				
				sendMessage("SECSYS: Device now not armed");
				display_string("SECSYS: Device now not armed\n");	
			}
		}
		
		// Deletes all message on sim
		if(toSend == 3)
		{
			sendStr("AT+CMGDA=\"DEL ALL\"\r");
			toSend = 4;
		}
		
		// Checks if alarm needs to be triggered
		if(armed && sensor && !isAlarm)
		{
			LEDS_OFF;
			RED_ON;
		
			isAlarm++;
			sensor = 0;
			
			BUZZER_ON;
			
			sendMessage("SECSYS: ALARM activated\n");
			display_string("SECSYS: ALARM activated\n");
		}
		if(!isAlarm)
		{
			BUZZER_OFF;
		}
		
		_delay_ms(100);
	}
}
