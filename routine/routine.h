#pragma once
#include <ucontext.h>

namespace yazi
{
	namespace routine
	{
		class Schedule;

		class Routine
		{
			friend class Schedule;

		 public:
			enum Status
			{
				RT_READY = 0,		// 就绪
				RT_RUNNING,			// 运行中
				RT_SUSPEND			// 挂起
			};

			Routine();
			virtual ~Routine();

			virtual void run() = 0;

			void resume();	// 恢复
			void yield();	// 挂起
			int status() const;

		 protected:
			static void func(void* ptr);

		 private:
			ucontext_t m_ctx;	// 保持当前协程上下文信息
			int m_status;
			char* m_stack;		// 当前上下文所需要的栈
			int m_stack_size;
			Schedule* m_s;
		};

	} // routine
} // yazi

