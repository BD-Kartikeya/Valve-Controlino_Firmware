//Firmware flashed to minho printer 05.12.24
//peltier coolind is set to LOW LOW instead of HIGH HIGH for safety, will be updated in future
//upper limnit of 61° added 

#include <OneWire.h>

// Global variables

byte portCounter = 0;    // Going through sensor ports
byte deviceCounter = 0;  // Going through devices on each port

// TemperatureSensorPorts
const byte portMaxNumber = 4;  // Maxmimum number of ports

OneWire ds(12);   //changed from A4 to 12(PB4)(MISO)
OneWire ds2(11);  //changed from A5 to 11 (PB3)(MOSI)
OneWire ds3(10);  //changed from 99 to 10(/SS)
OneWire ds4(88);  //not in use
OneWire ALLDS[portMaxNumber] = { ds, ds2, ds3, ds4 };


double curTempBuffer[portMaxNumber][4];  // Current Temperature
double curTemp[portMaxNumber];           // Current Temperature
double refTemp[portMaxNumber];           // Reference Temperature

byte inByte[4] = { B0, B0, B0, B0 };
// local variables
byte i, type_s;
byte present = 0;
byte data[12];
byte addr[8];
float celsius;
int Check;
int device = 0;
int tt = 0;
int j = 0;
int ii = 0;

int roomTemp = 40;  //Achtung wird und noch 5 addiert, auf 45 °C
//int status_A =0; //0 = NOTACTIVE, 1 = ACTIVE, 2 = HEAT, 3 = STOP, 4 = COOL
//int status_B =0; //0 = NOTACTIVE, 1 = ACTIVE, 2 = HEAT, 3 = STOP, 4 = COOL

int statusPeltier[4] = { 0, 0, 0, 0 };
int peltier[4][2] = { { 4, 8 }, { 5, 9 }, { 6, A4 }, { 7, A5 } };



// Pins
const int ledPin = 13;    // Led Pin
const int onOffPin = 77;  // On/Off-Pin ( On if High )
//const int peltier_A[2]={4, 8};
//const int peltier_B[2]={5, 9};
//const int peltier_C[2]={6, A4};
//const int peltier_D[2]={7, A5};
boolean activedPeltier[portMaxNumber] = { false, false, false, false };
boolean anyDevice[portMaxNumber] = { false, false, false, false };  // If there are any devices on one port
const double threshold = 0.5;

void setup() {

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      pinMode(peltier[i][j], OUTPUT);
    }
  }

  pinMode(A0, INPUT);

  stopPeltiers();
  pinMode(ledPin, OUTPUT);
  pinMode(onOffPin, INPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
  Serial.print("START:");
  Serial.print(portMaxNumber);
  Serial.println(";");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < portMaxNumber; j++) curTempBuffer[j][i] = 25;
  }
  for (int i = 0; i < 4; i++) {
    refTemp[i] = 36;              // As Starting refTemp
    curTemp[portMaxNumber] = 25;  // As Starting Temp
  }
  getTemp();
}


void stopPeltiers() {
  for (int j = 0; j < 4; j++) {
    stopPeltier(j);
  }
}

void giveCurTemp(int portNumber) {
  if (portNumber < 4) {
    if (anyDevice[portNumber]) {
      Serial.print("TEMP:");
      Serial.print(portNumber);
      Serial.print(":");
      Serial.print(curTemp[portNumber]);
      Serial.println(";");
    } else {
      Serial.print("TEMP:");
      Serial.print(portNumber);
      Serial.println(":E;");
    }
  }
  delay(5);
}

void giveStatus(int portNumber) {
  Serial.print("ACTIVE:");
  Serial.print(portNumber);
  Serial.print(":");
  Serial.print(activedPeltier[portNumber]);
  Serial.println(";");
}

void giveRef(int portNumber) {
  Serial.print("REFT:");
  Serial.print(portNumber);
  Serial.print(":");
  Serial.print(refTemp[portNumber]);
  Serial.println(";");
}

void deactivatePeltier(int printhead) {
  Serial.print("MSG:");
  Serial.print("Deactivate:");
  switch (printhead) {
    case 0:
      Serial.print("A");
      break;
    case 1:
      Serial.print("B");
      break;
    case 2:
      Serial.print("C");
      break;
    case 3:
      Serial.print("D");
      break;
  }
  Serial.println(";");
  digitalWrite(peltier[printhead][0], LOW);
  digitalWrite(peltier[printhead][1], LOW);
  statusPeltier[printhead] = 0;
}

void heatPeltier(int printhead) {
  Serial.print("INCR:");  //INCREASE wird nicht erkannt, evtl wegen der Reihung CRE
  Serial.print(printhead);
  Serial.print(":");
  Serial.print(refTemp[printhead]);
  Serial.println(";");
  digitalWrite(peltier[printhead][0], HIGH);
  digitalWrite(peltier[printhead][1], LOW);
  statusPeltier[printhead] = 2;
}

void stopPeltier(int printhead) {
  digitalWrite(peltier[printhead][0], LOW);
  digitalWrite(peltier[printhead][1], LOW);
  Serial.print("MSG:");
  switch (printhead) {
    case 0:
      Serial.print("Stop A");
      break;
    case 1:
      Serial.print("Stop B");
      break;
    case 2:
      Serial.print("Stop C");
      break;
    case 3:
      Serial.print("Stop D");
      break;
  }
  Serial.println(";");
  statusPeltier[printhead] = 3;
}

void coolPeltier(int printhead) {
  digitalWrite(peltier[printhead][0], LOW);
  digitalWrite(peltier[printhead][1], LOW);
  Serial.print("DECR:");
  Serial.print(printhead);
  Serial.print(":");
  Serial.print(refTemp[printhead]);
  Serial.println(";");
  statusPeltier[printhead] = 4;
}




void readTemp() {
  if (Serial.available() > 0) {
    int portNumber = inByte[1];
    // Give out the Temp
    if (portNumber < 4) {
      giveCurTemp(portNumber);
    }
    if (portNumber == 'A') {
      for (int i = 0; i < 4; i++) {
        giveCurTemp(i);
      }
    }
  }
}

void setTemp() {
  // Set Temp
  //     Serial.println("Case S");
  if (Check < 4) goto errorTemp;
  {
    if (Serial.available() > 0) {
      // Save new Informations
      //    inByte = 0;
      //    inByte = Serial.read();
      int deviceNumber = inByte[1];
      if (Serial.available() > 0) {
        // Save new Informations
        //    inByte = 0;
        //    inByte = Serial.read();
        int first = inByte[2] & 0xFF;
        if (first > 127) {
          first -= 256;
        }
        if (Serial.available() > 0) {
          // Save new Informations
          //    inByte = 0;
          //    inByte = Serial.read();
          int second = inByte[3] & 0xFF;
          if (second > 127) {
            second -= 256;
          }
          if (deviceNumber >= 0 && deviceNumber <= 4) {
            refTemp[deviceNumber] = ((double)first) + (((double)second) / 128);
            Serial.print("RECID:");
            Serial.print(deviceNumber);
            Serial.println(";");
            Serial.print("RECIT1:");
            Serial.print(first);
            Serial.println(";");
            Serial.print("RECIT2:");
            Serial.print(second);
            Serial.println(";");
            giveRef(deviceNumber);
          }
          if (deviceNumber == 'A') {
            Serial.print("RECID:");
            Serial.println("A;");
            Serial.print("RECIT1:");
            Serial.print(first);
            Serial.println(";");
            Serial.print("RECIT2:");
            Serial.print(second);
            Serial.println(";");
            for (int i = 0; i < portMaxNumber; i++) {
              refTemp[i] = ((double)first) + ((double)second / 128);
              giveRef(i);
            }
          }
        }
      }
    }
  }
errorTemp:;
  for (int i = 0; i < portMaxNumber; i++) {
    giveCurTemp(i);
  }
}


void activatePeltiers() {
  if ((inByte[1] >= 0) && (inByte[1] <= 3)) {
    activedPeltier[inByte[1]] = true;
    giveStatus(inByte[1]);
  }
  if (inByte[1] == 'A') {
    for (int i = 0; i < portMaxNumber; i++) {
      activedPeltier[i] = true;
      giveStatus(i);
    }
  }
}

void deactivatePeltiers() {
  if ((inByte[1] >= 0) && (inByte[1] <= 3)) {
    activedPeltier[inByte[1]] = false;
    giveStatus(inByte[1]);
  }
  if (inByte[1] == 'A') {
    for (int i = 0; i < portMaxNumber; i++) {
      activedPeltier[i] = false;
      giveStatus(i);
    }
  }
}

void checkSerial() {

  // Check if new Informations are at the port
  while (Serial.available() > 0) {
    Check = Serial.readBytesUntil('\n', inByte, 4);

    if (Check > 1 && inByte[0] == 'P') {
      //stopPeltier();
      //makeDrops();
    } else if (Check > 1) {  // 1 byte is not enough information
      //stopPeltier();
      switch (inByte[0]) {

        // Read Temp
        case 'R':
          readTemp();
          break;

        // Set up Temp
        case 'S':
          setTemp();
          break;

        // Activate Peltierssl
        case 'T':
          activatePeltiers();
          break;

        // Deactivate Peltiers
        case 'F':
          {
            deactivatePeltiers();
            //Serial.println("Test");
            break;
          }

        //New Printing Settings
        case 'N':
          {
            //newPrintingSettings();
            break;
          }

        //Printing Command
        case 'P':  //printingCommand();
          break;
      }
    }
  }
}

void getTemp() {
  // Read the Temps and save them into curTemp[portMaxNumber]
  // in case of multiple sensors take average
  while (portCounter < portMaxNumber) {
next:;
    // Serial.print("deviceCounter= ");
    // Serial.print(deviceCounter);
    // Serial.print("portCounter= ");
    // Serial.println(portCounter);
    if ((!ALLDS[portCounter].search(addr)) || (deviceCounter > 3)) {
      //   Serial.print("PORTEND:");
      //   Serial.print(portCounter);
      //   Serial.println(";");
      if (deviceCounter == 0) {

        curTemp[portCounter] = refTemp[portCounter];
        anyDevice[portCounter] = false;
      } else {
        curTemp[portCounter] = 0;
        anyDevice[portCounter] = true;
      }
      for (int j = 0; j < deviceCounter; j++) {
        curTemp[portCounter] += curTempBuffer[portCounter][j];
      }
      if (!(deviceCounter == 0)) curTemp[portCounter] /= deviceCounter;
      deviceCounter = 0;
      ALLDS[portCounter].reset_search();
      portCounter++;
      //delay(100);
      for (i = 0; i < 10; i++) {
        checkSerial();
        delay(10);
      }

      continue;
    }
    if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("ERROR:CRC;");
      goto next;
    }
    // Reading out the Sensors
    switch (addr[0]) {
      case 0x10:
        //Serial.println("  Chip = DS18S20");  // or old DS1820
        type_s = 1;
        break;
      case 0x28:
        //Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
      case 0x22:
        //Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
      default:
        //Serial.println("ERROR:DEVICE;");
        continue;
    }
    digitalWrite(ledPin, HIGH);
    ALLDS[portCounter].reset();
    ALLDS[portCounter].select(addr);
    ALLDS[portCounter].write(0x44, 1);  // start conversion, with parasite power on at the end
                                        // delay(1000);  // maybe 750ms is enough, maybe not
    for (i = 0; i < 100; i++) {
      checkSerial();
      delay(10);
    }  //AB ergänzt und entfernt 26.3.2018
    present = ALLDS[portCounter].reset();
    ALLDS[portCounter].select(addr);
    ALLDS[portCounter].write(0xBE);
    for (i = 0; i < 9; i++) {  // we need 9 bytes
      data[i] = ALLDS[portCounter].read();
    }
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3;         // 9 bit resolution default
      if (data[7] == 0x10) {  // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {  // just in case someday some1 wants other DS18XXXX sensors to be used
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;       // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3;  // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1;  // 11 bit res, 375 ms
      // default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    curTempBuffer[portCounter][deviceCounter] = (double)celsius;
    digitalWrite(ledPin, LOW);
    deviceCounter++;
  }

  // Compare to Set Temp
  for (j = 0; j < 4; j++) {
    //if (tt>=4) tt=0;
    //i=tt;
    //stopPeltier();
    if (activedPeltier[j]) {

      if (curTemp[j] > 61) {  //hard limit to stop heating
        stopPeltier(j);
      }

      if (j == 0 && statusPeltier[j] == 0) {
        statusPeltier[j] = 1;
        Serial.print("MSG:");
        Serial.print("Activate Temperature Control: ");
        Serial.print("A");
        Serial.println(";");
      }
      if (j == 1 && statusPeltier[j] == 0) {
        statusPeltier[j] = 1;
        Serial.print("MSG:");
        Serial.print("Activate Temperature Control: ");
        Serial.print("B");
        Serial.println(";");
      }
      if (j == 2 && statusPeltier[j] == 0) {
        statusPeltier[j] = 1;
        Serial.print("MSG:");
        Serial.print("Activate Temperature Control: ");
        Serial.print("C");
        Serial.println(";");
      }
      if (j == 3 && statusPeltier[j] == 0) {
        statusPeltier[j] = 1;
        Serial.print("MSG:");
        Serial.print("Activate Temperature Control: ");
        Serial.print("D");
        Serial.println(";");
      }

      double diff = curTemp[j] - refTemp[j];
      if (diff < -threshold && curTemp[j] < 61) {
        if (j == 0 && statusPeltier[j] != 2) heatPeltier(j);
        if (j == 1 && statusPeltier[j] != 2) heatPeltier(j);
        if (j == 2 && statusPeltier[j] != 2) heatPeltier(j);
        if (j == 3 && statusPeltier[j] != 2) heatPeltier(j);
      } else if (diff > threshold) {  //Temperatur liegt oberhalb Soll-Temperatur
        //Heizung wird gestoppt
        if (refTemp[j] >= roomTemp) {
          if (j == 0 && statusPeltier[j] != 3) stopPeltier(j);
          if (j == 1 && statusPeltier[j] != 3) stopPeltier(j);
          if (j == 2 && statusPeltier[j] != 3) stopPeltier(j);
          if (j == 3 && statusPeltier[j] != 3) stopPeltier(j);
        }
        //Sicherstellen, dass Kühlfunktion nicht überstrapaziert wird
        if (refTemp[j] < roomTemp && curTemp[j] < roomTemp + 5) {  //make sure current temp is less than 55
          if (j == 0 && statusPeltier[j] != 4) coolPeltier(j);
          if (j == 1 && statusPeltier[j] != 4) coolPeltier(j);
          if (j == 2 && statusPeltier[j] != 4) coolPeltier(j);
          if (j == 3 && statusPeltier[j] != 4) coolPeltier(j);
        }

      } else if (diff < threshold && diff > -threshold) {
        Serial.print("NOC:");  //NOCHANGE
        Serial.print(j);
        Serial.println(";");
        if (j == 0 && statusPeltier[j] != 3) stopPeltier(j);
        if (j == 1 && statusPeltier[j] != 3) stopPeltier(j);
        if (j == 2 && statusPeltier[j] != 3) stopPeltier(j);
        if (j == 3 && statusPeltier[j] != 3) stopPeltier(j);
      } else {
        stopPeltier(j);
      }
    }

    if (!activedPeltier[j]) {
      Serial.print("NOC:");  //NOCHANGE
      Serial.print(j);
      Serial.println(";");
      if (j == 0 && statusPeltier[j] != 0) deactivatePeltier(j);
      if (j == 1 && statusPeltier[j] != 0) deactivatePeltier(j);
      if (j == 2 && statusPeltier[j] != 0) deactivatePeltier(j);
      if (j == 3 && statusPeltier[j] != 0) deactivatePeltier(j);
    }
    giveStatus(j);
    giveCurTemp(j);
    tt++;
  }
}

void loop() {
  portCounter = 0;
  getTemp();
  checkSerial();
}
