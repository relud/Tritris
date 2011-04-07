
#define F_CPU 1000000UL // Tiny26 processing speed is 1 Mhz
#define true 1
#define false 0

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdlib.h>

void screen (void);
void input (void);
void progress (void);
void newgame (void);
char check (char s);
void animation (void);
char randnum (void);
void seedrand (void); 
void newpiece (void);
void report (void);
void clr (char r);

char seed;
char row[8][2]; // the led grid, 1 is on, 0 is off, array value specifies the row, second array 0 is non moving, 1 is moving
char in;
unsigned char score;

int main (void) 
{
	int i;
	
	DDRA = 0b11111111; // currently, Port A pin 7-0 are set to outputs

	DDRB = 0b10111111; // currently, Port B pin 7, and 5-0 are set to outputs, 6 is inut

	PORTA = 0b00000000; // currently, Port A are set to low

	PORTB = 0b00000000; // currently, Port B are set to low
	
	seedrand ();
	
	newgame ();
	animation ();
	newpiece ();
	
	while (1)  // infinite loop
	{
		for (i = 0; i < 500; i++)
		{
			screen ();
			input ();
		}

		progress ();
		
		if (!check (3))
		{
			newgame ();
			animation ();
			newpiece ();
		}
	}
}

void screen (void)
{
	char i;
	char j;
	char k;
	
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 8; j++)
		{
			PORTA = 255 - (1 << j);

			PORTB = row[j][i];
			_delay_us (50);
		
			PORTB = 0;
		}
	}
}

void input (void)
{
	char i;
	char j;
	
	PORTA = 0b11111111;
	
	for (i = 0; i < 3; i++)
	{
		PORTB = 0b00100000 >> i;
		_delay_us(10);
		if ((PINB & 0b01000000) != 0)
		{
			i = (i-1)*(-1);

			if (i != in)
			{
				if (i > -1)
				{
					if (check(i))
					{
						for (j = 0; j < 8; j++)
						{
							row[j][1] = (row[j][1] >> i) << (1 - i);
						}
					}
				}
				else
				{
					progress ();
				}
			
				in = i;
			}
			
			break;
		}
	}
	
	in = i;
}

void progress (void)
{
	char i;
	char j;
	
	if (check(2))
	{
		for (i = 7; i > 0; i--)
		{
			row[i][1] = row[i-1][1];
		}
		row[0][1] = 0;
	}
	else
	{
		for (i = 0; i < 8; i++)
		{
			row[i][0] = row[i][0] | row[i][1];
		}
		newpiece ();
	}
	
	
	for (i = 0; i < 8; i++)
	{
		if (row[i][0] == 0b00111111)
		{
			for (j = i; j > 0; j--)
			{
				row[j][0] = row[j-1][0];
			}
			
			row[0][0] = 0;
			
			score++;
		}
	}
}

void newgame (void)
{	
	in = 10;
	
	report ();
	
	score = 0;
	
	clr (0);
	clr (1);
}

char check (char s)
{
	char i;
	char a;
	char b;
	char c;
	char d;
	
	switch (s)
	{
		case 0:	a = 0;
			c = 0;
			d = 1;
			for (i = 0; i < 8; i++)
			{
				if ((row[i][1] & 0b00100000) != 0)
				{
					return false;
				}
			}
			break;
		case 1:	a = 1;
			c = 0;
			d = 0;
			for (i = 0; i < 8; i++)
			{
				if ((row[i][1] & 0b00000001) != 0)
				{
					return false;
				}
			}
			break;
		case 2:	a = 0;
			b = 6;
			c = 1;
			d = 0;
			if (row[7][1] != 0)
			{
				return false;
			}
			break;
		case 3:	a = 0;
			b = 6;
			c = 0;
			d = 0;
			break;
		default:a = s;
			b = s;
			c = s;
			d = s;
			break;
	}
	
	for (i = 0; i < 8; i++)
	{
		if ((row[i+c][0] & ((row[i][1] >> a) << d)) != 0)
		{
			return false;
		}
	}
	
	return true;
}

void animation (void)
{
	char i;
	int j;
	
	clr (0);
	
	for (i = 0; i < 8; i++)
	{
		row[i][0] = 0b00111111;
		
		for (j = 0; j < 200; j++)
		{
			screen();
		}
	}
	
	randnum ();
	
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			row[j][0] = row[j][0] - (0b00100000 >> i);
		}
		
		for (j = 0; j < 200; j++)
		{
			screen();
		}
	}
}

char randnum (void)
{	
	seed = (5 * seed + 41) % 53;
	
	return (9*seed/53);
}

void seedrand (void)
{
	seed = 42;
}

void newpiece (void)
{
	clr (1);
	
	switch (randnum())
	{
		case 0: row[0][1] = 0b00001100;
			row[1][1] = 0b00001000;
			break;
		case 1: row[0][1] = 0b00001100;
			row[1][1] = 0b00000100;
			break;
		case 2: row[0][1] = 0b00001000;
			row[1][1] = 0b00001100;
			break;
		case 3: row[0][1] = 0b00000100;
			row[1][1] = 0b00001100;
			randnum ();
			break;
		case 4: row[0][1] = 0b00011100;
			break;
		case 5: row[0][1] = 0b00001000;
			row[1][1] = 0b00001000;
			row[2][1] = 0b00001000;
			break;
		case 6: row[0][1] = 0b00000100;
			row[1][1] = 0b00000100;
			break;
		case 7: row[0][1] = 0b00011000;
			break;
		case 8: row[0][1] = 0b00001000;
			break;
		default: break;
	}
}

void report (void)
{
	int i;
	char s;
	
	clr (0);
	clr (1);
	
	s = score/10;

	for (i = 0; i < 6; i = i + 4)
	{
		switch (s)
		{
			case 0:	row[i][0]   = 0b00111110;
				row[i+1][0] = 0b00100010;
				row[i+2][0] = 0b00111110;
				break;
			case 1:	row[i+1][0] = 0b00111110;
				break;
			case 2:	row[i][0]   = 0b00111010;
				row[i+1][0] = 0b00101010;
				row[i+2][0] = 0b00101110;
				break;
			case 3:	row[i][0]   = 0b00100010;
				row[i+1][0] = 0b00101010;
				row[i+2][0] = 0b00111110;
				break;
			case 4:	row[i][0]   = 0b00001110;
				row[i+1][0] = 0b00001000;
				row[i+2][0] = 0b00111110;
				break;
			case 5:	row[i][0]   = 0b00101110;
				row[i+1][0] = 0b00101010;
				row[i+2][0] = 0b00111010;
				break;
			case 6:	row[i][0]   = 0b00111110;
				row[i+1][0] = 0b00101000;
				row[i+2][0] = 0b00111000;
				break;
			case 7:	row[i][0]   = 0b00000010;
				row[i+1][0] = 0b00000010;
				row[i+2][0] = 0b00111110;
				break;
			case 8:	row[i][0]   = 0b00111110;
				row[i+1][0] = 0b00101010;
				row[i+2][0] = 0b00111110;
				break;
			case 9:	row[i][0]   = 0b00001110;
				row[i+1][0] = 0b00001010;
				row[i+2][0] = 0b00111110;
				break;
			default:break;
		}
		
		s = score % 10;
	}
	
	row[7][0] = 0b00100000;

	for (i = 0; i < 1000; i++)
	{
		screen ();
	}
	
	clr (0);
	clr (1);
}

void clr (char r)
{
	char i;
	
	for (i = 0; i < 8; i++)
	{
		row[i][r] = 0;
	}
}
