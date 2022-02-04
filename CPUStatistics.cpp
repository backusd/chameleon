#include "CPUStatistics.h"


CPUStatistics::CPUStatistics() : 
	m_canReadCpu(true),
	m_cpuUsage(0),
	m_lastSampleTimeInSeconds(0.0)
{
	PDH_STATUS status;

	// Create a query object to poll cpu usage.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	// Set query object to poll all cpus in the system.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}
}

CPUStatistics::~CPUStatistics()
{
	this->Shutdown();
}


void CPUStatistics::Shutdown()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}

	return;
}


void CPUStatistics::Update(StepTimer const& timer)
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu)
	{
		// Only update the cpu usage once every second
		double totalSeconds = timer.GetTotalSeconds();
		if (totalSeconds - m_lastSampleTimeInSeconds > 1.0)
		{
			m_lastSampleTimeInSeconds = totalSeconds;

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}

	return;
}