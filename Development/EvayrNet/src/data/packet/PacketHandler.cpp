#include "data\packet\PacketHandler.h"
#include "data\stream\DataStream.h"
#include "data\messages\Messages.h"

using namespace EvayrNet;

PacketHandler::PacketHandler()
{
	memset(&m_Messages[0], 0, sizeof(m_Messages));
	RegisterDefaultMessages();
}

PacketHandler::~PacketHandler()
{
	for (uint8_t i = 0; i < UINT8_MAX; ++i)
	{
		if (m_Messages[i])
		{
			delete m_Messages[i];
			m_Messages[i] = nullptr;
		}
	}
}

void PacketHandler::RegisterMessage(Messages::Message* apMessage, uint8_t aOpCode)
{
	if (!m_Messages[aOpCode])
	{
		m_Messages[aOpCode] = apMessage;
	}
	else
	{
		throw std::system_error(0, std::system_category(), "Message OpCode already in use!");
	}
}

void PacketHandler::ProcessPacket(Packet& aPacket)
{
	aPacket.Decrypt();

	DataStreamReader reader = aPacket.GetDataStream().CreateReader();

	Messages::MessageHeader header;

	uint16_t packetSize = aPacket.GetDataSize();
	uint16_t bytesRead = 0;

	while(bytesRead < packetSize)
	{
		header.Deserialize(reader);
		if (header.size == 0) break;

		Messages::Message* pMessage = m_Messages[header.opcode];
		pMessage->Deserialize(reader);

		bytesRead += header.size;
	}
}

void PacketHandler::RegisterDefaultMessages()
{
	// Message Header
	auto pHeader = new Messages::MessageHeader();
	RegisterMessage(pHeader, pHeader->GetMessageOpcode());

	// Connection Request
	auto pConnectionRequest = new Messages::ConnectionRequest();
	RegisterMessage(pConnectionRequest, pConnectionRequest->GetMessageOpcode());

	// Connection Response
	auto pConnectionResponse = new Messages::ConnectionResponse();
	RegisterMessage(pConnectionResponse, pConnectionResponse->GetMessageOpcode());

	// Heartbeat
	auto pHeartbeat = new Messages::Heartbeat();
	RegisterMessage(pHeartbeat, pHeartbeat->GetMessageOpcode());

	// Disconnect
	auto pDisconnect = new Messages::Disconnect();
	RegisterMessage(pDisconnect, pDisconnect->GetMessageOpcode());

	// Client IP Addresses
	auto pClientIPAddresses = new Messages::ClientIPAddresses();
	RegisterMessage(pClientIPAddresses, pClientIPAddresses->GetMessageOpcode());
}

void PacketHandler::ProcessMessage()
{
}

void EvayrNet::Messages::MessageHeader_Receive(const Messages::MessageHeader& acMessage)
{
}
