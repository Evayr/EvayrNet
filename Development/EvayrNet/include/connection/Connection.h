#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <memory>
#include <vector>
#include <list>

#include "connection\IPAddress.h"
#include "data\messages\Messages.h"
#include "data\messages\MessageTypes.h"
#include "data\packet\Packet.h"

namespace EvayrNet
{
	class Connection
	{
	public:
		Connection(IPAddress aIPAddress, int16_t aConnectionID);
		~Connection();

		void Update();

		void AddMessage(std::shared_ptr<Messages::Message> apMessage, Messages::EMessageType aType);
		void AddCachedMessage();
		void RemoveCachedMessage();

		const IPAddress& GetIPAddress();
		uint8_t GetPacketCount() const;
		std::shared_ptr<Packet> GetPacket(uint8_t aPacketID);
		const std::list<std::shared_ptr<Messages::Message>>& GetCachedMessages();

		int16_t GetConnectionID() const;

	private:
		void SendHeartbeat();
		void ProcessMessage();

		IPAddress m_IPAddress;

		std::vector<std::shared_ptr<Packet>> m_Packets;
		std::list<std::shared_ptr<Messages::Message>> m_CachedMessages;

		int16_t m_ConnectionID;
	};
}

#endif
