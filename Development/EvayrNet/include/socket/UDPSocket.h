#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_

#include <cstdint>
#include <ctime>
#include <vector>

#include "data\packet\PacketHandler.h"
#include "data\messages\MessageTypes.h"
#include "connection\Connection.h"

namespace EvayrNet
{
	class UDPSocket
	{
	public:
		enum
		{
			kRetryConnectInterval = 3000,
			kConnectionAttempts = 3,
		};
		UDPSocket();
		~UDPSocket();

		void ConnectTo(const char* apIP, uint16_t aPort);
		void Disconnect();

		void Update();

		void AddMessage(std::shared_ptr<Messages::Message> apMessage, Messages::EMessageType aType, int16_t aConnectionID = -1);

		void SetTickRate(uint8_t aTickRateSend);

		void ProcessHeartbeat(const Messages::Heartbeat& acMessage);

		int16_t ProcessIPAddress(IPAddress aIPAddress);

		void SetConnected(bool aVal);
		bool IsConnected() const;
		Connection* GetNewestConnection();
		uint8_t GetActiveConnectionsCount() const;

		void SetConnectionID(int16_t aVal);
		int16_t GetConnectionID() const;

	private:
		void Connect();
		void SendPackets();
		void ReceivePackets();
		void UpdateConnections();
		void UpdateStatistics();
		void RemoveTimeIfExceedsAmount(std::list<clock_t>* apList, float aTime = 1000.f);

		void AddConnection(IPAddress aIPAddress, int16_t aConnectionID);

		// Tick rates
		uint8_t m_TickRateSend;
		clock_t m_ClockPerTickSend;
		clock_t m_SendClock;

		// Connecting
		clock_t m_ConnectClock;
		uint8_t m_ConnectionAttempts;
		int16_t m_ConnectionID; // Our connection ID where others can recognize us from
		bool m_Connecting : 1;
		bool m_Connected : 1;

	protected:
		virtual void Bind(uint16_t aPort) = 0;
		virtual void Send() = 0;
		virtual void Receive() = 0;

		// Packet Handler
		PacketHandler* m_pPacketHandler;

		// Connections
		std::vector<Connection> m_Connections;

		// Debugging info
		std::list<clock_t> m_PPSOut; // Packets per second - outgoing
		std::list<clock_t> m_PPSIn; // Packets per second - incoming
		std::list<clock_t> m_PPSLost; // Packets per second - lost
	};
}

#endif
