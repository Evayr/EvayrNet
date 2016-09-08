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

bool UDPSocket::IsConnected() const
{
	return m_Connected;
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
