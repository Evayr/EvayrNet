#include <iostream>
#include "NetworkManager.h"

int main()
{
	EvayrNet::NetworkManager net(7777, true);

	for (;;)
	{
		net.Update();
	}

	return 0;
}