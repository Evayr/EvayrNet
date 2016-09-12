#include <iostream>
#include "NetworkManager.h"

int main()
{
	EvayrNet::NetworkManager net(8888, false);
	net.ConnectTo("127.0.0.1", 7777);

	for (;;)
	{
		net.Update();
	}

	return 0;
}