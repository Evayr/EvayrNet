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
		g_Network->GetDebugger()->Print("Client accepted. Connection ID: " + std::to_string(g_Network->GetUDPSocket()->GetNewestConnection()->GetConnectionID()));

		// Welcome the client
		auto pMessage = std::make_shared<Messages::ConnectionResponse>();
		pMessage->response = Messages::EConnectionResult::RESULT_SUCCESS;
		pMessage->connectionID = g_Network->GetUDPSocket()->GetNewestConnection()->GetConnectionID();
		g_Network->SendReliable(pMessage, pMessage->connectionID);

		// Send callback to application
		if (m_OnPlayerAdd)
		{
			m_OnPlayerAdd(g_Network->GetUDPSocket()->GetNewestConnection()->GetConnectionID());
		}
	}
	else
	{
		g_Network->GetDebugger()->Print("Client connection request has been refused\n");
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
		g_Network->GetDebugger()->Print("Connection ID " + std::to_string(acMessage.connectionID) + " has disconnected.\n");
		pConnection->SetActive(false);

		// Send callback to application
		if (m_OnPlayerDisconnect)
		{
			m_OnPlayerDisconnect(pConnection->GetConnectionID(), Messages::EDisconnectReason::REASON_QUIT);
		}
	}
}
