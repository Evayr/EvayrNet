#include <iostream>
#include "NetworkManager.h"

/*
	*** SERVER ***
*/

int main()
{
	EvayrNet::NetworkManager net(7777, true);
	net.SetTickRate(144);

	for (;;)
	{
		net.Update();
	}

	return 0;
}