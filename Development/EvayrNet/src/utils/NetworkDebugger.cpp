#include "utils\NetworkDebugger.h"

using namespace EvayrNet;

NetworkDebugger::NetworkDebugger()
{
}

NetworkDebugger::~NetworkDebugger()
{
}

void NetworkDebugger::Print(const std::string& acText)
{
#ifdef _DEBUG
	printf("%s\n", acText);
#endif
}

void NetworkDebugger::Store(const std::string& acText, const std::string& acFileName, bool aPrintToConsole)
{
#ifdef _DEBUG
	m_fileStream.open("./Data/" + acFileName, std::ios::ate | std::ios::app | std::ios::in | std::ios::out);
	m_fileStream << acText << std::endl;
	m_fileStream.close();

	if (aPrintToConsole)
	{
		Print(acText);
	}
#endif
}
