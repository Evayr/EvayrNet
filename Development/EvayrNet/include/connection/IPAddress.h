#ifndef _IPADDRESS_H_
#define _IPADDRESS_H_

#include <cstdint>
#include <string>

namespace EvayrNet
{
	struct IPAddress
	{
		std::string m_Address;
		uint16_t m_Port;
	};
}

#endif
