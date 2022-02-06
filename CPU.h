#pragma once
#include "pch.h"
#include "StepTimer.h"


#pragma comment(lib, "pdh.lib")
#include <pdh.h>
#include <memory>


class CPU
{
public:
	CPU(std::shared_ptr<StepTimer> timer);
	~CPU();

	void Update();
	int GetCpuPercentage() { return static_cast<int>(m_cpuUsage); }

private:
	std::shared_ptr<StepTimer> m_timer;

	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	double m_lastSampleTimeInSeconds;
	long m_cpuUsage;
};