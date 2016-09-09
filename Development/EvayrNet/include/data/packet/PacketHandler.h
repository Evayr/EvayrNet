#ifndef _PACKETHANDLER_H_
#define _PACKETHANDLER_H_

#include "Packet.h"

namespace EvayrNet
{
	class PacketHandler
	{
	public:
		PacketHandler();
		~PacketHandler();

		void ProcessPacket(Packet aPacket);

	private:
		void ProcessMessage();
	};
}

#endif