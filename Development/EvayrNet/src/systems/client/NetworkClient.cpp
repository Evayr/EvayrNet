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
			printf("Successfully connected to the server! Our Connection ID is: %i\n", acMessage.connectionID);
			g_Network->GetUDPSocket()->SetConnected(true);
			g_Network->GetUDPSocket()->SetConnectionID(acMessage.connectionID);

			/* Test */
			auto pText1 = std::make_shared<Messages::PrintText>();
			pText1->text = "This is sequenced text number 1";

			auto pText2 = std::make_shared<Messages::PrintText>();
			pText2->text = "This is sequenced text number 2";

			auto pText3 = std::make_shared<Messages::PrintText>();
			pText3->text = "This is sequenced text number 3";

			g_Network->SendSequenced(pText1);
			g_Network->SendSequenced(pText2);
			g_Network->SendSequenced(pText3);

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

void NetworkClient::OnDisconnect(const Messages::Disconnect& acMessage)
{
	switch (acMessage.reason)
	{
		case Messages::EDisconnectReason::REASON_QUIT:
		{
			printf("Connection ID %i has quit\n", acMessage.connectionID);
			break;
		}

		case Messages::EDisconnectReason::REASON_TIMED_OUT:
		{
			printf("Connection ID %i has been timed out.\n", acMessage.connectionID);
			break;
		}

		case Messages::EDisconnectReason::REASON_KICKED:
		{
			printf("Connection ID %i has been kicked.\n", acMessage.connectionID);
			break;
		}

		case Messages::EDisconnectReason::REASON_BANNED:
		{
			printf("Connection ID %i has been banned.\n", acMessage.connectionID);
			break;
		}
	}

	if (acMessage.connectionID == 0)
	{
		g_Network->GetUDPSocket()->SetConnected(false);
	}
	else
	{
		Connection* pConnection = g_Network->GetUDPSocket()->GetConnection(acMessage.connectionID);
		if (pConnection)
		{
			pConnection->SetActive(false);
		}
	}
}
