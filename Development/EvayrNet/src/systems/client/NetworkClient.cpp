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
	switch (acMessage.response)
	{
		case Messages::EConnectionResult::RESULT_SUCCESS:
		{
			printf("Successfully connected to the server!\n");
			g_Network->GetUDPSocket()->SetConnected(true);
			g_Network->GetUDPSocket()->SetConnectionID(acMessage.connectionID);
			break;
		}

		case Messages::EConnectionResult::RESULT_SERVER_FULL:
		{
			printf("Failed to connect to the server: Server is full!\n");
			g_Network->GetUDPSocket()->SetConnected(false);
			break;
		}
	}
}

void NetworkClient::OnClientIPAddresses(const Messages::ClientIPAddresses& acMessage)
{

}
