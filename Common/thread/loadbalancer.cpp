
#include "stdafx.h"
#include "loadbalancer.h"

using namespace common;


cLoadBalancer::cLoadBalancer()
	: m_isThreadLoop(false)
{
}

cLoadBalancer::~cLoadBalancer()
{
	Clear();
}


// if threadCount -1, threadCount = processor count
bool cLoadBalancer::Init(const int threadCount //=-1
)
{
	Clear();

	int generateThreadCount = threadCount;
	if (threadCount < 0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		generateThreadCount = (int)si.dwNumberOfProcessors;
	}
	
	m_isThreadLoop = true;
	m_ctxs.reserve(generateThreadCount);
	for (int i = 0; i < generateThreadCount; ++i)
	{
		sThreadContext *ctx = new sThreadContext;
		ctx->name.Format("thread-%d", i);
		ctx->count = 0;
		ctx->thr = new std::thread(cLoadBalancer::ThreadFunction, this, ctx);
		m_ctxs.push_back(ctx);
	}
	return true;
}


bool cLoadBalancer::PushTask(cTask *task)
{
	RETV(!task, false);

	if (m_ctxs.empty())
	{
		assert(0);
		SAFE_DELETE(task);
		return false;
	}

	// load balance: find context has minimum task count
	sThreadContext *minCtx = nullptr;
	uint min = UINT_MAX;
	for (auto &ctx : m_ctxs)
		if (ctx->count < min)
		{
			min = ctx->count;
			minCtx = ctx;
		}
	RETV(!minCtx, false); // error occurred!

	{
		AutoCSLock cs(minCtx->cs);
		sMsg msg;
		msg.type = sMsg::StartTask;
		msg.task = task;
		minCtx->msgs.push(msg);
		++minCtx->count; // increase count to load balance
	}
	return true;
}


bool cLoadBalancer::RemoveTask(const StrId &taskName)
{
	RETV(m_ctxs.empty(), false);

	sMsg msg;
	msg.type = sMsg::RemoveTask;
	msg.name = taskName;
	msg.id = -1;
	for (auto &ctx : m_ctxs)
	{
		AutoCSLock cs(ctx->cs);
		ctx->msgs.push(msg);
	}
	return true;
}


bool cLoadBalancer::RemoveTask(const int taskId)
{
	RETV(m_ctxs.empty(), false);

	sMsg msg;
	msg.type = sMsg::RemoveTask;
	msg.id = taskId;
	for (auto &ctx : m_ctxs)
	{
		AutoCSLock cs(ctx->cs);
		ctx->msgs.push(msg);
	}
	return true;
}


// is initialize?
bool cLoadBalancer::IsInit()
{
	return !m_ctxs.empty();
}


// thread terminate, task not work
void cLoadBalancer::Terminate()
{
	m_isThreadLoop = false;
	for (auto &ctx : m_ctxs)
		if (ctx->thr->joinable())
			ctx->thr->join();

	for (auto &ctx : m_ctxs)
	{
		for (auto &task : ctx->tasks)
			delete task;
		delete ctx->thr;
		delete ctx;
	}
	m_ctxs.clear();
}


void cLoadBalancer::Clear()
{
	// not work all task
	Terminate();
}


// loadbalancer thread function
int cLoadBalancer::ThreadFunction(cLoadBalancer *balancer, sThreadContext *ctx)
{
	cTimer timer;
	timer.Create();
	double updateCountTime = 0.f;

	while (balancer->m_isThreadLoop)
	{
		// message process
		if (!ctx->msgs.empty())
		{
			AutoCSLock cs(ctx->cs);
			while (!ctx->msgs.empty())
			{
				const sMsg &msg = ctx->msgs.front();
				switch (msg.type)
				{
				case sMsg::StartTask:
					ctx->tasks.push_back(msg.task);
					break;

				case sMsg::RemoveTask:
				{
					if (msg.name.empty())
					{
						auto it = std::find_if(ctx->tasks.begin(), ctx->tasks.end()
							, [&](auto &a) { return msg.id == a->m_id; });
						if (ctx->tasks.end() != it)
						{
							delete *it;
							ctx->tasks.erase(it);
						}
					}
					else
					{
						auto it = std::find_if(ctx->tasks.begin(), ctx->tasks.end()
							, [&](auto &a) { return msg.name == a->m_name; });
						if (ctx->tasks.end() != it)
						{
							delete *it;
							ctx->tasks.erase(it);
						}
					}
				}
				break;
				default: assert(0); break;
				}
				ctx->msgs.pop();
			}
		}
		//~message process

		const double dt = timer.GetDeltaSeconds();
		for (auto it = ctx->tasks.begin(); it != ctx->tasks.end();)
		{
			cTask *task = *it;
			const cTask::eRunResult res = task->Run(dt);
			if (cTask::eRunResult::End == res)
			{
				it = ctx->tasks.erase(it);
				delete task;
			}
			else
			{
				++it;
			}
		}

		updateCountTime += dt;
		if (updateCountTime > 5.0) // update every 5 seconds
		{
			updateCountTime = 0.0;
			ctx->count = ctx->tasks.size();
		}

		// thread switching
		std::this_thread::sleep_for(
			std::chrono::milliseconds(ctx->tasks.empty() ? 100 : 1));
	}
	return 1;
}
