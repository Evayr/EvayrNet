#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include <memory>

#include "data\packet\PacketHandler.h"
#include "socket\UDPSocket.h"
#include "systems\NetworkSystem.h"

namespace EvayrNet
{
	class NetworkManager
	{
	public:
		enum
		{
			kDefaultPort = 12345,
			kDefaultTickRate = 60, // Hz
		};

		NetworkManager(uint16_t aPort, bool aIsServer);
		~NetworkManager();

		void Update();

		void ConnectTo(const char* apIP, uint16_t aPort);
		void Disconnect();

		void Send(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID = -1);
		void SendReliable(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID = -1);
		void SendSequenced(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID = -1);

		void RegisterMessage(Messages::Message* apMessage, uint8_t aOpCode);

		void SetTickRate(uint8_t aSendTickRate = kDefaultTickRate);

		bool IsConnected() const;

		NetworkSystem* GetNetworkSystem();
		UDPSocket* GetUDPSocket();

	private:
		void CreateSocket(uint16_t aPort);

		PacketHandler m_PacketHandler;
		std::unique_ptr<UDPSocket> m_pUDPSocket;
		std::unique_ptr<NetworkSystem> m_pNetworkSystem;
	};

	extern NetworkManager* g_Network;
}

#endif
