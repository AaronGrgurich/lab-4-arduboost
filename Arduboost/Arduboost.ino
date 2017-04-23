int pwmPin = 6;
int potPin = 0;
int fbPin = 1;

int latchPin = 4;
int serPin = 5;
int clkPin = 7;
int clrPin = 8;

int fbTarget = 100;
int maxTarget = 500;
int maxPwm = 120;

int currentIndex = 0;
byte currentSeg[4];

int currentPwm = 0;

int digpins[4] = {12,11,10,9};

float num2write = 0;

long lastMill = 0;

byte mapseg[36] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11100110, // 9
  0b11101110, // A
  0b00111110, // b
  0b10011100, // C
  0b01111010, // d
  0b10011110, // E
  0b10001110, // F
  0b11110110, // g
  0b01101110, // H
  0b00001100, // I
  0b01111000, // J
  0b01101110, // K
  0b00011100, // L
  0b10101000, // M
  0b00101010, // N
  0b11111100, // O
  0b11001110, // P
  0b11100110, // Q
  0b00001010, // R
  0b10110110, // S
  0b00011110, // t
  0b01111100, // U
  0b00111000, // v
  0b01010100, // W
  0b01101110, // X
  0b01110110, // Y
  0b11011010 // Z
};

void setup() {
  // put your setup code here, to run once:
  initializePwm();
  pinMode(latchPin,OUTPUT);
  digitalWrite(latchPin,HIGH);
  pinMode(serPin,OUTPUT);
  digitalWrite(serPin,LOW);
  pinMode(clrPin,OUTPUT);
  digitalWrite(clrPin,HIGH);
  pinMode(clkPin,OUTPUT);
  digitalWrite(clkPin,LOW);
  for(int i = 0; i < 4; i++) {
    pinMode(digpins[0],OUTPUT);
    digitalWrite(digpins[0],LOW);
  }
  Serial.begin(9600);
  delay(500);
  lastMill = millis();
}

void loop() {
  // Changing the pwm freq changes millis
  // This changes roughly 5 times a second
  if(millis() - lastMill > 10000) {
    num2write = analogRead(fbPin)*5.0/1024*7.8333;
    writeNumber(num2write);
    lastMill = millis();
  }
  updateDisplay();
  if(analogRead(fbPin) < analogRead(potPin)-30) {
    setPwm(currentPwm+1);
  } else if(analogRead(fbPin) > analogRead(potPin)+30) {
    setPwm(currentPwm-1);
  }
}

// Changes the pwm frequency of pin 6
void initializePwm() {
  byte mode = 0x01;
  TCCR0B = TCCR0B & 0b11111000 | mode;
  analogWrite(pwmPin,0);
}

// Writes a word of characters to the display
void writeWord(char* w) {
  for(int i = 0; i < 4; i++) {
    int j = i;
    currentSeg[i] = mapseg[char2index(w[i])];
  }
}

// Writes a float to the display
void writeNumber(float f) {
  String num = String(f);
  bool decimal = 0;
  for(int i = 0; i < 4; i++) {
    int j = i+decimal;
    currentSeg[i] = mapseg[char2index(num.charAt(j))];
    if(num.charAt(i+1)=='.') {
      decimal = 1;
      currentSeg[i] |= 0x01;
    }
  }
}

// Writes one character to the display
void writeCharacter(char c, int ind) {
  byte b = mapseg[char2index(c)];
  writeSegment(b,ind);
}

// Writes one segment to the display
void writeSegment(byte b, int ind) {
  for(int i = 0; i < 4; i++) {
    if(i!=ind) {
      digitalWrite(digpins[i],LOW);
    }
  }
  digitalWrite(clrPin,LOW);
  digitalWrite(clrPin,HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(serPin, clkPin, LSBFIRST, b);
  digitalWrite(latchPin,HIGH);
  digitalWrite(digpins[ind],HIGH);
}

// Refreshes the display, needed to display all 4 digits
void updateDisplay() {
  currentIndex++;
  if(currentIndex>3) {
    currentIndex=0;
  }
  writeSegment(currentSeg[currentIndex],currentIndex);
}

// Converts a character to the index of mapseg
int char2index(char c) {
  if(c>=48 && c<=57) {
    return c-48;
  } else if(c>=65 && c<=90) {
    return c-65+10;
  } else if(c>=97 && c<=122) {
    return c-97+10;
  }
}

// Safe sets the PWM frequency to never go above the maximum
void setPwm(int pwm) {
  currentPwm = min(max(pwm,0),maxPwm);
  analogWrite(pwmPin,currentPwm);
}

