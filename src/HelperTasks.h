void extractCommandArduino(String);
void commandArduino(String, String, String);
void TaskTestPublish(void *pvParameters);
void TaskFlowPublish(void *pvParameters);
void TaskTempPublish(void *pvParameters);

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
        // Serial.print(F("Test Publish Success! Published: "));
        String str = "pub:waterbox/W0001/test:" + String(count);
        Serial.println(str);
        extractCommandArduino(str);
        vTaskDelay(500);
      }
      vTaskDelay(1000);
    } else {
      // Serial.println("Publish Failed! Not Connected to MQTT");
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
        // Serial.println("Publish Flow Failed! Not Connected to MQTT");
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

void extractCommandArduino(String str) {
  // extract the command string sent by arduino through uart
  String command, topicFull, topic, message;
  int separatorPos = str.indexOf(":");
  if (separatorPos > -1) {
    command = str.substring(0,separatorPos);
    topicFull = str.substring(separatorPos+1, str.indexOf(":",separatorPos+1));
    topic = topicFull.substring(topicFull.indexOf("/",10)+1);
    separatorPos = str.indexOf(":",separatorPos+1);
    message = str.substring(separatorPos+1);

    commandArduino(command, topic, message);
    
    // Serial.println(command);
    // Serial.println(topic);
    // Serial.println(message);
  }
}

void commandArduino(String command, String topic, String message) {
  /**
   * *********** FUNCTION LIST *********** 
   * publish
   * set_time
   * get_time
   * set_time_rtc
   * get_time_rtc
   * get_info_waterbox
   * get_debit_count
   * get_debit_data
  */
  if (command == "pub") {
    if (topic == "flow_sensor") {
      flow_sensor_pub.publish(message.toFloat());
    } else if ("temp_sensor") {
      temp_sensor_pub.publish(message.toFloat());
    } else if ("test") {
      test_pub.publish(message.toFloat());
    } else {
      Serial.println("topic error");
    } 
  } else if ("set_time") {
      Serial.println(command);
  } else if ("get_time") {
      Serial.println(command);
  } else if ("set_time_rtc") {
      Serial.println(command);
  } else if ("get_time_rtc") {
      Serial.println(command);
  } else if ("get_info_waterbox") {
      Serial.println(command);
  } else if ("get_debit_count") {
      Serial.println(command);
  } else if ("get_debit_data") {
      Serial.println(command);
  } else {
      Serial.println("nothing todo...");
  } 
}