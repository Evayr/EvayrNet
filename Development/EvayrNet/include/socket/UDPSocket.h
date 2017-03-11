#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_

#include <cstdint>
#include <ctime>
#include <list>

#include "data\packet\PacketHandler.h"
#include "data\messages\MessageTypes.h"
#include "connection\Connection.h"
#include "utils\IDGenerator.h"

namespace EvayrNet
{
	class UDPSocket
	{
	public:
		enum
		{
			kRetryConnectInterval = 3000,
			kConnectionAttempts = 3,
			kServerConnectionID = 1,
		};
		UDPSocket();
		~UDPSocket();

		void ConnectTo(const char* acpIP, uint16_t aPort);
		void Disconnect();

		void Update();

		void AddMessage(std::shared_ptr<Messages::Message> apMessage, uint16_t aConnectionID = 0, bool aStoreACK = false);

		void SetTickRate(uint8_t aTickRateSend);

		void ProcessHeartbeat(const Messages::Heartbeat& acMessage);

		uint16_t ProcessIPAddress(const IPAddress& aIPAddress);

		void ProcessACKAcknowledgment(const Messages::AcknowledgeACK& acACK);

		void SetConnected(bool aVal);
		bool IsConnected() const;

		void AddConnection(const IPAddress& acIPAddress, bool aSendHeartbeats = false);
		Connection* GetNewestConnection();
		Connection* GetConnection(uint16_t aID);
		const uint8_t GetActiveConnectionsCount() const;

		void SetConnectionID(uint16_t aVal);
		const uint16_t GetConnectionID() const;

		const uint16_t GetPort() const;

		// Data
		const uint32_t GetIncomingPacketsPerSecond() const;
		const uint32_t GetOutgoingPacketsPerSecond() const;
		const uint32_t GetPacketsPerSecondLost() const;
		const uint32_t GetIncomingDataPerSecond() const;
		const uint32_t GetOutgoingDataPerSecond() const;

	protected:
		struct DataDebugInfo
		{
			DataDebugInfo(uint16_t aDataSize, clock_t aTime)
				: dataSize(aDataSize)
				, time(aTime)
			{}
			uint16_t dataSize;
			clock_t time;
		};

		virtual void Bind(uint16_t aPort) = 0;
		virtual void Send() = 0;
		virtual void Receive() = 0;

		// Packet Handler
		PacketHandler* m_pPacketHandler;

		// Messages
		uint16_t m_ACKMessageSize;

		// Connections
		std::list<Connection> m_Connections;
		IDGenerator m_ConnectionIDGenerator;

		// Debugging info
		std::list<clock_t> m_PPSOut; // Packets per second - outgoing
		std::list<clock_t> m_PPSIn; // Packets per second - incoming
		std::list<clock_t> m_PPSLost; // Packets per second - lost
		std::list<DataDebugInfo> m_DataPerSecondSent;
		std::list<DataDebugInfo> m_DataPerSecondReceived;

		// Socket
		uint16_t m_Port;

	private:
		void Connect();
		void SendPackets(bool aForceSend = false);
		void ReceivePackets();
		void UpdateConnections();
		void UpdateStatistics();
		void RemoveTimeIfExceedsAmount(std::list<clock_t>* apList, float aTime = 1000.f);
		void RemoveTimeIfExceedsAmount(std::list<DataDebugInfo>* apList, float aTime = 1000.f);

		void AddConnection(const IPAddress& aIPAddress, uint16_t aConnectionID);

		// Tick rates
		uint8_t m_TickRateSend;
		clock_t m_ClockPerTickSend;
		clock_t m_SendClock;

		// Connecting
		clock_t m_ConnectClock;
		uint8_t m_ConnectionAttempts;
		uint16_t m_ConnectionID; // Our connection ID where others can recognize us from
		bool m_Connecting : 1;
		bool m_Connected : 1;
	};
}

#endif
