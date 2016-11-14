#ifndef _DATASTREAM_H_
#define _DATASTREAM_H_

#include <string>
#include <cstdint>

#include "DataStreamWriter.h"
#include "DataStreamReader.h"

namespace EvayrNet
{
	class DataStream
	{
	public:
		DataStream();
		virtual ~DataStream();

		DataStreamWriter CreateWriter();
		DataStreamReader CreateReader();

		const char* GetDataStream() const;
		uint16_t GetDataStreamSize() const;

		void Clear();

	private:
		std::string m_DataStream;
	};
}

#endif