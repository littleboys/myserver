//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by 杜国超 on 16/12/6.
//  Copyright © 2016年 杜国超. All rights reserved.
//

#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
#include <netinet/tcp.h>
#include "../inc/tcpctrl.h"
#include "../../common/tools/inc/client_comm_engine.h"

/**
  * 函数名          : CTcpCtrl::CTcpCtrl
  * 功能描述        : CTCPCtrl构造函数
  * 返回值          ：无 
**/
CTcpCtrl::CTcpCtrl()
{
}

/**
  * 函数名          : CTcpCtrl::~CTcpCtrl
  * 功能描述        : CTCPCtrl析造函数
  * 返回值          ：无 
**/
CTcpCtrl::~CTcpCtrl()
{
}

/**
  * 函数名          : CTCPCtrl::Initialize
  * 功能描述        : 初始化CTCPCtrl对象的各成员变量
  * 返回值         ： int(成功：0 失败：错误码)
**/
int CTcpCtrl::Initialize()
{
    int iTmpI;
    int iTmpRet;
    BYTE* pbTmp;
    int iTmpIndex;

    mLastKeepaliveTime = 0;
    miRunFlag = 0;
    miWriteStatCount = 0;
    mSCTcpHead.Clear();
    miSendIndex = 0;
    mbHasRecv = 0;

    memset(&mstTcpStat,0,sizeof(mstTcpStat));

    //初始化客户端socket数组信息
    for (int i = 0; i < MAX_SOCKET_NUM; ++i)
    {
        memset(&mastSocketInfo[i],0,sizeof(TSocketInfo));
        //把socket句柄设为无效句柄
        mastSocketInfo[i].miSocket = INVALID_SOCKET;
    }

    //初始化epoll
    mpEpollevents = NULL;
    //初始化epoll socket
    iTmpRet = InitEpollSocket((short)port);
    if (0 != iTmpRet)
    {
        LOG_ERROR("default","InitEpollSocket failed! TCPserver init failed. ReusltCode = %d!",iTmpRet);
        return iTmpRet;   
    }
    LOG_INFO("default","InitEpollSocket successed! TCPserver init failed. ReusltCode = %d!",iTmpRet);

    return 0;
}

int CTcpCtrl::Run()
{
    return 0;
}

int CTcpCtrl::SetRunFlag(int iRunFlag)
{
    return 0;
}

/**
  * 函数名          : CTCPCtrl::InitEpollSocket
  * 功能描述        : 初始化Epoll socket
  * 返回值          ：void 
**/
int CTcpCtrl::InitEpollSocket(short shTmpport)
{
    int iTmpRet = 0;
    socklen_t iOptval = 0;
    //兼容不同的平台系统
    int iOpenLen = sizeof(int);
    int iTmpFlags = 0;
    struct linger ling = {0,0};
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(struct sockaddr_in));

    if (EphInit() == -1)
    {
        return 2;
    }

    if ((miSocket = EphSocket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        EphCleanUp();
        return 3;
    }

    /*SO_REUSEADDR
     *R允许启动一个监听服务器并捆绑其众所周知端口，即使以前建立的将此端口用做他们的本地端口的连接仍存在。这通常是重启监听服务器时出现，若不设置此选项，则bind时将出错。
     *允许在同一端口上启动同一服务器的多个实例，只要每个实例捆绑一个不同的本地IP地址即可。对于TCP，我们根本不可能启动捆绑相同IP地址和相同端口号的多个服务器。
     *允许单个进程捆绑同一端口到多个套接口上，只要每个捆绑指定不同的本地IP地址即可。这一般不用于TCP服务器。
     *允许完全重复的捆绑：当一个IP地址和端口绑定到某个套接口上时，还允许此IP地址和端口捆绑到另一个套接口上。一般来说，这个特性仅在支持多播的系统上才有，而且只对UDP套接口而言（TCP不支持多播）。
    */
    setsockopt(miSocket,SOL_SOCKET,SO_REUSEADDR,&iTmpFlags,sizeof(iTmpFlags));
    setsockopt(miSocket,SOL_SOCKET,SO_KEEPALIVE,&iTmpFlags,sizeof(iTmpFlags));
    //close()立刻返回，底层将未发送完的数据发送完成后再释放资源，即优雅退出。
    setsockopt(miSocket,SOL_SOCKET,SO_LINGER,&ling,sizeof(linger));
    //禁止Nagle’s Algorithm延时算法，立刻发送
    setsockopt(miSocket,IPPROTO_TCP,TCP_NODELAY,&iTmpFlags,sizeof(iTmpFlags));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(miSocket,(struct sockaddr *)&addr,sizeof(addr)) == -1)
    {
        LOG_ERROR("default","Bind socket Error!");
        EphClose(miSocket);
        EphCleanUp();
        return 4;
    }

    int iTmpOptLen = sizeof(socklen_t);
    int iTmpOptval = TCP_BUFFER_LEN;
    if (!setsockopt(miSocket,SOL_SOCKET,SO_RCVBUF,(const void*)&iTmpOptval,iTmpOptLen))
    {
        LOG_ERROR("default","Set Recv buffer size to %d failed",iTmpOptval);
        return -1;
    }

    if (!getsockopt(miSocket,SOL_SOCKET,SO_RCVBUF,(void*)&iTmpOptval,(socklen_t*)&iTmpOptLen))
    {
        LOG_INFO("default","Set Recv buffer size to %d",iTmpOptval); 
    }
    //设置接受队列大小
    iTmpRet = listen(miSocket,RECV_QUEUQ_MAX);
    if (-1 == iTmpRet)
    {
        LOG_ERROR("default","Listen %d connection failed",RECV_QUEUQ_MAX); 
        return -1;
    }

    //把epoll socket fd放入epoll socket集合中
    if (EphNewConn(miSocket) == -1)
    {
        EphClose(miSocket);
        EphCleanUp();
        return 5;
    }
}

/**
  * 函数名          : CTCPCtrl::EphInit
  * 功能描述        : 初始化Epoll event
  * 返回值          ：void 
**/
int CTcpCtrl::EphInit()
{
    //已经设置
    if (NULL != mpEpollevents)
    {
        return 0;
    }

    memset(&mstEpollEvent,0,sizeof(struct epoll_event));
    mstEpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP; //epoll监听 可读，错误，和挂起事件
    //初始化event触发事件信息
    mstEpollEvent.data.ptr = NULL;
    mstEpollEvent.data.fd = -1;

    mpEpollevents = (struct epoll_event*)malloc((MAX_SOCKET_NUM) * sizeof(struct epoll_event));
    if (NULL == mpEpollevents)
    {
        LOG_ERROR("default","malloc mpEpollevents Error!");
        return -1;
    }
    //生成epoll描述符
    if ((miKdpfd = epoll_create(MAX_SOCKET_NUM)) < 0)
    {
        LOG_ERROR("default","ERROR:%s",strerror(errno));
        LOG_ERROR("default","epoll create error!");
        return -1;
    }
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphSocket
  * 功能描述        : 初始化Epoll socket
  * 返回值          ：void 
**/
int CTcpCtrl::EphSocket(int iDomain,int iType,int iProtocol)
{
    int iTmpSfd = socket(iDomain,iType,iProtocol);
    int iTmpFlags = 1;

    if (iTmpSfd == -1)
    {
        LOG_ERROR("default","socket create error!");
        return -1;
    }

    //设置socket非阻塞
    if (ioctl(iTmpSfd,FIONBIO,&iTmpFlags) &&
        ((iTmpFlags = fcntl(iTmpSfd,F_GETFL,0)) < 0 || fcntl(iTmpSfd,F_GETFL, iTmpFlags | O_NONBLOCK) < 0))
    {
        LOG_ERROR("default","set socket nonblock error!");
        close(iTmpSfd);
        return -1;
    }
    return iTmpSfd;
}

/**
  * 函数名          : CTCPCtrl::EphSocket
  * 功能描述        : 关闭Epoll socket
  * 返回值          ：void 
**/
int CTcpCtrl::EphClose(int iSocketFd)
{
    close(iSocketFd);
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphCleanUp
  * 功能描述        : 清除epoll
  * 返回值          ：void 
**/
int CTcpCtrl::EphCleanUp()
{
    free(mpEpollevents);
    close(miKdpfd);
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphNewConn
  * 功能描述        : 创建连接
  * 返回值          ：void 
**/
int CTcpCtrl::EphNewConn(int iSocketFd)
{
    mstEpollEvent.data.fd = iSocketFd;
    if (epoll_ctl(miKdpfd,EPOLL_CTL_ADD,iSocketFd,&mstEpollEvent) < 0)
    {
        LOG_ERROR("default","create new connection error,socket fd:%d!",iSocketFd);
        return -1;
    }
    return 0;
}

/**
  * 函数名          : CTCPCtrl::RecvClientData
  * 功能描述        : 接客户端的数据
  * 返回值          ：void
**/
int CTcpCtrl::RecvClientData(short shIndex)
{
    int iTmpRet = 0;
    int iTmpRecvBytes = 0;
    int iTempRet;
    int iRecvBytes;
    int iOffset;
    char* pTemp;
    char* pTemp1;
    unsigned short unRecvLen;
    int nRecvAllLen;
    time_t tTempTime;
    int iSocket = mpSocketInfo->miSocket;
    unsigned short unLength = 0;
    iOffset = mpSocketInfo->miRecvBytes;
}

/**
  * 函数名          : CTCPCtrl::TcpRead
  * 功能描述        : 读取tcp数据
  * 返回值          ：void
**/
int CTcpCtrl::TcpRead(int iSocket, char *pBuf, int iLen)
{
    int iTmpRecvBytes = 0;
    while(1)
    {
        iTmpRecvBytes = read(iSocket,pBuf,iLen);
        //读取成功
        if (iTmpRecvBytes > 0)
        {
            return iTmpRecvBytes;
        }
        else
        {
            if (iTmpRecvBytes < 0 && errno == EINTR)
            {
                continue;
            }
            return iTmpRecvBytes;
        }
    }
}

