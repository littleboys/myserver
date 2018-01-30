//
//  tcpctrl.h
//  客户端tcp管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef __GATE_CTRL_H__
#define __GATE_CTRL_H__

#include <sys/epoll.h>
#include "gate_def.h"
#include "log.h"
#include "servertool.h"
#include "message.pb.h"
#include "base.h"
#include "config.h"
#include "net_work.h"
#include "c2s_handle.h"
#include "s2c_handle.h"

class CGateCtrl
{
public:
	//构造函数
	CGateCtrl();
	//析构函数
	~CGateCtrl();
	//准备run
	int PrepareToRun();
	//run
	int Run();
private:
	CC2sHandle *m_pC2sHandle;
	CS2cHandle *m_pS2cHandle;
};

#endif