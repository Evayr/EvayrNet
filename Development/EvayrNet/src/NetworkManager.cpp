#include "NetworkManager.h"
#include "socket\windows\WindowsUDPSocket.h"
#include "systems\client\NetworkClient.h"
#include "systems\server\NetworkServer.h"

using namespace EvayrNet;

EvayrNet::NetworkManager* EvayrNet::g_Network = nullptr;

NetworkManager::NetworkManager(bool aIsServer)
{
	CreateSocket();

	if (aIsServer)
	{
		m_pNetworkSystem = std::make_unique<NetworkServer>();
	}
	else
	{
		m_pNetworkSystem = std::make_unique<NetworkClient>();
	}

	g_Network = this;
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
	m_pUDPSocket = std::make_unique<WindowsUDPSocket>(&m_PacketHandler, kDefaultPort, kDefaultTickRateSend, kDefaultTickRateRecv);
#else
	// Linux?
#endif
}

void NetworkManager::Update()
{
	m_pUDPSocket->Update();
}

void NetworkManager::ConnectTo(const char* apIP, uint16_t aPort)
{
	m_pUDPSocket->ConnectTo(apIP, aPort);
}

void NetworkManager::Disconnect()
{
	m_pUDPSocket->Disconnect();
}

void NetworkManager::Send(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID)
{
	m_pUDPSocket->AddMessage(apMessage, Messages::EMessageType::MESSAGE_UNRELIABLE, aConnectionID);
}

void NetworkManager::SendReliable(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID)
{
	m_pUDPSocket->AddMessage(apMessage, Messages::EMessageType::MESSAGE_RELIABLE, aConnectionID);
}

void NetworkManager::SendSequenced(std::shared_ptr<Messages::Message> apMessage, int16_t aConnectionID)
{
	m_pUDPSocket->AddMessage(apMessage, Messages::EMessageType::MESSAGE_SEQUENCED, aConnectionID);
}

void NetworkManager::RegisterMessage(Messages::Message* apMessage, uint8_t aOpCode)
{
	m_PacketHandler.RegisterMessage(apMessage, aOpCode);
}

void NetworkManager::SetTickRates(uint8_t aSendTickRate, uint8_t aRecvTickRate)
{
	m_pUDPSocket->SetTickRates(aSendTickRate, aRecvTickRate);
}

bool NetworkManager::IsConnected() const
{
	return m_pUDPSocket->IsConnected();
}

std::shared_ptr<NetworkSystem> EvayrNet::NetworkManager::GetNetworkSystem()
{
	return m_pNetworkSystem;
}

std::shared_ptr<UDPSocket> NetworkManager::GetUDPSocket()
{
	return m_pUDPSocket;
}
