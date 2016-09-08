#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_

#include <cstdint>
#include <ctime>
#include <vector>

#include "data\messages\MessageTypes.h"
#include "connection\Connection.h"

namespace EvayrNet
{
	class UDPSocket
	{
	public:
		UDPSocket();
		~UDPSocket();

		void ConnectTo(const char* apIP, uint16_t aPort);
		void Disconnect();

		void Update();

		void AddMessage(std::shared_ptr<Messages::Message> apMessage, Messages::EMessageType aType, int16_t aConnectionID = -1);

		void SetTickRates(uint8_t aTickRateSend, uint8_t aTickRateRecv);

		bool IsConnected() const;

	private:
		void SendPackets();
		void ReceivePackets();
		void UpdateConnections();

		void AddConnection(IPAddress aIPAddress, int16_t aConnectionID);

		// Tick rates
		uint8_t m_TickRateSend;
		uint8_t m_TickRateRecv;

		clock_t m_ClockPerTickSend;
		clock_t m_ClockPerTickRecv;

		clock_t m_SendClock;
		clock_t m_RecvClock;

	protected:
		virtual void Send()=0;
		virtual void Receive()=0;

		// Connection
		bool m_Connecting : 1;
		bool m_Connected : 1;

		// Connections
		std::vector<Connection> m_Connections;
	};
}

#endif
