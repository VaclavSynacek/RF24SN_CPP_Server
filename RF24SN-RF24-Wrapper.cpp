#include "RF24SN-RF24-Wrapper.h"

#include <iostream>

#include "RF24.h"

//TODO load from program arguments
RF24 radio("/dev/spidev0.0",8000000 , 25);

uint64_t base_pipe = 0xF0F0F0F000LL;      // address of the pipe to the base.

void RF24Setup(void)
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

void RF24Loop(void)
{

    if ( radio.available() )
    {
        Packet packet;
        radio.read( &packet, sizeof(Packet) );
		processPacket(packet);
    }

}
