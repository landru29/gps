#ifndef __NEO_6M_H__
#define __NEO_6M_H__

#include <SoftwareSerial.h>
#include <Arduino.h>

#define NEO6M_NMEA_LEN      16

#define NEO6M_NMEA_ID_POS    7
#define NEO6M_DDC_POS        8
#define NEO6M_SERIAL_1_POS   9
#define NEO6M_SERIAL_2_POS   10
#define NEO6M_USB_POS        11
#define NEO6M_SPI_POS        12

#define NEO6M_GPGGA  0
#define NEO6M_GPGLL  1
#define NEO6M_GPGLV  2
#define NEO6M_GPGSA  3
#define NEO6M_GPRMC  4
#define NEO6M_GPVTG  5

class Neo6M {
  public:
    Neo6M(SoftwareSerial* gps);
    void setConfig(char NMEA_num, bool enable);
    size_t read(char* buffer, size_t len);

    static int nextStart(char* buffer, int len);

  private:
    SoftwareSerial* gps;
    static const char CFG_MSG[NEO6M_NMEA_LEN];

    static void insertChecksum(char packet[], const byte len);
    void sendPacket(char packet[], const byte len);
};

#endif
