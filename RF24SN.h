#ifndef RF24SN_h
#define RF24SN_h

#include "RF24SN-Packet.h"

void processPacket(Packet packet);
void processPublishPacket(Packet packet);
void processRequestPacket(Packet packet);

void setup(void);
void loop(void);
int main(int argc, char** argv);

#endif