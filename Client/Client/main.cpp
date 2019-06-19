#include <iostream>
#include "Network.h"
#include <string>


int main()
{
	
	
	Network network;
	
	network.initNetwork();

	std::string message;
	while (network.m_loop == 1) 
	{

		std::getline(std::cin, message);
		message = network.m_username + ": " + message;
		network.sendMessage(message);
		
		char* buffer = network.reciveMessage();
		
		std::cout << "Server: " << buffer << std::endl;
		if (strstr(buffer, "Server disconnected."))
		{
			network.m_loop = 0;
		}
			
 	}
	
	network.closeConnection();
	system("Exit");
}