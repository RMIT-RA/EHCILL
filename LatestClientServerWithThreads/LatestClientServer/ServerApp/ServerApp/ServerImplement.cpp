//#include "stdafx.h" 
#include "ServerImplement.h"

unsigned int ServerImplement::client_id; 

#define ON  1
#define OFF  0

//#undef HOST_INIT_WAKEUP 1
#define HOST_INIT_WAKEUP 1

ServerImplement::ServerImplement(void)
{
    // id's to assign clients for our table
    client_id = 0;

	HCILL_Init();

	//re-set the CTS_FLAG
	CTS_FLAG = ON;

	//set the RTS_FLAG
	RTS_FLAG = OFF;

	//set the timer to off
	timer = OFF;

    // set up the server network to listen 
    network = new ServerNetwork(); 

}

//This returns 'true' if the data received is an EHCILL opcode and 'false' otherwise
int ServerImplement::IsHCILLOpCode(char data)
{
   return (( (data == HCILL_GO_TO_SLEEP_IND) 
             || (data == HCILL_GO_TO_SLEEP_ACK)
             || (data == HCILL_WAKE_UP_IND)
             || (data == HCILL_WAKE_UP_ACK)
            ));
}

//This initializes the EHCILL state machine
void ServerImplement::HCILL_Init(void)
{
   HCILL_State  = HCILL_State_t::hsAwake;
   //HCILL_Lock   = 0;
}

//This will return the current state of state machine.
HCILL_State_t ServerImplement::HCILL_GetState(void)
{
   return(HCILL_State);
}

void ServerImplement::update() 
{

    // get new clients
    if(network->acceptNewClient(client_id))
    {
        printf("client %d has been connected to the server\n",client_id); 

        client_id++;
    }

	receiveFromClients();
}

//This function goes through all the clients that have been saved in the sessions table as each were connected. 
//Then call receiveData() on all of them and get the data into network_data buffer. 
//Then deserialize the packet and switch between whether it is an INIT or ACTION packet.
void ServerImplement::receiveFromClients()
{
    Packet packet;

    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;

    for(iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
    {
        // get data for that client
        int data_length = network->receiveData(iter->first, network_data);

        if (data_length <= 0) 
        {
            //no data recieved
            continue;
        }

        int i = 0;
        while (i < (unsigned int)data_length) 
        {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);

            switch (packet.packet_type) {

                case INIT_CONNECTION:

                    printf("\nHost received init packet from Baseband Controller\n");
					//Added later so that server also can send the packets back
					Sleep(2000); //Manual Sleep
					sendPacket(INIT_ACK);
                    break;

				case HCILL_GO_TO_SLEEP_IND:

                    printf("Host received GO_TO_SLEEP_IND from baseband controller\nHost is going to sleep..\n");
					if(HCILL_GetState() == HCILL_State_t::hsAwake)
					{
						Sleep(2000); //Manual Sleep
						sendPacket(HOST_RTS_HIGH);
						RTS_FLAG = ON;
						Sleep(2000); //Manual Sleep
						sendPacket(HCILL_GO_TO_SLEEP_ACK);
						HCILL_State = HCILL_State_t::hsSleep;

						//Sleep for 6 seconds and Host initiate wakeup
						Sleep(6000);

#if defined(HOST_INIT_WAKEUP)
						//***************HOST INITIATED WAKEUP ********************
						printf("\n\nHOST INITIATED WAKEUP\n");
						HCILL_State = HCILL_State_t::hsHostInitWakeup;
						sendPacket(HCILL_WAKE_UP_IND);
						sendPacket(HOST_RTS_LOW);
						RTS_FLAG = OFF;
						HCILL_State = HCILL_State_t::hsWaitForWakeAck;
						
						//*********************************************************
#endif // defined (HOST_INIT_WAKEUP)
					}
                    break;
				
				case HCILL_GO_TO_SLEEP_ACK:
					//BC never send this...so do nothing.
					break;

				case HCILL_WAKE_UP_IND:
					printf("Host received WAKE_UP_IND from baseband controller\nHost is going to wake up..\n");
					if(HCILL_State == HCILL_State_t::hsWaitForWakeAck) //& (CTS_FLAG == OFF) // Wakeup Collision 1
					{
						HCILL_State = HCILL_State_t::hsAwake;
						Sleep(2000); //Manual Sleep
						sendPacket(HCILL_WAKE_UP_IND);
					}
					else if(HCILL_State == HCILL_State_t::hsControllerInitWakeup)
					{
						Sleep(2000); //Manual Sleep
						sendPacket(HCILL_WAKE_UP_ACK);
						HCILL_State = HCILL_State_t::hsAwake;
					}
					//can even combine above two blocks with "if(HCILL_State == HCILL_State_t::hsWaitForWakeAck) | (HCILL_State == HCILL_State_t::hsControllerInitWakeup)
					break;

				case HCILL_WAKE_UP_ACK:
					printf("Host got ACK and is waking up..\n");
					if(HCILL_State == HCILL_State_t::hsWaitForWakeAck)
					{
						HCILL_State = HCILL_State_t::hsAwake;
					}
					break;

				case BC_CTS_HIGH:
					printf("\n\nBc pulls CTS high\n");
						//Start actual timer
						timer = ON;
						CTS_FLAG = ON;
						break;

				case BC_CTS_LOW:  // After waiting 150us
					if((CTS_FLAG) && (timer))
					{
						printf("BC pulls CTS low after 150us... so this is BC initieated Wakeup\n");
						//stop actual timer
						CTS_FLAG = OFF;
						timer = OFF;
						HCILL_State = HCILL_State_t::hsControllerInitWakeup;
						Sleep(2000); //Manual Sleep
						sendPacket(HOST_RTS_LOW);
						RTS_FLAG = OFF;
						//HCILL_State = HCILL_State_t::hsControllerInitWakeup;

					}
					break;
                default:

                    printf("error in packet types\n");
                    break;
            }
        }
    }
}

//Send action packets to all connected clients
void ServerImplement::sendPacket(PacketTypes pkt_type)
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
	packet.packet_type = pkt_type;

    packet.serialize(packet_data);

    network->sendToAll(packet_data,packet_size);
}