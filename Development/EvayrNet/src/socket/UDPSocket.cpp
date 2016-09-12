#include "socket\UDPSocket.h"

using namespace EvayrNet;

UDPSocket::UDPSocket()
	// Connection
	: m_Connecting(false)
	, m_Connected(false)
	, m_ConnectClock(0)
	, m_ConnectionAttempts(0)
	
	// Tick rates
	, m_TickRateSend(0)
	, m_ClockPerTickSend(0)
	, m_SendClock(clock())

	, m_pPacketHandler(nullptr)
{
}

UDPSocket::~UDPSocket()
{
}

void UDPSocket::ConnectTo(const char* apIP, uint16_t aPort)
{
	m_Connections.clear();

	IPAddress ip;
	ip.m_Address = apIP;
	ip.m_Port = aPort;

	m_Connections.push_back(Connection(ip, 0));

	m_Connected = false;
	m_Connecting = true;

	m_ConnectionAttempts = 0;
	m_ConnectClock = clock() - kRetryConnectInterval; // Start right away

	printf("Connecting to %s:%i...\n", ip.m_Address.c_str(), aPort);
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
	Connect();
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

void UDPSocket::SetTickRates(uint8_t aTickRateSend)
{
	m_TickRateSend = aTickRateSend;
	m_ClockPerTickSend = clock_t(1000.f / (float)aTickRateSend);
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

void UDPSocket::SetConnected(bool aVal)
{
	m_Connected = aVal;
	m_Connecting = false;
}

bool UDPSocket::IsConnected() const
{
	return m_Connected;
}

Connection* UDPSocket::GetNewestConnection()
{
	if (m_Connections.size() > 0)
	{
		return &m_Connections[m_Connections.size() - 1];
	}
	else
	{
		return nullptr;
	}
}

uint8_t UDPSocket::GetActiveConnectionsCount() const
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

void UDPSocket::Connect()
{
	if (!m_Connecting || m_Connected) return;

	if (clock() - m_ConnectClock < kRetryConnectInterval) return;
	m_ConnectClock = clock();

	auto pRequest = std::make_shared<Messages::ConnectionRequest>();
	AddMessage(pRequest, Messages::EMessageType::MESSAGE_UNRELIABLE, 0);

	m_ConnectionAttempts += 1;

	if (m_ConnectionAttempts >= kConnectionAttempts)
	{
		printf("Failed to connect to the server: No response.\n");
		m_Connected = false;
		m_Connecting = false;
	}
	else
	{
		printf("No response yet, retrying...\n");
	}
}

void UDPSocket::SendPackets()
{
	// Tick rate
	if (!m_Connected && !m_Connecting) return;
	if (clock() - m_SendClock < m_ClockPerTickSend) return;
	m_SendClock = clock();

	// Send packets
	Send();
}

void UDPSocket::ReceivePackets()
{
	// Tick rate
	if (!m_Connected && !m_Connecting) return;

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
