#include <iostream>
#include "NetworkManager.h"

/*
	*** SERVER ***
*/

void OnConnectionResult(EvayrNet::Messages::EConnectionResult aResult)
{
	printf("OnConnectionResult callback. Result: %u\n", aResult);
}

void OnDisconnect(EvayrNet::Messages::EDisconnectReason aReason)
{
	printf("OnDisconnect callback. Reason: %u\n", aReason);
}

void OnPlayerAdd(uint16_t aConnectionID)
{
	printf("OnPlayerAdd callback. Connection ID: %u\n", aConnectionID);
}

void OnPlayerDisconnect(uint16_t aConnectionID, EvayrNet::Messages::EDisconnectReason aReason)
{
	printf("OnPlayerDisconnect callback. ConnectionID: %u | Reason: %u\n", aConnectionID, aReason);
}

int main()
{
	EvayrNet::NetworkManager net(7777, true);

	net.RegisterOnConnectionResultCallback(OnConnectionResult);
	net.RegisterOnDisconnectCallback(OnDisconnect);
	net.RegisterOnPlayerAddCallback(OnPlayerAdd);
	net.RegisterOnPlayerDisconnectCallback(OnPlayerDisconnect);

	net.SetTickRate(144);

	uint32_t dataLogInterval = 1000; // ms
	clock_t m_dataClock = clock();

	for (;;)
	{
		net.Update();

		if (uint32_t(clock() - m_dataClock) >= dataLogInterval)
		{
			m_dataClock = clock();

			if (net.IsServer() && net.GetActiveConnectionsCount() > 0U)
			{
				printf("Data log: | Active connections: %u | Packet Per Second in: %u | PPS out: %u | PPS lost: %u | Bytes per second in: %u | Bytes per second out: %u |\n"
				, net.GetActiveConnectionsCount(), net.GetIncomingPacketsPerSecond(), net.GetOutgoingPacketsPerSecond(), net.GetPacketsPerSecondLost(), net.GetIncomingDataPerSecond(), net.GetOutgoingDataPerSecond());
				//for (uint16_t i = 0; i < net.GetActiveConnectionsCount(); ++i)
				//{
				//	printf("| Connection ID %u ping: %u |\n", net.GetPing());
				//}
			}
		}
	}

	return 0;
}