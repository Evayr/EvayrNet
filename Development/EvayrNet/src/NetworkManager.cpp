#include "NetworkManager.h"
#include "socket\windows\WindowsUDPSocket.h"

using namespace EvayrNet;

std::unique_ptr<NetworkManager> g_Network = std::make_unique<NetworkManager>();

NetworkManager::NetworkManager()
{
}

NetworkManager::~NetworkManager()
{
	if (IsConnected())
	{
		Disconnect();
	}
}

void NetworkManager::CreateSocket()
{
#if defined(_WIN64) || defined(_WIN32)
	m_UDPSocket = std::make_unique<WindowsUDPSocket>(kDefaultPort, kDefaultTickRateSend, kDefaultTickRateRecv);
#else
	// Linux? MacOS?
#endif
}

void NetworkManager::Update()
{
	m_UDPSocket->Update();
}

void NetworkManager::ConnectTo(const char* apIP, uint16_t aPort)
{
	m_UDPSocket->ConnectTo(apIP, aPort);
}

void NetworkManager::Disconnect()
{
	m_UDPSocket->Disconnect();
}

void NetworkManager::Send(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID)
{
	m_UDPSocket->AddMessage(apMessage, Messages::EMessageType::MESSAGE_UNRELIABLE, aConnectionID);
}

void NetworkManager::SendReliable(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID)
{
	m_UDPSocket->AddMessage(apMessage, Messages::EMessageType::MESSAGE_RELIABLE, aConnectionID);
}

void NetworkManager::SendSequenced(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID)
{
	m_UDPSocket->AddMessage(apMessage, Messages::EMessageType::MESSAGE_SEQUENCED, aConnectionID);
}

void NetworkManager::SetTickRates(uint8_t aSendTickRate, uint8_t aRecvTickRate)
{
	m_UDPSocket->SetTickRates(aSendTickRate, aRecvTickRate);
}

bool NetworkManager::IsConnected() const
{
	return m_UDPSocket->IsConnected();
}
