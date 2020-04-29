#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"

Clock Clock::Master = Clock(nullptr);

// -----------------------------------------------------------------------
Clock::Clock()
{
	SetParent(&Master);
}

// -----------------------------------------------------------------------
Clock::Clock( Clock* parent )
{
	SetParent(parent);
}

// -----------------------------------------------------------------------
Clock::~Clock()
{
	SetParent(nullptr);
}

// -----------------------------------------------------------------------
void Clock::Step( double deltaSeconds )
{

	deltaSeconds *= m_dilation;

	if(IsPaused())
	{
		deltaSeconds = 0.0;
	}

	deltaSeconds = Clamp(deltaSeconds, 0.0, m_frameLimit);

	m_frameCount++;
	m_totalTime += deltaSeconds;
	m_frameTime = deltaSeconds;

	for(auto* child: m_children)
	{
		child->Step(deltaSeconds);
	}
}

// -----------------------------------------------------------------------
void Clock::SetParent( Clock* parent )
{
	// If we already have a parent;
	if(m_parent)
	{
		// Go through our parent and check all their children;
		for(auto child: m_parent->m_children)
		{
			// If one of their child is us, then set that to nullptr;
			// And remove our parent;
			if(child == this)
			{
				child = nullptr;
				m_parent = nullptr;
			}
		}
	}

	// Set our parent to the new parent;
	m_parent = parent;

	// If we received a new parent, and not nullptr, then make us a child;
	if(m_parent)
	{
		m_parent->m_children.push_back(this);
	}
}

// -----------------------------------------------------------------------
void Clock::SetFrameLimit( double frameLimit )
{
	m_frameLimit = frameLimit;
}

void Clock::DilateClock( double dialateAmount )
{
	m_dilation += dialateAmount;

	m_dilation = Clamp(m_dilation, 0.0, 10.0);
}

// -----------------------------------------------------------------------
void Clock::ClockSystemBeginFrame()
{
	double currentTime = GetCurrentTimeSeconds();
	double deltaSeconds = currentTime - m_lastFrameTime;
	m_lastFrameTime = currentTime;

	Clock::Master.Step(deltaSeconds);
}

