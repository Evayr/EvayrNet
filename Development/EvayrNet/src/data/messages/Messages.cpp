/* This file was automatically generated. Do not modify! */
/* For more info, find protocol.txt / codegen.py or contact @Ivar Slotboom */

#include "data\messages\Messages.h"

namespace EvayrNet
{
namespace Messages
{

Message::Message() {}
Message::~Message() {}

MessageHeader::MessageHeader()
{
}

MessageHeader::~MessageHeader()
{
}

void MessageHeader::Serialize(EvayrNet::DataStreamWriter& aWriter)
{
	// Serialize header
	aWriter.Write(GetMessageSize()); // message size
	aWriter.Write(uint8_t(0)); // opcode

	// Serialize member variables
	aWriter.Write(size);
	aWriter.Write(opcode);
}

void MessageHeader::Deserialize(EvayrNet::DataStreamReader& aReader)
{
	// Deserialize member variables
	aReader.Read(size);
	aReader.Read(opcode);

	// Notify receiver that a message has been processed
	MessageHeader_Receive(*this);
}

uint16_t MessageHeader::GetMessageSize()
{
	return 6;
}

uint8_t MessageHeader::GetMessageOpcode()
{
	return 0;
}

ConnectionRequest::ConnectionRequest()
{
}

ConnectionRequest::~ConnectionRequest()
{
}

void ConnectionRequest::Serialize(EvayrNet::DataStreamWriter& aWriter)
{
	// Serialize header
	aWriter.Write(GetMessageSize()); // message size
	aWriter.Write(uint8_t(1)); // opcode

	// Serialize member variables
}

void ConnectionRequest::Deserialize(EvayrNet::DataStreamReader& aReader)
{
	// Deserialize member variables

	// Notify receiver that a message has been processed
	ConnectionRequest_Receive(*this);
}

uint16_t ConnectionRequest::GetMessageSize()
{
	return 3;
}

uint8_t ConnectionRequest::GetMessageOpcode()
{
	return 1;
}

ConnectionResponse::ConnectionResponse()
{
}

ConnectionResponse::~ConnectionResponse()
{
}

void ConnectionResponse::Serialize(EvayrNet::DataStreamWriter& aWriter)
{
	// Serialize header
	aWriter.Write(GetMessageSize()); // message size
	aWriter.Write(uint8_t(2)); // opcode

	// Serialize member variables
	aWriter.Write(response);
	aWriter.Write(connectionID);
}

void ConnectionResponse::Deserialize(EvayrNet::DataStreamReader& aReader)
{
	// Deserialize member variables
	aReader.Read(response);
	aReader.Read(connectionID);

	// Notify receiver that a message has been processed
	ConnectionResponse_Receive(*this);
}

uint16_t ConnectionResponse::GetMessageSize()
{
	return 6;
}

uint8_t ConnectionResponse::GetMessageOpcode()
{
	return 2;
}

Heartbeat::Heartbeat()
{
}

Heartbeat::~Heartbeat()
{
}

void Heartbeat::Serialize(EvayrNet::DataStreamWriter& aWriter)
{
	// Serialize header
	aWriter.Write(GetMessageSize()); // message size
	aWriter.Write(uint8_t(3)); // opcode

	// Serialize member variables
	aWriter.Write(connectionID);
	aWriter.Write(id);
	aWriter.Write(ping);
}

void Heartbeat::Deserialize(EvayrNet::DataStreamReader& aReader)
{
	// Deserialize member variables
	aReader.Read(connectionID);
	aReader.Read(id);
	aReader.Read(ping);

	// Notify receiver that a message has been processed
	Heartbeat_Receive(*this);
}

uint16_t Heartbeat::GetMessageSize()
{
	return 10;
}

uint8_t Heartbeat::GetMessageOpcode()
{
	return 3;
}

Disconnect::Disconnect()
{
}

Disconnect::~Disconnect()
{
}

void Disconnect::Serialize(EvayrNet::DataStreamWriter& aWriter)
{
	// Serialize header
	aWriter.Write(GetMessageSize()); // message size
	aWriter.Write(uint8_t(4)); // opcode

	// Serialize member variables
	aWriter.Write(reason);
	aWriter.Write(connectionID);
}

void Disconnect::Deserialize(EvayrNet::DataStreamReader& aReader)
{
	// Deserialize member variables
	aReader.Read(reason);
	aReader.Read(connectionID);

	// Notify receiver that a message has been processed
	Disconnect_Receive(*this);
}

uint16_t Disconnect::GetMessageSize()
{
	return 6;
}

uint8_t Disconnect::GetMessageOpcode()
{
	return 4;
}

ClientIPAddresses::ClientIPAddresses()
{
}

ClientIPAddresses::~ClientIPAddresses()
{
}

void ClientIPAddresses::Serialize(EvayrNet::DataStreamWriter& aWriter)
{
	// Serialize header
	aWriter.Write(GetMessageSize()); // message size
	aWriter.Write(uint8_t(5)); // opcode

	// Serialize member variables
	aWriter.Write(uint16_t(ips.size()));
	for (size_t i = 0; i < ips.size(); ++i)
	{
		aWriter.Write(ips[i]);
	}

	aWriter.Write(uint16_t(ports.size()));
	for (size_t i = 0; i < ports.size(); ++i)
	{
		aWriter.Write(ports[i]);
	}

	aWriter.Write(uint16_t(connectionIds.size()));
	for (size_t i = 0; i < connectionIds.size(); ++i)
	{
		aWriter.Write(connectionIds[i]);
	}

}

void ClientIPAddresses::Deserialize(EvayrNet::DataStreamReader& aReader)
{
	// Deserialize member variables
	uint16_t ips_count = 0;
	aReader.Read(ips_count);
	for (size_t i = 0; i < ips_count; ++i)
	{
		std::string t;
		aReader.Read(t);
		ips.push_back(t);
	}
	uint16_t ports_count = 0;
	aReader.Read(ports_count);
	for (size_t i = 0; i < ports_count; ++i)
	{
		uint16_t t;
		aReader.Read(t);
		ports.push_back(t);
	}
	uint16_t connectionIds_count = 0;
	aReader.Read(connectionIds_count);
	for (size_t i = 0; i < connectionIds_count; ++i)
	{
		uint16_t t;
		aReader.Read(t);
		connectionIds.push_back(t);
	}

	// Notify receiver that a message has been processed
	ClientIPAddresses_Receive(*this);
}

uint16_t ClientIPAddresses::GetMessageSize()
{
	uint16_t arraysSize = 0;
	for (size_t i = 0; i < ips.size(); ++i) {arraysSize += uint16_t(ips[i].size() + 2); }
	arraysSize += uint16_t(ports.size()) * 2;
	arraysSize += uint16_t(connectionIds.size()) * 2;

	return 3 + arraysSize;
}

uint8_t ClientIPAddresses::GetMessageOpcode()
{
	return 5;
}

} // namespace Messages
} // namespace EvayrNet

