#include "data\packet\PacketHandler.h"
#include "data\stream\DataStream.h"

using namespace EvayrNet;

PacketHandler::PacketHandler()
{
	RegisterDefaultMessages();
}

PacketHandler::~PacketHandler()
{
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

	for (;;)
	{
		header.Deserialize(reader);
		if (header.size == 0) break;

		Messages::Message* pMessage = m_Messages[header.opcode];
		pMessage->Deserialize(reader);
	}
}

void PacketHandler::RegisterDefaultMessages()
{
	// Message Header
	std::shared_ptr<Messages::MessageHeader> pHeader = std::make_shared<Messages::MessageHeader>();
	RegisterMessage(pHeader.get(), pHeader->GetMessageOpcode());

	// Connection Request
	std::shared_ptr<Messages::ConnectionRequest> pConnectionRequest = std::make_shared<Messages::ConnectionRequest>();
	RegisterMessage(pConnectionRequest.get(), pConnectionRequest->GetMessageOpcode());

	// Connection Response
	std::shared_ptr<Messages::ConnectionResponse> pConnectionResponse = std::make_shared<Messages::ConnectionResponse>();
	RegisterMessage(pConnectionResponse.get(), pConnectionResponse->GetMessageOpcode());

	// Client IP Addresses
	std::shared_ptr<Messages::ClientIPAddresses> pClientIPAddresses = std::make_shared<Messages::ClientIPAddresses>();
	RegisterMessage(pClientIPAddresses.get(), pClientIPAddresses->GetMessageOpcode());
}

void PacketHandler::ProcessMessage()
{
}

void MessageHeader_Receive(const Messages::MessageHeader& acMessage)
{
}
