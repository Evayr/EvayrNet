#include <iostream>
#include "NetworkManager.h"

/*
	*** CLIENT ***
*/

int main()
{
	EvayrNet::NetworkManager net(8888, false);
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