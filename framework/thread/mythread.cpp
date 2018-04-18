//
// mythread.cpp
// 线程基类
// Created by DGuco on 17-10-12.
// Copyright © 2016年 DGuco. All rights reserved.
//

#include "base.h"
#include "mythread.h"
#include "log.h"

CMyThread::CMyThread(const std::string &threadName)
	: m_iRunStatus(rt_init),
	  m_pThread(NULL),
	  m_sThreadName(threadName)
{
}

CMyThread::~CMyThread()
{
}

int CMyThread::Run()
{
	m_iRunStatus = rt_running;
	//创建线程
	if (m_pThread != NULL) {
		m_pThread = new std::thread(mem_fn(&CMyThread::RunFunc), this);
	}
	return 0;
}

void CMyThread::ThreadFunc()
{
	while (true) {
		// 该过程需要在线程锁内完成
		std::unique_lock<std::mutex> lk(m_condMut);
		m_iRunStatus = rt_blocked;
		LOG_DEBUG("default", "Thread [{}] would blocked.", m_sThreadName);
		data_cond.wait(lk, [this]
		{
			return (m_iRunStatus == rt_stopped || !IsToBeBlocked());
		});

		// 如果线程需要停止则终止线程
		if (m_iRunStatus == rt_stopped) {
			//退出线程
			LOG_DEBUG("default", "Thread [{}] exit.", m_sThreadName);
			pthread_exit((void *) 0);
		}

		//意外唤醒线程,继续阻塞
		if (IsToBeBlocked()) {
			continue;
		}

		// 线程状态变为rt_running
		m_iRunStatus = rt_running;
		lk.unlock();
		RunFunc();
	}
}

int CMyThread::WakeUp()
{
	// 该过程需要在线程锁内完成
	std::lock_guard<std::mutex> guard(m_condMut);
	if (!IsToBeBlocked() && m_iRunStatus == rt_blocked) {
		// 向线程发出信号以唤醒
		data_cond.notify_one();
	}

	return 0;
}

int CMyThread::StopThread()
{
	std::lock_guard<std::mutex> guard(m_condMut);
	m_iRunStatus = rt_stopped;
	data_cond.notify_one();
	// 等待该线程终止
	Join();
	LOG_DEBUG("default", "Thread [{}] stopped.", m_sThreadName);
	return 0;
}

void CMyThread::StopForce()
{
	//退出线程
	LOG_DEBUG("default", "Thread [{}] exit.", m_sThreadName);
	pthread_exit((void *) 0);
}

void CMyThread::Join()
{
	if (m_pThread->joinable()) {
		m_pThread->join();
	}
}

int CMyThread::GetStatus()
{
	std::lock_guard<std::mutex> guard(m_condMut);
	return m_iRunStatus;
}