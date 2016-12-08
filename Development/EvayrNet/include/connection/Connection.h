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
			kDefaultHeartbeatInterval = 250, // ms
			kDefaultConnectionTimout = 5000, // ms
		};

		struct CachedMessage
		{
			std::shared_ptr<Messages::Message> m_pMessage;
			uint8_t m_ackID;
		};

		Connection(const IPAddress& acIPAddress, uint16_t aConnectionID, bool aSendHeartbeats, uint16_t aHeartbeatInterval = kDefaultHeartbeatInterval, uint32_t aConnectionTimeout = kDefaultConnectionTimout);
		~Connection();

		void Update();

		void AddMessage(const std::shared_ptr<Messages::Message>& apMessage);
		void AddCachedMessage(const std::shared_ptr<Messages::Message>& apMessage, uint8_t aACKID);
		void RemoveCachedMessage(uint8_t aACKID);

		const IPAddress& GetIPAddress();
		uint8_t GetPacketCount() const;
		std::shared_ptr<Packet> GetPacket(uint8_t aPacketID);
		const std::vector<Connection::CachedMessage>& GetCachedMessages();
		void ClearPackets();

		uint16_t GetConnectionID() const;

		void SetActive(bool aVal);
		bool IsActive() const;

		void ProcessACK(const Messages::ACK& acACK);
		void ProcessHeartbeat(const Messages::Heartbeat& acMessage);

		uint32_t GetPing() const;

		void EnableAutoHeartbeat();
		void DisableAutoHeartbeat();

	private:
		bool ACKIsNewer(uint8_t aID) const;
		void UpdateLifetime();
		void SendHeartbeat(bool aForceSend);
		bool HeartbeatIsNewer(uint8_t aID) const;

		// Sending / receiving data
		std::vector<std::shared_ptr<Packet>> m_Packets;
		std::vector<CachedMessage> m_CachedMessages;

		// Connectivity
		IPAddress m_IPAddress;
		uint16_t m_ConnectionID;
		bool m_Active : 1;

		// ACKs
		uint8_t m_newestSendACKID;
		uint8_t m_newestReceiveACKID;

		// Heartbeat
		bool m_SendHeartbeats : 1;
		uint16_t m_heartbeatInterval;
		uint32_t m_connectionTimeout;
		clock_t m_HeartbeatClock;
		uint8_t m_HeartbeatID;

		// Ping
		clock_t m_PingClock;
		uint32_t m_Ping;
	};
}

#endif
