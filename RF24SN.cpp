#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <map>  

#include "RF24.h"
#include "mosquitto.h"

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

std::map<string, float> messageStore;

struct mosquitto *mqtt;

static void on_message(struct mosquitto *m, void *udata, const struct mosquitto_message *msg)
{
    if (msg == NULL) { return; }
    
    float value;
    const char* pch = ((char *)msg->payload);
    stringstream(pch) >> value;
    
    string topic = msg->topic;
    
    messageStore[topic] = value;

    cout << "received MQTT message, topic: " <<  topic << " value: " << value << endl;
}



void setup(void)
{

	mqtt = mosquitto_new(NULL, true, NULL);
	mosquitto_connect(mqtt, "localhost", 1883, 60);

	mosquitto_message_callback_set(mqtt, on_message);
	mosquitto_subscribe(mqtt, NULL, "RF24SN/out/+/+", 0);


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
	
	std::stringbuf topic;      
  	std::ostream ts (&topic);  
	ts << "RF24SN/in/" << ((int) packet.nodeId) << "/" << ((int) packet.sensorId);
	
	std::stringbuf payload;      
  	std::ostream ps (&payload);  
	ps << packet.value;
	
	mosquitto_publish(mqtt, NULL, topic.str().c_str(), payload.str().size(), payload.str().c_str(), 0, false);
}

void processRequestPacket(Packet packet)
{
	cout << "request: ";
	printPacket(packet);
	packet.packetType = RESPONSE_PACKET;
	
	std::stringbuf topic;      
  	std::ostream ts (&topic);  
	ts << "RF24SN/out/" << ((int) packet.nodeId) << "/" << ((int) packet.sensorId);
	
	packet.value = messageStore[topic.str()];
	
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

    mosquitto_loop(	mqtt, 50, 42 /* the answer to life universe and everything */ );

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
