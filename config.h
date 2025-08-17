
#include <Wire.h>
#include <AHT20.h>
#include <Adafruit_BMP280.h>
#include <FastLED.h>
#include <Adafruit_AHTX0.h>

#define DEVICE "ESP32-C6-Zero"
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"


#define LED_BUILTIN 13 //blue LED user conrolable
#define RGB_LED 8 //controlable RGB LED
#define NUM_LEDS 1
#define DATA_PIN 8
#define BRIGHTNESS 16

/* Zigbee OTA configuration */
#define OTA_UPGRADE_RUNNING_FILE_VERSION    0x01010100  // Increment this value when the running image is updated
#define OTA_UPGRADE_DOWNLOADED_FILE_VERSION 0x01010101  // Increment this value when the downloaded image is updated
#define OTA_UPGRADE_HW_VERSION              0x0101      // The hardware version, this can be used to differentiate between different hardware versions

/* Zigbee temperature sensor configuration */
#define TEMP_SENSOR_ENDPOINT_NUMBER 10
#define PRESSURE_SENSOR_ENDPOINT_NUMBER 11
#define ANALOG_DEVICE_ENDPOINT_NUMBER 12
#define WIND_SPEED_SENSOR_ENDPOINT_NUMBER 9

#define RADIUS 0.028 // 28 mm de rayon
#define AVGINTERVALS 6 //# of intervale for average speed should be WIND_AVG / WIND_INSTATNT

#define WIND_INSTANT 5000UL //intervale to compute the instant speed 5000 ms
#define WIND_AVG 30000UL // reporting interval 30 sec
#define SCL 1
#define SDA 0

#define INTERRUPT 3
#define RAININTRPT 5

typedef struct {
    float temperature;
    float temperature2;
    float pressure;
    int humidity;
    float hit;
    float speed;
    float gust;
    int hitcnt;   // number of intervals used in wind average calculation
    float avgspeed; // average speed
}
measure;


void scanDevices(TwoWire *w);

void thp( measure &val);
void setwind();
void setrain();
void IRAM_ATTR counter();
float Speed();
float rain();
void IRAM_ATTR countRain();
