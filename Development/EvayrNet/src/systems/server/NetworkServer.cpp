#include "systems\server\NetworkServer.h"
#include <memory>
#include "data\messages\MessageTypes.h"
#include "NetworkManager.h"

using namespace EvayrNet;

NetworkServer::NetworkServer()
	: m_MaxPlayerCount(kDefaultMaxPlayers)
{
	// Set server connection ID - is always 0
	g_Network->GetUDPSocket()->SetConnectionID(0);

	m_IsServer = true;
}

NetworkServer::~NetworkServer()
{
}

void NetworkServer::SetMaxPlayers(uint8_t aPlayerCount)
{
	m_MaxPlayerCount = aPlayerCount;
}

void NetworkServer::OnConnectionRequest(const Messages::ConnectionRequest& acMessage)
{
	if (g_Network->GetUDPSocket()->GetActiveConnectionsCount() < m_MaxPlayerCount)
	{
		printf("Client accepted\n");

		// Welcome the client
		std::shared_ptr<Messages::ConnectionResponse> pMessage = std::make_shared<Messages::ConnectionResponse>();
		pMessage->response = Messages::EConnectionResult::RESULT_SUCCESS;
		pMessage->connectionID = g_Network->GetUDPSocket()->GetNewestConnection()->GetConnectionID();
		g_Network->SendReliable(pMessage, pMessage->connectionID);
	}
	else
	{
		printf("Client refused\n");
		// Refuse the client
		std::shared_ptr<Messages::ConnectionResponse> pMessage = std::make_shared<Messages::ConnectionResponse>();
		pMessage->response = Messages::EConnectionResult::RESULT_SERVER_FULL;
		pMessage->connectionID = g_Network->GetUDPSocket()->GetNewestConnection()->GetConnectionID();
		g_Network->SendReliable(pMessage, pMessage->connectionID);
		g_Network->GetUDPSocket()->GetNewestConnection()->SetActive(false);
	}
}

void NetworkServer::OnConnectionResponse(const Messages::ConnectionResponse& acMessage)
{
	// Server doesn't need to process a connection response
}

void NetworkServer::OnDisconnect(const Messages::Disconnect& acMessage)
{
	Connection* pConnection = g_Network->GetUDPSocket()->GetConnection(acMessage.connectionID);
	
	if (pConnection)
	{
		pConnection->SetActive(false);
	}
}

void NetworkServer::OnClientIPAddresses(const Messages::ClientIPAddresses& acMessage)
{
	// Server doesn't need to process this
}

