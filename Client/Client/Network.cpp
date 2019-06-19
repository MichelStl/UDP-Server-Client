#include <iostream>
#include <string>
#include "Network.h";



int Network::initNetwork() {
	std::cout << "Server IP: ";
	std::cin >> m_ipAddress;
	std::cout << "Username: ";
	std::cin >> m_username;
	const char* ip = m_ipAddress.c_str();

	WSADATA wsaData;
	if (WSAStartup(0x0101, &wsaData) != 0)
	{
		std::cout << "Network init error" << std::endl;
		system("Pause");
		return 1;
	}


	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET) //Validation socket
	{
		std::cout << "Socket error" << std::endl;
		WSACleanup();
		system("Pause");
		return 1;
	}

	//créer structure memoire pour client
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT); 
	addr.sin_addr.s_addr = inet_addr(ip);//Listen on wifi...
	sendMessage(m_username + " is connected.");
	return 0;
}

void Network::sendMessage(std::string message)
{
	sendto(sd, message.c_str(), message.length(), 0, (const sockaddr*)&addr, sizeof(addr));

}

char* Network::reciveMessage()
{
	//Recevoir réponse du serveur
	int addrlen = sizeof(addr);
	char buffer[2048];
	int count = recvfrom(sd, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&addr, &addrlen);

	if (count < 0)
	{
		std::string serv = "Server disconnected.";
		strcpy_s(buffer, serv.c_str());
		closesocket(sd);
		WSACleanup();
		
	}

	buffer[count] = '\0';
	return buffer;
}

void Network::closeConnection()
{
	closesocket(sd); // Socket close
	WSACleanup();
}