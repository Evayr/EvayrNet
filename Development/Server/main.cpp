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

	for (;;)
	{
		net.Update();
	}

	return 0;
}