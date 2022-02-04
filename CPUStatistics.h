#pragma once
#include "pch.h"
#include "StepTimer.h"


#pragma comment(lib, "pdh.lib")
#include <pdh.h>
#include <memory>

class CPUStatistics
{
public:
	CPUStatistics();
	~CPUStatistics();

	void Shutdown();
	void Update(std::shared_ptr<StepTimer> timer);
	int GetCpuPercentage() { return static_cast<int>(m_cpuUsage); }

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	double m_lastSampleTimeInSeconds;
	long m_cpuUsage;
};