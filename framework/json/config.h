//
//  config.h
//  读取配置文件
//  Created by dguco on 17-2-16.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef SERVER_CONFIG_H_H
#define SERVER_CONFIG_H_H
#include <memory>
#include "../base/servertool.h"
#include "json.h"

class CServerConfig : public CSingleton<CServerConfig>,public MyJson::Json
{
public:
    CServerConfig();
    ~CServerConfig();
    int Parse();
    void Clear();

public:
    const int GetTcpServerId() const {return m_iTcpServerId;}
    const string GetTcpHost() const {return m_sTcpHost;}
    const int GetTcpPort() const {return m_iTcpPort;}

    const int GetGateServerId() const {return m_iGateServerId;}
    const string GetGateHost() const {return m_sGateHost;}
    const int GetGatePort() const {return m_iGatePort;}

    const int GetGameServerId() const {return m_iGameServerId;}
    const string GetGameHost() const {return m_sGameHost;}
    const int GetGamePort() const {return m_iGamePort;}

    const int GetDbServerId() const {return m_iDbServerId;}
    const string GetDbHost() const {return m_sDbHost;}
    const int GetDbPort() const {return m_iDbPort;}
    const string GetDbInfo() const {return m_sDblInfo;}
    const int GetDbSleep() const {return m_iDbSleepTime};
    const int GetDbLoop() const { return m_iDbLoop;}

    const int GetTcpKeepAlive() const {return m_iTcpKeepAlive;}
    const int GetServetTick() const {return m_iServerTick;}
    const int GetCheckTimeOutGap() const {return m_iChecktimeOutGap;}
    const int GetSocketTimeOut() const {return m_iSokcetTimeout;}
    const int GetProxySize() const { return m_iProxySize;}

private:
    //TCPserver
    int m_iTcpServerId;
    string m_sTcpHost;
    int m_iTcpPort;

    //gateserver
    int m_iGateServerId;
    string m_sGateHost;
    int m_iGatePort;

    //gameserver
    int m_iGameServerId;
    string m_sGameHost;
    int m_iGamePort;

    //dbserver
    int m_iDbServerId;
    string m_sDbHost;
    int m_iDbPort;
    string m_sDblInfo;
    int m_iDbSleepTime;
    int m_iDbLoop;

    //心跳
    int m_iTcpKeepAlive;
    int m_iServerTick;
    int m_iChecktimeOutGap;
    int m_iSokcetTimeout;
    int m_iProxySize;
};

template<class CServerConfig> CServerConfig* CSingleton<CServerConfig>::spSingleton = NULL;
#endif //SERVER_CONFIG_H_H
