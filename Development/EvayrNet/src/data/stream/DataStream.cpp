#include "data/stream/DataStream.h"

using namespace EvayrNet;

DataStream::DataStream()
{

}

DataStream::~DataStream()
{

}

DataStreamWriter DataStream::CreateWriter()
{
	return DataStreamWriter(m_DataStream);
}

DataStreamReader DataStream::CreateReader()
{
	return DataStreamReader(m_DataStream);
}

const char* DataStream::GetDataStream() const
{
	return m_DataStream.c_str();
}

uint16_t DataStream::GetDataStreamSize() const
{
	return (uint16_t)m_DataStream.size();
}

void DataStream::Clear()
{
	m_DataStream.clear();
}