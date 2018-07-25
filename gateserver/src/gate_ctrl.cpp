//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#include <share_mem.h>
#include "my_assert.h"
#include "../inc/gate_ctrl.h"

template<> std::shared_ptr<CGateCtrl> CSingleton<CGateCtrl>::spSingleton = NULL;

CGateCtrl::CGateCtrl()
	: m_pNetWork(std::make_shared<CNetWork>()),
	  m_pClientManager(std::make_shared<CClientManager>(m_pNetWork)),
	  m_pServerManager(std::make_shared<CServerManager>(m_pNetWork)),
	  m_pSingleThead(std::make_shared<CThreadPool>(1))
{
}

CGateCtrl::~CGateCtrl()
{
}

int CGateCtrl::PrepareToRun()
{
#ifdef _DEBUG_
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::trace);
#else
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::info);
#endif
	//读取配置文件
	ReadConfig();
	m_pServerManager->PrepareToRun();
	m_pClientManager->PrepareToRun();
	return 0;
}

int CGateCtrl::Run()
{
	LOG_INFO("default", "Libevent run with net module {}",
			 event_base_get_method(reinterpret_cast<const event_base *>(m_pNetWork->GetEventReactor()
				 ->GetEventBase())));
	//libevent事件循环
	m_pNetWork->DispatchEvents();
}

shared_ptr<CThreadPool> &CGateCtrl::GetSingleThreadPool()
{
	return m_pSingleThead;
}

shared_ptr<CClientManager> &CGateCtrl::GetClientManager()
{
	return m_pClientManager;
}

shared_ptr<CServerManager> &CGateCtrl::GetServerManager()
{
	return m_pServerManager;
}

shared_ptr<CNetWork> &CGateCtrl::GetNetWork()
{
	return m_pNetWork;
}

void CGateCtrl::ReadConfig()
{
	std::shared_ptr<CServerConfig> &tmpConfig = CServerConfig::CreateInstance();
	string filePath = "../config/serverinfo.json";
	if (-1 == tmpConfig->LoadFromFile(filePath)) {
		LOG_ERROR("default", "Get ServerConfig failed");
		exit(0);
	}
}

