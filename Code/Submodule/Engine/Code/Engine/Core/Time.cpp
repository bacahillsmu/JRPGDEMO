#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
ProfileTimer::ProfileTimer(const char* label, double printThreshold /* = 0.0*/)
{
	m_profileTimerName = label;
	m_printThreshold = printThreshold;

	Start();
}

//-----------------------------------------------------------------------------------------------
ProfileTimer::~ProfileTimer()
{
	if(m_timerRunning)
	{
		Stop();
	}
}

//-----------------------------------------------------------------------------------------------
void ProfileTimer::ResetTimers()
{
	m_startTime = 0.0;
	m_endTime = 0.0;
	m_elapsedTime = 0.0;
}

//-----------------------------------------------------------------------------------------------
void ProfileTimer::Start()
{
	ResetTimers();
	m_startTime = GetCurrentTimeSeconds();

	m_timerRunning = true;
}

//-----------------------------------------------------------------------------------------------
void ProfileTimer::Stop()
{
	m_elapsedTime = GetCurrentTimeSeconds() - m_startTime;
	m_elapsedTime *= 1000.0;

	if (m_elapsedTime > m_printThreshold)
	{
		DebuggerPrintf("Label %s took: %f ms.\n", m_profileTimerName, m_elapsedTime);
	}

	m_timerRunning = false;
}

//-----------------------------------------------------------------------------------------------
double ProfileTimer::GetElapsedTime()
{
	if(m_timerRunning)
	{
		Stop();
	}

	return m_elapsedTime;
}

//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );

	return ( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}

//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}



