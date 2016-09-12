#ifndef _NETWORKSYSTEM_H_
#define _NETWORKSYSTEM_H_

#include "data\messages\Messages.h"

namespace EvayrNet
{
	class NetworkSystem
	{
	public:
		NetworkSystem();
		~NetworkSystem();

		virtual void OnConnectionRequest(const Messages::ConnectionRequest& acMessage) = 0;
		virtual void OnConnectionResponse(const Messages::ConnectionResponse& acMessage) = 0;
		virtual void OnDisconnect(const Messages::Disconnect& acMessage) = 0;
		virtual void OnClientIPAddresses(const Messages::ClientIPAddresses& acMessage) = 0;

		bool IsServer() const;

	protected:
		bool m_IsServer : 1;
	};
}

#endif
