#include "utils\NetworkDebugger.h"
#include "NetworkManager.h"

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

	std::fstream file;
	file.open(acFileName, std::ios::ate | std::ios::app | std::ios::in | std::ios::out);

	if (!file.is_open())
	{
		g_Network->GetDebugger()->Print("Failed to open file \"" + acFileName + "\"!");
	}
	else
	{
		file << acText;
		if (aAddNewline)
		{
			file << std::endl;
		}
		file.close();
	}

	if (aPrintToConsole)
	{
		Print(acText);
	}
}
