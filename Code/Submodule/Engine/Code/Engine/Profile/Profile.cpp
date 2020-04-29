#include "Engine/Profile/Profile.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Memory/BlockAllocator.hpp"
#include "Engine/Memory/Memory.hpp"
#include "Engine/Log/Log.hpp"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

// -----------------------------------------------------------------------
ProfilerReport* g_theProfilerReport = nullptr;

static thread_local profiler_node_t* t_ActiveNode = nullptr;
static thread_local int t_ProfilerDepth = 0;

static std::vector<profiler_node_t*> g_History;
static std::shared_mutex g_HistoryLock;
static double g_maxHistoryTime = 10.0;

static bool g_isProfilerPaused = false;

// -----------------------------------------------------------------------
static profiler_node_t* AllocNode()
{
	profiler_node_t* node = (profiler_node_t*)g_theBlockAllocatorOnlyForProfilerNodeT->alloc(sizeof(profiler_node_t));
	if(node)
	{
		node->refCount = 1;
	}

	GUARANTEE_OR_DIE(node, "Returned nullptr when requested a block.");

	return node;
}

// -----------------------------------------------------------------------
bool ProfilerSystemInit()
{
	g_theBlockAllocatorOnlyForProfilerNodeT = new BlockAllocator();

	size_t size = 1'000'000'000;
	void* buffer = UntrackedAlloc(size);
	g_theBlockAllocatorOnlyForProfilerNodeT->init(buffer, size, sizeof(profiler_node_t), alignof(profiler_node_t));

	return true;
}

// -----------------------------------------------------------------------
void ProfilerSystemDeinit()
{
	g_theBlockAllocatorOnlyForProfilerNodeT->deinit();

	delete g_theBlockAllocatorOnlyForProfilerNodeT;
	g_theBlockAllocatorOnlyForProfilerNodeT = nullptr;
}

// -----------------------------------------------------------------------
void ProfilerSetMaxHistoryTime(double seconds_)
{
	g_maxHistoryTime = seconds_;
}

// -----------------------------------------------------------------------
void ProfilerUpdate()
{
	double time = GetCurrentTimeSeconds();

	for(auto it = g_History.begin(); it != g_History.end();)
	{
		if ((time - (*it)->startTime) > g_maxHistoryTime)
		{
			ProfileReleaseTree((*it));
			it = g_History.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// -----------------------------------------------------------------------
void ProfilerPause()
{
	g_isProfilerPaused = true;
}

// -----------------------------------------------------------------------
void ProfilerResume()
{
	g_isProfilerPaused = false;
}

// -----------------------------------------------------------------------
void ProfilePush(const char* label_)
{
	profiler_node_t* newNode = AllocNode();

	newNode->parent = t_ActiveNode;
	newNode->lastChild = nullptr;
	newNode->previousSibling = nullptr;
	newNode->threadID = std::this_thread::get_id();

	newNode->m_allocCount = t_allocCount;
	newNode->m_allocBytes = t_allocBytes;
	newNode->m_freeCount = t_freeCount;
	newNode->m_freeBytes = t_freeBytes;

	// Setup now
	newNode->startTime = GetCurrentTimeSeconds();
	strcpy_s(newNode->m_label, label_);

	if (t_ActiveNode != nullptr)
	{
		t_ActiveNode->AddChild(newNode);
	}

	// This is now my active node;
	t_ActiveNode = newNode;

	++t_ProfilerDepth;
}

// -----------------------------------------------------------------------
void ProfilePop()
{
	GUARANTEE_OR_DIE(t_ProfilerDepth > 0, "ProfilerDepth is less than 0 and we tried to pop.");
	--t_ProfilerDepth;

	if (t_ActiveNode == nullptr) 
	{
		return;
	}

	t_ActiveNode->endTime = GetCurrentTimeSeconds();

	t_ActiveNode->m_allocCount = t_allocCount - t_ActiveNode->m_allocCount;
	t_ActiveNode->m_allocBytes = t_allocBytes - t_ActiveNode->m_allocBytes;
	t_ActiveNode->m_freeCount = t_freeCount - t_ActiveNode->m_freeCount;
	t_ActiveNode->m_freeBytes = t_freeBytes - t_ActiveNode->m_freeBytes;

	profiler_node_t* oldActive = t_ActiveNode;
	t_ActiveNode = t_ActiveNode->parent;
	if (t_ActiveNode == nullptr)
	{
		std::scoped_lock<std::shared_mutex> look(g_HistoryLock);
		g_History.push_back(oldActive);
	}
}

// -----------------------------------------------------------------------
void ProfileBeginFrame(const char* label_ /*= "frame"*/)
{
	GUARANTEE_OR_DIE(t_ActiveNode == nullptr, "ActiveNode is not null at the top of the frame.");

	ProfilePush(label_);
}

// -----------------------------------------------------------------------
void ProfileEndFrame()
{
	ProfilePop();

	GUARANTEE_OR_DIE(t_ActiveNode == nullptr, "Profile::ActiveNode is not null at the top of the frame.");
}

// -----------------------------------------------------------------------
static void FreeNode(profiler_node_t* node)
{
	g_theBlockAllocatorOnlyForProfilerNodeT->free(node);
}

// -----------------------------------------------------------------------
static void FreeTree(profiler_node_t* node)
{
	if(!node)
	{
		return;
	}

	if(node->lastChild)
	{
		FreeTree(node->lastChild);
	}

	if(node->previousSibling)
	{
		FreeTree(node->lastChild);
	}

	FreeNode(node);
}

// -----------------------------------------------------------------------
void ProfileReleaseTree(profiler_node_t* node_)
{
	uint newCount = ::InterlockedDecrement(&node_->refCount);
	if(newCount == 0)
	{
		FreeTree(node_);
	}
}

// -----------------------------------------------------------------------
profiler_node_t* GetTreeFromHistory(uint historyBack /*= 0*/)
{
	std::scoped_lock lock(g_HistoryLock);

	profiler_node_t* node = nullptr;

	size_t index = g_History.size() - 1;
	size_t historypull = index - historyBack;
	if(historypull > 0)
	{
		node = g_History[historypull];
	}
	else
	{
		ERROR_AND_DIE("Asking for a node in history that we do not have a frame for.");
	}

	return node;
}

// -----------------------------------------------------------------------
profiler_node_t* GetTreeFromHistoryForThread(std::thread::id threadID, uint historyBack /*= 0*/)
{
	std::scoped_lock lock(g_HistoryLock);
	
	uint threadChecks = 0;
	auto it = g_History.end();
	while(it != g_History.begin())
	{
		if((*it)->threadID == threadID)
		{
			++threadChecks;
			if(threadChecks == historyBack)
			{
				return (*it);
			}
		}

		--it;
	}

	ERROR_AND_DIE("Asked for too many frames back for a thread that did not have enough frames in history.");
}

// -----------------------------------------------------------------------
reporter_node_t::reporter_node_t(profiler_node_t* profiler_node, reporter_node_t* reporter_node_parent)
{
	strcpy_s(m_label, profiler_node->m_label);
	m_callCount = 1;
	m_parent = reporter_node_parent;
	m_totalTime = profiler_node->endTime - profiler_node->startTime;
	m_selfTime = m_totalTime;

	m_allocCount = profiler_node->m_allocCount;
	m_allocBytes = profiler_node->m_allocBytes;
	m_freeCount = profiler_node->m_freeCount;
	m_freeBytes = profiler_node->m_freeBytes;

	if(profiler_node->lastChild != nullptr)
	{
		GetChildrenFromProfilerNode(profiler_node->lastChild, this);
	}
	if (profiler_node->previousSibling != nullptr)
	{
		GetChildrenFromProfilerNode(profiler_node->previousSibling, this->m_parent);
	}

	GetSelfTime();
}

// -----------------------------------------------------------------------
void reporter_node_t::SortTotalTime()
{
	std::sort(m_children.begin(), m_children.end(), TotalTimeComp());
}

// -----------------------------------------------------------------------
void reporter_node_t::SortSelfTime()
{
	std::sort(m_children.begin(), m_children.end(), SelfTimeComp());
}

// -----------------------------------------------------------------------
void reporter_node_t::GetSelfTime()
{
	double childrenTime = 0.0;
	for(auto& child : m_children)
	{
		childrenTime += child.m_totalTime;
	}

	m_selfTime -= childrenTime;
}

// -----------------------------------------------------------------------
void reporter_node_t::GetChildrenFromProfilerNode(profiler_node_t* profiler_node, reporter_node_t* reporter_node_parent)
{
	if(profiler_node)
	{
		reporter_node_parent->m_children.push_back(reporter_node_t(profiler_node, reporter_node_parent));
	}
}

// -----------------------------------------------------------------------
void ProfilerReport::GetFrameInHistory(uint historyBack /*= 1*/)
{
	profiler_node_t* sample = GetTreeFromHistory(historyBack);

	m_root = new reporter_node_t(sample, nullptr);

	if(m_flat)
	{
		GetFlatTree(m_root);

		LogReport(m_profilernodes);
		LogFlush();
		m_profilernodes.clear();
	}
	else
	{
		if(m_totaltimesort)
		{
			m_root->SortTotalTime();
		}
		else
		{
			m_root->SortSelfTime();
		}

		LogReport(m_root);
		LogFlush();
	}

	delete m_root;
	m_root = nullptr;
}

// -----------------------------------------------------------------------
reporter_node_t* ProfilerReport::GetRoot() const
{
	return m_root;
}

// -----------------------------------------------------------------------
void ProfilerReport::GetFlatTree(reporter_node_t* reporter)
{
	profiler_node_t* pn = new profiler_node_t();

	pn->callCount = 1;
	strcpy_s(pn->m_label, reporter->m_label);
	pn->totalTime = reporter->m_totalTime;
	pn->selfTime = reporter->m_selfTime;

	if(m_profilernodes.size() == 0)
	{
		m_profilernodes.push_back(pn);
	}
	else
	{
		bool push = true;

		for (int i = 0; i < m_profilernodes.size(); i++)
		{
			if(strcmp(m_profilernodes[i]->m_label, pn->m_label) == 0)
			{
				m_profilernodes[i]->callCount++;
				m_profilernodes[i]->totalTime += pn->totalTime;
				m_profilernodes[i]->selfTime += pn->selfTime;
				push = false;
			}
		}

		if(push)
		{
			m_profilernodes.push_back(pn);
		}
	}

	if (reporter->m_children.size() > 0)
	{
		for (auto& child : reporter->m_children)
		{
			GetFlatTree(&child);
		}
	}

	std::sort(m_profilernodes.begin(), m_profilernodes.end(), FlatTotalTimeComp());
	//std::sort(m_children.begin(), m_children.end(), FlatSelfTimeComp());

	
}
