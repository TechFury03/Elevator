#include <Wire.h>
#include <Keypad.h>

#define receiveDataArraySIZE 3
#define startAdres 10
#define eindAdres 13
#define motorOmhoog 12
#define motorOmlaag 11
#define buzzerPin 13

#define muziekG 391
#define muziekE 330
#define muziekC 261
bool toggleBool = true;   
  
int huidigeVerdieping, destinationVerdieping;

//Keypad
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  pinMode(motorOmhoog, OUTPUT);
  pinMode(motorOmlaag, OUTPUT);
  
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  checkKeypad();
  vraagVerdieping();
  stuurVerdieping();
  motor();
  delay (5); 
}

void vraagVerdieping(){
  char receiveDataArray[receiveDataArraySIZE];                //IRSensor|KnopOmhoog|KnopOmlaag

  //Vraag aan elke verdieping data
  for(int i = startAdres; i < eindAdres+1; i++){   
    Wire.requestFrom(i, receiveDataArraySIZE);                //vraag aan verdieping i en verwacht 3 bytes aan data
    bool IRsensor, knopOmhoog, knopOmlaag;
    Serial.print("Verdieping ");
    Serial.print(i);
    Serial.println("\nIRSensor | KnopOmhoog | KnopOmlaag");  
    while(Wire.available()){
      for(int j = 0; j < receiveDataArraySIZE; j++){
        receiveDataArray[j] = Wire.read();
        Serial.print(receiveDataArray[j]);
        Serial.print(" | ");
      }     
      Serial.print("\n");   
      
      if(receiveDataArray[0] == '0'){                                    //IR neemt iets waar
        IRsensor = true;
        huidigeVerdieping = i - startAdres;                              //adressen starten op 10
      }
      if(receiveDataArray[1] == '0'){
        knopOmhoog = true;
        destinationVerdieping = i - startAdres;                         //adressen starten op 10
      }
      if(receiveDataArray[2] == '0'){
        knopOmlaag = true; 
        destinationVerdieping = i - startAdres;                         //adressen starten op 10
      }   
    }
    Serial.print("Destination verdieping: ");
    Serial.print(destinationVerdieping);
    Serial.print("  Huidige verdieping: ");
    Serial.print(huidigeVerdieping);
    Serial.print("\n");    
  } 
}

void stuurVerdieping(){
  //Stuur dataArray naar alle verdiepingen
  for(int i = startAdres; i <= eindAdres; i++){                               
    Wire.beginTransmission(i);
    Wire.write(huidigeVerdieping);
    Wire.endTransmission();
  }
}

void checkKeypad(){
  char customKey = customKeypad.getKey();
  if ((customKey) && ((customKey - '0') <= (eindAdres-10))){
    Serial.println("keypad key:");
    Serial.println(customKey);
    destinationVerdieping = customKey - '0';                //om van char naar int te gaan
  }
}

void motor(){
  bool richting;                                             //true = omhoog, false = omlaag
  bool stilHangen = false;
  bool muziek = false;   
  
  //bepaal richting
  if(destinationVerdieping > huidigeVerdieping){            //omhoog
      richting = true;
  } else if (destinationVerdieping < huidigeVerdieping){    //omlaag
      richting = false;
  } else {
      stilHangen = true;
  }
  
  if(stilHangen){
    digitalWrite(motorOmhoog, LOW);
    digitalWrite(motorOmlaag, LOW);
    muziek = true;   
  }

  //speel muziekje
  if((muziek)&& (toggleBool)){
    tone(buzzerPin,muziekG, 500);
    delay(550);
    tone(buzzerPin,muziekE, 500);
    delay(550);
    tone(buzzerPin,muziekC, 500);
    delay(550);
    toggleBool = false;
  }

  //zet motor aan
  if((richting)&& (!stilHangen)){
    Serial.print("Motor omhoog\n");
    digitalWrite(motorOmhoog, HIGH);
    analogWrite(motorOmlaag, 0);
    toggleBool = true;
  } else if (!stilHangen){
    Serial.print("Motor omlaag\n");
    digitalWrite(motorOmhoog, LOW);
    analogWrite(motorOmlaag, 100);
    toggleBool = true;
  }
}
