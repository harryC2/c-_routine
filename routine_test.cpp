#if 0 // 协程 示例 1
#include <iostream>
#include <ucontext.h>
#include <sys/unistd.h>

int main()
{
	int i = 0;

	// 定义上下文结构体变量
	ucontext_t ctx;

	// 获取当前上下文，保存到 ctx 中
	getcontext(&ctx);

	printf("i = %d\n", i++);

	sleep(1);

	// 恢复上下文，从 ctx 中恢复
	setcontext(&ctx);

	return 0;
}
#endif

#if 0 // 协程 示例 2
#include <iostream>
#include <ucontext.h>
#include <sys/unistd.h>

void foo()
{
	printf("this is child context\n");
}

int main()
{
	// 给协程分配一个栈
	char stack[1024 * 64] = { 0 };

	// 定义上下文结构体变量
	ucontext_t child, main;

	// 获取当前上下文
	getcontext(&child);

	// 分配栈空间，uc_stack.ss_sp 指向栈顶
	child.uc_stack.ss_sp = stack;
	child.uc_stack.ss_size = sizeof(stack);
	child.uc_stack.ss_flags = 0;

	// 指定后序的上下文
	child.uc_link = &main;

	// 修改上下文，给上下文指定入口函数
	makecontext(&child, (void (*)())foo, 0);

	// 切换上下文到 child，保存当前上下文到 main
	swapcontext(&main, &child);

	printf("this is main context\n");

	return 0;
}
#endif

#if 1 // 封装的协程 示例
#include <iostream>
#include "routine/schedule.h"
//#include <routine/schedule.h>
using namespace yazi::routine;

class ARoutine : public Routine
{
 public:
	ARoutine() = delete;
	ARoutine(int num) : m_num(num) {}
	~ARoutine() = default;

	virtual void run()
	{
		for (int i = 0; i < m_num; i++)
		{
			printf("a run: num=%d\n", i);
			yield();
			printf("a run: num=%d end\n", i);
		}
	}

 private:
	int m_num;
};

class BRoutine : public Routine
{
 public:
	BRoutine() = delete;
	BRoutine(int num) : m_num(num) {}
	~BRoutine() = default;

	virtual void run()
	{
		for (int i = 0; i < m_num; i++)
		{
			printf("b run: num=%d\n", i);
			yield();
		}
	}

 private:
	int m_num;
};

int main()
{
	// 调度器创建全局共享栈
	Schedule s;
	s.create(1024 * 16);

	auto a = new ARoutine(5);
	s.append(a);

	auto b = new BRoutine(10);
	s.append(b);

	s.run();

	return 0;
}
#endif