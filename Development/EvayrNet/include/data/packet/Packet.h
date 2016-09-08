#ifndef _PACKET_H_
#define _PACKET_H_

#include <list>
#include <memory>

#include "data\stream\DataStream.h"
#include "data\messages\Messages.h"

namespace EvayrNet
{
	class Message;

	class Packet
	{
	public:
		enum
		{
			kMaxPacketSize = 4096,
		};

		Packet();
		~Packet();

		void AddMessage(std::shared_ptr<Messages::Message> apMessage);
		void RemoveMessage(std::shared_ptr<Messages::Message> apMessage);

		void Serialize();
		void Deserialize();

		void Encrypt();
		void Decrypt();

		const char* GetData() const;
		uint16_t GetDataSize() const;

	private:
		DataStream m_DataStream;
		
		std::list<std::shared_ptr<Messages::Message>> m_Messages;
	};
}

#endif
