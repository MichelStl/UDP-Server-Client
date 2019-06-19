#include <iostream>
#include <chrono>
#include <ctime>
#include "serveur.h"
#include <array>
#include <stdio.h>
#include <regex>

Serveur::Serveur()
{
	WSADATA wsaData;
	if (WSAStartup(0x0101, &wsaData) != 0)
	{
		std::cout << "Network Init Error." << std::endl;
		system("Pause");
		return;
	}


	m_sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sd == INVALID_SOCKET) //Validation socket
	{
		std::cout << "Socket Error." << std::endl;
		WSACleanup();
		system("Pause");
		return;
	}

	//créer structure memoire pour serveur
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT); //Cast en bigendian
	addr.sin_addr.s_addr = htonl(INADDR_ANY);//Ecoute sur filaire wifi...

	//Associer la structure au socket (bind)
	if (bind(m_sd, (sockaddr*)&addr, sizeof(addr)) != 0)
	{
		std::cout << "Bind Error." << std::endl;
		closesocket(m_sd); // Ferme le socket
		WSACleanup();
		system("Pause");
		return;
	}

	while (m_fermeture != 1)
	{
		RecvMessage();
		SndMessage();
	}
}
Serveur::~Serveur()
{
	closesocket(m_sd); // Ferme le socket
	WSACleanup(); // Ferme l'environnement réseau pour windows
}

void Serveur::RecvMessage()
{
	//Recevoir un paquet bloquant
	int clientlen = sizeof(m_client);

	int count = recvfrom(m_sd, m_buffer, sizeof(m_buffer) - 1, 0, (sockaddr*)&m_client, &clientlen);

	if (count < 0)
	{
		std::cout << ERR_MESSAGE << ": " << "Buffer overflow!" << std::endl;

		for (int i = 0; i <= BUFFER_SIZE; i++)//Protection contre l'injection.
			m_buffer[i] = '\0';
		for (int i = 0; i < ERR_MESSAGE_LEN; i++)
			m_buffer[i] = ERR_MESSAGE[i];
	}

	m_buffer[count] = '\0';
	std::cout << m_buffer << std::endl;
}

void Serveur::SndMessage()
{
	std::regex reg("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
	std::smatch matchs;
	//Traiter le paquet et repondre
	std::string commande(m_buffer);

	if (std::regex_match(commande, reg))
	{
		connected = "Connected.";
		sendto(m_sd, connected.c_str(), connected.length(), 0, (const sockaddr*)&m_client, sizeof(m_client));
	}
	int pos = commande.find(": ");
	commande.replace(0, pos + 1, "");

	if (commande.find("bye ") == 0)
	{
		connected = "Disconnected.";
		sendto(m_sd, connected.c_str(), connected.length(), 0, (const sockaddr*)&m_client, sizeof(m_client));
		//m_fermeture = 1;
	}
	else if (commande.find("date ") == 0)
	{
		auto s = std::chrono::system_clock::now();
		auto e = std::chrono::system_clock::now();
		std::time_t date = std::chrono::system_clock::to_time_t(e);
		char dateTime[30];
		ctime_s(dateTime, sizeof(dateTime), &date);
		commande = dateTime;
		sendto(m_sd, commande.c_str(), commande.length(), 0, (const sockaddr*)&m_client, sizeof(m_client));

	}
	else if (commande.find("user ") == 0)
	{
#ifdef _WIN32
		const unsigned long bufferSize = 255;
		unsigned long size = bufferSize;
		char usager[bufferSize];
		GetUserName(usager, &size);
		commande = usager;
		sendto(m_sd, commande.c_str(), commande.length(), 0, (const sockaddr*)&m_client, sizeof(m_client));
#endif
	}
	else if (commande.find("exec ") == 0)
	{
		commande.erase(0, 5);
		int count = 0;
		int charCount = 0;
		std::string comm = commande.substr();
		char* cmd = new char[comm.size() + 1];
		strcpy_s(cmd, strlen(cmd), comm.c_str());

		std::string result = "";
		FILE* pipe = _popen(cmd, "r");

		if (!pipe)
		{
			sendto(m_sd, ERR_MESSAGE, ERR_MESSAGE_LEN, 0, (const sockaddr*)&m_client, sizeof(m_client));
			return;
		}
		try {
			while (fgets(m_buffer, sizeof(m_buffer), pipe) != NULL && count < 10 && charCount <= 300) {
				result += m_buffer;
				charCount += result.size();
				count++;
			}
		}
		catch (...) {
			sendto(m_sd, ERR_MESSAGE, ERR_MESSAGE_LEN, 0, (const sockaddr*)&m_client, sizeof(m_client));
			_pclose(pipe);
			throw;
		}
		if (result == "")
		{
			_pclose(pipe);
			sendto(m_sd, ERR_MESSAGE, ERR_MESSAGE_LEN, 0, (const sockaddr*)&m_client, sizeof(m_client));
			return;
		}

		_pclose(pipe);

		sendto(m_sd, result.c_str(), result.length(), 0, (const sockaddr*)&m_client, sizeof(m_client));
	}
	else if (commande != "" && !std::regex_match(commande, reg))
	{
		std::cout << "Answer: ";
		std::cin >> commande;
		sendto(m_sd, commande.c_str(), commande.length(), 0, (const sockaddr*)&m_client, sizeof(m_client));

	}
}
