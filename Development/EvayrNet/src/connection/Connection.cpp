#include "connection\Connection.h"

using namespace EvayrNet;

Connection::Connection(IPAddress aIPAddress, int16_t aConnectionID)
	: m_IPAddress(aIPAddress)
	, m_ConnectionID(aConnectionID)
{
}

Connection::~Connection()
{
}

void Connection::Update()
{
}

void Connection::AddMessage(std::shared_ptr<Messages::Message> apMessage, Messages::EMessageType aType)
{
	// Check if there's a packet available - if not, create a new packet
	if (m_Packets.size() == 0)
	{
		m_Packets.push_back(std::make_shared<Packet>());
	}
	else if (m_Packets[m_Packets.size() - 1]->GetDataSize() + apMessage->GetMessageSize() >= Packet::kMaxPacketSize)
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

int16_t Connection::GetConnectionID() const
{
	return m_ConnectionID;
}

void Connection::SendHeartbeat()
{
}

void Connection::ProcessMessage()
{
}
