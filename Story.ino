#include <Wire.h>
#define verdiepingAdres 10

#define IRSensor 5
#define KnopOmlaag 3
#define KnopOmhoog 9
#define KnopOmhoogLED 2
#define KnopOmlaagLED 10

#define segmentDataPin 4
#define segmentLatchPin 7
#define segmentClockPin 8

#define roodLED 6
#define groenLED 11

byte symbol, symbols[] {
B11111100, // 0
B01100000, // 1
B11011010, // 2
B11110010, // 3
B01100110, // 4
B10110110, // 5
B10111110, // 6
B11100000, // 7
B11111110, // 8
B11110110, // 9
B11101110, // A
B00111110, // B
B10011100, // C
B01111010, // D
B10011110, // E
B10001110  // F
};

int huidigeVerdieping;
bool knopOmhoogLEDstand, knopOmlaagLEDstand, IRstand;

void setup() {
  pinMode(IRSensor,INPUT);
  pinMode(KnopOmlaag,INPUT);
  pinMode(KnopOmhoog,INPUT);
  pinMode(KnopOmhoogLED,OUTPUT);
  pinMode(KnopOmlaagLED, OUTPUT);
  pinMode(segmentDataPin,OUTPUT);
  pinMode(segmentLatchPin, OUTPUT);
  pinMode(segmentClockPin, OUTPUT);

  Wire.begin(verdiepingAdres);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  Serial.begin(9600);

  knopOmhoogLEDstand = false;
  knopOmlaagLEDstand = false;
  //huidigeVerdieping = 5;
}

void loop() {
  
  /*Serial.print("\nKnopOmlaag = ");
  Serial.print(digitalRead(KnopOmlaag));
  Serial.print("  |  KnopOmhoog = ");
  Serial.print(digitalRead(KnopOmhoog));
  Serial.print("  |  IRsensor = ");
  Serial.print(digitalRead(IRSensor));
  */
  if(digitalRead(KnopOmhoog) == LOW){
    digitalWrite(KnopOmhoogLED,HIGH);
    knopOmhoogLEDstand = true;
  }
  
  if(digitalRead(KnopOmlaag) == LOW){
    digitalWrite(KnopOmlaagLED,HIGH);
    knopOmlaagLEDstand = true;    
  }
  
  if(digitalRead(IRSensor) == LOW){
    digitalWrite(KnopOmlaagLED,LOW);
    digitalWrite(KnopOmhoogLED,LOW);
    analogWrite(roodLED,0);         //analog want pwm pin
    analogWrite(groenLED,255);      //analog want pwm pin
    knopOmhoogLEDstand = false;
    knopOmlaagLEDstand = false;
    IRstand = true;
  } else {
    analogWrite(roodLED,255);       //analog want pwm pin
    analogWrite(groenLED,0);        //analog want pwm pin
    IRstand = false;
  } 
}

void segmentDisplay(){
  symbol = symbols[huidigeVerdieping];
  digitalWrite(segmentLatchPin,LOW);
  shiftOut(segmentDataPin, segmentClockPin, LSBFIRST, symbol);
  digitalWrite(segmentLatchPin,HIGH);
}

void requestEvent(){
  char dataArray[3]; //IRSensor|KnopOmhoog|KnopOmlaag

  if(IRstand){
    dataArray[0] = '0';
    //Serial.print("IRsensor neemt iets waar    ");
  } else {
    dataArray[0] = '1';
  }
  
  if(knopOmhoogLEDstand){
    dataArray[1] = '0';
    //Serial.print("Omhoogknop ingedrukt    ");   
  } else {
    dataArray[1] = '1';
  }
  
  if(knopOmlaagLEDstand){
    dataArray[2] = '0';
    //Serial.print("Omlaagknop ingedrukt    ");
  } else {
    dataArray[2] = '1';
  }

  Wire.write(dataArray);
}
 
void receiveEvent(int howMany){   
    huidigeVerdieping = Wire.read();
    Serial.print("Huidige verdieping: ");
    Serial.print(huidigeVerdieping);
    Serial.print("\n");
    segmentDisplay();
}
