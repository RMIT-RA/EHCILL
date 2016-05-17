
//#include "stdafx.h" 
#include "NetworkServices.h"

//This funcion takes a socket type object to send the message through, 
//a pointer to the buffer where the message we want to send is located, 
//and the size of the message. 
//Function will return an int value representing the number of bytes it successfully sent 
//or an error value if there was a problem sending through the socket.
int NetworkServices::sendMessage(SOCKET curSocket, char * message, int messageSize)
{
    return send(curSocket, message, messageSize, 0);
}

//This functiontakes a socket type object to check for any data that is available 
//for reading at that socket on the network. It will place any data read into our "buffer", 
//and will require a buffer size to indicate how much is the maximum it can read each time receiveMessage() is called.
//The function returns an int value representing the number of bytes it read into our buffer 
//or an error if there was a problem receiving from the socket.
int NetworkServices::receiveMessage(SOCKET curSocket, char * buffer, int bufSize)
{
    return recv(curSocket, buffer, bufSize, 0);
}