#ifndef _NETWORKSIMULATOR_H_
#define _NETWORKSIMULATOR_H_

#include <cstdint>
#include <memory>
#include <list>

#include "data/messages/messages.h"

namespace EvayrNet
{
	class NetworkSimulator
	{
	public:
		NetworkSimulator();
		~NetworkSimulator();

		void Update();

		void ProcessMessage(std::shared_ptr<Messages::Message> apMessage, uint16_t aConnectionID, bool aStoreACK);

		void StartSimulation(const uint32_t acMinimumDelayMS, const uint32_t acRandomDelayMS, const float acPacketLossPct, const float acPacketDuplicationPct);
		void StopSimulation();

		const bool IsSimulating() const;

	private:
		void SendMessages(bool aForceSend = false);

		bool m_simulating;

		uint32_t m_minimumDelay;
		uint32_t m_randomDelay;

		uint32_t m_packetLoss; // Percentage * 1000 (is easier with rand())
		uint32_t m_packetDuplication; // Percentage * 1000 (is easier with rand())

		struct ModifiedMessage
		{
			std::shared_ptr<Messages::Message> pMessage;
			uint16_t connectionID;
			bool storeACK;

			int32_t sendTime; // clock() + delay
		};
		std::list<ModifiedMessage> m_messagesToSend;
	};
}

#endif
