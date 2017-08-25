//
// Created by dguco on 17-2-16.
//
#include "config.h"

CServerConfig::CServerConfig()
{
    Clear();
}

CServerConfig::~CServerConfig()
{

}

int CServerConfig::Parse()
{
    m_iTcpKeepAlive = m_Obj["keepalive"].GetInt();
    m_iServerTick = m_Obj["servertick"].GetInt();
    m_iChecktimeOutGap = m_Obj["checktimeOutGap"].GetInt();
    m_iSokcetTimeout = m_Obj["socketTimeout"].GetInt();
    m_iProxySize = m_Obj["proxySize"].GetInt();

    rapidjson::Value& tcpinfo = m_Obj["tcpinfo"];
    m_iTcpPort =  tcpinfo["port"].GetInt();
    m_sTcpHost =  tcpinfo["host"].GetString();
    m_iTcpServerId = tcpinfo["serverid"].GetInt();

    rapidjson::Value& gameinfo = m_Obj["gameinfo"];
    m_iGameServerId =  gameinfo["serverid"].GetInt();
    m_iGamePort =  gameinfo["port"].GetInt();
    m_sGameHost =  gameinfo["host"].GetString();

    rapidjson::Value& gateinfo = m_Obj["gateinfo"];
    m_iGateServerId =  gateinfo["serverid"].GetInt();
    m_iGatePort =  gateinfo["port"].GetInt();
    m_sGateHost =  gateinfo["host"].GetString();

    rapidjson::Value& dbinfo = m_Obj["dbinfo"];
    m_iDbServerId =  dbinfo["serverid"].GetInt();
    m_iDbPort =  dbinfo["port"].GetInt();
    m_sDbHost =  dbinfo["host"].GetString();
    m_sDblInfo = dbinfo["mysqlinfo"].GetString();
    m_iDbSleepTime = dbinfo["sleeptime"].GetInt();
    m_iDbLoop = dbinfo["loop"].GetInt();

    return 0;
}

void CServerConfig::Clear()
{
    m_iSokcetTimeout = 0;
    m_iChecktimeOutGap = 0;
    m_iTcpPort = 0;
    m_iGateServerId = 0;
    m_iGatePort = 0;
    m_iGameServerId = 0;
    m_iGamePort = 0;
    m_iTcpKeepAlive = 0;
    m_iServerTick = 0;
    m_sGameHost.clear();
    m_sGateHost.clear();
    m_sTcpHost.clear();
}

int CServerConfig::getM_iProxySize() const {
    return m_iProxySize;
}
