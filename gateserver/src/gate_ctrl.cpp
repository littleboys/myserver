//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#include <my_macro.h>
#include <sharemem.h>
#include "../inc/gate_ctrl.h"

template<> CGateCtrl *CSingleton<CGateCtrl>::spSingleton = NULL;

CGateCtrl::CGateCtrl()
	: m_pC2sHandle(new CC2sHandle(eNetModule::NET_SYSTEM)),
	  m_pS2cHandle(new CS2cHandle),
	  m_pSingThead(new CThreadPool(1))
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);
	system("touch ./cspipefile");
	char *pcTmpCSPipeID = getenv("CS_PIPE_ID");
	int iTmpCSPipeID = 0;
	if (pcTmpCSPipeID) {
		iTmpCSPipeID = atoi(pcTmpCSPipeID);
	}
	key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
	BYTE *pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
	MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize, EIMode::SHM_INIT);
	CC2sHandle::m_pC2SPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S);

	system("touch ./scpipefile");
	char *pcTmpSCPipeID = getenv("SC_PIPE_ID");
	int iTmpSCPipeID = 0;
	if (pcTmpSCPipeID) {
		iTmpSCPipeID = atoi(pcTmpSCPipeID);
	}
	key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
	BYTE *pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
	MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize, EIMode::SHM_INIT);
	CS2cHandle::m_pS2CPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_S2C);
}

CGateCtrl::~CGateCtrl()
{
	SAFE_DELETE(m_pC2sHandle);
	SAFE_DELETE(m_pS2cHandle);
	SAFE_DELETE(m_pSingThead);
}

int CGateCtrl::Run()
{
	m_pS2cHandle->CreateThread();
	m_pC2sHandle->Run();
}

CThreadPool *CGateCtrl::GetSingThreadPool()
{
	return m_pSingThead;
}

CC2sHandle *CGateCtrl::GetCC2sHandle()
{
	return m_pC2sHandle;
}

CS2cHandle *CGateCtrl::GetCS2cHandle()
{
	return m_pS2cHandle;
}

int CGateCtrl::PrepareToRun()
{
	//初始化日志
	INIT_ROLLINGFILE_LOG("default", "../log/gatesvrd.log", LEVEL_DEBUG);
	CServerConfig *pTmpConfig = new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		exit(0);
	}
	m_pC2sHandle->PrepareToRun();
	return 0;
}
