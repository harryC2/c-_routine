#pragma once

#include <list>
//#include <routine/routine.h>
#include "routine.h"
namespace yazi
{
	namespace routine
	{

		class Schedule
		{
			friend class Routine;

		 public:
			Schedule();
			~Schedule();

			void create(int stack_size = 1024 * 16);
			void append(Routine* r);
			bool empty() const;
			int size() const;

			void run();

		 private:
			ucontext_t m_main;    			// 主协程
			char* m_stack;            		// 运行时的栈，所有协程共享
			int m_stack_size;
			std::list<Routine*> m_queue;    // 需要调度的全部协程

		};

	} // routine
} // yazi
