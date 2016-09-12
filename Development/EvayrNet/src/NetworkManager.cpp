#include "NetworkManager.h"
#include "socket\windows\WindowsUDPSocket.h"
#include "systems\client\NetworkClient.h"
#include "systems\server\NetworkServer.h"

using namespace EvayrNet;

EvayrNet::NetworkManager* EvayrNet::g_Network = nullptr;

NetworkManager::NetworkManager(uint16_t aPort, bool aIsServer)
{
	g_Network = this;

	CreateSocket(aPort);

	if (aIsServer)
	{
		m_pNetworkSystem = std::make_unique<NetworkServer>();
		m_pUDPSocket->SetConnected(true);
	}
	else
	{
		m_pNetworkSystem = std::make_unique<NetworkClient>();
	}
}

NetworkManager::~NetworkManager()
{
	if (IsConnected())
	{
		Disconnect();
	}
}

void NetworkManager::CreateSocket(uint16_t aPort)
{
#if defined(_WIN64) || defined(_WIN32)
	m_pUDPSocket = std::make_unique<WindowsUDPSocket>(&m_PacketHandler, aPort, kDefaultTickRate);
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

void NetworkManager::SetTickRate(uint8_t aSendTickRate)
{
	m_pUDPSocket->SetTickRate(aSendTickRate);
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
