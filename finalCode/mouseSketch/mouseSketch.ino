#define MDATA 5
#define MCLK 6
//Clock is white, data is orange.
//Power is blue and ground is green

int totalX = 0;
int totalY = 0;

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
  delayMicroseconds(100);
}

void setup()
{
  Serial.begin(9600);
  mouse_init();
}

void loop()
{
  char mstat;
  char mx;
  char my;

  /* get a reading from the mouse */
  mouse_write(0xeb);  /* give me data! */
  mouse_read();      /* ignore ack */
  mstat = mouse_read();
  mx = mouse_read();
  my = mouse_read();
  totalX = totalX + int(mx);
  totalY = totalY+int(my);
  Serial.print("X: ");
  Serial.print(totalX, DEC);
  Serial.print("\tY: ");
  Serial.println(totalY, DEC);
  /* send the data back up */
  //Serial.print(mstat, BIN);
  //Serial.print("\tX=");
  //Serial.print(mx, DEC);
  //Serial.print("\tY=");
  //Serial.print(my, DEC);
  //Serial.println();
  delay(20);
}
