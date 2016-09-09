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

		bool operator==(IPAddress a) const
		{
			if (a.m_Address == m_Address && a.m_Port == m_Port) return true;
			else return false;
		}
	};
}

#endif
