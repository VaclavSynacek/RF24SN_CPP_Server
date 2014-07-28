#include "RF24SN.h"

#include "RF24SN-Packet.h"
#include "RF24SN-MQTT-Wrapper.h"
#include "RF24SN-RF24-Wrapper.h"

#include <iostream>

using namespace std;

void setup(void)
{
	mqttSetup();
	RF24Setup();
}

void processPacket(Packet packet)
{
	
	if (packet.packetType == PUBLISH_PACKET) processPublishPacket(packet);
	else if (packet.packetType == REQUEST_PACKET) processRequestPacket(packet);
	else
	{
		cerr << "got strangely corrupted packet: " << endl;
		printPacket(packet);
	}	
	
}

void processPublishPacket(Packet packet)
{
	cout << "publish: ";
	printPacket(packet);
	packet.packetType = PUBACK_PACKET;
	sendPacket(packet);
		
	mqttPublish(packet.nodeId, packet.sensorId, packet.value);
}

void processRequestPacket(Packet packet)
{
	cout << "request: ";
	printPacket(packet);
	packet.packetType = RESPONSE_PACKET;
	
	packet.value = mqttRequest(packet.nodeId, packet.sensorId);
	
	sendPacket(packet);
}

void loop(void)
{
	RF24Loop();
	mqttLoop();
}


int main(int argc, char** argv)
{
//TODO parse arguments to handle "--vervose" to prety print received packets

        setup();
        cout << "setup done, entering loop" << endl;
        while(1)
                loop();

        return 0;
}
