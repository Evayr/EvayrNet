#ifndef _PACKETHANDLER_H_
#define _PACKETHANDLER_H_

namespace EvayrNet
{
	class PacketHandler
	{
	public:
		PacketHandler();
		~PacketHandler();

		void ProcessPacket();

	private:
		void ProcessMessage();
	};
}

#endif