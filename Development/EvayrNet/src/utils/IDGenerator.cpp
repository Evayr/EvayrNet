#include "utils\IDGenerator.h"

IDGenerator::IDGenerator(uint32_t aMaximum)
	: m_max(aMaximum)
	, m_current(0)
{
}

IDGenerator::~IDGenerator()
{
}

void IDGenerator::Skip(uint32_t aCount)
{
	m_current += aCount;
}

void IDGenerator::Reset()
{
	m_current = 0;
	m_freedIDs.clear();
}

uint32_t IDGenerator::GenerateID()
{
	// Try to grab a freed ID, as that is guaranteed to be lower
	if (m_freedIDs.size() != 0)
	{
		return GrabLowestFreedID();
	}
	else
	{
		return (m_current++);
	}
}

void IDGenerator::FreeID(uint32_t aID)
{
	// Don't add in duplicates
	auto it = std::find(m_freedIDs.begin(), m_freedIDs.end(), aID);
	if (it != m_freedIDs.end()) return;

	m_freedIDs.push_back(aID);
}

uint32_t IDGenerator::GrabLowestFreedID()
{
	uint32_t ret = -1; // Set to max value
	for (auto it = m_freedIDs.begin(); it != m_freedIDs.end(); ++it)
	{
		if (*it < ret)
		{
			ret = *it;
		}
	}

	m_freedIDs.remove(ret);
	return ret;
}
