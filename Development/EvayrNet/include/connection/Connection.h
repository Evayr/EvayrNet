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
			kResendDelay = 5, // ms - rough estimation for the processing delay / ping differential
		};

		struct CachedACKMessage
		{
			std::shared_ptr<Messages::Message> m_pMessage;
			uint8_t m_ACKID;
			clock_t m_TimeSent;
		};

		struct SequencedMessage
		{
			Messages::Message m_Message;
			uint8_t m_SequenceID;
		};

		Connection(const IPAddress& acIPAddress, uint16_t aConnectionID, bool aSendHeartbeats, uint16_t aHeartbeatInterval = kDefaultHeartbeatInterval, uint32_t aConnectionTimeout = kDefaultConnectionTimout);
		~Connection();

		void Update();

		void AddMessage(const std::shared_ptr<Messages::Message>& apMessage, bool aStoreACK, uint16_t aExtraMessageSize = 0);

		// ACK
		void AddCachedACKMessage(const std::shared_ptr<Messages::Message>& apMessage, uint8_t aACKID);
		void RemoveCachedACKMessage(uint8_t aACKID);

		// Sequenced
		void AddCachedSequencedMessage(std::shared_ptr<Messages::Message> apMessage);
		void ExecuteSequencedMessages();

		const IPAddress& GetIPAddress();
		uint8_t GetPacketCount() const;
		std::shared_ptr<Packet> GetPacket(uint8_t aPacketID);
		const std::vector<Connection::CachedACKMessage>& GetCachedACKMessages();
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
		void UpdateLifetime();
		void ResendMessages();

		void SendHeartbeat(bool aForceSend);
		bool ACKIsNewer(uint8_t aID) const;
		bool SequenceIsNewer(uint8_t aID) const;
		bool HeartbeatIsNewer(uint8_t aID) const;

		// Sending / receiving data
		std::vector<std::shared_ptr<Packet>> m_Packets;
		std::vector<CachedACKMessage> m_CachedACKMessages;
		std::list<std::shared_ptr<Messages::Message>> m_SequencedMessages;

		// Connectivity
		IPAddress m_IPAddress;
		uint16_t m_ConnectionID;
		bool m_Active : 1;

		// ACKs
		uint8_t m_NewestSendACKID;
		uint8_t m_NewestReceiveACKID;

		// Sequences
		uint8_t m_NewestSendSequenceID;
		uint8_t m_NewestReceiveSequenceID;

		// Heartbeat
		bool m_SendHeartbeats : 1;
		uint16_t m_HeartbeatInterval;
		uint32_t m_ConnectionTimeout;
		clock_t m_HeartbeatClock;
		uint8_t m_HeartbeatID;

		// Ping
		clock_t m_PingClock;
		uint32_t m_Ping;
	};
}

#endif
