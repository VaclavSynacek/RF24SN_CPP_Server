#ifndef RF24SN_MQTT_Wrapper_h
#define RF24SN_MQTT_Wrapper_h

#include <string>

void onMessage(struct mosquitto *m, void *udata, const struct mosquitto_message *msg);
void mqttSetup(void);
void mqttPublish(int nodeId, int sensorId, float value);
float mqttRequest(int nodeId, int sensorId);
std::string mqttTopic(int nodeId, int sensorId, bool in);
void mqttLoop(void);

#endif