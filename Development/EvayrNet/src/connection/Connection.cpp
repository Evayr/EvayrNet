#include "connection\Connection.h"
#include "NetworkManager.h"

using namespace EvayrNet;

Connection::Connection(IPAddress aIPAddress, int16_t aConnectionID, bool aSendHeartbeats)
	: m_IPAddress(aIPAddress)
	, m_ConnectionID(aConnectionID)
	, m_Active(true)
	, m_HeartbeatID(0)
	, m_HeartbeatClock(clock())
	, m_PingClock(clock())
	, m_Ping(0)
	, m_SendAutoHeartbeats(aSendHeartbeats)
{
}

Connection::~Connection()
{
}

void Connection::Update()
{
	UpdateLifetime();
}

void Connection::AddMessage(std::shared_ptr<Messages::Message> apMessage, Messages::EMessageType aType)
{
	// Check if there's a packet available - if not, create a new packet
	if (m_Packets.size() == 0)
	{
		m_Packets.push_back(std::make_shared<Packet>());
	}
	else if (m_Packets[m_Packets.size() - 1]->GetMessagesSize() + apMessage->GetMessageSize() >= Packet::kMaxPacketSize)
	{
		m_Packets.push_back(std::make_shared<Packet>());
	}

	// Add message
	m_Packets[m_Packets.size() - 1]->AddMessage(apMessage);

	if (aType != Messages::EMessageType::MESSAGE_UNRELIABLE)
	{
		m_CachedMessages.push_back(apMessage);
	}
}

void Connection::AddCachedMessage()
{
}

void Connection::RemoveCachedMessage()
{
}

const IPAddress& Connection::GetIPAddress()
{
	return m_IPAddress;
}

uint8_t EvayrNet::Connection::GetPacketCount() const
{
	return uint8_t(m_Packets.size());
}

std::shared_ptr<Packet> Connection::GetPacket(uint8_t aPacketID)
{
	return m_Packets[aPacketID];
}

const std::list<std::shared_ptr<Messages::Message>>& Connection::GetCachedMessages()
{
	return m_CachedMessages;
}

void Connection::ClearPackets()
{
	m_Packets.clear();
}

int16_t Connection::GetConnectionID() const
{
	return m_ConnectionID;
}

void Connection::SetActive(bool aVal)
{
	m_Active = aVal;
}

bool Connection::IsActive() const
{
	return m_Active;
}

void Connection::ProcessHeartbeat(const Messages::Heartbeat& acMessage)
{
	if (HeartbeatIsNewer(acMessage.id))
	{
		m_HeartbeatID = acMessage.id;

		// Update ping
		if (g_Network->GetNetworkSystem()->IsServer())
		{
			m_Ping = uint32_t(clock() - m_PingClock);	
		}
		else
		{
			m_PingClock = clock();
			m_Ping = acMessage.ping;
		}

		//printf("Heartbeat with ID %i received. Our ping is %u\n", acMessage.id, m_Ping);

		// Reply to the heartbeat if we're a client
		if (!g_Network->GetNetworkSystem()->IsServer())
		{
			SendHeartbeat(true);
		}
	}
}

uint32_t EvayrNet::Connection::GetPing() const
{
	return m_Ping;
}

void Connection::EnableAutoHeartbeat()
{
	m_SendAutoHeartbeats = true;
}

void Connection::DisableAutoHeartbeat()
{
	m_SendAutoHeartbeats = false;
}

bool Connection::HeartbeatIsNewer(uint8_t aID)
{
	if (m_HeartbeatID < aID)
	{
		if (aID - m_HeartbeatID <= UINT8_MAX / 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (m_HeartbeatID - aID >= UINT8_MAX / 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void Connection::UpdateLifetime()
{
	if (!m_Active) return;

	if (clock() - m_PingClock > kConnectionTimout)
	{
		printf("Connection with ID %i has timed out.\n", m_ConnectionID);
		m_Packets.clear();
		m_Active = false;
	}

	if (m_SendAutoHeartbeats)
	{
		SendHeartbeat(false);
	}
}

void Connection::SendHeartbeat(bool aForceSend)
{
	// If it's time to send a heartbeat
	if (!aForceSend)
	{
		if (clock() - m_HeartbeatClock < kHeartbeatInterval) return;
		m_HeartbeatClock = clock();
	}

	// Make a heartbeat message
	m_HeartbeatID++;
	auto pHeartbeat = std::make_shared<Messages::Heartbeat>();
	pHeartbeat->id = m_HeartbeatID;
	pHeartbeat->connectionID = g_Network->GetUDPSocket()->GetConnectionID();
	pHeartbeat->ping = m_Ping;

	// Reset ping timer
	if (g_Network->GetNetworkSystem()->IsServer())
	{
		m_PingClock = clock();
	}

	// Add to packet
	g_Network->Send(pHeartbeat, m_ConnectionID);
}
