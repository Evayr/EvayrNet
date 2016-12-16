#include "systems\NetworkSystem.h"
#include "NetworkManager.h"

using namespace EvayrNet;

NetworkSystem::NetworkSystem()
{
}

NetworkSystem::~NetworkSystem()
{
}

void NetworkSystem::RegisterOnConnectionResultCallback(std::function<void(EvayrNet::Messages::EConnectionResult)> aCallback)
{
	m_OnConnectionResult = aCallback;
}

void NetworkSystem::RegisterOnDisconnectCallback(std::function<void(EvayrNet::Messages::EDisconnectReason)> aCallback)
{
	m_OnDisconnect = aCallback;
}

void NetworkSystem::RegisterOnPlayerAddCallback(std::function<void(uint16_t)> aCallback)
{
	m_OnPlayerAdd = aCallback;
}

void NetworkSystem::RegisterOnPlayerDisconnectCallback(std::function<void(uint16_t, EvayrNet::Messages::EDisconnectReason)> aCallback)
{
	m_OnPlayerDisconnect = aCallback;
}

bool NetworkSystem::IsServer() const
{
	return m_IsServer;
}

// Handles
void EvayrNet::Messages::ConnectionRequest_Receive(const Messages::ConnectionRequest& acMessage)
{
	g_Network->GetNetworkSystem()->OnConnectionRequest(acMessage);
}

void EvayrNet::Messages::ConnectionResponse_Receive(const Messages::ConnectionResponse& acMessage)
{
	g_Network->GetNetworkSystem()->OnConnectionResponse(acMessage);
}

void EvayrNet::Messages::Disconnect_Receive(const Messages::Disconnect& acMessage)
{
	g_Network->GetNetworkSystem()->OnDisconnect(acMessage);
}
