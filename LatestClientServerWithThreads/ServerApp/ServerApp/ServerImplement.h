#include <thread>
#include <iostream>
#include "ServerNetwork.h"
#include "NetworkData.h"

class ServerImplement
{

public:

	std::thread recv_Thread;
	std::thread send_Thread;

	//used as a Global variable
	int CTS_FLAG;
	int RTS_FLAG;
	int timer;
	HCILL_State_t HCILL_State;

    ServerImplement(void);
    ~ServerImplement(void);

    void update();

	void receiveFromClients();
	void sendToClients();

	//Returns 'true' if the data received is an EHCILL opcode and 'false' otherwise
	int IsHCILLOpCode(char data);

	//Initializes the EHCILL state machine
	void HCILL_Init(void);

	//This function will return the current state of state machine.
	HCILL_State_t HCILL_GetState(void);

	void sendPacket(PacketTypes pkt_type);

	

private:

   // IDs for the clients connecting for table in ServerNetwork 
    static unsigned int client_id;

   // The ServerNetwork object 
    ServerNetwork* network;

	// data buffer
	char network_data[MAX_PACKET_SIZE];

	int data_length;

};