#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include <memory>
#include <functional>

#include "data\packet\PacketHandler.h"
#include "socket\UDPSocket.h"
#include "systems\NetworkSystem.h"
#include "utils\NetworkSimulator.h"

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

		// Connectivity
		void ConnectTo(const char* apIP, uint16_t aPort);
		void Disconnect();

		// Sending of messages
		void Send(std::shared_ptr<Messages::Message> apMessage, uint16_t aConnectionID = 0);
		void SendReliable(std::shared_ptr<Messages::Message> apMessage, uint16_t aConnectionID = 0, bool aStoreACK = true);
		void SendSequenced(std::shared_ptr<Messages::Message> apMessage, uint16_t aConnectionID = 0, bool aStoreACK = true);

		// Message registration
		void RegisterMessage(std::unique_ptr<Messages::Message> apMessage, uint8_t aOpCode);

		// Tick rate
		void SetTickRate(uint8_t aSendTickRate = kDefaultTickRate);

		// Simulation
		void StartSimulation(const uint32_t acMinimumDelayMS, const uint32_t acRandomDelayMS, const float acPacketLossPct, const float acPacketDuplicationPct);
		void StopSimulation();

		// Callback registration
		void RegisterOnConnectionResultCallback(std::function<void(EvayrNet::Messages::EConnectionResult)> aCallback);
		void RegisterOnDisconnectCallback(std::function<void(EvayrNet::Messages::EDisconnectReason)> aCallback);
		void RegisterOnPlayerAddCallback(std::function<void(uint16_t)> aCallback);
		void RegisterOnPlayerDisconnectCallback(std::function<void(uint16_t, EvayrNet::Messages::EDisconnectReason)> aCallback);

		// Connectivity gets
		const bool IsConnected() const;
		const bool IsServer() const;

		// Classes gets
		NetworkSystem* GetNetworkSystem();
		UDPSocket* GetUDPSocket();

		// Debugging info
		const uint32_t GetNewestPing(uint16_t aConnectionID = UDPSocket::kServerConnectionID) const;
		const uint32_t GetAveragePing(uint16_t aConnectionID = UDPSocket::kServerConnectionID) const;
		const uint32_t GetIncomingPacketsPerSecond() const;
		const uint32_t GetOutgoingPacketsPerSecond() const;
		const uint32_t GetPacketsPerSecondLost() const;
		const uint32_t GetIncomingDataPerSecond() const;
		const uint32_t GetOutgoingDataPerSecond() const;
		const uint16_t GetActiveConnectionsCount() const;

	private:
		void CreateSocket(uint16_t aPort);

		bool m_isServer;

		PacketHandler m_PacketHandler;
		std::unique_ptr<UDPSocket> m_pUDPSocket;
		std::unique_ptr<NetworkSystem> m_pNetworkSystem;
		NetworkSimulator m_NetworkSimulator;
	};

	extern NetworkManager* g_Network;
}

#endif
