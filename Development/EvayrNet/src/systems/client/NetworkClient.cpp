#include "systems\client\NetworkClient.h"
#include "data\messages\Messages.h"
#include "data\messages\MessageTypes.h"
#include "NetworkManager.h"

using namespace EvayrNet;

NetworkClient::NetworkClient()
{
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
			break;
		}

		case Messages::EConnectionResult::RESULT_SERVER_FULL:
		{
			break;
		}
	}
}

void NetworkClient::OnClientIPAddresses(const Messages::ClientIPAddresses& acMessage)
{

}
