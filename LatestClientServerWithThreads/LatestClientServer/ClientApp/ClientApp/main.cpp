
// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"

#include "ClientImplement.h"
// used for multi-threading
#include <process.h>

void serverLoop(void *);
void clientLoop(void);


ClientImplement * client;


int main()
{


	// create thread with arbitrary argument for the run function
	//_beginthread( serverLoop, 0, (void*)12);

	// initialize the client
	client = new ClientImplement();

	//Thiswill hold the console so that we can see the output from ServerNetwork and 
	//it can be extended to do other things.
	clientLoop();
}

void serverLoop(void * arg) 
{ 
    while(true) 
    {
       // server->update();
    }
}

void clientLoop()
{
    while(true)
    {
        //do game stuff
       client->update();
    }
}