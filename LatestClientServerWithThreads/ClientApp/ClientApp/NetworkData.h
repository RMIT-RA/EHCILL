#pragma once
#include <string.h>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

    INIT_CONNECTION = 0x00,
    INIT_ACK = 0x01,
	HCILL_GO_TO_SLEEP_IND = 0x30,
	HCILL_GO_TO_SLEEP_ACK = 0x31,
	HCILL_WAKE_UP_IND = 0x32,
	HCILL_WAKE_UP_ACK = 0x33,
	BC_CTS_HIGH = 0x02,
	BC_CTS_LOW = 0x03,
	HOST_RTS_HIGH = 0x04,
	HOST_RTS_LOW = 0x05

};

//This contains the different states for HCILL state machine
enum HCILL_State_t
{ 
   hsAwake=0,
   hsWaitForWakeAck,
   hsHostInitWakeup,
   hsControllerInitWakeup,
   hsSleep
};

// This defines a packet struct to serve as a container to our data to be sent.
//The packet_type field will be filled using the values in the enumerator we just created.
//The serialize function is used to convert the packet_type data into bytes that we can send over the network.
//The deserialize function is used to convert bytes received over the network back into packet_type data that we can interpret.
struct Packet {

    unsigned int packet_type;

    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};