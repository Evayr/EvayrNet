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
}

void PacketHandler::RegisterMessage(std::unique_ptr<Messages::Message> apMessage, uint8_t aOpCode)
{
	if (!m_Messages[aOpCode])
	{
		m_Messages[aOpCode] = std::move(apMessage);
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

		std::unique_ptr<Messages::Message>& pMessage = m_Messages[header.opcode];
		pMessage->Deserialize(reader);

		bytesRead += header.size;
	}
}

void PacketHandler::RegisterDefaultMessages()
{
	// Message Header
	auto pHeader = std::make_unique<Messages::MessageHeader>();
	RegisterMessage(std::move(pHeader), pHeader->GetMessageOpcode());

	// Connection Request
	auto pConnectionRequest = std::make_unique<Messages::ConnectionRequest>();
	RegisterMessage(std::move(pConnectionRequest), pConnectionRequest->GetMessageOpcode());

	// Connection Response
	auto pConnectionResponse = std::make_unique<Messages::ConnectionResponse>();
	RegisterMessage(std::move(pConnectionResponse), pConnectionResponse->GetMessageOpcode());

	// Heartbeat
	auto pHeartbeat = std::make_unique<Messages::Heartbeat>();
	RegisterMessage(std::move(pHeartbeat), pHeartbeat->GetMessageOpcode());

	// Disconnect
	auto pDisconnect = std::make_unique<Messages::Disconnect>();
	RegisterMessage(std::move(pDisconnect), pDisconnect->GetMessageOpcode());

	// Client IP Addresses
	auto pClientIPAddresses = std::make_unique<Messages::ClientIPAddresses>();
	RegisterMessage(std::move(pClientIPAddresses), pClientIPAddresses->GetMessageOpcode());
}

void PacketHandler::ProcessMessage()
{
}

void EvayrNet::Messages::MessageHeader_Receive(const Messages::MessageHeader& acMessage)
{
}
