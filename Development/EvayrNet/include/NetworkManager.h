#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include <memory>

#include "data\packet\PacketHandler.h"
#include "socket\UDPSocket.h"

namespace EvayrNet
{
	class NetworkManager
	{
	public:
		enum
		{
			kDefaultPort = 37475,
			kDefaultTickRateSend = 60,
			kDefaultTickRateRecv = 60,
		};
		NetworkManager();
		~NetworkManager();

		void CreateSocket();

		void Update();

		void ConnectTo(const char* apIP, uint16_t aPort);
		void Disconnect();

		void Send(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID = -1);
		void SendReliable(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID = -1);
		void SendSequenced(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID = -1);

		void RegisterMessage(Messages::Message* apMessage, uint8_t aOpCode);

		void SetTickRates(uint8_t aSendTickRate = 60, uint8_t aRecvTickRate = 60);

		bool IsConnected() const;

	private:
		PacketHandler m_PacketHandler;
		std::unique_ptr<UDPSocket> m_UDPSocket;
	};

	extern std::unique_ptr<NetworkManager> g_Network;
}

#endif
