
#include "config.h"


Stream *console;
uint32_t deviceOnline = 0x00;

extern AHT20 aht;
extern Adafruit_AHTX0 aht0;
extern Adafruit_BMP280 bmp;
volatile int count = 0;
volatile int rcount = 0;   //amount of bucket tips.




//const byte interruptPin = INTERRUPT;
const byte rinterruptPin = RAININTRPT;

enum {
    POWERMANAGE_ONLINE  = _BV(0),
    DISPLAY_ONLINE      = _BV(1),
    RADIO_ONLINE        = _BV(2),
    GPS_ONLINE          = _BV(3),
    PSRAM_ONLINE        = _BV(4),
    SDCARD_ONLINE       = _BV(5),
    AXDL345_ONLINE      = _BV(6),
    BME280_ONLINE       = _BV(7),
    BMP280_ONLINE       = _BV(8),
    BME680_ONLINE       = _BV(9),
    QMC6310_ONLINE      = _BV(10),
    QMI8658_ONLINE      = _BV(11),
    PCF8563_ONLINE      = _BV(12),
    OSC32768_ONLINE      = _BV(13),
};


void scanDevices(TwoWire *w)
{
    uint8_t err, addr;
    int nDevices = 0;
    uint32_t start = 0;

    console->println("I2C Devices scanning");
    for (addr = 1; addr < 127; addr++) {
        start = millis();
        w->beginTransmission(addr); delay(2);
        err = w->endTransmission();
        if (err == 0) {
            nDevices++;
            switch (addr) {
            case 0x77:
            case 0x76:
                console->println("\tFind BMX280 Sensor!");
                deviceOnline |= BME280_ONLINE;
                break;
            case 0x34:
                console->println("\tFind AXP192/AXP2101 PMU!");
                deviceOnline |= POWERMANAGE_ONLINE;
                break;
            case 0x3C:
                console->println("\tFind SSD1306/SH1106 dispaly!");
                deviceOnline |= DISPLAY_ONLINE;
                break;
            case 0x51:
                console->println("\tFind PCF8563 RTC!");
                deviceOnline |= PCF8563_ONLINE;
                break;
            case 0x1C:
                console->println("\tFind QMC6310 MAG Sensor!");
                deviceOnline |= QMC6310_ONLINE;
                break;
            default:
                console->print("\tI2C device found at address 0x");
                if (addr < 16) {
                    console->print("0");
                }
                console->print(addr, HEX);
                console->println(" !");
                break;
            }

        } else if (err == 4) {
            console->print("Unknow error at address 0x");
            if (addr < 16) {
                console->print("0");
            }
            console->println(addr, HEX);
        }
    }
    if (nDevices == 0)
        console->println("No I2C devices found\n");

    console->println("Scan devices done.");
    console->println("\n");
}


void thp( measure &val){
  /* 
  return a structure with measured values from the sensors
  REQUIRES initialization of I2C devices:
    AHT20 aht;
    Adafruit_BMP280 bmp;
  */
  sensors_event_t humidity, temp;
  
  aht0.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  console->print("Getting data from AHT20\t");
  val.temperature = temp.temperature; //aht.getTemperature();
  val.humidity = humidity.relative_humidity; //aht.getHumidity();
  console->println(val.temperature);
  console->print("Getting data from BMP\t");
  val.pressure = bmp.readPressure() / 100.0;
  val.temperature2=bmp.readTemperature();
  console->print(val.temperature2);
  console->print("\t");
  console->println(val.pressure);
  console->print("Getting data from count\t");
  val.hit= float(count);
  console->println(val.hit);
}


void IRAM_ATTR counter() {
  static unsigned long last_micros = 0;
  unsigned long interrupt_time = millis();
  if ( interrupt_time - last_micros > 50UL ) {
    count += 1;                   // increment count
    }
    last_micros = interrupt_time;
}


void IRAM_ATTR countRain() {
  static unsigned long last_micros = 0;
  unsigned long interrupt_time = millis();
  console->print(".");
  if ( interrupt_time - last_micros > 50UL ) {
    rcount += 1;                   // increment count
    }
    last_micros = interrupt_time;
}

float Speed() {
/*
calculate the linear speed from magnets hit count, the routine gets called when average timer lapse (thus after measureperiod ms).
REQUIRES:
count: number of magnet detection
measureperiod: averaging period for rotational counts (ms)
RADIUS= distance between HAL detector and rotation axis centre.

PRVIDES:
sets global variable float speed
*/
  noInterrupts();
  int interval = WIND_INSTANT / 1000; //anemometer averaging interval (sec)
  char text[128];  //text buffer for sprintf
  int local = count;
  float speed;
  float RPM;

  int tour = local / 4; //3 magnets, it takes 4 hits to detect a full 360°
  //speed = 3.1415926 * 2 * float(tour) * RADIUS / float(interval) ;

  RPM = float(local) * ( 60 / interval);  // tour/minutes
  speed = RPM * RADIUS * 60 * 3.1415926 * 2. / 1000.; // vitesse radiale km/hr = vitesse angulaire (rad/min) * 2 Pi * rayon (m) * 60 (min/hr)  / 1000 (m/km)
  
  if (local > 0) {
    sprintf(text,"  Speed calculation: Count %d \tRPM %8.2f (T/min)\tspeed %8.4f (km/hr) ", local, RPM, speed);
    console->println(text);
  }
  count=0;
  interrupts();
  return speed;
}

float rain(){
    /* rain fall is amount of water that fell down in 24 hours at 8:30 in the morning */

    noInterrupts();
    int local = rcount;
    float millis = 0.550053 * local;
    console->println(local);
    interrupts();
    return millis;
}



void setwind(){
  /* et the interrupt mechanism that intercept the magnet detection and accumulates its counting */
  pinMode(INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT), counter, RISING);
  //timermeasure.set(measureperiod, Speed);
  //TTNuplink.set(ttnuplinkperiod, TTNsend);  
}

void setrain() {

  console->print("setting rain interupt pin :");
  console->println(RAININTRPT);
  pinMode(RAININTRPT,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RAININTRPT), countRain, RISING);
}