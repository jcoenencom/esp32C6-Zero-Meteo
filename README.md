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

Anemometer:

![image4](/images/windvane1.jpeg)

![image5](/images/windvane2.jpeg)

![image6](/images/windvane3.jpeg)

![image7](/images/windvane4.jpeg)

Wind Vane (being designed usign freecad).

![Sensor Box](/images/SensorBox.jpeg)
![Main Body](/images/MainBody.jpeg)



Connexions

![Circuit](/images/Circuit.png)

In order for zigbee2mqtt to recognize the endpoints, the on4kch_ws.mjs has to be copied into 
/opt/zigbee2mqtt/data/external_converters/on4kch_ws.mjs
And zigbee2mqtt restarted


Overview of the system

![Overview](/images/overview.png)

On [this](https://www.jcoenen.com/Meteo/esp32C6Zero.php) page you will find the real time readings from the station installed on my balcony.

The php page is done from a mix of css, javascript and php.

It connects to my externally reachable mosquitto mqtt broker that bridges with my internal broker to get a selected subset of messages from an instance of zigbee2mqtt and subscribe to the esp32 mqtt messages.

The esp32 itself connects the zigbee network and regularily sends messages to the sonoff concentrator attached to Z2M.

Values received are displayed in the page as they come in.