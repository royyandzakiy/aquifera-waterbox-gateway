/************ Global State (you don't need to change this!) ******************/

// Create an WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_USERNAME, MQTT_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for MQTT follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe commands_sub = Adafruit_MQTT_Subscribe(&mqtt, "waterbox/W0001/commands"); // used to retrieve actions that is needed to be done by Waterbox
Adafruit_MQTT_Subscribe temp_sensor_sub = Adafruit_MQTT_Subscribe(&mqtt, "waterbox/W0001/temp_sensor");
Adafruit_MQTT_Subscribe flow_sensor_sub = Adafruit_MQTT_Subscribe(&mqtt, "waterbox/W0001/flow_sensor");
Adafruit_MQTT_Subscribe test_sub = Adafruit_MQTT_Subscribe(&mqtt, "waterbox/W0001/test");

Adafruit_MQTT_Publish temp_sensor_pub = Adafruit_MQTT_Publish(&mqtt, "waterbox/W0001/temp_sensor");
Adafruit_MQTT_Publish flow_sensor_pub = Adafruit_MQTT_Publish(&mqtt, "waterbox/W0001/flow_sensor");
Adafruit_MQTT_Publish test_pub = Adafruit_MQTT_Publish(&mqtt, "waterbox/W0001/test");