#include "connection\Connection.h"
#include "NetworkManager.h"

using namespace EvayrNet;

Connection::Connection(const IPAddress& acIPAddress, uint16_t aConnectionID, bool aSendHeartbeats, uint16_t aHeartbeatInterval, uint32_t aConnectionTimeout)
	: m_IPAddress(acIPAddress)
	, m_ConnectionID(aConnectionID)
	, m_Active(true)
	, m_HeartbeatInterval(aHeartbeatInterval)
	, m_ConnectionTimeout(aConnectionTimeout)
	, m_NewestSendACKID(0)
	, m_NewestReceiveACKID(0)
	, m_NewestSendSequenceID(1)
	, m_NewestReceiveSequenceID(1)
	, m_HeartbeatClock(clock())
	, m_HeartbeatID(0)
	, m_PingClock(clock())
	, m_TempPingsRecorded(0)
	, m_SendHeartbeats(aSendHeartbeats)
{
}

Connection::~Connection()
{
}

void Connection::Update()
{
	UpdateLifetime();
	ResendMessages();
}

void Connection::AddMessage(const std::shared_ptr<Messages::Message>& apMessage, bool aStoreACK, uint16_t aExtraMessageSize)
{
	// Check if there's a packet available - if not, create a new packet
	if (m_Packets.size() == 0)
	{
		m_Packets.push_back(std::make_shared<Packet>());
	}
	else if (m_Packets[m_Packets.size() - 1]->GetMessagesSize() + apMessage->GetMessageSize() + aExtraMessageSize >= Packet::kMaxPacketSize)
	{
		m_Packets.push_back(std::make_shared<Packet>());
	}

	// Add message
	m_Packets[m_Packets.size() - 1]->AddMessage(apMessage);

	// Add ACK is necessary - This can only happen for new messages.
	// Messages that are re-sent will keep their ACK and Sequence ID.
	if (apMessage->m_MessageType != Messages::EMessageType::MESSAGETYPE_UNRELIABLE && aStoreACK)
	{
		// Add Sequence ID if necessary
		// Apply this first so that the cached ACK message has the correct SequenceID attached to it in case it has to re-send the message
		if (apMessage->m_MessageType == Messages::EMessageType::MESSAGETYPE_SEQUENCED)
		{
			// Apply SequenceID to message
			apMessage->m_SequenceID = m_NewestSendSequenceID;

			// Increment for next time
			m_NewestSendSequenceID++;

			// Don't allow SequenceIDs to be 0 - that would mean that there's no SequenceID attached to this message
			if (m_NewestSendSequenceID == 0)
			{
				m_NewestSendSequenceID++;
			}
		}

		// Apply ACK
		m_NewestSendACKID++;

		auto pACK = std::make_shared<Messages::ACK>();
		pACK->connectionID = g_Network->GetUDPSocket()->GetConnectionID();
		pACK->id = m_NewestSendACKID;
		pACK->m_MessageType = Messages::EMessageType::MESSAGETYPE_UNRELIABLE;

		AddMessage(pACK, false, apMessage->GetMessageSize()); // Make sure the ACK fits in the same packet as the relevant message
		AddCachedACKMessage(apMessage, m_NewestSendACKID);
	}
}

void Connection::AddCachedACKMessage(const std::shared_ptr<Messages::Message>& apMessage, uint8_t aACKID)
{
	CachedACKMessage message;
	message.m_pMessage = apMessage;
	message.m_ACKID = aACKID;
	message.m_TimeSent = clock();

	m_CachedACKMessages.push_back(message);
}

void Connection::RemoveCachedACKMessage(uint8_t aACKID)
{
	for (size_t i = 0; i < m_CachedACKMessages.size(); ++i)
	{
		if (m_CachedACKMessages[i].m_ACKID == aACKID)
		{
			m_CachedACKMessages.erase(m_CachedACKMessages.begin() + i);
		}
	}
}

void Connection::AddCachedSequencedMessage(std::shared_ptr<Messages::Message> apMessage)
{
	// Add the message
	if (m_SequencedMessages.size() == 0)
	{
		m_SequencedMessages.push_back(apMessage);
	}
	else
	{
		// Don't add previous messages
		if (!SequenceIsNewer(apMessage->m_SequenceID)) return;

		// Don't duplicate newer messages
		bool hasMessage = false;
		for (auto& it = m_SequencedMessages.begin(); it != m_SequencedMessages.end(); ++it)
		{
			if ((*it)->m_SequenceID == apMessage->m_SequenceID)
			{
				hasMessage = true;
				break;
			}
		}
		if (hasMessage) return;

		// Sort message by putting it into the correct order
		auto& lowestIt = m_SequencedMessages.begin();
		for (auto& it = m_SequencedMessages.begin(); it != m_SequencedMessages.end(); ++it)
		{
			if (apMessage->m_SequenceID < (*it)->m_SequenceID)
			{
				lowestIt = it;
				continue;
			}
			break;
		}

		m_SequencedMessages.insert(lowestIt, apMessage);
	}
}

void Connection::ExecuteSequencedMessages()
{
	// Execute as many messages as possible
	for (auto& it = m_SequencedMessages.begin(); it != m_SequencedMessages.end();)
	{
		if ((*it)->m_SequenceID == m_NewestReceiveSequenceID)
		{
			(*it)->Execute();
			m_SequencedMessages.erase(it++);

			m_NewestReceiveSequenceID++;

			// Don't allow sequence ID to be 0
			if (m_NewestReceiveSequenceID == 0)
			{
				m_NewestReceiveSequenceID++;
			}
			continue;
		}

		// Our message hasn't arrived yet. Wait for it.
		break;
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

const std::vector<Connection::CachedACKMessage>& Connection::GetCachedACKMessages()
{
	return m_CachedACKMessages;
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
		m_NewestReceiveACKID = acACK.id;

		// Send acknowledgment of the ACK we've been given
		auto ackSend = std::make_shared<Messages::AcknowledgeACK>();
		ackSend->connectionID = g_Network->GetUDPSocket()->GetConnectionID();
		ackSend->id = acACK.id;
		g_Network->Send(ackSend, acACK.connectionID);
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
			PushPing(uint32_t(clock() - m_PingClock));
		}
		else
		{
			m_PingClock = clock();
			PushPing(acMessage.ping);
		}

		// Reply to the heartbeat if we're a client
		if (!g_Network->GetNetworkSystem()->IsServer())
		{
			SendHeartbeat(true);
		}
	}
}

void EvayrNet::Connection::PushPing(uint32_t aPing)
{
	for (uint8_t i = kPingStorageCount - 1; i > 0; --i)
	{
		m_RecentPings[i] = m_RecentPings[i - 1];
	}
	m_RecentPings[0] = aPing;

	if (m_TempPingsRecorded < kPingStorageCount)
	{
		m_TempPingsRecorded++;
	}
}

const uint32_t EvayrNet::Connection::GetNewestPing() const
{
	return m_RecentPings[0];
}

const uint32_t EvayrNet::Connection::GetAveragePing() const
{
	if (m_TempPingsRecorded == 0) return 0;

	uint32_t avg = 0;

	for (uint8_t i = 0; i < m_TempPingsRecorded; ++i)
	{
		avg += m_RecentPings[i];
	}
	avg /= m_TempPingsRecorded;

	return avg;
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
	if (m_NewestReceiveACKID < aID)
	{
		if (aID - m_NewestReceiveACKID <= UINT8_MAX / 2)
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
		if (m_NewestReceiveACKID - aID >= UINT8_MAX / 2)
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

bool EvayrNet::Connection::SequenceIsNewer(uint8_t aID) const
{
	if (m_NewestReceiveSequenceID < aID)
	{
		if (aID - m_NewestReceiveSequenceID <= UINT8_MAX / 2)
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
		if (m_NewestReceiveSequenceID - aID >= UINT8_MAX / 2)
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
	if (!g_Network->IsConnected()) return;
	if (!m_Active) return;
	if (GetConnectionID() == 0) return;

	if (uint32_t(clock() - m_PingClock) > m_ConnectionTimeout)
	{
		g_Network->GetDebugger()->Print("Connection with ID " + std::to_string(m_ConnectionID) + " has timed out.\n");
		m_Packets.clear();
		m_Active = false;

		// Connection lost with the server
		if (m_ConnectionID == UDPSocket::kServerConnectionID)
		{
			g_Network->Disconnect();
		}
	}

	if (m_SendHeartbeats)
	{
		SendHeartbeat(false);
	}
}

void Connection::ResendMessages()
{
	clock_t now = clock();

	for (auto message : m_CachedACKMessages)
	{
		if (uint32_t(now - message.m_TimeSent) >= GetAveragePing() + kResendDelay)
		{
			// Reset timer
			message.m_TimeSent = now;

			// Re-send it over the network
			if (message.m_pMessage->m_MessageType == Messages::EMessageType::MESSAGETYPE_RELIABLE)
			{
				g_Network->SendReliable(message.m_pMessage, m_ConnectionID, false);
			}
			else
			{
				g_Network->SendSequenced(message.m_pMessage, m_ConnectionID, false);
			}
		}
	}
}

void Connection::SendHeartbeat(bool aForceSend)
{
	// If it's time to send a heartbeat
	if (!aForceSend)
	{
		if (clock() - m_HeartbeatClock < m_HeartbeatInterval) return;
		m_HeartbeatClock = clock();
	}

	// Make a heartbeat message
	m_HeartbeatID++;
	auto pHeartbeat = std::make_shared<Messages::Heartbeat>();
	pHeartbeat->id = m_HeartbeatID;
	pHeartbeat->connectionID = g_Network->GetUDPSocket()->GetConnectionID();
	pHeartbeat->ping = GetAveragePing();

	// Reset ping timer
	if (g_Network->GetNetworkSystem()->IsServer())
	{
		m_PingClock = clock();
	}

	// Add to packet
	g_Network->Send(pHeartbeat, m_ConnectionID);
}
