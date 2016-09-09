#include "socket\UDPSocket.h"

using namespace EvayrNet;

UDPSocket::UDPSocket()
	// Connection
	: m_Connecting(false)
	, m_Connected(false)
	
	// Tick rates
	, m_TickRateSend(0)
	, m_TickRateRecv(0)
	, m_ClockPerTickSend(0)
	, m_ClockPerTickRecv(0)
	, m_SendClock(clock())
	, m_RecvClock(clock())

	, m_PacketHandler(PacketHandler())
{
}

UDPSocket::~UDPSocket()
{
}

void UDPSocket::ConnectTo(const char* apIP, uint16_t aPort)
{
	
}

void UDPSocket::Disconnect()
{
	if (m_Connected)
	{
		m_Connected = false;
	}
}

void UDPSocket::Update()
{
	ReceivePackets();
	UpdateConnections();
	SendPackets();
}

void UDPSocket::AddMessage(std::shared_ptr<Messages::Message> apMessage, Messages::EMessageType aType, int16_t aConnectionID)
{
	if (aConnectionID == -1)
	{
		for (size_t i = 0; i < m_Connections.size(); ++i)
		{
			m_Connections[i].AddMessage(apMessage, aType);
		}
	}
	else
	{
		m_Connections[aConnectionID].AddMessage(apMessage, aType);
	}
}

void UDPSocket::SetTickRates(uint8_t aTickRateSend, uint8_t aTickRateRecv)
{
	m_TickRateSend = aTickRateSend;
	m_TickRateRecv = aTickRateRecv;

	m_ClockPerTickSend = clock_t(1000.f / (float)aTickRateSend);
	m_ClockPerTickRecv = clock_t(1000.f / (float)aTickRateRecv);
}

int16_t UDPSocket::CheckConnection(IPAddress aIPAddress)
{
	// Checks if the connection is new. If so, create a new Connection
	// Returns the ConnectionID of the connection
	for (size_t i = 0; i < m_Connections.size(); ++i)
	{
		if (m_Connections[i].GetIPAddress() == aIPAddress && m_Connections[i].IsActive())
		{
			return m_Connections[i].GetConnectionID();
		}
	}

	// No connection found - create new one
	int16_t connectionID = int16_t(m_Connections.size());
	m_Connections.push_back(Connection(aIPAddress, connectionID));
	return connectionID;
}

bool UDPSocket::IsConnected() const
{
	return m_Connected;
}

std::shared_ptr<Connection> UDPSocket::GetNewestConnection()
{
	if (m_Connections.size() > 0)
	{
		std::shared_ptr<Connection> pConnection = std::make_shared<Connection>(m_Connections[m_Connections.size() - 1]);
		return std::shared_ptr<Connection>();
	}
	else
	{
		return nullptr;
	}
}

uint8_t EvayrNet::UDPSocket::GetActiveConnectionsCount() const
{
	uint8_t count = 0;

	for (size_t i = 0; i < m_Connections.size(); ++i)
	{
		if (m_Connections[i].IsActive())
		{
			count += 1;
		}
	}

	return count;
}

void UDPSocket::SendPackets()
{
	// Tick rate
	if (!m_Connected) return;
	if (clock() - m_SendClock < m_ClockPerTickSend) return;
	m_SendClock = clock();

	// Send packets
	Send();
}

void UDPSocket::ReceivePackets()
{
	// Tick rate
	if (!m_Connected) return;
	if (clock() - m_SendClock < m_ClockPerTickSend) return;
	m_SendClock = clock();

	// Receive data
	Receive();
}

void UDPSocket::UpdateConnections()
{
	for (size_t i = 0; i < m_Connections.size(); ++i)
	{
		m_Connections[i].Update();
	}
}

void UDPSocket::AddConnection(IPAddress aIPAddress, int16_t aConnectionID)
{
	m_Connections.push_back(std::move(Connection(aIPAddress, aConnectionID)));
}
