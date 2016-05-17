#pragma once
#include <winsock2.h>
#include <Windows.h>

class NetworkServices
{

	public:

	//wrapper functions which will be used by both the server and the client
	static int sendMessage(SOCKET curSocket, char * message, int messageSize);
	static int receiveMessage(SOCKET curSocket, char * buffer, int bufSize);

};

