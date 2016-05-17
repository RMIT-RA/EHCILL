#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"

class ClientImplement
{

public:

	int CTS_FLAG, RTS_FLAG;
	HCILL_State_t HCILL_State;

    ClientImplement();
    ~ClientImplement(void);

    ClientNetwork* network; 

	void HCILL_Init(void);

	void sendPacket(PacketTypes pkt_type);

	char network_data[MAX_PACKET_SIZE];

	void update();
};