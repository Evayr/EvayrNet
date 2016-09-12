#include "systems\NetworkSystem.h"
#include "NetworkManager.h"

using namespace EvayrNet;

NetworkSystem::NetworkSystem()
{
}

NetworkSystem::~NetworkSystem()
{
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

void EvayrNet::Messages::ClientIPAddresses_Receive(const Messages::ClientIPAddresses& acMessage)
{
	g_Network->GetNetworkSystem()->OnClientIPAddresses(acMessage);
}
