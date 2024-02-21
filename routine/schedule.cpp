//#include <routine/schedule.h>
#include "schedule.h"
#include <cstring>
using namespace yazi::routine;

Schedule::Schedule() : m_stack(nullptr), m_stack_size(0)
{
}

Schedule::~Schedule()
{
	if (m_stack != nullptr)
	{
		delete[] m_stack;
		m_stack = nullptr;
		m_stack_size = 0;
	}

	for (auto it : m_queue)
	{
		delete it;
	}
	m_queue.clear();
}

void Schedule::create(int stack_size)
{
	m_stack = new char[stack_size];
	std::memset(m_stack, 0, stack_size);
	m_stack_size = stack_size;
}

void Schedule::append(Routine* r)
{
	r->m_s = this;
	m_queue.push_back(r);
}

bool Schedule::empty() const
{
	return m_queue.empty();
}

int Schedule::size() const
{
	return m_queue.size();
}

void Schedule::run()
{
	while(!m_queue.empty())
	{
		auto r = m_queue.front();
		m_queue.pop_front();
		r->resume();
	}
}