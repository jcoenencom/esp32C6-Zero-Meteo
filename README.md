# esp32C6-Zero-Meteo
Weather station based on ESP32-C6 Zero (waveshare)

The weather station has been developped initially with a D1 mini (esp8266), and providing data to a mosquitto mqtt broker, which in turn would provide data to FHEM or homeassistant (or else)

Unfortunately the D1 was not very stable (most likely a mistake or more in the code).

I moved it to a C6 in order to experiment with the zigbee stack from expressif (in the arudino IDE).

![image1](/images/station_1.jpeg)

![image2](/images/station_2.jpeg)

![image3](/images/station_3.jpeg)