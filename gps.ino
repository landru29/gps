#include <SoftwareSerial.h>
#include "neo_6m.h"

#define ENABLE_OUTPUT
#define ENABLE_SD

#ifdef ENABLE_SD
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#endif

#define RX 6
#define TX 7 
#define CS 4

#define BITRATE 9600
#define BUFFER_SIZE 256


SoftwareSerial gps(RX, TX);
char buffer[BUFFER_SIZE];
Neo6M* neo6m;

#ifdef ENABLE_SD
unsigned int globalIndex;
File dataFile;
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
#ifdef ENABLE_OUTPUT
  Serial.begin(BITRATE);
#endif
  gps.begin(BITRATE);

#ifdef ENABLE_OUTPUT
  while (!Serial) {
    ;
  }
  Serial.println("Ready");
#endif

#ifdef ENABLE_SD
  #ifdef ENABLE_OUTPUT
  Serial.print("Initializing SD card...");
  #endif

  // see if the card is present and can be initialized:
  if (!SD.begin(CS)) {
    #ifdef ENABLE_OUTPUT
    Serial.println("Card failed, or not present");
    #endif
    while (1);
  }

  #ifdef ENABLE_OUTPUT
  Serial.println("card initialized.");
  #endif

  globalIndex = readIntFromEEPROM(0) + 1;

  String filename = String("datalog") + globalIndex + ".txt";


  writeIntIntoEEPROM(0, globalIndex);
  dataFile = SD.open(filename, FILE_WRITE);

   #ifdef ENABLE_OUTPUT
  Serial.println(filename);
  #endif

#endif
  neo6m = new Neo6M(&gps);

  neo6m->setConfig(NEO6M_GPGGA, true);
  neo6m->setConfig(NEO6M_GPGLL, false);
  neo6m->setConfig(NEO6M_GPGLV, false);
  neo6m->setConfig(NEO6M_GPGSA, false);
  neo6m->setConfig(NEO6M_GPRMC, true);
  neo6m->setConfig(NEO6M_GPVTG, true);

  int start = -1;
  while (start<0) {
    size_t count = neo6m->read(buffer, BUFFER_SIZE);
    start = Neo6M::nextStart(buffer, count);
    if (start>=0) {
      output(&buffer[start], count-start);
    }
  }

}

void loop() {
  size_t count = neo6m->read(buffer, BUFFER_SIZE);
  if (count) {
    output(buffer, count);
  }
}


#ifdef ENABLE_SD
void writeIntIntoEEPROM(int address, unsigned int number) { 
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}

unsigned int readIntFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}
#endif

void output(char* buf, int len) {
#ifdef ENABLE_OUTPUT
  Serial.write(buf, len);
#endif

#ifdef ENABLE_SD
  digitalWrite(LED_BUILTIN, HIGH); 
  dataFile.write(buf, len);
  dataFile.flush();
  digitalWrite(LED_BUILTIN, LOW); 
#endif
}
