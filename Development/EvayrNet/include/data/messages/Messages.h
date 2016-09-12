/* This file was automatically generated. Do not modify! */
/* For more info, find protocol.txt / codegen.py or contact @Ivar Slotboom */

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <vector>
#include <string>

#include "data\messages\MessageTypes.h"

#include "data\stream\DataStreamReader.h"
#include "data\stream\DataStreamWriter.h"

namespace EvayrNet
{
namespace Messages
{

class Message
{
public:
	Message::Message();
	Message::~Message();

	virtual void Serialize(EvayrNet::DataStreamWriter& aWriter){}
	virtual void Deserialize(EvayrNet::DataStreamReader& aReader){}

	virtual uint16_t GetMessageSize(){ return 0; }
	virtual uint8_t GetMessageOpcode(){ return 0; }
};

class MessageHeader : public Message
{
public:
	MessageHeader();
	~MessageHeader();

	void Serialize(EvayrNet::DataStreamWriter& aWriter);
	void Deserialize(EvayrNet::DataStreamReader& aReader);

	uint16_t GetMessageSize();
	uint8_t GetMessageOpcode();

	uint16_t size;
	uint8_t opcode;
};
extern void MessageHeader_Receive(const Messages::MessageHeader& acMessage);

class ConnectionRequest : public Message
{
public:
	ConnectionRequest();
	~ConnectionRequest();

	void Serialize(EvayrNet::DataStreamWriter& aWriter);
	void Deserialize(EvayrNet::DataStreamReader& aReader);

	uint16_t GetMessageSize();
	uint8_t GetMessageOpcode();

};
extern void ConnectionRequest_Receive(const Messages::ConnectionRequest& acMessage);

class ConnectionResponse : public Message
{
public:
	ConnectionResponse();
	~ConnectionResponse();

	void Serialize(EvayrNet::DataStreamWriter& aWriter);
	void Deserialize(EvayrNet::DataStreamReader& aReader);

	uint16_t GetMessageSize();
	uint8_t GetMessageOpcode();

	uint8_t response;
	int16_t connectionID;
};
extern void ConnectionResponse_Receive(const Messages::ConnectionResponse& acMessage);

class Heartbeat : public Message
{
public:
	Heartbeat();
	~Heartbeat();

	void Serialize(EvayrNet::DataStreamWriter& aWriter);
	void Deserialize(EvayrNet::DataStreamReader& aReader);

	uint16_t GetMessageSize();
	uint8_t GetMessageOpcode();

	int16_t connectionID;
	uint8_t id;
	uint32_t ping;
};
extern void Heartbeat_Receive(const Messages::Heartbeat& acMessage);

class Disconnect : public Message
{
public:
	Disconnect();
	~Disconnect();

	void Serialize(EvayrNet::DataStreamWriter& aWriter);
	void Deserialize(EvayrNet::DataStreamReader& aReader);

	uint16_t GetMessageSize();
	uint8_t GetMessageOpcode();

	uint8_t reason;
	int16_t connectionID;
};
extern void Disconnect_Receive(const Messages::Disconnect& acMessage);

class ClientIPAddresses : public Message
{
public:
	ClientIPAddresses();
	~ClientIPAddresses();

	void Serialize(EvayrNet::DataStreamWriter& aWriter);
	void Deserialize(EvayrNet::DataStreamReader& aReader);

	uint16_t GetMessageSize();
	uint8_t GetMessageOpcode();

	std::vector<std::string> ips;
	std::vector<uint16_t> ports;
	std::vector<uint16_t> connectionIds;
};
extern void ClientIPAddresses_Receive(const Messages::ClientIPAddresses& acMessage);

} // namespace Messages
} // namespace EvayrNet

#endif
