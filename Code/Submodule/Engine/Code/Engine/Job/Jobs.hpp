#pragma once
#include "Engine/Async/AsyncQueue.hpp"
#include "Engine/Semaphore/Semaphore.hpp"

#include <vector>
#include <atomic>

typedef unsigned int uint;

class Job;

enum eJobCategory : int
{
	JOBCATEGORY_GENERIC = 0,
	JOBCATEGORY_MAIN,
	JOBCATEGORY_RENDER,

	JOBCATEGORY_COUNT
};

// ----------------------------------------------------------------------------
// JobCategory;
// ----------------------------------------------------------------------------
class JobCategory
{

public:

	void Enqueue(Job* job);
	void TryDequeue(Job*& outJob);		// Return nullptr if no Job is ready;
	void Dequeue(Job*& outJob);			// Returns nullptr iif system is shutdown, otherwise block until Job is ready;

	void EnqueueFinished(Job* job);
	void TryDequeueFinished(Job*& outJob);

public:

	AsyncQueue<Job*> m_pendingQueue;
	AsyncQueue<Job*> m_finishQueue;
};

// ----------------------------------------------------------------------------
// Job;
// ----------------------------------------------------------------------------
class Job
{

public:

	Job();
	virtual ~Job() {}

	using finish_cb = std::function<void(Job*)>;

	virtual void Execute() = 0;

	void AddSuccessor(Job* job);
	void AddPredecessor(Job* job);

	void SetFinishCallback(finish_cb cb);

public:

	bool TryStart();
	void FinishJob();

	int m_jobCategory = JOBCATEGORY_GENERIC;

	std::vector<Job*> m_successors;
	std::atomic<int> m_predecessorCount = 0;
	std::mutex jobLock;

	finish_cb m_finishCallback;
};

// ----------------------------------------------------------------------------
// JobSystem;
// ----------------------------------------------------------------------------
class JobSystem
{

public:

	void Startup(int numberGenericThreads = -1, int numberCategories = JOBCATEGORY_COUNT);
	void Shutdown();

	void Run(Job* job);

	inline bool IsRunning() { return m_isRunning; }

	// Return number of Jobs processed;
	void ProcessJobCategoryForMS(int jobCategory, uint ms); // Process until no more Jobs, or MS has passed;
	bool ProcessJobCategory(int jobCategory);			   // Process category until no more Jobs;
	bool ProcessFinishCallbacksForJobCategory(int jobCategory);		   // Process category until no more Jobs;
	void FinishJobsForJobCategory(int jobCategory);		   // Process category until no more Jobs;

	Semaphore m_signal;
	inline void WaitForWork()	{ m_signal.Acquire(); }
	inline void SignalWork()	{ m_signal.Release(1); }
	inline void SignalAll()		{ m_signal.ReleaseAll(); }

private:	

	bool m_isRunning = false;
	
};

extern JobSystem* g_theJobSystem;
