#include <iostream>
#include "NetworkManager.h"

/*
	*** CLIENT ***
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
	EvayrNet::NetworkManager net(8888, false);
	
	net.RegisterOnConnectionResultCallback(OnConnectionResult);
	net.RegisterOnDisconnectCallback(OnDisconnect);
	net.RegisterOnPlayerAddCallback(OnPlayerAdd);
	net.RegisterOnPlayerDisconnectCallback(OnPlayerDisconnect);

	net.ConnectTo("127.0.0.1", 7777);
	net.SetTickRate(144);

	net.GetDebugger()->Enable();

	net.StartSimulation(150, 50, 1.f, 1.f);

	clock_t startClock = clock();
	float timeoutTime = 10000;

	uint32_t dataLogInterval = 250; // ms
	clock_t m_dataClock = clock();

	while(clock() - startClock < timeoutTime)
	{
		net.Update();

		if (clock() - m_dataClock >= dataLogInterval)
		{
			m_dataClock = clock();

			if (net.IsConnected())
			{
				if (net.GetNewestPing() != 0)
				{
					net.GetDebugger()->SaveString(std::to_string(net.GetNewestPing()), "Pings.csv", false, true);
				}
				//printf("Data log: | Latest ping: %u | Average ping: %u | Packet Per Second in: %u | PPS out: %u | PPS lost: %u | Bytes per second in: %u | Bytes per second out: %u |\n"
				//	, net.GetNewestPing(), net.GetAveragePing(), net.GetIncomingPacketsPerSecond(), net.GetOutgoingPacketsPerSecond(), net.GetPacketsPerSecondLost(), net.GetIncomingDataPerSecond(), net.GetOutgoingDataPerSecond());
			}
		}
	}

	net.Disconnect();

	system("pause");

	return 0;
}