#include "socket\UDPSocket.h"
#include "NetworkManager.h"

using namespace EvayrNet;

UDPSocket::UDPSocket()
	// Connection
	: m_Connecting(false)
	, m_Connected(false)
	, m_ConnectionID(-1)
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

	// When connecting to a server, the server is always connection id 0
	// Also don't allow the client to send heartbeats - the server should be managing that
	m_Connections.push_back(Connection(ip, 0, false));

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
		// Send disconnect message to everyone
		auto pDisconnectMessage = std::make_shared<Messages::Disconnect>();
		pDisconnectMessage->connectionID = m_ConnectionID;
		pDisconnectMessage->reason = Messages::EDisconnectReason::REASON_QUIT;
		AddMessage(pDisconnectMessage, Messages::EMessageType::MESSAGE_UNRELIABLE);
		SendPackets(true);

		m_Connected = false;
	}
}

void UDPSocket::Update()
{
	Connect();
	ReceivePackets();
	UpdateConnections();
	SendPackets();
	UpdateStatistics();
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

void UDPSocket::SetTickRate(uint8_t aTickRateSend)
{
	m_TickRateSend = aTickRateSend;
	m_ClockPerTickSend = clock_t(1000.f / (float)aTickRateSend);
}

void UDPSocket::ProcessHeartbeat(const Messages::Heartbeat& acMessage)
{
	for (size_t i = 0; i < m_Connections.size(); ++i)
	{
		if (m_Connections[i].GetConnectionID() == acMessage.connectionID)
		{
			m_Connections[i].ProcessHeartbeat(acMessage);
			return;
		}
	}
}

int16_t UDPSocket::ProcessIPAddress(IPAddress aIPAddress)
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
	printf("Adding %s:%u as a new connection...\n", aIPAddress.m_Address.c_str(), aIPAddress.m_Port);
	int16_t connectionID = int16_t(m_Connections.size());
	m_Connections.push_back(Connection(aIPAddress, connectionID, g_Network->GetNetworkSystem()->IsServer()));
	return connectionID;
}

void UDPSocket::SetConnected(bool aVal)
{
	m_Connected = aVal;
	m_Connecting = false;

	if (!m_Connected)
	{
		m_Connections.clear();
	}
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

Connection* UDPSocket::GetConnection(int16_t aID)
{
	for (size_t i = 0; i < m_Connections.size(); ++i)
	{
		if (m_Connections[i].GetConnectionID() == aID)
		{
			return &m_Connections[i];
		}
	}

	return nullptr;
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

void UDPSocket::SetConnectionID(int16_t aVal)
{
	m_ConnectionID = aVal;
}

int16_t UDPSocket::GetConnectionID() const
{
	return m_ConnectionID;
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

void UDPSocket::SendPackets(bool aForceSend)
{
	if (!m_Connected && !m_Connecting) return;

	// Tick rate
	if (!aForceSend)
	{
		if (clock() - m_SendClock < m_ClockPerTickSend) return;
		m_SendClock = clock();
	}

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

void UDPSocket::UpdateStatistics()
{
	RemoveTimeIfExceedsAmount(&m_PPSOut);
	RemoveTimeIfExceedsAmount(&m_PPSIn);
	RemoveTimeIfExceedsAmount(&m_PPSLost);
	//printf("PPS OUT: %i | PPS IN: %i | PPS LOST: %i\n", m_PPSOut.size(), m_PPSIn.size(), m_PPSLost.size());
}

void UDPSocket::RemoveTimeIfExceedsAmount(std::list<clock_t>* apList, float aTime)
{
	if (apList->size() == 0) return;

	const clock_t now = clock();

	auto it = apList->begin();
	while (it != apList->end())
	{
		auto prev_it = it;
		it++;
		if (now - *prev_it >= aTime)
		{
			apList->erase(prev_it);
		}
	}
}

void UDPSocket::AddConnection(IPAddress aIPAddress, int16_t aConnectionID)
{
	m_Connections.push_back(std::move(Connection(aIPAddress, aConnectionID, g_Network->GetNetworkSystem()->IsServer())));
}

// Handles
void EvayrNet::Messages::Heartbeat_Receive(const Messages::Heartbeat& acMessage)
{
	g_Network->GetUDPSocket()->ProcessHeartbeat(acMessage);
}
