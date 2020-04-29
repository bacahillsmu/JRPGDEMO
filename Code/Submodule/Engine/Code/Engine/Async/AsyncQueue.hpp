#pragma once

#include <queue>
#include <mutex>

// ------------------------------------------------------------------------------------------------
template <typename T>
class AsyncQueue
{
	
public:

	void Enqueue(T const& v);
	bool Dequeue(T* out);

	inline bool IsEmpty() { return m_queue.size() == 0; }

private:

	std::queue<T> m_queue;
	std::mutex m_mutex;
};


// ------------------------------------------------------------------------------------------------
template <typename T>
void AsyncQueue<T>::Enqueue(T const& v)
{
	std::scoped_lock<std::mutex> lock(m_mutex);
	m_queue.push(v);
}

template <typename T>
bool AsyncQueue<T>::Dequeue(T* out)
{
	std::scoped_lock<std::mutex> lock(m_mutex);
	if(IsEmpty())
	{
		return false;
	}
	else
	{
		*out = m_queue.front();
		m_queue.pop();
		return true;
	}
}