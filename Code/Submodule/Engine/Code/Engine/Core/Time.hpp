#pragma once

class ProfileTimer
{

public:

	ProfileTimer(const char* label, double printThreshold = 0.0);
	~ProfileTimer();

	void ResetTimers();
	void Start();
	void Stop();

	double GetElapsedTime();

public:

	const char* m_profileTimerName = "";
	double m_printThreshold = 0.0f;

	double m_startTime = 0.0;
	double m_endTime = 0.0;
	double m_elapsedTime = 0.0;

	bool m_timerRunning = false;
};

// Static Function;
double GetCurrentTimeSeconds();