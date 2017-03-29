#if defined(_WIN64) || defined(_WIN32)

#include "socket\windows\WindowsUDPSocket.h"
#include <iostream>

using namespace EvayrNet;

WindowsUDPSocket::WindowsUDPSocket(PacketHandler* apPacketHandler, uint16_t aPort, uint8_t aTickRate)
{
	m_pPacketHandler = apPacketHandler;
	m_Port = aPort;
	SetTickRate(aTickRate);

	// Start WinSock
	WSAData data;
	int32_t result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0)
	{
		throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to initialize WSA.");
	}

	// Set up socket
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_Socket == INVALID_SOCKET)
	{
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket.");
	}

	u_long mode = 1; // non-blocking mode
	ioctlsocket(m_Socket, FIONBIO, &mode);

	// Bind socket to port
	Bind(aPort);
}

WindowsUDPSocket::~WindowsUDPSocket()
{
	closesocket(m_Socket);
	WSACleanup();
}

void EvayrNet::WindowsUDPSocket::Bind(uint16_t aPort)
{
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(aPort);

	int32_t result = bind(m_Socket, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
	if (result < 0)
	{
		throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to bind the socket.");
	}
	else
	{
		g_Network->GetDebugger()->Print("Successfully bound the socket to port " + std::to_string(aPort) + ".");
	}
}

void WindowsUDPSocket::Send()
{
	for (auto& connection : m_Connections)
	{
		// Prepare outgoing IP Address
		sockaddr_in address;
		IPAddress ip = connection.GetIPAddress();

		address.sin_family = AF_INET; //IPV4
		address.sin_addr.s_addr = inet_addr(ip.m_Address.c_str());
		address.sin_port = htons(ip.m_Port);

		// Serialize, encrypt and then send
		for (auto j = 0; j < connection.GetPacketCount(); ++j)
		{
			// Prepare packet
			connection.GetPacket(j)->Serialize();
			connection.GetPacket(j)->Encrypt();

			// Logging
			m_DataPerSecondSent.push_back(DataDebugInfo(connection.GetPacket(j)->GetMessagesSize(), clock()));

			// Send data
			int32_t result = sendto(m_Socket, connection.GetPacket(j)->GetData(), connection.GetPacket(j)->GetMessagesSize(), 0, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
			if (result < 0)
			{
				throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to send data.");
			}

			m_PPSOut.push_back(clock());
		}

		connection.ClearPackets();
	}
}

void WindowsUDPSocket::Receive()
{
	sockaddr_in addrOther;
	char buffer[Packet::kMaxPacketSize];
	int32_t otherLen = sizeof(addrOther);

	int32_t messageSize = (int32_t)recvfrom(m_Socket, buffer, Packet::kMaxPacketSize, 0, (struct sockaddr*)&addrOther, &otherLen);

	if (messageSize == SOCKET_ERROR) // -1
	{
		//g_Network->GetDebugger()->Print("Error receiving data! Error code: " + std::to_string(WSAGetLastError()));
	}

	else if (messageSize > 0)
	{
		// Log data
		m_DataPerSecondReceived.push_back(DataDebugInfo(messageSize, clock()));

		// Process IP Address
		IPAddress ip;
		ip.m_Address = inet_ntoa(addrOther.sin_addr);
		ip.m_Port = uint16_t(htons(addrOther.sin_port));
		ProcessIPAddress(ip);

		// Process the packet
		Packet packet;
		packet.SetData(buffer, (uint16_t)messageSize);
		m_pPacketHandler->ProcessPacket(packet);
		m_PPSIn.push_back(clock());
	}
}

#endif
