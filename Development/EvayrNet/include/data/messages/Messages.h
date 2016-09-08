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

	virtual void Serialize(EvayrNet::DataStreamWriter aWriter){}
	virtual void Deserialize(EvayrNet::DataStreamReader aReader){}

	virtual uint16_t GetMessageSize(){ return 0; }
};

class ConnectionRequest : public Message
{
public:
	ConnectionRequest();
	~ConnectionRequest();

	void Serialize(EvayrNet::DataStreamWriter aWriter);
	void Deserialize(EvayrNet::DataStreamReader aReader);

	uint16_t GetMessageSize();
	static uint8_t GetMessageOpcode();

};
extern void ConnectionRequest_Receive(const Messages::ConnectionRequest& acMessage);

class ConnectionResponse : public Message
{
public:
	ConnectionResponse();
	~ConnectionResponse();

	void Serialize(EvayrNet::DataStreamWriter aWriter);
	void Deserialize(EvayrNet::DataStreamReader aReader);

	uint16_t GetMessageSize();
	static uint8_t GetMessageOpcode();

	uint8_t response;
};
extern void ConnectionResponse_Receive(const Messages::ConnectionResponse& acMessage);

class ClientIPAddresses : public Message
{
public:
	ClientIPAddresses();
	~ClientIPAddresses();

	void Serialize(EvayrNet::DataStreamWriter aWriter);
	void Deserialize(EvayrNet::DataStreamReader aReader);

	uint16_t GetMessageSize();
	static uint8_t GetMessageOpcode();

	std::vector<std::string> ips;
	std::vector<uint16_t> ports;
	std::vector<uint16_t> connectionIds;
};
extern void ClientIPAddresses_Receive(const Messages::ClientIPAddresses& acMessage);

} // namespace Messages
} // namespace EvayrNet

#endif
