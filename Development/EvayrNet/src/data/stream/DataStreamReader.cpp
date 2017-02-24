#include "data/stream/DataStreamReader.h"

using namespace EvayrNet;

DataStreamReader::DataStreamReader(std::string& a_DataStream)
	: m_DataStream(a_DataStream)
	, m_ReadPosition(0)
{

}

DataStreamReader::~DataStreamReader()
{

}

const uint8_t* DataStreamReader::ReadRaw(const uint8_t* acpData, const uint32_t acLength)
{
	memcpy(&acpData, &m_DataStream[m_ReadPosition], acLength);
	m_ReadPosition += acLength;

	return acpData;
}

void DataStreamReader::Read(std::string& aVar)
{
	uint16_t length;
	Read(length);
	aVar.append(m_DataStream.substr(m_ReadPosition, length));
	m_ReadPosition += length;
}

void DataStreamReader::Read(float& aVar)
{
	memcpy(((uint8_t*)&aVar), &m_DataStream[m_ReadPosition], sizeof(float));
	m_ReadPosition += sizeof(float);
}

void DataStreamReader::Read(double& aVar)
{
	memcpy(((uint8_t*)&aVar), &m_DataStream[m_ReadPosition], sizeof(double));
	m_ReadPosition += sizeof(double);
}

void EvayrNet::DataStreamReader::Read(int16_t& aVar)
{
	memcpy(((uint8_t*)&aVar), &m_DataStream[m_ReadPosition], sizeof(int16_t));
	m_ReadPosition += sizeof(int16_t);
}

void EvayrNet::DataStreamReader::Read(int8_t& aVar)
{
	memcpy(((uint8_t*)&aVar), &m_DataStream[m_ReadPosition], sizeof(int8_t));
	m_ReadPosition += sizeof(int8_t);
}

void EvayrNet::DataStreamReader::Read(uint16_t& aVar)
{
	memcpy(((uint8_t*)&aVar), &m_DataStream[m_ReadPosition], sizeof(uint16_t));
	m_ReadPosition += sizeof(uint16_t);
}

void EvayrNet::DataStreamReader::Read(uint8_t& aVar)
{
	memcpy(((uint8_t*)&aVar), &m_DataStream[m_ReadPosition], sizeof(uint8_t));
	m_ReadPosition += sizeof(uint8_t);
}

void DataStreamReader::SkipBytes(uint32_t aLength)
{
	m_ReadPosition += aLength;
}