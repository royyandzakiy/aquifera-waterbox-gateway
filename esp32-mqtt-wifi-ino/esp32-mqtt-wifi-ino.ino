#include "WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "credentials.h"

#define ARDUINO_MAIN_CORE 0
#define ARDUINO_TASK_CORE 1

#ifndef CREDENTIALS_H
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "YOUR_WIFI_SSID"
#define WLAN_PASS       "YOUR_WIFI_PASSWORD"

/************************* Adafruit.io Setup *********************************/

#define MQTT_SERVER      "YOUR_MQTT_BROKER"
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    "YOUR_MQTT_BROKER_USERNAME"
#define MQTT_KEY         "YOUR_MQTT_BROKER_PASSWORD"

#endif

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

/*************************** Sketch Code ************************************/

void setupWifi();
void setupMQTT();
void MQTT_connect();

void getMQTTMessages();
void TaskTestPublish(void *pvParameters);

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Waterbox Initialize..."));

  // Connect to WiFi access point.
  setupWifi();

  // Setup MQTT subscription
  setupMQTT();

  Serial.println(F("Waterbox Setup Complete."));
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive
  MQTT_connect();

  // Get messages from subscribed topics
  getMQTTMessages();

  // ping the server to keep the mqtt connection alive
  if(!mqtt.ping()) {
    mqtt.disconnect();
  }

}

void TaskTestPublish(void *pvParameters) {
  // test publish
  (void) pvParameters;
  int count = 0;
  
  for(;;) {
    if (mqtt.connected()) {      
      count++;
      // Publish
      if (!test_pub.publish(count))
        Serial.println(F("Publish Failed."));
      else {
        Serial.print(F("Test Publish Success! Published: "));
        Serial.println(count);
        vTaskDelay(500);
      }
      vTaskDelay(1000);
    } else {
      Serial.println("Publish Failed! Not Connected to MQTT");
      vTaskDelay(1000);
    }
  }
}

void TaskFlowPublish(void *pvParameters) {
  (void) pvParameters;
  int flow_read = 0;
  int flow_read_old = flow_read; // change this to use a pointer instead
  
  for(;;) {
    // read flow here
    // flow_read = read_flow_sensor();
    
    // if there is new data, publish
    if (flow_read != flow_read_old) {
      flow_read_old = flow_read;
      
      if (mqtt.connected()) {
        if (!flow_sensor_pub.publish(flow_read))
          Serial.println(F("Publish Failed."));
        else {
          Serial.print(F("Flow Publish Success! Published: "));
          Serial.println(flow_read);
        }
      } else {
        Serial.println("Publish Flow Failed! Not Connected to MQTT");
      }
    }
  }
}

void TaskTempPublish(void *pvParameters) {
  (void) pvParameters;
  int temp_read = 0;
  int temp_read_old = temp_read; // change this to use a pointer instead
  
  for(;;) {
    // read flow here
    // flow_read = read_flow_sensor();
    
    // if there is new data, publish
    if (temp_read != temp_read_old) {
      temp_read_old = temp_read;
      
      if (mqtt.connected()) {
        if (!temp_sensor_pub.publish(temp_read))
          Serial.println(F("Publish Failed."));
        else {
          Serial.print(F("Temp Publish Success! Published: "));
          Serial.println(temp_read);
        }
      } else {
        Serial.println("Publish Temp Failed! Not Connected to MQTT");
      }
    }
  }
}

void setupMQTT() {
  Serial.println(F("MQTT Setup..."));
  mqtt.subscribe(&temp_sensor_sub);
  mqtt.subscribe(&flow_sensor_sub);
  mqtt.subscribe(&test_sub);

  xTaskCreatePinnedToCore(
    TaskTestPublish
    ,  "TaskTestPublish"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_TASK_CORE
  );  

  xTaskCreatePinnedToCore(
    TaskTempPublish
    ,  "TaskTempPublish"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_TASK_CORE
  );  

  xTaskCreatePinnedToCore(
    TaskFlowPublish
    ,  "TaskFlowPublish"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_TASK_CORE
  );  
}

void getMQTTMessages() {
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    Serial.println("Message Incoming!");

    // Check from which Topic did the message come from
    if (subscription == &flow_sensor_sub) {
      Serial.print(F("Flow Sensor: "));
      Serial.println((char *)flow_sensor_sub.lastread);
    }
    if (subscription == &temp_sensor_sub) {
      Serial.print(F("Temp Sensor: "));
      Serial.println((char *)temp_sensor_sub.lastread);
    }
    if (subscription == &test_sub) {
      Serial.print(F("Test Feed: "));
      Serial.println((char *)test_sub.lastread);
    }
  }
}

void setupWifi() {
  Serial.println(F("Wifi Setup..."));
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
