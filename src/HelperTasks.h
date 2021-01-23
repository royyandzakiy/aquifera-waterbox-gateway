
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