#include "data/stream/DataStreamWriter.h"

using namespace EvayrNet;

DataStreamWriter::DataStreamWriter(std::string& a_DataStream)
	: m_DataStream(a_DataStream)
{

}

DataStreamWriter::~DataStreamWriter()
{

}

void DataStreamWriter::WriteRaw(const uint8_t* acpData, const uint32_t acLength)
{
	m_DataStream.append((char*)acpData, acLength);
}

void DataStreamWriter::Write(const std::string& acVar)
{
	Write((uint16_t)acVar.size());
	m_DataStream.append(acVar);
}

void DataStreamWriter::Write(const float& acVar)
{
	WriteRaw((const uint8_t*)&acVar, sizeof(float));
}

void DataStreamWriter::Write(const double& acVar)
{
	WriteRaw((const uint8_t*)&acVar, sizeof(double));
}

void EvayrNet::DataStreamWriter::Write(const int16_t& acVar)
{
	WriteRaw((const uint8_t*)&acVar, sizeof(int16_t));
}

void EvayrNet::DataStreamWriter::Write(const int8_t& acVar)
{
	WriteRaw((const uint8_t*)&acVar, sizeof(int8_t));
}

void DataStreamWriter::Write(const uint16_t& acVar)
{
	WriteRaw((const uint8_t*)&acVar, sizeof(uint16_t));
}

void DataStreamWriter::Write(const uint8_t& acVar)
{
	WriteRaw((const uint8_t*)&acVar, sizeof(uint8_t));
}
