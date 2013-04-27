const int motorPin1 = 11;
const int motorPin2 = 10;
const int enable = 3;
const int toggle = 13;

int toggleState = 0;

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(toggle, INPUT);
  digitalWrite(enable, HIGH);
}

void loop() {
  toggleState = digitalRead(toggle);
  if (toggleState == HIGH){
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  }else if(toggleState == LOW){
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin1, LOW);
  }
}

  

