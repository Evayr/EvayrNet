#ifndef _DATASTREAMREADER_H_
#define _DATASTREAMREADER_H_

#include <string>
#include <cstdint>

namespace EvayrNet
{
	class DataStreamReader
	{
	public:
		DataStreamReader(std::string& a_DataStream);
		~DataStreamReader();

		const uint8_t* ReadRaw(const uint8_t* acpData, const uint32_t acLength);

		template <typename Type>
		void Read(Type& acVar)
		{
			acVar = (Type)ReadRaw((uint8_t*)acVar, sizeof(Type));
		}

		void Read(std::string& aVar);
		void Read(float& aVar);
		void Read(double& aVar);

		void SkipBytes(uint32_t aLength);

	private:
		std::string& m_DataStream;
		uint32_t m_ReadPosition;
	};

}

#endif