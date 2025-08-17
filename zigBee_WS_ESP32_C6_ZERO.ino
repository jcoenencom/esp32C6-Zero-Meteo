#include "config.h"

AHT20 aht;
Adafruit_AHTX0 aht0;

Adafruit_BMP280 bmp;

extern Stream *console;
extern volatile int count;
extern volatile int rcount;

int previous=0;
ZigbeeTempSensor zbTempSensor = ZigbeeTempSensor(TEMP_SENSOR_ENDPOINT_NUMBER);
ZigbeePressureSensor zbPressureSensor = ZigbeePressureSensor(PRESSURE_SENSOR_ENDPOINT_NUMBER);
//ZigbeeWindSpeedSensor zbWindSpeedSensor = ZigbeeWindSpeedSensor(WIND_SPEED_SENSOR_ENDPOINT_NUMBER);
ZigbeeAnalog zbAnalogDevice = ZigbeeAnalog(ANALOG_DEVICE_ENDPOINT_NUMBER);
ZigbeeAnalog zbWindSpeedSensor = ZigbeeAnalog(WIND_SPEED_SENSOR_ENDPOINT_NUMBER);

uint8_t button = BOOT_PIN;
unsigned long windinstant_t=0;
unsigned long wind_avg_t=0;
CRGB leds[NUM_LEDS];


void setup() {
  Serial.begin(115200);
  
  unsigned long startTime = millis();
  while (!Serial && millis() - startTime < 3000) {
  // Wait for up to 5 seconds for a serial connection
  // Adjust the timeout value (5000 milliseconds in this case) as needed
  }
  console = &Serial;
  pinMode(button, INPUT_PULLUP);

  Serial.println("Started serial");
  Serial.println(DEVICE);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  //pinMode(SDA, INPUT_PULLUP);
  //pinMode(SCL, INPUT_PULLUP);
  Wire.begin(SDA,SCL);
  Serial.println("\nI2C Scanner");
  scanDevices(&Wire);


  if (aht0.begin() == false) {
    console->println("AHT20 not detected");
  } else {
   console->println("Found AHT20");
  }
  bmp.begin(0x77);
  setwind();
  setrain();

  leds[0] = CRGB::Green;    //RGB LED RED whe starting
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  zbTempSensor.setManufacturerAndModel("ON4KCH", "ZigbeeMeteoStation");
  zbTempSensor.addOTAClient(OTA_UPGRADE_RUNNING_FILE_VERSION, OTA_UPGRADE_DOWNLOADED_FILE_VERSION, OTA_UPGRADE_HW_VERSION);


  zbTempSensor.setMinMaxValue(10, 50);
  zbTempSensor.setTolerance(1);
  zbTempSensor.addHumiditySensor(0, 100, 1);
  // Optional: Time cluster configuration (default params, as this device will revieve time from coordinator)
  zbTempSensor.addTimeCluster();

  //zbPressureSensor.setMinMaxValue(0, 10000);
  //zbPressureSensor.setTolerance(1);
// Set up analog input
zbAnalogDevice.addAnalogInput();

// second analog input gust

zbWindSpeedSensor.addAnalogInput();

//zbAnalogDevice.setAnalogInputApplication(ESP_ZB_ZCL_AI_RPM_OTHER);


  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbWindSpeedSensor);


  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbTempSensor);
  Zigbee.addEndpoint(&zbPressureSensor);
  //Zigbee.addEndpoint(&zbWindSpeedSensor);
  Zigbee.addEndpoint(&zbAnalogDevice);
Serial.println("Starting Zigbee...");
  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
    }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  leds[0] = CRGB::Red;
  FastLED.show();

digitalWrite(LED_BUILTIN, LOW);   //set RGB to green indicate zigbee network connected

// Set reporting interval for windspeed measurement in seconds, must be called after Zigbee.begin()
  // min_interval and max_interval in seconds, delta (WindSpeed change in m/s)
  // if min = 1 and max = 0, reporting is sent only when windspeed changes by delta
  // if min = 0 and max = 10, reporting is sent every 10 seconds or windspeed changes by delta
  // if min = 0, max = 10 and delta = 0, reporting is sent every 10 seconds regardless of windspeed change

zbPressureSensor.setReporting(0, 600, 1);
zbTempSensor.setReporting(0,300,0);
zbAnalogDevice.setAnalogInputReporting(0,600,1);
zbWindSpeedSensor.setAnalogInputReporting(0, 600, 1);



  // Start Zigbee OTA client query, first request is within a minute and the next requests are sent every hour automatically
  zbTempSensor.requestOTAUpdate();

}

void mesure( measure &val) {
  // Measure sensor values
  thp(val);

  // Update temperature and humidity values in Temperature sensor EP
  if (val.temperature > -20) {zbTempSensor.setTemperature(val.temperature);}
  if (val.humidity > 0) {zbTempSensor.setHumidity(val.humidity);}

  if (val.pressure > 80) {zbPressureSensor.setPressure(int(val.pressure));}
  
  // Report temperature and humidity values
  zbTempSensor.report();
  Serial.printf("Reported temperature: %.2f°C, Humidity: %.2f%%\r\n", val.temperature, val.humidity);
  zbPressureSensor.report();
  Serial.printf("Reported pressure: %.2f°mBar\r\n", val.pressure);
  zbAnalogDevice.setAnalogInput(val.avgspeed);
  Serial.printf("Reported gust wind speed (km/hr): %6.2f\r\n", val.gust);
  zbWindSpeedSensor.setAnalogInput(val.gust);
  zbWindSpeedSensor.reportAnalogInput();
  Serial.printf("Reported avg wind speed (km/hr): %6.2f\r\n", val.avgspeed);
  zbAnalogDevice.reportAnalogInput();
  // Add small delay to allow the data to be sent before going to sleep
  delay(100);

}



void loop() {

    if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();

    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 10000) {
        // If key pressed for more than 10secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        // Optional set reset in factoryReset to false, to not restart device after erasing nvram, but set it to endless sleep manually instead
        Zigbee.factoryReset(false);
        Serial.println("Going to endless sleep, press RESET button or power off/on the device to wake up");
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
        esp_deep_sleep_start();
      }
    }
  }
  measure val;
  int hits;

// take a measure from sensor and report to zigbee
  mesure(val); 
  char retval[256];
  

  
  
  windinstant_t = millis();
  wind_avg_t = millis();
  val.hitcnt=1; // # of interval for wind average calculation

  while (true) {
    unsigned long now = millis();
    // compute instant speed over the WIND_INSTANT inerval
    // store the result in val mesure structure
    if ((now - windinstant_t) >= WIND_INSTANT ) {
      console->print("Counted: ");
      console->println(hits);
// calculate and store "instant speed"
      val.speed = Speed();
      float factor = float(val.hitcnt - 1)/val.hitcnt;
      float temp = val.avgspeed * factor;
      //sprintf(retval,"Interval: %d\tFactor %6.2f\tAVG*factor %6.2f\tlast speed %6.2f \tavg speed %6.2f\n", val.hitcnt, factor, temp, val.speed, val.avgspeed);
      //console->println(retval);
      
      val.avgspeed = temp + (val.speed / val.hitcnt);
      zbAnalogDevice.setAnalogInput(val.avgspeed);
      float pluie = rain();
      int tilted = rcount;
      sprintf(retval,"Interval: %d\tlast speed %6.2f \tavg speed %6.2f\t tilted: %d \tRAIN: %6.2f\n", val.hitcnt, val.speed, val.avgspeed,tilted,pluie);
      console->println(retval);
      if (val.hitcnt >= 6) {
        val.hitcnt = 1;
        val.gust = val.speed;
      } else {
        val.hitcnt += 1;
        if ( val.speed > val.gust ){
          val.gust = val.speed;
        }
      }
      previous = hits;
      windinstant_t = now; 
    } else if ( (now - wind_avg_t) >= WIND_AVG) {
      console->println("Value read from sensors");
      mesure(val); 
      sprintf(retval,"AHT20 - Temp: %6.2f °C Humidity: %d %%\tBMP280 - Temp: %6.2f °C Pressure: %7.2f hPa\tlast speed %6.2f \tavg speed %6.2f\n",val.temperature, val.humidity,val.temperature2, val.pressure, val.speed, val.avgspeed);
      console->println(retval);
      wind_avg_t = now;
    }
    if (Serial.available() > 0 ) {
      String comd = Serial.readString();
      if (comd.startsWith("scan")) {
        scanDevices(&Wire);
      } else if (comd.startsWith("read")) {
        thp(val);
      } else if (comd.startsWith("raz")) {
        ESP.restart();
      } else if (comd.startsWith("who")) {
        console->println(DEVICE);
      }
      console->println(comd);
    }
  }
  delay(10000);
}