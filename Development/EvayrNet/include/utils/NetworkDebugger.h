#ifndef _NETWORKDEBUGGER_H_
#define _NETWORKDEBUGGER_H_

#include <string>
#include <fstream>

namespace EvayrNet
{
	class NetworkDebugger
	{
	public:
		NetworkDebugger();
		~NetworkDebugger();

		void Print(const std::string& acText);
		void Store(const std::string& acText, const std::string& acFileName, bool aPrintToConsole = false);

	private:
		std::ofstream m_fileStream;
	};
}

#endif
