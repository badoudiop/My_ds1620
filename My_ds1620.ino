#include <Arduino.h>
#include <TM1637Display.h>

#include "ds1620.h"



// Module connection pins (Digital Pins)
#define CLK 6
#define DIO 5

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

TM1637Display display(CLK, DIO);

int y,t , temp1,temp2,dizaine,diz , unit , decimale,deci1,deci2,mama;// ne pas utiliser uint8_t , ça ne marche pas .

void setup()
{
Serial.begin(9600);

  y=0;t=0;

pinMode(spi_clk,OUTPUT);
pinMode(spi_reset,OUTPUT);

	start();
	SPI_init();
	pause(200);
 display.setBrightness(0x0f);


}
void loop()
{
  uint8_t data[]={1,2,3,4};
	charger();
   
   t=(y&0x01)*5;
decimale=t;

  dizaine=y/2;
	  diz=dizaine/10;
unit=dizaine%10;
//******************
  data[0] = display.encodeDigit(diz);// uint8_t data[]={diz,unit,0b1100011,t)};ne marche pas , les chiffres sont mal formé ; 
  data[1] =  display.encodeDigit(unit);
  data[2] = 0b1100011;//pour les degrés ne marche pas avec encodeDigit car on obtient un nombre au lieu du symbole degré.
  data[3] = display.encodeDigit(t);
  display.setSegments(data);
display.setBrightness(0x0f);
  delay(TEST_DELAY);

//*************************
/*
uint8_t data[] ={diz,unit,0b01100011,t};
display.setSegments(data);
display.setBrightness(0x0f);
*/
//delay(TEST_DELAY);
Serial.println(dizaine);
Serial.println(diz);
Serial.println(unit);
Serial.println(t);

    _delay_ms(500);
}


void do_haut(void)
{
pinMode(spi_do,OUTPUT);
digitalWrite(spi_do,HIGH);
pause(5);
}
	
void do_bas(void)
{
pinMode(spi_do,OUTPUT);
digitalWrite(spi_do,LOW);
pause(5);
}

void clock(void)
{
digitalWrite(spi_clk,LOW);
pause(5);
digitalWrite(spi_clk,HIGH);
pause(5);
}

void start(void){
digitalWrite(spi_reset,HIGH);
pause(5);
//****************
}

	void stop(void)
	{
digitalWrite(spi_reset,LOW);
pause(5);
	}


void SPI_init	(void)
{
	start();
//;config command protocole 	
	putbyte(0x0C);

//;output polarity active hight + conversion non CONTINUE : one bit shot 
	putbyte(0x03);
	stop();
	//pause(200);

}

void pause(unsigned int j)
{
while(j--) _delay_us(1);
}

void charger(void)
{
deb_convert();
	mesure();
	fin_convert();
	}

void deb_convert(void)
{

	start();
	putbyte(0xEE);
	pause(200);
	stop();
	
}

void fin_convert(void)
{	
	start();
putbyte(0x22);
	pause(200);
stop();
}

void mesure(void)
{	
uint8_t z=0;
	start();
	putbyte(0xAA);
  _delay_ms(750);
	y= getbyte();
 z=getbyte();
 temp1=y;
	stop();	
	
	//return y;
	}


void putbyte (unsigned char x)
{


if(x&0x01) do_haut(); 
else do_bas();
clock();

if(x&0x02) do_haut(); 
else do_bas();
clock();

if(x&0x04) do_haut(); 
else do_bas();
clock();

if(x&0x08) do_haut(); 
else do_bas();
clock();

if(x&0x10) do_haut(); 
else do_bas();
clock();

if(x&0x20) do_haut(); 
else do_bas();
clock();

if(x&0x40) do_haut(); 
else do_bas();
clock();

if(x&0x80) do_haut(); 
else do_bas();
clock();


}

unsigned int getbyte(void)
{
unsigned int x=0;
pinMode(spi_do,INPUT);
clock();
for(int i=0;i<8;i++){if((digitalRead(spi_do))&0x01) x|=(1<<i);clock();}
return x;
}
//*************************************
uint8_t read_slope()//=counter_per_c
{
  uint8_t counter=0,counter1=0;
start();
putbyte(0xA9);
counter=getbyte();
counter1=getbyte();
stop();
return counter1*16+counter;
}

uint8_t read_counter()//=counter_remain
{
  uint8_t counter=0,counter1=0;
start();
putbyte(0xA0);
counter=getbyte();
counter1=getbyte();
stop();
return counter1*16+counter;
}


float realtemp()
{
  float latemp=0;
charger();
latemp=temp1/2-0.25+(read_slope()-read_counter())/(float)read_slope();
return latemp;
}