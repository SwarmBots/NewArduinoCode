
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define MDATA 5
#define MCLK 6
//Clock is white, data is orange.
//Power is blue and ground is green
//#define LeftMotorOne 3
//#define LeftMotorTwo 4
//#define RightMotorOne 7
//#define RightMotorTwo 8

int totalX = 0;
int totalY = 0;
int leftRemaining = 0;
int rightRemaining = 0;
int source = 0; //First bot
int bot = 4; //t means that this is a bot, f for computer

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

typedef enum { role_ping_out = 1, role_pong_back } role_e;

role_e role = role_pong_back;
void gohi(int pin)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

void golo(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void mouse_write(char data)
{
  char i;
  char parity = 1;
  gohi(MDATA);
  gohi(MCLK);
  delayMicroseconds(300);
  golo(MCLK);
  delayMicroseconds(300);
  golo(MDATA);
  delayMicroseconds(10);
  gohi(MCLK);
  while (digitalRead(MCLK) == HIGH)
    ;
  for (i=0; i < 8; i++) {
    if (data & 0x01) {
      gohi(MDATA);
    } 
    else {
      golo(MDATA);
    }
    while (digitalRead(MCLK) == LOW)
      ;
    while (digitalRead(MCLK) == HIGH)
      ;
    parity = parity ^ (data & 0x01);
    data = data >> 1;
  }  
  if (parity) {
    gohi(MDATA);
  } 
  else {
    golo(MDATA);
  }
  while (digitalRead(MCLK) == LOW)
    ;
  while (digitalRead(MCLK) == HIGH)
    ;
  gohi(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH)
    ;
  while ((digitalRead(MCLK) == LOW) || (digitalRead(MDATA) == LOW))
    ;
  golo(MCLK);
}

char mouse_read(void)
{
  char data = 0x00;
  int i;
  char bit = 0x01;

  //  Serial.print("reading byte from mouse\n");
  /* start the clock */
  gohi(MCLK);
  gohi(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH)
    ;
  delayMicroseconds(5);  /* not sure why */
  while (digitalRead(MCLK) == LOW) /* eat start bit */
    ;
  for (i=0; i < 8; i++) {
    while (digitalRead(MCLK) == HIGH)
      ;
    if (digitalRead(MDATA) == HIGH) {
      data = data | bit;
    }
    while (digitalRead(MCLK) == LOW)
      ;
    bit = bit << 1;
  }
  /* eat parity bit, which we ignore */
  while (digitalRead(MCLK) == HIGH)
    ;
  while (digitalRead(MCLK) == LOW)
    ;
  /* eat stop bit */
  while (digitalRead(MCLK) == HIGH)
    ;
  while (digitalRead(MCLK) == LOW)
    ;

  golo(MCLK);
  return data;
}

void mouse_init()
{
  gohi(MCLK);
  gohi(MDATA);
  mouse_write(0xff);
  mouse_read(); 
  mouse_read();  
  mouse_read();  
  mouse_write(0xf0); 
  mouse_read(); 
}

void setup(void)
{
  Serial.begin(57600);
  mouse_init();

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  role = role_pong_back; //should be   initialized to listen to master
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();

}

void loop(void)
{
  char mouseX;
  char mouseY;
  
  mouse_write(0xeb);
  mouse_read(); //Throw away acknoledgement of data reciept
  mouse_read(); //Throw away button and wheel info
  mouseX = mouse_read();
  mouseY = mouse_read();
  totalX = totalX + int(mouseX);
  totalY = totalY + int(mouseY);

  if (radio.available()){
    unsigned long recievedInfo;
    bool done = false;
    while (!done){
      done = radio.read(&recievedInfo, sizeof(unsigned long));
      //Wait for the hub to switch back
      delay(10);
    }
    radio.stopListening();
    
    int mouseInfo[4] = {source, bot, totalX, totalY};
    radio.write(&mouseInfo, sizeof(int[4]));
    
    radio.startListening();
  }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
