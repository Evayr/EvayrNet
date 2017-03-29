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

		void Enable();
		void Disable();

		void Print(const std::string& acText);
		void SaveString(const std::string& acText, const std::string& acFileName, bool aPrintToConsole = false, bool aAddNewline = true);

	private:
		bool m_enabled;
	};
}

#endif
