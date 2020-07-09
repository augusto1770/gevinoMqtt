#include "EspATMqtt.h"


WiFiClient wifiClient;
WiFiServer webServer(80);
PubSubClient mqttClient(wifiClient);
FlashStorage(flash_storage, ConfigData);

EspATMqtt::EspATMqtt(char* apSsid_par,char* mqtt_client_name)
{

  apSsid = apSsid_par;
  mqttClientName = mqtt_client_name;
}


boolean EspATMqtt::init(Stream& serial){

  WiFi.init(Serial);
   /* if wifi module is connected */
  if(checkWifiModule()){
   restoreConfigurationData();


   if(hasConfigurationData()){
    if(connectWifi()){
      connectMqtt();
    }
  }else{
    startWifiManager();
  }
} 

}
void EspATMqtt::setWifiConnectionCallback(WIFI_CONNECTION_CALLBACK_SIGNATURE){
    this->wifiConnectionCallback = wifiConnectionCallback;
}


void EspATMqtt::setMqttConnectionCallback(MQTT_CONNECTION_CALLBACK_SIGNATURE){
    this->mqttConnectionCallback = mqttConnectionCallback;

}

void EspATMqtt::setMqttMessageCallback(MQTT_MESSAGE_CALLBACK_SIGNATURE){
     this->mqttMessageCallback = mqttMessageCallback;

}




void EspATMqtt::reset(){

  clearConfigurationData();
  
  if(connectWifi()){
      connectMqtt();
  }

}

boolean EspATMqtt::checkWifiModule(){

  // initialize ESP module
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    SerialUSB.println();
    SerialUSB.println("Communication with WiFi module failed!");
    // don't continue
    return false;
  }

  return true;

}

boolean EspATMqtt::configAP(){

  WiFi.disconnect();
  delay(1000);
  WiFi.endAP(true); // to disable default automatic start of persistent AP at startup

  //  use this lines for custom SoftAP IP. it determines the IP of stations too
  IPAddress ip(192, 168, 10, 1);
  WiFi.configureAP(ip);

  LOG_PRINTLN();
  LOG_PRINT("Start AP with SSID: ");
  LOG_PRINTLN(apSsid);

  int status = WiFi.beginAP(apSsid); // for AP open without passphrase/encryption
  // int status = WiFi.beginAP(ssid, pass);

  if (status == WL_AP_LISTENING) {
    LOG_PRINTLN();
    LOG_PRINTLN("AP started");
    printApStatus();

    char ssid[33];
    WiFi.apSSID(ssid);
    LOG_PRINT("Connect your computer to the WiFi network ");
    LOG_PRINT(ssid);
    LOG_PRINTLN();
    IPAddress ip = WiFi.apIP();
    LOG_PRINT("and enter http://");
    LOG_PRINT(ip);
    LOG_PRINTLN(" in a Web browser");

    currentMode = MODE_AP;
    return true;

  } else {
    LOG_PRINTLN();
    LOG_PRINTLN("AP failed to start.");
    return false;
  }


}


void EspATMqtt::startWifiManager() {

  if(!configAP()){
    return;
  }
  scanNetwork();
  webServer.begin();

}



boolean EspATMqtt::connectWifi() {

/*
  if(WiFi.status() == WL_CONNECTED){
    return true;
  }
*/
  forceSTAMode();
  forceResetDhcpMode();
  delay(2000);
  
  // We start by connecting to a WiFi network
  while (WiFi.status() != WL_CONNECTED && !wifiConnectionAborted) {

    LOG_PRINTLN();
    LOG_PRINT("Attempting to connect to WPA SSID: ");
    LOG_PRINT(configData.wifi_ssid);
   // LOG_PRINT(" Password:");
   // LOG_PRINTLN(configData.wifi_password);


    int status = WiFi.begin(configData.wifi_ssid, configData.wifi_password);
    LOG_PRINTLN();

    if (status != WL_CONNECTED) {
      LOG_PRINTLN("Failed to connect to AP");
      wifiRetryCount++;
    } else {
      LOG_PRINTLN("You're connected to the network");
      printWifiData();
      wifiRetryCount = 0;
      currentMode = WIFI_ENABLED;
      return true;
    }

    if(wifiRetryCount > WIFI_MAX_RETRY){
      wifiConnectionAborted = true;
      return false;
    }

    delay(5000);
  }

}

void EspATMqtt::loop(){

  webserverLoop();
  mqttLoop();
  keepAlive();

}


void EspATMqtt::webserverLoop(){


  /*GESTIONE DEL WEBSERVER */
  WiFiClient webClient = webServer.available();
  if (webClient && webClient.available()) { // if !available yet, we return to this client in next loop
    char line[64];
    int l = webClient.readBytesUntil('\n', line, sizeof(line));
    line[l] = 0;
    webClient.find((char*) "\r\n\r\n");

    if (strncmp_P(line, PSTR("POST"), strlen("POST")) == 0) {
      l = webClient.readBytes(line, sizeof(line));
      line[l] = 0;

      const char* paramDelims = "&";

      String ssidToken = String(strtok(line, paramDelims));
      String passwordToken = String(strtok(NULL, paramDelims));
      String mqttServerToken = String(strtok(NULL, paramDelims));
      String mqttPortToken = String(strtok(NULL, paramDelims));
     // String mqttUsernameToken = String(strtok(NULL, paramDelims));
     // String mqttPasswordToken = String(strtok(NULL, paramDelims));



      String par_ssid = ssidToken.substring(ssidToken.indexOf("ssid=")>-1?5:0);
      String par_pass = passwordToken.substring(passwordToken.indexOf("pass=")>-1?5:0);
      String par_mqtt_server = mqttServerToken.substring(mqttServerToken.indexOf("mqtt_s=")>-1?7:0);
      int par_mqtt_port = mqttPortToken.substring(mqttPortToken.indexOf("mqtt_p=")>-1?7:0).toInt();
     // String par_mqtt_username = mqttUsernameToken.substring(mqttUsernameToken.indexOf("mqtt_user=")>-1?10:0);
     // String par_mqtt_password = mqttPasswordToken.substring(mqttPasswordToken.indexOf("mqtt_password=")>-1?15:0);



      LOG_PRINT("SSID:");
      LOG_PRINTLN(par_ssid);
      LOG_PRINT("Password:");
      LOG_PRINTLN(par_pass);
      LOG_PRINT("mqtt server:");
      LOG_PRINTLN(par_mqtt_server);
      LOG_PRINT("mqtt port:");
      LOG_PRINTLN(par_mqtt_port);
      /*
      LOG_PRINT("mqtt user:");
      LOG_PRINTLN(par_mqtt_username);
      LOG_PRINT("mqtt password:");
      LOG_PRINTLN(par_mqtt_password);
      */


/*
      // send a response before attemting to connect to the WiFi network
      // because it will reset the SoftAP and disconnect the client station
      webClient.println(F("HTTP/1.1 200 OK"));
      webClient.println(F("Connection: close"));
      webClient.println(F("Refresh: 10"));
      webClient.println();

      webClient.println(FPSTR(HTTP_HEAD));
      webClient.println(FPSTR(HTTP_STYLE));
      webClient.println(FPSTR(HTTP_SCRIPT));
      webClient.println(FPSTR(HTTP_HEAD_END));
      webClient.println(FPSTR(HTTP_PORTAL_WAITING));
      webClient.println(FPSTR(HTTP_END));


      webClient.stop();
*/
      LOG_PRINTLN();

      par_ssid.toCharArray(configData.wifi_ssid,100);
      par_pass.toCharArray(configData.wifi_password,100);
      par_mqtt_server.toCharArray(configData.mqtt_server,100);
    //  par_mqtt_username.toCharArray(configData.mqtt_username,100);
    //  par_mqtt_password.toCharArray(configData.mqtt_password,100);

      configData.mqtt_port = par_mqtt_port;
      configData.valid = true;


      webServer.end();
      WiFi.endAP(true);
      LOG_PRINTLN("END AP");

      if(connectWifi()){
        connectMqtt();
        return;
      }else{
        LOG_PRINTLN("Connection failed");
        return;
      }

    } else {

      webClient.println(F("HTTP/1.1 200 OK"));
      webClient.println(F("Connection: close"));
      webClient.println();


      webClient.println(FPSTR(HTTP_HEAD));
      webClient.println(FPSTR(HTTP_STYLE));
      webClient.println(FPSTR(HTTP_SCRIPT));
      webClient.println(FPSTR(HTTP_HEAD_END));
      webClient.println(FPSTR(HTTP_PORTAL_HEADER));

      String form = FPSTR(HTTP_PORTAL_FORM);

      form.replace("{select_options}", networkScanSelectOptions);

          webClient.println(form);

      //webClient.println(FPSTR(HTTP_PORTAL_FORM));
      webClient.println(FPSTR(HTTP_END));


      webClient.stop();
      int status = WiFi.status();
      if (status == WL_CONNECTED) {
        LOG_PRINTLN("Connection successful. Ending AP.");
        //webServer.end();
        //WiFi.endAP(true);
      }
    } 
  }


}




void EspATMqtt::printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  LOG_PRINT("IP Address: ");
  LOG_PRINTLN(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  LOG_PRINT("MAC address: ");
  printMacAddress(mac);

}

void EspATMqtt::printCurrentNet() {
  // print the SSID of the network you're attached to:
  LOG_PRINT("SSID: ");
  LOG_PRINTLN(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  LOG_PRINT("BSSID: ");

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  LOG_PRINT("signal strength (RSSI):");
  LOG_PRINTLN(rssi);

}

//https://bbs.espressif.com/viewtopic.php?t=6034
//https://www.esp8266.com/viewtopic.php?f=8&t=10617
void EspATMqtt::forceResetDhcpMode(){

  String response = "";
    Serial.print("AT+CWDHCP=1,1\r\n"); // send the read character to the esp8266
    long int time = millis();
    while( (time+1000) > millis())
    {
      while(Serial.available())
      {
        // The esp has data so display its output to the serial window
        char c = Serial.read(); // read the next character.
        response+=c;
      }
    }

    LOG_PRINT("RESET MODULE: " +response);
    delay(2000);

    //return response == "OK";
  }
  void EspATMqtt::forceSTAMode(){

    String response = "";
    Serial.print("AT+CWMODE=1\r\n"); // send the read character to the esp8266
    long int time = millis();
    while( (time+1000) > millis())
    {
      while(Serial.available())
      {
        // The esp has data so display its output to the serial window
        char c = Serial.read(); // read the next character.
        response+=c;
      }
    }

    LOG_PRINT("SET WIFI MODE: " +response);
    delay(2000);

  }


  void EspATMqtt::scanNetwork() {

    networkScanSelectOptions = "";
  // scan for nearby networks:
    LOG_PRINTLN("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    {
      LOG_PRINTLN("Couldn't get a WiFi connection");
      return;
    }

  // print the list of networks seen:
    LOG_PRINT("number of available networks: ");
    LOG_PRINTLN(numSsid);


  // print the network number and name for each network found:
    for (int thisNet = 0; thisNet < numSsid; thisNet++) {    
      LOG_PRINT(thisNet + 1);
      LOG_PRINT(") ");
      LOG_PRINT("Signal: ");
      LOG_PRINT(WiFi.RSSI(thisNet));
      LOG_PRINT(" dBm");
      LOG_PRINT("\tChannel: ");
      LOG_PRINT(WiFi.channel(thisNet));
      byte bssid[6];
      LOG_PRINT("\t\tBSSID: ");
    //printMacAddress(WiFi.BSSID(thisNet, bssid));
      LOG_PRINT("\tEncryption: ");
      LOG_PRINT(getEncryptionType(WiFi.encryptionType(thisNet)));
      LOG_PRINT("\t\tSSID: ");
      LOG_PRINTLN(WiFi.SSID(thisNet));
      SerialUSB.flush();

      networkScanSelectOptions += "<option value=\'"+String(WiFi.SSID(thisNet))+"\'>";
      networkScanSelectOptions += String(WiFi.SSID(thisNet)) + "("+String(getEncryptionType(WiFi.encryptionType(thisNet)))+")";
      networkScanSelectOptions += "</option>";


    }


    LOG_PRINTLN();

  }

  char* EspATMqtt::getEncryptionType(int thisType) {
  // read the encryption type and print out the name:
    switch (thisType) {
      case ENC_TYPE_WEP:
      return "WEP";
      break;
      case ENC_TYPE_TKIP:
      return "WPA";
      break;
      case ENC_TYPE_CCMP:
      return "WPA2";
      break;
      case ENC_TYPE_NONE:
      return "None";
      break;
      case ENC_TYPE_AUTO:
      return "Auto";
      break;
      case ENC_TYPE_UNKNOWN:
      default:
      return "Unknown";
      break;
    }
  }


  void EspATMqtt::printApStatus() {

    char ssid[33];
    WiFi.apSSID(ssid);
    LOG_PRINT("AP SSID: ");
    LOG_PRINTLN(ssid);

    LOG_PRINT("AP can handle ");
    LOG_PRINT(WiFi.apMaxConnections());
    LOG_PRINTLN(" stations.");

    if (WiFi.apEncryptionType() == ENC_TYPE_NONE) {
      LOG_PRINTLN("AP is open (no encryption)");
    } else {
      LOG_PRINT("Encryption of AP is ");
      switch (WiFi.apEncryptionType()) {
        case ENC_TYPE_TKIP:
        LOG_PRINTLN("WPA.");
        break;
        case ENC_TYPE_CCMP:
        LOG_PRINTLN("WPA2.");
        break;
      }
      char pass[65];
      WiFi.apPassphrase(pass);
      LOG_PRINT("AP passphrase: ");
      LOG_PRINTLN(pass);
    }

    uint8_t mac[6];
    WiFi.apMacAddress(mac);
    LOG_PRINT("AP MAC: ");
    printMacAddress(mac);

    IPAddress ip = WiFi.apIP();
    LOG_PRINT("AP IP Address: ");
    LOG_PRINTLN(ip);
  }


  void EspATMqtt::printMacAddress(byte mac[]) {
    for (int i = 5; i >= 0; i--) {
      if (mac[i] < 16) {
        LOG_PRINT("0");
      }
      LOG_PRINT(mac[i]);
      if (i > 0) {
        LOG_PRINT(":");
      }
    }
    LOG_PRINTLN();
  }



  boolean EspATMqtt::clearConfigurationData(){
    wifiRetryCount = 0;
    mqttRetryCount = 0;
    currentMode = 0;
    configData.valid = false;
    LOG_PRINTLN("Clear configuration");

    saveConfigurationData();

  }

  boolean EspATMqtt::saveConfigurationData(){

    flash_storage.write(configData);

    LOG_PRINTLN("Save configuration");
    return true;
  }

  boolean EspATMqtt::restoreConfigurationData(){

    configData = flash_storage.read();
    LOG_PRINTLN("Restore configuration");

    return true;

  }

  boolean EspATMqtt::hasConfigurationData(){

    return configData.valid && configData.wifi_ssid != NULL && configData.mqtt_server != NULL;

  }


  boolean EspATMqtt::connectMqtt(){

  /*CONNECT MQTT SERVER*/
    mqttClient.setServer(configData.mqtt_server, configData.mqtt_port);
    mqttClient.setCallback(mqttMessageCallback);
  }


  void EspATMqtt::mqttLoop(){

    int status = WiFi.status();
    if (status == WL_CONNECTED) {
      if (!mqttClient.connected()) {
        mqttReconnect();
      }
      mqttClient.loop();
    }
  }


  boolean EspATMqtt::mqttReconnect() {

  // Loop until we're reconnected
    while (!mqttClient.connected() && !mqttConnectionAborted) {
      LOG_PRINT("Attempting MQTT connection...");
    // Attempt to connect

      boolean conn = false;
      if(configData.mqtt_username != NULL && strcmp(configData.mqtt_username,"") != 0 ){

        conn =  mqttClient.connect(mqttClientName,configData.mqtt_username,configData.mqtt_password);

      }else{
       conn =  mqttClient.connect(mqttClientName);

      }

      if (conn) {
        LOG_PRINTLN("connected");


        mqttRetryCount = 0;
        currentMode = MQTT_ENABLED;

        mqttConnectionCallback();
        saveConfigurationData();
        return true;

      } else {
        LOG_PRINT("failed, rc=");
        LOG_PRINT(mqttClient.state());
        LOG_PRINTLN(" try again in 5 seconds");
      // Wait 5 seconds before retrying
        mqttRetryCount++;

        if(mqttRetryCount > MQTT_MAX_RETRY){

          mqttConnectionAborted = true;
          return false;
        }

        delay(5000);
      }
    }
  }


  void EspATMqtt::mqttPublish(char* topic,char* message){

    mqttClient.publish(topic, message);


  }

  void EspATMqtt::keepAlive(){

  /*Se non è stata configurata la wifi o la connessione con mqtt broker è fallita
  * viene riaperto il portale di configurazione
  */
    if(wifiConnectionAborted || mqttConnectionAborted){
      wifiConnectionAborted = false;
      mqttConnectionAborted = false;
      wifiRetryCount = 0;
      mqttRetryCount = 0;
      clearConfigurationData();
      startWifiManager();

    }
  }

  void EspATMqtt::mqttSubscribe(char* topic){
    mqttClient.subscribe(topic);
  } 







