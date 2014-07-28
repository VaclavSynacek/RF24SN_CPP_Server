#include <iostream>
#include "RF24SN-Packet.h"

using namespace std;

void printPacket(Packet packet)
{
	cout << "packetType: " << ((int)packet.packetType) << " nodeId: " << ((int)packet.nodeId) << " sensorId: " << ((int)packet.sensorId)<< " value: " << packet.value << endl;
} 
