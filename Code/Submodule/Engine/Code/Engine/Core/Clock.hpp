#pragma once

#include <vector>

class Clock
{

public:

	Clock();
	Clock(Clock* parent);
	~Clock();

	void Step(double deltaSeconds);
	void SetParent(Clock* parent);
	void SetFrameLimit(double frameLimit);

	inline void Pause() { m_paused = true; }
	inline void Resume() { m_paused = false; }
	inline void ForcePause() { m_paused = true; }
	inline void ForceResume() { m_paused = false; }
	void DilateClock( double dialateAmount );

	inline bool IsPaused() { return m_paused; }
	inline double GetDilation() { return m_dilation; }


	void ClockSystemBeginFrame();


public:

	bool m_paused = false;

	uint64_t m_frameCount = 0;
	double m_totalTime = 0.0;
	double m_frameTime = 0.0;

	double m_frameLimit = 999999999.0;
	double m_dilation = 1.0;

	unsigned int m_pauseCount = 0u;

	Clock* m_parent = nullptr;
	std::vector<Clock*> m_children;

	double m_lastFrameTime = 0.0;
	static Clock Master;
};


