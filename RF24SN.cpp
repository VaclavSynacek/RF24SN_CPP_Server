#include <cstdlib>
#include <iostream>

#include "RF24.h"

//TODO load from program arguments
RF24 radio("/dev/spidev0.0",8000000 , 25);

uint64_t base_pipe = 0xF0F0F0F000LL;      // address of the pipe to the base.

typedef struct{
  uint8_t packetType; 		// 0 reserved, 1=publish, 2=puback, 3=request, 4=response
  uint8_t reserved;
  uint8_t nodeId; 			//sensor node id, also used for return address calcualtion
  uint8_t sensorId; 		//sensor id
  float value; 				//sensor reading
} Packet;

    static const uint8_t NO_PACKET = 0;
    static const uint8_t PUBLISH_PACKET = 1;
    static const uint8_t PUBACK_PACKET = 2;
    static const uint8_t REQUEST_PACKET = 3;
    static const uint8_t RESPONSE_PACKET = 4;



void setup(void)
{

  radio.begin();
  radio.setPayloadSize(sizeof(Packet));
  //radio.setChannel(4);
  //radio.setDataRate(RF24_250KBPS);
  //radio.setRetries(0,15);

  radio.openReadingPipe(1,base_pipe);
  radio.setAutoAck(1, false);
  radio.startListening();

//TODO prety print based on --verbose argument
//  printf("\n\rRF24SensorHub!\n");
  radio.printDetails();

  printf("size of packet   = %hu\n", sizeof(Packet));

}


void sendPacket(Packet packet)
{
	radio.stopListening();
	radio.openWritingPipe(base_pipe+packet.nodeId); //open pipe unique to the node
	radio.setAutoAck(0, false);
	radio.write( &packet, sizeof(Packet) );
	radio.startListening();
}

void printPacket(Packet packet)
{
	cout << "packetType: " << ((int)packet.packetType) << " nodeId: " << ((int)packet.nodeId) << " sensorId: " << ((int)packet.sensorId)<< " value: " << packet.value << endl;
} 

void processPublishPacket(Packet packet)
{
	cout << "publish: ";
	printPacket(packet);
	packet.packetType = PUBACK_PACKET;
	sendPacket(packet);
	//TODO MQTT publish code here
}

void processRequestPacket(Packet packet)
{
	cout << "request: ";
	printPacket(packet);
	packet.packetType = RESPONSE_PACKET;
	packet.value = 3.14; //TODO replace with value obtained from MQTT
	sendPacket(packet);
}

void loop(void)
{

    if ( radio.available() )
    {
        Packet packet;
        radio.read( &packet, sizeof(Packet) );

		if (packet.packetType == PUBLISH_PACKET) processPublishPacket(packet);
		else if (packet.packetType == REQUEST_PACKET) processRequestPacket(packet);
		else
		{
			cerr << "got strangely corrupted packet: " << endl;
			printPacket(packet);
		}
		


    }

    //to keep system load low
    delay(100);

}


	


int main(int argc, char** argv)
{
//TODO parse arguments to handle "--vervose" to prety print received packets

        setup();
        cout << "setup done, entering listening loop" << endl;
        while(1)
                loop();

        return 0;
}
