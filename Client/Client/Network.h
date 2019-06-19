#ifndef NETWORK_H
#define NETWORK_H
#include<Windows.h>

#define PORT 6666

class Network {
public:
	int initNetwork();
	void sendMessage(std::string message);
	char* reciveMessage();
	void closeConnection();

private:
	sockaddr_in addr;
	SOCKET sd;
	std::string m_ipAddress;
public:
	std::string m_username;
	int m_loop = 1;
	std::string m_message;

};

#endif
