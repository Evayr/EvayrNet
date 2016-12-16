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

	clock_t startClock = clock();
	float timeoutTime = 5000;

	while(clock() - startClock < timeoutTime)
	{
		net.Update();
	}

	net.Disconnect();

	system("pause");

	return 0;
}