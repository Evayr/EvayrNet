#include "data\packet\PacketHandler.h"
#include "data\stream\DataStream.h"
#include "data\messages\Messages.h"
#include "NetworkManager.h"

using namespace EvayrNet;

PacketHandler::PacketHandler()
{
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
	Messages::ACK ack;

	const uint16_t packetSize = aPacket.GetDataSize();
	uint16_t bytesRead = 0;

	// Error-checking / logging
	struct DeserializedMessage
	{
		DeserializedMessage(const std::string& acName, uint16_t aSize)
			: name(acName)
			, size(aSize)
		{}
		std::string name;
		uint16_t size;
	};
	std::list<DeserializedMessage> messagesRead;

	while(bytesRead < packetSize)
	{
		// Read message header
		reader.Read(header.size);
		if (header.size == 0) break; // Packet is corrupt / spoofed / invalid. Stop reading and discard the message.
		reader.Read(header.opcode);
		reader.Read(header.sequenceID);
		reader.Read(header.connectionID);

		// Deserialize message
		//g_Network->GetDebugger()->Print("Received a message from connectionID %u\n", header.connectionID);
		std::shared_ptr<Messages::Message> pMessage = m_Messages[header.opcode]->CreateInstance();
		pMessage->m_ConnectionID = header.connectionID;

		if (pMessage->GetMessageSize() + bytesRead <= packetSize)
		{
			messagesRead.push_back(DeserializedMessage(pMessage->GetMessageName(), pMessage->GetMessageSize()));
			pMessage->Deserialize(reader);
		}
		else
		{
			g_Network->GetDebugger()->Print("ERROR! Not enough bytes to deserialize message (%u bytes). Messages read were: " + std::to_string(packetSize));
			uint8_t id = 0;
			uint16_t totalBytes = 0;
			for (auto it : messagesRead)
			{
				totalBytes += it.size;
				g_Network->GetDebugger()->Print("ID " + std::to_string(id) + ": \"" + it.name.c_str() + "\" (" + std::to_string(it.size) + " bytes, " + std::to_string(totalBytes) + "/" + std::to_string(packetSize) + ")");
				id++;
			}
			break;
		}

		// Process SequenceID if it has one
		if (header.sequenceID == 0 || header.opcode == ack.GetMessageOpcode())
		{
			pMessage->Execute();
		}
		else
		{
			// Store sequenced message, execute it if possible
			Connection* pConnection = g_Network->GetUDPSocket()->GetConnection(header.connectionID);
			if (pConnection)
			{
				pMessage->m_SequenceID = header.sequenceID;
				pConnection->AddCachedSequencedMessage(pMessage);
				pConnection->ExecuteSequencedMessages();
			}
		}

		bytesRead += header.size;
	}
}

void PacketHandler::RegisterDefaultMessages()
{
	// Message Header
	auto pHeader = std::make_unique<Messages::MessageHeader>();
	RegisterMessage(std::move(pHeader), pHeader->GetMessageOpcode());

	// ACK
	auto pACK = std::make_unique<Messages::ACK>();
	RegisterMessage(std::move(pACK), pACK->GetMessageOpcode());

	// Acknowledgment of the ACK
	auto pAcknowledgeACK = std::make_unique<Messages::AcknowledgeACK>();
	RegisterMessage(std::move(pAcknowledgeACK), pAcknowledgeACK->GetMessageOpcode());

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

	// Print Text
	auto pPrintText = std::make_unique<Messages::PrintText>();
	RegisterMessage(std::move(pPrintText), pPrintText->GetMessageOpcode());
}

void PacketHandler::ProcessMessage()
{
}

void EvayrNet::Messages::MessageHeader_Receive(const Messages::MessageHeader& acMessage)
{
}

void EvayrNet::Messages::ACK_Receive(const Messages::ACK& acMessage)
{
	Connection* pConnection = g_Network->GetUDPSocket()->GetConnection(acMessage.connectionID);

	if (pConnection)
	{
		pConnection->ProcessACK(acMessage);
	}
	else
	{
		g_Network->GetDebugger()->Print("ERROR! ACK with Connection ID " + std::to_string(acMessage.connectionID) + " is nullptr!");
	}
}
