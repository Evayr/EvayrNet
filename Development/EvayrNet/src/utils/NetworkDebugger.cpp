#include "utils\NetworkDebugger.h"

using namespace EvayrNet;

NetworkDebugger::NetworkDebugger()
	: m_enabled(false)
{
}

NetworkDebugger::~NetworkDebugger()
{
}

void EvayrNet::NetworkDebugger::Enable()
{
	m_enabled = true;
}

void EvayrNet::NetworkDebugger::Disable()
{
	m_enabled = false;
}

void NetworkDebugger::Print(const std::string& acText)
{
	if (!m_enabled) return;

	printf("%s\n", acText.c_str());
}

void NetworkDebugger::SaveString(const std::string& acText, const std::string& acFileName, bool aPrintToConsole, bool aAddNewline)
{
	if (!m_enabled) return;

	m_fileStream.open("./Data/" + acFileName, std::ios::ate | std::ios::app | std::ios::in | std::ios::out);
	m_fileStream << acText;
	if (aAddNewline)
	{
		m_fileStream << std::endl;
	}
	m_fileStream.close();

	if (aPrintToConsole)
	{
		Print(acText);
	}
}
