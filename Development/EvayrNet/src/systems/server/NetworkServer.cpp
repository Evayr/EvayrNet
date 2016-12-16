#include "systems\server\NetworkServer.h"
#include <memory>
#include "data\messages\MessageTypes.h"
#include "NetworkManager.h"

using namespace EvayrNet;

NetworkServer::NetworkServer()
	: m_MaxPlayerCount(kDefaultMaxPlayers)
{
	// Assign ourselves as server
	m_IsServer = true;

	// Set server connection ID - is always 0
	g_Network->GetUDPSocket()->SetConnectionID(0);

	// Add ourselves as connection ID 0
	IPAddress ip;
	ip.m_Address = "127.0.0.1";
	ip.m_Port = g_Network->GetUDPSocket()->GetPort();
	g_Network->GetUDPSocket()->AddConnection(ip);
	g_Network->GetUDPSocket()->GetNewestConnection()->SetActive(false);
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
		printf("Client accepted. Connection ID: %u\n", g_Network->GetUDPSocket()->GetNewestConnection()->GetConnectionID());

		// Welcome the client
		auto pMessage = std::make_shared<Messages::ConnectionResponse>();
		pMessage->response = Messages::EConnectionResult::RESULT_SUCCESS;
		pMessage->connectionID = g_Network->GetUDPSocket()->GetNewestConnection()->GetConnectionID();
		g_Network->SendReliable(pMessage, pMessage->connectionID);
	}
	else
	{
		printf("Client refused\n");
		// Refuse the client
		auto pMessage = std::make_shared<Messages::ConnectionResponse>();
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
		printf("Connection ID %i has disconnected.\n", acMessage.connectionID);
		pConnection->SetActive(false);
	}
}
