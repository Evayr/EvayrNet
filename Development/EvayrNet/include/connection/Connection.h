#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <memory>
#include <vector>
#include <list>
#include <ctime>

#include "connection\IPAddress.h"
#include "data\messages\Messages.h"
#include "data\messages\MessageTypes.h"
#include "data\packet\Packet.h"

namespace EvayrNet
{
	class Connection
	{
	public:
		enum
		{
			kHeartbeatInterval = 250, // ms
			kConnectionTimout = 5000, // ms
		};

		Connection(IPAddress aIPAddress, int16_t aConnectionID, bool aSendHeartbeats);
		~Connection();

		void Update();

		void AddMessage(std::shared_ptr<Messages::Message> apMessage, Messages::EMessageType aType);
		void AddCachedMessage();
		void RemoveCachedMessage();

		const IPAddress& GetIPAddress();
		uint8_t GetPacketCount() const;
		std::shared_ptr<Packet> GetPacket(uint8_t aPacketID);
		const std::list<std::shared_ptr<Messages::Message>>& GetCachedMessages();
		void ClearPackets();

		int16_t GetConnectionID() const;

		void SetActive(bool aVal);
		bool IsActive() const;

		void ProcessHeartbeat(const Messages::Heartbeat& acMessage);

		uint32_t GetPing() const;

		void EnableAutoHeartbeat();
		void DisableAutoHeartbeat();

	private:
		void UpdateLifetime();
		void SendHeartbeat(bool aForceSend);
		bool HeartbeatIsNewer(uint8_t aID);

		// Sending / receiving data
		std::vector<std::shared_ptr<Packet>> m_Packets;
		std::list<std::shared_ptr<Messages::Message>> m_CachedMessages;

		// Connectivity
		IPAddress m_IPAddress;
		int16_t m_ConnectionID;
		bool m_Active : 1;

		// Heartbeat
		bool m_SendAutoHeartbeats : 1;
		clock_t m_HeartbeatClock;
		uint8_t m_HeartbeatID;

		// Ping
		clock_t m_PingClock;
		uint32_t m_Ping;
	};
}

#endif
