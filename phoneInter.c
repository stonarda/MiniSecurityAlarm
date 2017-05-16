/* Used to interface with the GSM module */

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <string.h>

volatile uint8_t receiveSize = 0, receiveComp = 0,
	sendSize = 0, sendPos = 0, initComplete = 0;
volatile char receive[255], send[201];

// Function to call when line of info received
void (*toCall) (char*);

// Used for checking for substrings
uint8_t strContain(char* str, char* in)
{
	if(strstr(in, str) != NULL)
	{
		return 1;
	}
	return 0;
}

void sendChar(uint8_t data)
{
	UDR1 = data;
	while(!(UCSR1A & (1 << TXC1)));
}

uint8_t getChar(void)
{
	while(!(UCSR1A & (1 << RXC1)));
	return UDR1;
}

// Called to send whole strint
void sendStr(char* str)
{
	// WAits for current sending to complete
	while(sendSize != sendPos);

	// Adds to send buffer
	sendSize = 0;
	while(str[sendSize] != '\0')
	{
		send[sendSize] = str[sendSize];
		sendSize++;
	}

	sendPos = 1;
	sendChar(str[0]);
}

/// returns last received string
void getStr(char* str)
{
	while(!receiveComp);

	uint8_t cnt;
	for(cnt = 0; cnt < receiveSize; cnt++)
	{
		str[cnt] = receive[cnt];
	}
	str[cnt] = '\0';

	receiveSize = 0;
	receiveComp = 0;
}

// Called on receiving
ISR(USART1_RX_vect)
{
	char data = UDR1;

	// Checks buffer not full
	if(receiveSize < 255)
	{
		// Saves data
		receive[receiveSize] = data;
		receiveSize++;
		receive[receiveSize] = '\0';
	
		// Checks if end of data possible
		if(receiveSize > 1)
		{
			if((receive[receiveSize-1] == '\n')
				| (receive[receiveSize-1] == '\r'))
			{
				// End of data reached
				receiveComp = 1;
				if(initComplete)
				{
					// Init stage passed so calls method
					receiveSize = 0;
					receiveComp = 0;
					toCall(receive);
				}
			}
		}
	}
}

ISR(USART1_TX_vect)
{
	if(sendSize != sendPos)
	{
		sendChar(send[sendPos]);
		sendPos++;
	}
}

// To initialise, 0=error, 1=success
void usart_Init(void (*toCallFun)(char*))
{
	toCall = toCallFun;

	uint16_t baudPrescale = ((F_CPU / (9600 * 16UL)) - 1);

	// Setting to 8 bits being set
	UBRR1H = (baudPrescale >> 8);
	UBRR1L = baudPrescale;

	// Enable Receiver and Transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) |
		(1 << RXCIE1) | (1 << TXCIE1);
	
	// Using 8-bit data length in frame with one stop bit
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

// Function allows baud rate to be detected
//  and also sets up GSM module
//  send command and checks ok
//  sets to SMS mode
//  stops command echoe
void sync(void)
{
	char data[100];
	
	do
	{
		sendStr("AT\r");
		getStr(data);
	} while(!strContain("OK", data));
	
	_delay_ms(100);
	
	do
	{
		sendStr("AT+CMGF=1\r");
		getStr(data);
	} while(!strContain("OK", data));

	do
	{
		sendStr("ATE01\r");
		getStr(data);
	} while(!strContain("OK", data));
	
	initComplete++;
	receiveSize = 0;
	receiveComp = 0;
}

// Empties received buffer and sets all to NULL
void clearReceived(void)
{
	receiveSize = 0;
	receiveComp = 0;

	uint8_t cnt = 0;
	for(cnt = 0; cnt < 255; cnt++)
	{
		receive[cnt] = '\0';
	}
}
