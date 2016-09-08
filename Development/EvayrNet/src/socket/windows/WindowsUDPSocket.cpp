#if defined(_WIN64) || defined(_WIN32)

#include "socket\windows\WindowsUDPSocket.h"

using namespace EvayrNet;

WindowsUDPSocket::WindowsUDPSocket(uint8_t aTickRateSend, uint8_t aTickRateRecv)
{
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
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
	}
}

WindowsUDPSocket::~WindowsUDPSocket()
{
	WSACleanup();
}

void WindowsUDPSocket::Send()
{
	for (size_t i = 0; i < m_Connections.size(); ++i)
	{
		// Process IP Address
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

			int32_t result = sendto(m_Socket, m_Connections[i].GetPacket(j)->GetData(), m_Connections[i].GetPacket(j)->GetDataSize(), 0, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
			if (result < 0)
			{
				throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to sendto");
			}
		}
	}
}

void WindowsUDPSocket::Receive()
{

}

#endif
