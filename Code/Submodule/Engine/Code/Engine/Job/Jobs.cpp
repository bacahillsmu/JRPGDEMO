#include "Engine/Job/Jobs.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"

JobSystem* g_theJobSystem = nullptr;
JobCategory* m_jobCategories = nullptr;
std::vector<std::thread> m_genericThreads;
uint genericThreadCount = 1;


static void GenericThreadWorkerMain()
{
	// Only process Generic Threads when the JobSystem is running;
	while(g_theJobSystem->IsRunning())
	{
		g_theJobSystem->WaitForWork();

		while(g_theJobSystem->ProcessJobCategory(JOBCATEGORY_GENERIC));
	}

	g_theJobSystem->FinishJobsForJobCategory(JOBCATEGORY_GENERIC);
}

// ----------------------------------------------------------------------------
// JobCategory;
// ----------------------------------------------------------------------------
void JobCategory::Enqueue(Job* job)
{
	m_pendingQueue.Enqueue(job);
	g_theJobSystem->SignalWork();
}

// ----------------------------------------------------------------------------
void JobCategory::TryDequeue(Job*& outJob)
{
	m_pendingQueue.Dequeue(&outJob);
}

// ----------------------------------------------------------------------------
void JobCategory::Dequeue(Job*& outJob)
{
	bool haveJob = false;
	do 
	{
		if (!g_theJobSystem->IsRunning())
		{
			break;
		}

		haveJob = m_pendingQueue.Dequeue(&outJob);

	} while (!haveJob);
}

// ----------------------------------------------------------------------------
void JobCategory::EnqueueFinished(Job* job)
{
	m_finishQueue.Enqueue(job);
	g_theJobSystem->SignalWork();
}

// ----------------------------------------------------------------------------
void JobCategory::TryDequeueFinished(Job*& outJob)
{
	m_finishQueue.Dequeue(&outJob);
}

// ----------------------------------------------------------------------------
// Job;
// ----------------------------------------------------------------------------
Job::Job()
{
	m_predecessorCount = 1;
}

// ----------------------------------------------------------------------------
void Job::AddSuccessor(Job* job)
{
	job->AddPredecessor(this);
}

// ----------------------------------------------------------------------------
void Job::AddPredecessor(Job* job)
{
	++m_predecessorCount;
	
	{
		std::scoped_lock lock(jobLock);
		job->m_successors.push_back(this);
	}
}

// ----------------------------------------------------------------------------
void Job::SetFinishCallback(finish_cb cb)
{
	m_finishCallback = cb;
}

// ----------------------------------------------------------------------------
bool Job::TryStart()
{
	int newCount = --m_predecessorCount;
	ASSERT(newCount >= 0);

	if(newCount == 0)
	{
		m_jobCategories[m_jobCategory].Enqueue(this);
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
void Job::FinishJob()
{
	// Go through our job's successors;
	for(Job* jobSuccessor : m_successors)
	{
		// If we have a successor, then try to start it;
		jobSuccessor->TryStart();
	}

	// If we have a finish call back;
	if(m_finishCallback)
	{
		// Add ourselves to the finish queue;
		m_jobCategories[m_jobCategory].EnqueueFinished(this);
	}
	else // If we don't have a finish call back, then delete ourselves;
	{
		delete this;
	}
}

// ----------------------------------------------------------------------------
// JobSystem;
// ----------------------------------------------------------------------------
void JobSystem::Startup(int numberGenericThreads /*= -1*/, int numberCategories /*= JOBCATEGORY_COUNT*/)
{
	// Don't Startup if we are already Running;
	if(m_isRunning)
	{
		return;
	}
	
	// If we ask for more Generic Threads than we have, set it to max and save 1 for main thread;
	if (numberGenericThreads >= (int)std::thread::hardware_concurrency())
	{
		numberGenericThreads = (int)std::thread::hardware_concurrency() - 1;
	}
	else if(numberGenericThreads <= -(int)std::thread::hardware_concurrency())
	{
		numberGenericThreads = -(int)std::thread::hardware_concurrency() + 1;
	}

	m_isRunning = true;
	m_jobCategories = new JobCategory[numberCategories];
	
	// If we ask for a negative amount, then use all BUT that amount for Generic Threads;
	if(numberGenericThreads < 0)
	{
		genericThreadCount = std::thread::hardware_concurrency() + numberGenericThreads;
	}
	else if(numberGenericThreads > 0) // ...unless we specifically ask for a positive amount;
	{
		genericThreadCount = numberGenericThreads;
	}

	m_signal.Create(0, genericThreadCount);

	for (uint i = 0; i < genericThreadCount; i++)
	{
		m_genericThreads.emplace_back(GenericThreadWorkerMain);
	}
}

// ----------------------------------------------------------------------------
void JobSystem::Shutdown()
{
	m_isRunning = false;
	g_theJobSystem->SignalAll();

	for (uint i = 0; i < genericThreadCount; i++)
	{
		m_genericThreads[i].join();
	}

	delete[] m_jobCategories;
	m_jobCategories = nullptr;
}

// ----------------------------------------------------------------------------
void JobSystem::Run(Job* job)
{
	job->TryStart();
}

// ----------------------------------------------------------------------------
void JobSystem::ProcessJobCategoryForMS(int jobCategory, uint ms)
{
	double start = GetCurrentTimeSeconds() * 1000.0;
	double end = 0.0;
	
	bool timeLimitPassed = false;
	do 
	{
		ProcessJobCategory(jobCategory);

		end = GetCurrentTimeSeconds() * 1000.0;
		timeLimitPassed = (end - start) > ms ? true : false;

	} while (!timeLimitPassed);
}

// ----------------------------------------------------------------------------
bool JobSystem::ProcessJobCategory(int jobCategory)
{
	Job* job = nullptr;
	m_jobCategories[jobCategory].TryDequeue(job);

	if(job)
	{
		job->Execute();
		job->FinishJob();

		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
bool JobSystem::ProcessFinishCallbacksForJobCategory(int jobCategory)
{
	Job* job = nullptr;
	m_jobCategories[jobCategory].TryDequeueFinished(job);

	if (job)
	{
		job->m_finishCallback(job);

		delete job;
		job = nullptr;

		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
void JobSystem::FinishJobsForJobCategory(int jobCategory)
{
	while(true)
	{
		Job* job = nullptr;
		m_jobCategories[jobCategory].TryDequeue(job);

		if (job)
		{
			job->Execute();
			job->FinishJob();
		}
		else
		{
			break;
		}
	}
}


