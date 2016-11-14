#ifndef _DATASTREAMWRITER_H_
#define _DATASTREAMWRITER_H_

#include <string>
#include <cstdint>

namespace EvayrNet
{
	class DataStreamWriter
	{
	public:
		DataStreamWriter(std::string& a_DataStream);
		~DataStreamWriter();

		void WriteRaw(const uint8_t* acpData, const uint32_t acLength);

		template <typename Type>
		void Write(const Type& acVar)
		{
			WriteRaw((const uint8_t*)&acVar, sizeof(Type));
		}

		void Write(const std::string& acVar);
		void Write(const float& acVar);
		void Write(const double& acVar);

	private:
		std::string& m_DataStream;
	};
}

#endif