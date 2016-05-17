
// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"
#include "ServerImplement.h"

// used for multi-threading
#include <process.h>

void serverLoop(void *);
void clientLoop(void);

ServerImplement * server;

extern HCILL_State_t HCILL_State;


int main()
{

   // initialize the server
    server = new ServerImplement();

	// create thread with arbitrary argument for the run function
	_beginthread( serverLoop, 0, (void*)12);


	//Thiswill hold the console so that we can see the output from ServerNetwork and 
	//it can be extended to do other things.
	while(true)
	{

	}

	return 0;
}

void serverLoop(void * arg) 
{ 
    while(true) 
    {
        server->update();
    }
}

void clientLoop()
{
    while(true)
    {
        //do game stuff
       //client->update();
    }
}