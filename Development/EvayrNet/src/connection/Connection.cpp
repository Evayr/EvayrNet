#include "connection\Connection.h"
#include "NetworkManager.h"

using namespace EvayrNet;

Connection::Connection(const IPAddress& acIPAddress, uint16_t aConnectionID, bool aSendHeartbeats, uint16_t aHeartbeatInterval, uint32_t aConnectionTimeout)
	: m_IPAddress(acIPAddress)
	, m_ConnectionID(aConnectionID)
	, m_Active(true)
	, m_heartbeatInterval(aHeartbeatInterval)
	, m_connectionTimeout(aConnectionTimeout)
	, m_newestSendACKID(0)
	, m_newestReceiveACKID(0)
	, m_HeartbeatClock(clock())
	, m_HeartbeatID(0)
	, m_PingClock(clock())
	, m_Ping(0)
	, m_SendHeartbeats(aSendHeartbeats)
{
}

Connection::~Connection()
{
}

void Connection::Update()
{
	UpdateLifetime();
}

void Connection::AddMessage(const std::shared_ptr<Messages::Message>& apMessage)
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

	// Add ACK is necessary
	if (apMessage->messageType != Messages::EMessageType::MESSAGE_UNRELIABLE)
	{
		m_newestSendACKID++;
		//printf("Adding in an ACK with ID %u for message \"%s\"\n", m_newestSendACKID, apMessage->GetMessageName());

		auto pACK = std::make_shared<Messages::ACK>();
		pACK->connectionID = g_Network->GetUDPSocket()->GetConnectionID();
		pACK->id = m_newestSendACKID;
		pACK->messageType = Messages::EMessageType::MESSAGE_UNRELIABLE;
		AddMessage(pACK);

		AddCachedMessage(apMessage, m_newestSendACKID);
	}
}

void Connection::AddCachedMessage(const std::shared_ptr<Messages::Message>& apMessage, uint8_t aACKID)
{
	CachedMessage message;
	message.m_pMessage = apMessage;
	message.m_ackID = aACKID;

	m_CachedMessages.push_back(message);
}

void Connection::RemoveCachedMessage(uint8_t aACKID)
{
	for (size_t i = 0; i < m_CachedMessages.size(); ++i)
	{
		if (m_CachedMessages[i].m_ackID == aACKID)
		{
			printf("Removing cached message with ACK ID %u\n", aACKID);
			m_CachedMessages.erase(m_CachedMessages.begin() + i);
		}
	}
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

const std::vector<Connection::CachedMessage>& Connection::GetCachedMessages()
{
	return m_CachedMessages;
}

void Connection::ClearPackets()
{
	m_Packets.clear();
}

uint16_t Connection::GetConnectionID() const
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

void Connection::ProcessACK(const Messages::ACK& acACK)
{
	if (ACKIsNewer(acACK.id))
	{
		m_newestReceiveACKID = acACK.id;

		// Send acknowledgment of the ACK we've been given
		auto ackSend = std::make_unique<Messages::AcknowledgeACK>();
		ackSend->connectionID = g_Network->GetUDPSocket()->GetConnectionID();
		ackSend->id = acACK.id;
		g_Network->Send(std::move(ackSend), acACK.connectionID);
	}
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
	m_SendHeartbeats = true;
}

void Connection::DisableAutoHeartbeat()
{
	m_SendHeartbeats = false;
}

bool Connection::HeartbeatIsNewer(uint8_t aID) const
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

bool Connection::ACKIsNewer(uint8_t aID) const
{
	if (m_newestReceiveACKID < aID)
	{
		if (aID - m_newestReceiveACKID <= UINT8_MAX / 2)
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
		if (m_newestReceiveACKID - aID >= UINT8_MAX / 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

void Connection::UpdateLifetime()
{
	if (!m_Active) return;

	if (clock() - m_PingClock > m_connectionTimeout)
	{
		printf("Connection with ID %i has timed out.\n", m_ConnectionID);
		m_Packets.clear();
		m_Active = false;
	}

	if (m_SendHeartbeats)
	{
		SendHeartbeat(false);
	}
}

void Connection::SendHeartbeat(bool aForceSend)
{
	// If it's time to send a heartbeat
	if (!aForceSend)
	{
		if (clock() - m_HeartbeatClock < m_heartbeatInterval) return;
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
