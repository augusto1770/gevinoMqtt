#include <ArduinoJson.h>
#include <EspATMqtt.h>

#include "config.h"
#include "gevino_civile_io.h"

EspATMqtt mqttManager(AP_SSID,MQTT_CLIENT_ID);

int buttonState[4];
int buttonPressCount[4];

/* init Gevino Board*/
void initBoard(){

  // initialize SerialUSB for debugging
  SerialUSB.begin(115200);
  // initialize serial for ESP module
  Serial.begin(115200);

  //Wait SerialUSB
  while(!SerialUSB);

  //Wait Serial
  while(!Serial);

}

void setup() {

  /*
  configData.valid = true;
  configData.mqtt_port = 1883;
  String("TP-LINK").toCharArray(configData.wifi_ssid,100);
  String("192.168.0.108").toCharArray(configData.mqtt_server,100);
  */

      
  initBoard();
  gevino_io_setup();

  mqttManager.init(Serial);

  mqttManager.setMqttConnectionCallback(mqttConnectionCallback);
  mqttManager.setMqttMessageCallback(mqttMessageCallback);


}


void mqttConnectionCallback(){
  
  SerialUSB.print("ON CONNECTION");
  refreshStatus();
  mqttManager.mqttSubscribe(SET_TOPIC);
  mqttManager.mqttSubscribe(STATUS_TOPIC);
  mqttManager.mqttSubscribe(RESET_TOPIC);
  
}

void mqttMessageCallback(char* topic, byte* payload, unsigned int length) {

  SerialUSB.print("Message arrived [");
  SerialUSB.print(topic);
  SerialUSB.print("] ");
  for (int i = 0; i < length; i++) {
    SerialUSB.print((char)payload[i]);
  }
  SerialUSB.println();

  if(strstr(topic,SET_TOPIC)){
    handleSet(payload,length);
  }else if(strstr(topic,STATUS_TOPIC)){
    refreshStatus();
  }else if(strstr(topic,RESET_TOPIC)){
    mqttManager.reset();
  }

}

void refreshStatus(){

  const int capacity = JSON_OBJECT_SIZE(7);
  StaticJsonDocument<capacity> doc;
  doc["out1"] = digitalRead(K1) == 1 ?"ON":"OFF";
  doc["out2"] = digitalRead(K2) == 1 ?"ON":"OFF";
  doc["out3"] = digitalRead(K3) == 1 ?"ON":"OFF";
  doc["out4"] = digitalRead(K4) == 1 ?"ON":"OFF";
  doc["out5"] = digitalRead(K5) == 1 ?"ON":"OFF";
  doc["out6"] = digitalRead(K6) == 1 ?"ON":"OFF";
  doc["out7"] = digitalRead(K7) == 1 ?"ON":"OFF";

  char output[128];
  serializeJson(doc, output);

  mqttManager.mqttPublish(RESPONSE_TOPIC, output);

}

void handleSet(byte* payload, unsigned int length){

  DynamicJsonDocument doc(length);

  deserializeJson(doc, payload);
  JsonObject obj = doc.as<JsonObject>();

  // You can use a String to get an element of a JsonObject
  // No duplication is done.
  if(obj.containsKey("out1")){
    String out1 = obj[String("out1")];
    digitalWrite(K1,out1.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out1:"+out1);
  }

  if(obj.containsKey("out2")){
    String out2 = obj[String("out2")];
    digitalWrite(K2,out2.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out2:"+out2);
  }

  if(obj.containsKey("out3")){
    String out3 = obj[String("out3")];
    digitalWrite(K3,out3.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out3:"+out3);
  }

  if(obj.containsKey("out4")){
    String out4 = obj[String("out4")];
    digitalWrite(K4,out4.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out4:"+out4);
  }


  if(obj.containsKey("out5")){
    String out5 = obj[String("out5")];
    digitalWrite(K5,out5.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out5:"+out5);
  }

  if(obj.containsKey("out5")){
    String out5 = obj[String("out5")];
    digitalWrite(K5,out5.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out5:"+out5);
  }


  if(obj.containsKey("out6")){
    String out6 = obj[String("out6")];
    digitalWrite(K6,out6.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out6:"+out6);
  }

  if(obj.containsKey("out7")){
    String out7 = obj[String("out7")];
    digitalWrite(K7,out7.equals("ON")?HIGH:LOW);
    refreshStatus();
    SerialUSB.println("Status Out7:"+out7);
  }
}



void loop() {

  mqttManager.loop();

  inOutLoop();



}


void inOutLoop(){

  if(RE(In1, buttonState[0])){
    buttonPressCount[0]++;
    SerialUSB.println("RISING EDGE 1");
    if(buttonPressCount[0]%5== 0){
        digitalWrite(K1,digitalRead(K1)==HIGH?LOW:HIGH);
    }else  if(buttonPressCount[0]%5== 1){
        digitalWrite(K2,digitalRead(K2)==HIGH?LOW:HIGH);
    }else  if(buttonPressCount[0]%5== 2){
        digitalWrite(K3,digitalRead(K3)==HIGH?LOW:HIGH);
    }else if(buttonPressCount[0]%5== 3){
        digitalWrite(K4,digitalRead(K4)==HIGH?LOW:HIGH);
    }else if(buttonPressCount[0]%5== 4){
        digitalWrite(K5,digitalRead(K5)==HIGH?LOW:HIGH);
    }
    refreshStatus();
  }

  if(RE(In2, buttonState[1])){
     buttonPressCount[1]++;
        SerialUSB.println("RISING EDGE 2");
        digitalWrite(K2,digitalRead(K2)==HIGH?LOW:HIGH);
          refreshStatus();
  }


  if(RE(In3, buttonState[2])){
     buttonPressCount[2]++;
        SerialUSB.println("RISING EDGE 3");
        digitalWrite(K3,digitalRead(K3)==HIGH?LOW:HIGH);
          refreshStatus();
  }


  if(RE(In4, buttonState[3])){
     buttonPressCount[3]++;
        SerialUSB.println("RISING EDGE 4");
        digitalWrite(K4,digitalRead(K4)==HIGH?LOW:HIGH);
        refreshStatus();
  }



  
}
