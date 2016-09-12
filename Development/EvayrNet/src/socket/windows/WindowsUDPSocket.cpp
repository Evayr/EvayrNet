#if defined(_WIN64) || defined(_WIN32)

#include "socket\windows\WindowsUDPSocket.h"

using namespace EvayrNet;

WindowsUDPSocket::WindowsUDPSocket(PacketHandler* apPacketHandler, uint16_t aPort, uint8_t aTickRateSend, uint8_t aTickRateRecv)
{
	m_pPacketHandler = apPacketHandler;
	SetTickRates(aTickRateSend, aTickRateRecv);

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
		printf("Successfully bound the socket to port %i.\n", aPort);
	}
}

void WindowsUDPSocket::Send()
{
	for (size_t i = 0; i < m_Connections.size(); ++i)
	{
		// Prepare outgoing IP Address
		sockaddr_in address;
		IPAddress ip = m_Connections[i].GetIPAddress();

		address.sin_family = AF_INET; //IPV4
		address.sin_addr.s_addr = inet_addr(ip.m_Address.c_str());
		address.sin_port = htons(ip.m_Port);

		// Serialize, encrypt and then send
		for (auto j = 0; j < m_Connections[i].GetPacketCount(); ++j)
		{
			m_Connections[i].GetPacket(j)->Serialize();
			m_Connections[i].GetPacket(j)->Encrypt();

			int32_t result = sendto(m_Socket, m_Connections[i].GetPacket(j)->GetData(), m_Connections[i].GetPacket(j)->GetMessagesSize(), 0, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
			if (result < 0)
			{
				throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to send data.");
			}

			printf("Sent %i bytes of data to %s:%i\n", m_Connections[i].GetPacket(j)->GetMessagesSize(), m_Connections[i].GetIPAddress().m_Address.c_str(), m_Connections[i].GetIPAddress().m_Port);

			m_Connections[i].ClearPackets();
		}
	}
}

void WindowsUDPSocket::Receive()
{
	sockaddr_in addrOther;
	uint8_t buffer[Packet::kMaxPacketSize];
	int32_t otherLen = sizeof(addrOther);

	int32_t messageSize = (int32_t)recvfrom(m_Socket, (char*)buffer, Packet::kMaxPacketSize, 0, (struct sockaddr*)&addrOther, &otherLen);

	if (messageSize == SOCKET_ERROR) // -1
	{
		//printf("Error receiving data! Error code: %d\n", WSAGetLastError());
	}

	else if (messageSize > 0)
	{
		printf("Received %i bytes of data...\n", messageSize);

		// Process IP Address
		IPAddress ip;
		ip.m_Address = inet_ntoa(addrOther.sin_addr);
		ip.m_Port = uint16_t(htons(addrOther.sin_port));
		CheckConnection(ip);

		// Process the packet
		Packet packet;
		packet.SetData((char*)buffer, (uint16_t)messageSize);
		m_pPacketHandler->ProcessPacket(packet);
	}
}

#endif
