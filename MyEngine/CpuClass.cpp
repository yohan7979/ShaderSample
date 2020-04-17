#include "CpuClass.h"



CpuClass::CpuClass()
{
}

CpuClass::CpuClass(const CpuClass & other)
{
}


CpuClass::~CpuClass()
{
}

void CpuClass::Initialize()
{
	PDH_STATUS status;

	m_canReadCpu = true;

	// Query Object�� �����Ѵ�.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	// Query Object�� �����Ѵ�.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	m_lastSampleTime = false;
	m_cpuUsage = 0;
}

void CpuClass::Shutdown()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}
}

void CpuClass::Frame()
{
	// �������� ���� ȣ���ϳ�, 1�ʿ� �ѹ��� �����Ѵ�.
	PDH_FMT_COUNTERVALUE value;
	
	if (m_canReadCpu)
	{
		if (GetTickCount() > (m_lastSampleTime + 1000))
		{
			m_lastSampleTime = GetTickCount();

			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);
			m_cpuUsage = value.longValue;
		}
	}
}

int CpuClass::GetCpuPercentage()
{
	if (m_canReadCpu)
		return (int)m_cpuUsage;
	else
		return 0;
}
