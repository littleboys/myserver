//
//  acceptor.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "base.h"
#include "buffev_interface.h"
#include "net_inc.h"
#include "socket.h"
#include "event_reactor.h"
#include "event.h"
#include "net_addr.h"


class CAcceptor: public IBufferEvent<RECV_BUF_LEN>
{
	enum eAcceptorState
	{
		eAS_Disconnected = 0,
		eAS_Connected,
	};

public:
	//构造函数
	CAcceptor(SOCKET socket, IEventReactor *pReactor, CNetAddr *netAddr);
	//析构函数
	virtual ~CAcceptor();
	//设置读写回调
	void SetCallbackFunc(FuncAcceptorOnDisconnected pOnDisconnected,
						 FuncAcceptorOnSomeDataSend pOnSomeDataSend,
						 FuncAcceptorOnSomeDataRecv pOnSomeDataRecv);
	//获取该连接的ip
	void GetRemoteIpAddress(char *szBuf, unsigned int uBufSize);
	//发送数据
	PipeResult Send(const void *pData, unsigned int uSize);
	//获取连接socket
	CSocket GetSocket() const;
	//关闭连接
	void ShutDown();
	//当前是否连接
	bool IsConnected();

private:
	//bufferEvent 无效处理
	void BuffEventAvailableCall() override;
	//event buffer 创建成功后处理
	void AfterBuffEventCreated() override;
private:
	//读回调
	static void lcb_OnPipeRead(struct bufferevent *bev, void *arg);
	//写回调
	static void lcb_OnPipeWrite(bufferevent *bev, void *arg);
	//事件回调
	static void lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg);
	//获取连接状态
	eAcceptorState GetState();
	//设置连接状态
	void SetState(eAcceptorState eState);
	//处理socket错误
	void ProcessSocketError();

private:
	CSocket m_Socket;
	CNetAddr *m_pNetAddr;
	eAcceptorState m_eState;

	FuncAcceptorOnDisconnected m_pFuncOnDisconnected;
	FuncAcceptorOnSomeDataSend m_pFuncOnSomeDataSend;
	FuncAcceptorOnSomeDataRecv m_pFuncOnSomeDataRecv;
};

#endif