#ifndef _WINDOWSUDPSOCKET_H_
#define _WINDOWSUDPSOCKET_H_

#if defined(_WIN64) || defined(_WIN32)
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>

#include "socket\UDPSocket.h"

namespace EvayrNet
{
	class WindowsUDPSocket : public UDPSocket
	{
	public:
		WindowsUDPSocket(const PacketHandler& acPacketHandler, uint16_t aPort, uint8_t aTickRateSend, uint8_t aTickRateRecv);
		~WindowsUDPSocket();

	private:
		void Bind(uint16_t aPort);
		void Send();
		void Receive();

		SOCKET m_Socket;
	};
}
#else
namespace EvayrNet
{
	class WindowsUDPSocket;
}
#endif

#endif
