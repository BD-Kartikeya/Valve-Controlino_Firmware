#include <Controllino.h>

long gating = 450;
int valveNumber=0;
int repetitions=1;
int tt=0;
int pneumaticNumber=0;
int pneumaticState=0;
int j=0;
int i=0;
int Check;
boolean dropShot=false;
byte inByte[4] = {B0, B0, B0, B0};
int dropGunStatus[4]={0,0,0,0};
boolean statePH[4]={false, false, false, false};

// Pins
const int ledPin = 13; // Led Pin
const int onOffPin = 77; // On/Off-Pin ( On if High )



const int valve[3] = {4, 5, 6};  //{PD4, PD5, PD6}; // DOD A,B,C UV C(PD6)
const int pneumExtr[2] = {A4, A5}; //Pneum. valve A,B

const int button_in[4] = {A0, A1, A2, A3}; //Digital Read Pins to check if button of potentially mounted DropGun was pressed
const int button_on[3] = {99,98,97}; //Digital Out Pins to set 5 V outputs on for potentially mounted DropGun 7/ ISEL hat keine DropGun Funktion


void setup() {

//Make Valve pins as out pins
  for (int i = 0; i < 3; i++) {
   pinMode(valve[i], OUTPUT);
  }

//Make Pneum pins as out pins
  for (int i = 0; i < 2; i++) {
   pinMode(pneumExtr[i], OUTPUT);
  }


  for (int i = 0; i < 4; i++) {
    pinMode(button_in[i], INPUT);
  }

   Serial.begin(9600);
   Serial.print("Valve On");
   pinMode (LED_BUILTIN, OUTPUT);

   Serial.begin(9600);
   Serial.print("Valve On");
   pinMode (LED_BUILTIN, OUTPUT);

 }

void turnOnUV() {
  digitalWrite(valve[2], HIGH); //was
  digitalWrite(LED_BUILTIN, HIGH);
}

void turnOffUV() {
    digitalWrite(valve[2], LOW);
    digitalWrite(LED_BUILTIN, LOW);
}

void readInpuPort (){

      for (int i = 0; i < 4; i++) {
        if (digitalRead(button_in[i])==HIGH) {
          digitalWrite(LED_BUILTIN, HIGH);
            if (statePH[i]==false) {
              Serial.print("XA:");
              Serial.print(i);
              Serial.print(":");
              Serial.print("Activated");
              Serial.println(";");
              statePH[i]=true;
            }
        }
        else if (digitalRead(button_in[i])==LOW){
          digitalWrite(LED_BUILTIN, LOW);
          if (statePH[i]==true) {
            Serial.print("XD:");
            Serial.print(i);
            Serial.print(":");
            Serial.print("Deactivated");
            Serial.println(";");
            statePH[i]=false;
          }
        }
    }
 }

 

void makeDrop() {

  //If the gating time is set to 9000 (here 24464) it means permant opening of the valve
  //For protection of DOD valves this command is fowarded in PH0 and PH1 to two distinct pins (D6 and D7), which activiate the pneumatic valves at the back of the printer
    if (gating==24464){
    digitalWrite(LED_BUILTIN, HIGH);
    if (valveNumber==0)
      digitalWrite(pneumExtr[valveNumber], HIGH); //Pneum A

    if (valveNumber==1)
      digitalWrite(pneumExtr[valveNumber], HIGH); //Pneum B

    if (valveNumber==2){
      turnOnUV();
    } 
  }

    if (gating==0){
    digitalWrite(LED_BUILTIN, LOW);
          for (int i = 0; i < 3; i++) {
          digitalWrite(valve[valveNumber], LOW);
          }
          for (int i = 0; i < 2; i++) {
          digitalWrite(pneumExtr[valveNumber], LOW);
          }
    }
    
    if (gating<=16000){
    digitalWrite(valve[valveNumber], HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delayMicroseconds(gating);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(valve[valveNumber], LOW);
    }
    
    else if (gating>16000 && gating!=24464){
    digitalWrite(valve[valveNumber], HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay((long)gating/1000);
    digitalWrite(valve[valveNumber], LOW);
    digitalWrite(LED_BUILTIN, LOW);
    }
    delay(5);
    Serial.print("D:");
    Serial.print(valveNumber);
    Serial.print(":");
    Serial.print(gating);
    Serial.println(";");
    
 }


void newPrintingSettings() {
  int first=0;
  int second =0;
   // Set new Printing Settings
  if (Check >= 4) {
        int deviceNumber = inByte[1];
        first = inByte[2];
        second = inByte[3];
        if (deviceNumber >= 0 && deviceNumber <= 3) valveNumber = deviceNumber;
        gating = (int)round(first*pow(10, second-1));
        /*Serial.print("S:");
        Serial.print(valveNumber);
        Serial.print(":");
        Serial.print(gating);
        Serial.println(";");
        */
        //delay(5);
  }
}


void newPneumaticSettings() {
 
  int first=0;
  int second =0;
   // Set new Printing Settings
  if (Check >= 2) {
        int deviceNumber = inByte[1];
        int state = inByte[2];
        if (deviceNumber >= 0 && deviceNumber <= 3) pneumaticNumber = deviceNumber;
        if (state >= 0 ) pneumaticState = state;

  }

  if(pneumaticState==0){
  digitalWrite(7, LOW); //Digital 3
  digitalWrite(8, LOW); //Digital 4
  digitalWrite(9, LOW); //Digital 5
  }

  if(pneumaticState==1){
  digitalWrite(7, HIGH); //Digital 3
  digitalWrite(8, LOW); //Digital 4
  digitalWrite(9, LOW); //Digital 5
  }

  if(pneumaticState==2){
  digitalWrite(7, LOW); //Digital 3
  digitalWrite(8, HIGH); //Digital 4
  digitalWrite(9, LOW); //Digital 5
  }

  if(pneumaticState==3){  //sent if Hotend is use, Valve=2 from controller, move all ZHUB up
  digitalWrite(7, LOW); //Digital 3
  digitalWrite(8, LOW); //Digital 4
  digitalWrite(9, LOW); //Digital 5
  }
  // I
  if(pneumaticState==4){  //since controller will send Pneumatic state=4 when Ph C is uded for UV/DOD
  digitalWrite(7, LOW); //Digital 3
  digitalWrite(8, LOW); //Digital 4
  digitalWrite(9, HIGH); //Digital 5
  }

    delay(2000);    //We could reduce this delay in future

    Serial.print("N:");
    Serial.print(pneumaticNumber);
    Serial.print(":");
    Serial.print(pneumaticState);
    Serial.println(";");

    
}


/*
void activateDropGun() {
  int first=0;
  int second =0;
   // Set new Printing Settings
  if (Check >= 4) {
        int deviceNumber = inByte[1];
        first = inByte[2];
        second = inByte[3];
        if (deviceNumber >= 0 && deviceNumber <= 1) valveNumber = deviceNumber;
        if (first>=1) gating=first;
        if (second>=1) repetitions=second;
        if (first>=1){
          dropGunStatus[valveNumber]=1;
          delay(5);
          digitalWrite(button_on[valveNumber],HIGH);
          Serial.print("G:");
          Serial.print(valveNumber);
          Serial.print(":");
          Serial.print("Activated");
          Serial.println(";");
          
        }
        if (first==0) {
          dropGunStatus[valveNumber]=0;
          delay(5);
          digitalWrite(button_on[valveNumber],LOW);
          Serial.print("G:");
          Serial.print(valveNumber);
          Serial.print(":");
          Serial.print("Deactivated");
          Serial.println(";");
        }
  }
}
*/


void checkSerial() {
  // Check if new Informations are at the port
  while (Serial.available() > 0) {
    Check = Serial.readBytesUntil('\n', inByte, 4);

    

    if (Check > 1) { // 1 byte is not enough information

      //stopPeltier();
      switch (inByte[0]) {

        //New Printing Settings & Printing Command
        case 'P': {
          newPrintingSettings();
          makeDrop();
          break;
      }

        //New Pneumatic Settings & Pneumatic Command
        case 'N': {
          newPneumaticSettings();
          break;
      }
      
      case 'G': {
          //activateDropGun();
          break;
      }
      case 'U': {
          turnOnUV();
          break;
      }
       case 'O': {
          turnOffUV();
          break;
      }
  }
}}}


void loop() {
 
  checkSerial();
  //readInpuPort(); commented out to disable buttons on the printers //26.11.2024, refer to roboabstractionlayer, search "XA" for button functionality
  
}
