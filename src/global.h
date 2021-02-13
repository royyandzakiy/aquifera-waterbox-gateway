/************ Global State (you don't need to change this!) ******************/

// Create an WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
PubSubClient mqtt(client);

/****************************** Feeds ***************************************/

const char* commands_topic = "waterbox/W0001/commands";
const char* flow_sensor_topic = "waterbox/W0001/flow_sensor";
const char* temp_sensor_topic = "waterbox/W0001/temp_sensor";
const char* test_topic = "waterbox/W0001/test";