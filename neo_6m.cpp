#include "neo_6m.h"

static const char Neo6M::CFG_MSG[NEO6M_NMEA_LEN] = {
  0xB5, // Header char 1
  0x62, // Header char 2
  0x06, // class
  0x01, // id
  0x08, // length LSB
  0x00, // length MSB
  0xF0, // payload (NMEA sentence ID char 1)
  0x00, // payload (NMEA sentence ID char 2)
  0x00, // payload I/O Target 0 - DDC           - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 1 - Serial Port 1 - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 2 - Serial Port 2 - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 3 - USB           - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 4 - SPI           - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 5 - Reserved      - (1 - enable sentence, 0 - disable)
  0x00, // CK_A
  0x00  // CK_B
};

Neo6M::Neo6M(SoftwareSerial* gps) {
  this->gps = gps;
}

size_t Neo6M::read(char* buffer, size_t len) {
   if (this->gps->available()>0) {
    return this->gps->readBytes(buffer, len);
   }
   return 0;
}

static int Neo6M::nextStart(char* buffer, int len) {
  for (int i=0; i<len; i++) {
    if (buffer[i] == '$') {
      return i;
    }
  }
  return -1;
}

static void Neo6M::insertChecksum(char packet[], const byte len) {
  uint8_t ck_a = 0;
  uint8_t ck_b = 0;

  // exclude the first and last two bytes in packet
  for (byte i = 2; i < (len - 2); i++)
  {
    ck_a += packet[i];
    ck_b += ck_a;
  }

  packet[len - 2] = ck_a;
  packet[len - 1] = ck_b;
}

void Neo6M::sendPacket(char packet[], const byte len) {
  this->gps->write(packet, len);
}

void Neo6M::setConfig(char NMEA_num, bool enable) {
 char configPacket[NEO6M_NMEA_LEN];
  memcpy(configPacket, CFG_MSG, NEO6M_NMEA_LEN);

  if (enable)
    configPacket[NEO6M_SERIAL_1_POS] = 1;

  configPacket[NEO6M_NMEA_ID_POS] = NMEA_num;
  Neo6M::insertChecksum(configPacket, NEO6M_NMEA_LEN);

  this->sendPacket(configPacket, NEO6M_NMEA_LEN);
}
