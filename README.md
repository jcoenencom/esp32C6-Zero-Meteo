# esp32C6-Zero-Meteo
Weather station based on ESP32-C6 Zero (waveshare)

The weather station has been developped initially with a D1 mini (esp8266), and providing data to a mosquitto mqtt broker, which in turn would provide data to FHEM or homeassistant (or else)

Unfortunately the D1 was not very stable (most likely a mistake or more in the code).

I moved it to a C6 in order to experiment with the zigbee stack from expressif (in the arudino IDE).

The 3D design comes from [makeworld](https://makerworld.com/en/models/111567-weather-station-anemometer?from=search#profileId-119379)

An additional rain gauge also from [makerworld](https://makerworld.com/en/models/139091-rain-gauge-zigbee-pluviometro-zigbee#profileId-151529) printed

![image1](/images/station_1.jpeg)

![image2](/images/station_2.jpeg)

![image3](/images/station_3.jpeg)

Wind vane:

![image4](/images/windvane1.jpeg)

![image5](/images/windvane2.jpeg)

![image6](/images/windvane3.jpeg)

![image7](/images/windvane4.jpeg)


Connexions

![Circuit](/images/Circuit.png)

In order for zigbee2mqtt to recognize the endpoints, the on4kch_ws.mjs has to be copied into 
/opt/zigbee2mqtt/data/external_converters/on4kch_ws.mjs
And zigbee2mqtt restarted