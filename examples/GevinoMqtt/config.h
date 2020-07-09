#define AP_SSID "MyGevino001"
#define MQTT_CLIENT_ID "Gevino-0001"

#define STATUS_TOPIC "Gevino-0001/status"
#define SET_TOPIC "Gevino-0001/set"
#define RESET_TOPIC "Gevino-0001/reset"
#define RESPONSE_TOPIC "Gevino-0001"

//macro for detection af rasing edge
#define RE(signal, state) (state=(state<<1)|(signal&1)&3)==1
