#include "data\packet\Packet.h"

using namespace EvayrNet;

Packet::Packet()
{
}

Packet::~Packet()
{
}

void Packet::AddMessage(std::shared_ptr<Messages::Message> apMessage)
{
	m_Messages.push_back(apMessage);
}

void Packet::RemoveMessage(std::shared_ptr<Messages::Message> apMessage)
{
	for (auto pMessage : m_Messages)
	{
		if (pMessage == apMessage)
		{
			m_Messages.remove(pMessage);
		}
		break;
	}

	m_DataStream.GetDataStream();
}

void Packet::Serialize()
{
	m_DataStream.Clear();
	DataStreamWriter writer = m_DataStream.CreateWriter();
	
	for (auto pMessage : m_Messages)
	{
		pMessage->Serialize(writer);
	}
}

void Packet::Deserialize()
{
	DataStreamReader reader = m_DataStream.CreateReader();

	for (auto pMessage : m_Messages)
	{
		pMessage->Deserialize(reader);
	}
}

void Packet::Encrypt()
{
}

void Packet::Decrypt()
{
}

const char* Packet::GetData() const
{
	return m_DataStream.GetDataStream();
}

uint16_t Packet::GetDataSize() const
{
	uint16_t size = 0;

	for (auto pMessage : m_Messages)
	{
		size += pMessage->GetMessageSize();
	}

	return size;
}
