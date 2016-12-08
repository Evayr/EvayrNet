#ifndef _IDGENERATOR_H_
#define _IDGENERATOR_H_

#include <cstdint>
#include <list>

class IDGenerator
{
public:
	IDGenerator(uint32_t aMaximum);
	~IDGenerator();

	void Skip(uint32_t aCount = 1);
	void Reset();

	uint32_t GenerateID();
	void FreeID(uint32_t aID);

private:
	uint32_t GrabLowestFreedID();

	uint32_t m_max;
	uint32_t m_current;

	std::list<uint32_t> m_freedIDs;
};

#endif