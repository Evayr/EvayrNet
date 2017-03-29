#include "systems\client\NetworkClient.h"
#include "data\messages\Messages.h"
#include "data\messages\MessageTypes.h"
#include "NetworkManager.h"

using namespace EvayrNet;

NetworkClient::NetworkClient()
{
	m_IsServer = false;
}

NetworkClient::~NetworkClient()
{
}

void NetworkClient::OnConnectionRequest(const Messages::ConnectionRequest& acMessage)
{
	// Client doesn't need to process this
}

void NetworkClient::OnConnectionResponse(const Messages::ConnectionResponse& acMessage)
{
	if (g_Network->GetUDPSocket()->IsConnected()) return;

	switch (acMessage.response)
	{
		case Messages::EConnectionResult::RESULT_SUCCESS:
		{
			g_Network->GetDebugger()->Print("Successfully connected to the server! Our Connection ID is: " + std::to_string(acMessage.connectionID));
			g_Network->GetUDPSocket()->SetConnected(true);
			g_Network->GetUDPSocket()->SetConnectionID(acMessage.connectionID);
			break;
		}

		case Messages::EConnectionResult::RESULT_SERVER_FULL:
		{
			g_Network->GetDebugger()->Print("Failed to connect to the server: Server is full!\n");
			g_Network->GetUDPSocket()->SetConnected(false);
			break;
		}
	}

	// Send callback to application
	if (m_OnConnectionResult)
	{
		m_OnConnectionResult(Messages::EConnectionResult(acMessage.response));
	}
}

void NetworkClient::OnDisconnect(const Messages::Disconnect& acMessage)
{
	switch (acMessage.reason)
	{
		case Messages::EDisconnectReason::REASON_QUIT:
		{
			g_Network->GetDebugger()->Print("Connection ID " + std::to_string(acMessage.connectionID) + " has quit");
			break;
		}

		case Messages::EDisconnectReason::REASON_TIMED_OUT:
		{
			g_Network->GetDebugger()->Print("Connection ID " + std::to_string(acMessage.connectionID) + " has been timed out.");
			break;
		}

		case Messages::EDisconnectReason::REASON_KICKED:
		{
			g_Network->GetDebugger()->Print("Connection ID " + std::to_string(acMessage.connectionID) + " has been kicked.");
			break;
		}

		case Messages::EDisconnectReason::REASON_BANNED:
		{
			g_Network->GetDebugger()->Print("Connection ID " + std::to_string(acMessage.connectionID) + " has been banned.");
			break;
		}
	}

	if (acMessage.connectionID == 0)
	{
		g_Network->GetUDPSocket()->SetConnected(false);

		// Send callback to application
		if (m_OnDisconnect)
		{
			m_OnDisconnect(Messages::EDisconnectReason(acMessage.reason));
		}
	}
	else
	{
		Connection* pConnection = g_Network->GetUDPSocket()->GetConnection(acMessage.connectionID);
		if (pConnection)
		{
			pConnection->SetActive(false);
		}

		// Send callback to application
		if (m_OnPlayerDisconnect)
		{
			m_OnPlayerDisconnect(acMessage.connectionID, Messages::EDisconnectReason(acMessage.reason));
		}
	}
}
