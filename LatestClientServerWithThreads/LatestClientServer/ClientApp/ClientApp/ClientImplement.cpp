//#include "stdafx.h"
#include "ClientImplement.h" 

#define ON  1
#define OFF  0

#undef BC_INIT_WAKEUP 1
//#define BC_INIT_WAKEUP 1

ClientImplement::ClientImplement(void)
{
    network = new ClientNetwork();

	HCILL_Init();

	//re-set the CTS_FLAG
	CTS_FLAG = ON;

	//set the RTS_FLAG
	RTS_FLAG = OFF;



	// send init packet
	// Having the client send an INIT_CONNECTION packet when it is first connected to the server inside its constructor
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = INIT_CONNECTION;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//This initializes the EHCILL state machine
void ClientImplement::HCILL_Init(void)
{
   HCILL_State  = HCILL_State_t::hsAwake;
   //HCILL_Lock   = 0;
}

void ClientImplement::sendPacket(PacketTypes pkt_type)
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
	packet.packet_type = pkt_type;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//This function which will continuously receive action packets from the server 
//and send action packets in repsonse
void ClientImplement::update()
{
    Packet packet;
    int data_length = network->receivePackets(network_data);

    if (data_length <= 0) 
    {
        //no data recieved
        return;
    }

    int i = 0;
    while (i < (unsigned int)data_length) 
    {
        packet.deserialize(&(network_data[i]));
        i += sizeof(Packet);

        switch (packet.packet_type) {

			case INIT_ACK:
				printf("\n\n\nBC received INIT_ACK from the Host\n");	
				Sleep(2000); //Manual Sleep
				sendPacket(HCILL_GO_TO_SLEEP_IND);
				break;

			case HOST_RTS_HIGH:
				printf("Host pulls RTS high\n");
				RTS_FLAG = ON;
				
				break;

			case HCILL_GO_TO_SLEEP_ACK:
				printf("BC got ACK and is going to sleep....\n");
				HCILL_State  = HCILL_State_t::hsSleep;
				//Sleep for 5s and initiate wakeup (by baseband controller)
				Sleep(6000);

#if defined (BC_INIT_WAKEUP)
				//************* BASEBAND CONTROLLER INITIATED WAKEUP ******************
				printf("\n\nBASEBAND CONTROLLER INITIATED WAKEUP\n");
				HCILL_State = HCILL_State_t::hsControllerInitWakeup;
				sendPacket(BC_CTS_HIGH);
				CTS_FLAG = ON;
				Sleep(3000);  //equivalent to 150us
				sendPacket(BC_CTS_LOW);
				CTS_FLAG = OFF;
				//*********************************************************************
#endif // defined (BC_INIT_WAKEUP)
				break;

			case HOST_RTS_LOW:
				printf("\n\nHost pulls RTS low\n");
				RTS_FLAG = OFF;
				Sleep(2000); //Manual Sleep
				if(HCILL_State == HCILL_State_t::hsControllerInitWakeup) 
				{
					sendPacket(HCILL_WAKE_UP_IND);
				}
				else //if(HCILL_State == HCILL_State_t::hsHostInitWakeup)
				{
					sendPacket(HCILL_WAKE_UP_ACK);
					HCILL_State == HCILL_State_t::hsAwake;
					printf("BC waking up....\n");
				}
				break;

			case HCILL_WAKE_UP_ACK:
				printf("BC got ACK and is waking up...\n");
				HCILL_State = HCILL_State_t::hsAwake;
				//Be awake for 6s
				Sleep(6000);

				break;
				
			case HCILL_WAKE_UP_IND:
				printf("\n\nbGot WAKE_UP_IND from Host.... So this is Host Initiated Wakeup.\n");
				Sleep(2000); //Manual Sleep
				if(RTS_FLAG == OFF)
				{
					sendPacket(HCILL_WAKE_UP_ACK);
				}
				break;

			default:
					printf("error in packet types\n");
					break;
        }
    }
}