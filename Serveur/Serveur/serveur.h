#ifndef SERVEUR_H
#define SERVEUR_H

#include <string>


#define ERR_MESSAGE "ERREUR"
#define ERR_MESSAGE_LEN 6
#define BUFFER_SIZE 2048
#define PORT 6666

#ifdef _WIN32
#include <Windows.h>
#include <WinBase.h>
#endif 

#ifndef _WIN32
#define _popen(a, b) popen(a,b) 
#define _pclose(c) pclose(c)
#define closesocket(d) close(d)

typedef int SOCKET //defini socket pour linux
#endif

class Serveur
{
public:
	Serveur();
	~Serveur();
	void RecvMessage();
	void SndMessage();

private:
	SOCKET m_sd;
	sockaddr_in m_client;
	char m_buffer[BUFFER_SIZE];
	int m_fermeture;
	std::string connected;
};
#endif