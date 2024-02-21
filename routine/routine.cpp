#include <cstring>
#include <iostream>
//#include <routine/routine.h>
#include "routine.h"
//#include <routine/schedule.h>
#include "schedule.h"
using namespace yazi::routine;

Routine::Routine() : m_status(RT_READY), m_stack(nullptr), m_stack_size(0)
{
}

Routine::~Routine()
{
	if (m_stack != nullptr)
	{
		delete[] m_stack;
		m_stack = nullptr;
		m_stack_size = 0;
	}
}

void Routine::resume()
{
	switch (m_status)
	{
	case RT_READY:
	{
		getcontext(&m_ctx);
		m_ctx.uc_stack.ss_sp = m_s->m_stack;
		m_ctx.uc_stack.ss_size = m_s->m_stack_size;
		m_ctx.uc_stack.ss_flags = 0;
		m_ctx.uc_link = &(m_s->m_main);
		m_status = RT_RUNNING;
		// 设置m_ctx,使当前上下文m_ctx 下次被调用时会从func开始执行；
		makecontext(&m_ctx, (void (*)())Routine::func, 1, this);
		// 保存当前上下文到ms-m_main,设置当前程序上下文为m_ctx;
		swapcontext(&(m_s->m_main), &m_ctx);
		break;
	}
	case RT_SUSPEND:
	{
		// 将当前协程的内容拷贝到全局栈
		std::memcpy(m_s->m_stack + m_s->m_stack_size - m_stack_size, m_stack, m_stack_size);
		m_status = RT_RUNNING;
		swapcontext(&(m_s->m_main), &m_ctx);//这个已经把当前堆栈的函数指针 插入到主堆栈了，会直接运行当前协程的堆栈
		break;
	}
	default:
		break;
	}
}

void Routine::yield()
{
	// 栈是由高地址向低地址增长
	char dummy = 0; // 用来判断当前运行的协程信息能否被保存得下
	char* number =  m_s->m_stack + m_s->m_stack_size;
	int vlaue = m_s->m_stack + m_s->m_stack_size - &dummy;
	if (m_stack_size < m_s->m_stack + m_s->m_stack_size - &dummy)
	{
		if (m_stack != nullptr)
		{
			delete[] m_stack;
			m_stack = nullptr;
		}
		//堆区地址是重低地址到高地址存储
		m_stack_size = m_s->m_stack + m_s->m_stack_size - &dummy;
		m_stack = new char[m_stack_size];
	}
	// 将全局协程栈中的内容保存到 当前维护的 m_stack 中，便于后续恢复
	std::memcpy(m_stack, &dummy, m_stack_size);
	m_status = RT_SUSPEND;
	m_s->append(this);
	// 将之前主程序的上下文作为程序执行上下文，然后将当前上下文保存在m_ctx中；
	swapcontext(&m_ctx, &(m_s->m_main));
}

int Routine::status() const
{
	return m_status;
}

void Routine::func(void* ptr)
{
	auto r = (Routine*)ptr;
	r->run();
	delete r;
}
