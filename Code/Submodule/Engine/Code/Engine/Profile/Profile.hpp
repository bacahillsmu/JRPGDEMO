#pragma once

#include <thread>
#include <shared_mutex>

#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)
#define PROFILE_SCOPE( tag ) ProfileScope COMBINE(__scopeLog, __LINE__) ## (tag)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__);

typedef unsigned int uint;

bool ProfilerSystemInit();
void ProfilerSystemDeinit();

//PrilferGetHistory();

// Cleanup Trees that are older than X seconds;
void ProfilerSetMaxHistoryTime(double seconds_);

void ProfilerUpdate();

// Stop recording of Trees, disables creation of new Trees, current Trees can finish;
void ProfilerPause();

// Resumes recording of Trees, new creations of Trees;
void ProfilerResume();

// RECORDING
// Pushes a new child node, and marks it as the active one;
void ProfilePush(const char* label_);

// Pops the active node in the Tree, errors if there is none;
void ProfilePop();



// A context can be thought of as a frame of work;
// Optional in the system, but can add some 
//  additional error checking to make sure people are using it correctly;
void ProfileBeginFrame(const char* label_ = "Label");
void ProfileEndFrame();

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
struct profiler_node_t
{
	// Parent;
	profiler_node_t* parent = nullptr;

	// Children;
	profiler_node_t* lastChild = nullptr;
	profiler_node_t* previousSibling = nullptr;

	char m_label[64];

	// Memory;
	size_t m_allocCount = 0;
	size_t m_allocBytes = 0;
	size_t m_freeCount = 0;
	size_t m_freeBytes = 0;

	// Timing;
	double startTime = 0;
	double endTime = 0;
	double totalTime = 0;
	double selfTime = 0;

	std::thread::id threadID;
	uint refCount = 0;
	uint callCount = 0;

	void AddChild(profiler_node_t* child_)
	{
		child_->parent = this;
		child_->previousSibling = lastChild;
		lastChild = child_;
	}
};


// -----------------------------------------------------------------------
void ProfileReleaseTree(profiler_node_t* node_);
profiler_node_t* GetTreeFromHistory(uint historyBack = 0);
profiler_node_t* GetTreeFromHistoryForThread(std::thread::id threadID, uint historyBack = 0);

// -----------------------------------------------------------------------
class reporter_node_t;

class ProfilerReport
{

public:

	void AppendFlatView(profiler_node_t root);
	void AppendTreeView(profiler_node_t* root);

	void GetFrameInHistory(uint historyBack = 1);
	reporter_node_t* GetRoot() const;
	inline void SetFlat(bool set) { m_flat = set; }
	inline void SetSort(bool set) { m_totaltimesort = set; }

	void GetFlatTree(reporter_node_t* reporter);

private:

	reporter_node_t* m_root = nullptr;
	bool m_flat = false;
	bool m_totaltimesort = true;
	std::vector<profiler_node_t*> m_profilernodes;


};

class reporter_node_t
{

public:

	reporter_node_t(profiler_node_t* profiler_node, reporter_node_t* reporter_node_parent);
	~reporter_node_t() {}

	void SortTotalTime();
	void SortSelfTime();

	void GetSelfTime();
	void GetChildrenFromProfilerNode(profiler_node_t* profiler_node, reporter_node_t* reporter_node_parent);

	bool operator < (const reporter_node_t& rn) const
	{
		return (m_totalTime < rn.m_totalTime);
	}

public:

	reporter_node_t* m_parent = nullptr; // parent in tree view, root node in flat view
	std::vector<reporter_node_t> m_children;

	char m_label[64];
	uint m_callCount;

	double m_totalTime = 0; 			// total time spent at this node
	double m_selfTime = 0; 			// time spent here not accounted for by children

	// Memory Tracking;
	size_t m_allocCount = 0; 		
	size_t m_allocBytes = 0;
	size_t m_freeCount = 0;
	size_t m_freeBytes = 0;
};

struct TotalTimeComp
{
	bool operator()(const reporter_node_t& lhs, const reporter_node_t& rhs) const
	{
		return lhs.m_totalTime > rhs.m_totalTime;
	}
};

struct SelfTimeComp
{
	bool operator()(const reporter_node_t& lhs, const reporter_node_t& rhs) const
	{
		return lhs.m_selfTime > rhs.m_selfTime;
	}
};

struct FlatTotalTimeComp
{
	bool operator()(const profiler_node_t* lhs, const profiler_node_t* rhs) const
	{
		return lhs->totalTime > rhs->totalTime;
	}
};

struct FlatSelfTimeComp
{
	bool operator()(const profiler_node_t* lhs, const profiler_node_t* rhs) const
	{
		return lhs->selfTime > rhs->selfTime;
	}
};

class ProfileScope
{
public:

	ProfileScope(const char* label) { ProfilePush(label); }
	~ProfileScope() { ProfilePop(); }
};



extern ProfilerReport* g_theProfilerReport;

