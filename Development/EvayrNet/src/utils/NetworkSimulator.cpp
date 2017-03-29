#include "utils\NetworkSimulator.h"
#include "NetworkManager.h"

using namespace EvayrNet;

NetworkSimulator::NetworkSimulator()
	: m_simulating(false)
	, m_minimumDelay(0)
	, m_randomDelay(0)
	, m_packetLoss(0)
	, m_packetDuplication(0)
{
	srand(clock());
}

NetworkSimulator::~NetworkSimulator()
{
}

void NetworkSimulator::Update()
{
	if (!m_simulating) return;

	SendMessages();
}

void NetworkSimulator::ProcessMessage(std::shared_ptr<Messages::Message> apMessage, uint16_t aConnectionID, bool aStoreACK)
{
	// Do we drop it?
	if (uint32_t(rand() % 100000U) < m_packetLoss)
	{
		// Packet dropped
		return;
	}

	// Do we duplicate it?
	if (uint32_t(rand() % 100000U) < m_packetDuplication)
	{
		auto pMessage = std::make_shared<Messages::Message>(*apMessage);
		ProcessMessage(pMessage, aConnectionID, aStoreACK);
		return;
	}

	// Add delay
	int32_t delay = int32_t(m_minimumDelay + (rand() % m_randomDelay));

	// Add message to send list
	ModifiedMessage message;
	message.pMessage = apMessage;
	message.connectionID = aConnectionID;
	message.storeACK = aStoreACK;
	message.sendTime = int32_t(clock() + delay);
	m_messagesToSend.push_back(message);

	// Debug
	//g_Network->GetDebugger()->Print("Delaying a message called \"" + apMessage->GetMessageName() + "\" with " + std::to_string(delay) + "ms");
}

void NetworkSimulator::StartSimulation(const uint32_t acMinimumDelayMS, const uint32_t acRandomDelayMS, const float acPacketLossPct, const float acPacketDuplicationPct)
{
	g_Network->GetDebugger()->Print("Started simulation with " + std::to_string(acMinimumDelayMS) + " + ~" + std::to_string(acRandomDelayMS) + "ms, " + std::to_string(acPacketLossPct) + "%% packet loss and " + std::to_string(acPacketDuplicationPct) + "%% packet duplication.");
	m_simulating = true;

	m_minimumDelay = acMinimumDelayMS;
	m_randomDelay = acRandomDelayMS;

	m_packetLoss = uint32_t(acPacketLossPct * 1000.f);
	m_packetDuplication = uint32_t(acPacketDuplicationPct * 1000.f);
}

void NetworkSimulator::StopSimulation()
{
	g_Network->GetDebugger()->Print("Stopped the simulation.");
	m_simulating = false;

	// Send the remainder of the messages so we don't drop them all or send them later whenever a newer simulation starts.
	SendMessages(true);
}

const bool NetworkSimulator::IsSimulating() const
{
	return m_simulating;
}

void NetworkSimulator::SendMessages(bool aForceSend)
{
	clock_t now = clock();

	for (auto it = m_messagesToSend.begin(); it != m_messagesToSend.end(); ++it)
	{
		if (int32_t(it->sendTime - now) <= 0 || aForceSend)
		{
			g_Network->GetUDPSocket()->AddMessage(it->pMessage, it->connectionID, it->storeACK);
			it = m_messagesToSend.erase(it);
		}
	}
}
