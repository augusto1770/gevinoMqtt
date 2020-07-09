  #include <Arduino.h>
  #include <WiFiEspAT.h>
  #include <FlashStorage.h>
  #include <PubSubClient.h>

  #define LOG_OUTPUT SerialUSB
  #define LOG_PRINT(msg) LOG_OUTPUT.print(msg)
  #define LOG_PRINTLN(msg) LOG_OUTPUT.println(msg)

  #define WIFI_MAX_RETRY 5
  #define MQTT_MAX_RETRY 5

  #if defined(ESP8266) || defined(ESP32)
  #include <functional>
  #define MQTT_MESSAGE_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> mqttMessageCallback
  #define MQTT_CONNECTION_CALLBACK_SIGNATURE std::function<void()> mqttConnectionCallback
  #define WIFI_CONNECTION_CALLBACK_SIGNATURE std::function<void()> wifiConnectionCallback

  #else
  #define MQTT_MESSAGE_CALLBACK_SIGNATURE void (*mqttMessageCallback)(char*, uint8_t*, unsigned int)
  #define MQTT_CONNECTION_CALLBACK_SIGNATURE void (*mqttConnectionCallback)()
  #define WIFI_CONNECTION_CALLBACK_SIGNATURE void (*wifiConnectionCallback)()

  #endif



#define FPSTR(p)      ((const char *)(p))

const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><meta http-equiv=\"X-UA-Compatible\"content=\"IE=edge\"><meta name=\"viewport\"content=\"width=device-width\"><title></title>";
const char HTTP_STYLE[] PROGMEM           = "<style> .c{text-align:center;}div,input{padding:5px;font-size:1em;}input{width:100%}div,select{padding:5px;font-size:1em;}select{width:100%;}body{text-align:center;font-family:verdana;}button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float:right;width:64px;text-align:right;} .l{background:url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\")no-repeat left center;background-size:1em;} </style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();} </script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body>";
const char HTTP_PORTAL_HEADER[] PROGMEM   = "<div style=\"display:inline-block;\"><div style=\"width:100%;background: #efbd00;padding:30px\" ><h1 style='text-align:center;'>Gevino MQTT Configuration</h1></div>";
const char HTTP_PORTAL_WAITING[] PROGMEM  = "<h2 style='text-align:center;'>Waiting...</h2>";
const char HTTP_PORTAL_FORM[] PROGMEM     = "<p>Enter valid parameters,please.</p><h3>Network</h3><form action='/'method='POST'><p>SSID<br/><select style='text-align:center;'name='ssid'> {select_options} </select></p><p>Password<br/><input name='pass'type='password' /></p><br/><h3>MQTT</h3><p>Server<br/><input name='mqtt_s'type='text' /></p><p>Port<br/><input name='mqtt_p'type='text' /></p><p>Username<br/><input name='mqtt_user'type='text' /></p><p>Password<br/><input name='mqtt_password'type='password' /></p><p><button type='submit'value='Submit' />Salva</button></p></form>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";




typedef struct {
  boolean valid;
  char mqtt_server[100];
  int mqtt_port;
  char wifi_ssid[100];
  char wifi_password[100];
  char mqtt_username[100];
  char mqtt_password[100];

} ConfigData;



  #define MODE_AP 1001
  #define WIFI_ENABLED 1002
  #define MQTT_ENABLED 1003




class EspATMqtt {
public:
  EspATMqtt(char* = "myAP", char* = "myMQTT");

  boolean init(Stream&);

  void setWifiConnectionCallback(WIFI_CONNECTION_CALLBACK_SIGNATURE);
  void setMqttConnectionCallback(MQTT_CONNECTION_CALLBACK_SIGNATURE);
  void setMqttMessageCallback(MQTT_MESSAGE_CALLBACK_SIGNATURE);

  void reset();
  void scanNetwork();
  boolean clearConfigurationData();

  void loop();

  void mqttSubscribe(char*);
  void mqttPublish(char* ,char* );




private:

  boolean configAP();
  void startWifiManager();
  boolean connectWifi();
  boolean checkWifiModule();
  void webserverLoop();
  void printWifiData();
  void printCurrentNet();
  void forceResetDhcpMode();
  void forceSTAMode();
  char* getEncryptionType(int);
  void printApStatus();
  void printMacAddress(byte[]);
  void mqttLoop();
  boolean connectMqtt();
  boolean mqttReconnect();
  void keepAlive();
  boolean saveConfigurationData();
  boolean restoreConfigurationData();
  boolean hasConfigurationData();


  char* apSsid;
  char* mqttClientName;


  int wifiRetryCount = 0;
  int mqttRetryCount = 0;
  int currentMode = 0;

  boolean wifiConnectionAborted = false;
  boolean mqttConnectionAborted = false;

  String networkScanSelectOptions;
  ConfigData configData;

  MQTT_MESSAGE_CALLBACK_SIGNATURE;
  MQTT_CONNECTION_CALLBACK_SIGNATURE;
  WIFI_CONNECTION_CALLBACK_SIGNATURE;

};
