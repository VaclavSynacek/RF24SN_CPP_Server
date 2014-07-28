#include "RF24SN-MQTT-Wrapper.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include <mosquitto.h>

using namespace std;

std::map<string, float> messageStore;

struct mosquitto *mqtt;

void onMessage(struct mosquitto *m, void *udata, const struct mosquitto_message *msg)
{
    if (msg == NULL) { return; }
    
    float value;
    const char* pch = ((char *)msg->payload);
    stringstream(pch) >> value;
    
    string topic = msg->topic;
    
    messageStore[topic] = value;

    cout << "received MQTT message, topic: " <<  topic << " value: " << value << endl;
}

void mqttSetup(void)
{
	mqtt = mosquitto_new(NULL, true, NULL);
	mosquitto_connect(mqtt, "localhost", 1883, 60);

	mosquitto_message_callback_set(mqtt, onMessage);
	mosquitto_subscribe(mqtt, NULL, "RF24SN/out/+/+", 0);
}


void mqttPublish(int nodeId, int sensorId, float value)
{
    string topic = mqttTopic(nodeId, sensorId, true);
	
	std::stringbuf payload;      
  	std::ostream ps (&payload);  
	ps << value;
	
	mosquitto_publish(mqtt, NULL, topic.c_str(), payload.str().size(), payload.str().c_str(), 0, false);
}

float mqttRequest(int nodeId, int sensorId)
{
    string topic = mqttTopic(nodeId, sensorId, true);
	return messageStore[topic];
}


string mqttTopic(int nodeId, int sensorId, bool in)
{
	
	std::stringbuf topic;      
  	std::ostream ts (&topic);  
	ts << "RF24SN/" << (in?"in":"out") << "/" << nodeId << "/" << sensorId;
	
	return topic.str();
	
}


void mqttLoop(void)
{

    mosquitto_loop(	mqtt, 50, 42 /* the answer to life universe and everything */ );

}
	

