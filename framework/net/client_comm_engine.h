#ifndef CLIENT_COMM_ENGINE_H_
#define CLIENT_COMM_ENGINE_H_

class CMessage;
class CTcpHead;

#include "../message/message_interface.h"
#include "../message/message.pb.h"
#include "ccrypto.h"

class ClientCommEngine
{
public:
	static unsigned char tKey[16];
	static unsigned char* tpKey;
	/**
	 * @param pBuff         client上行数据
	 * @param unBuffLen     数据长度
	 * @param pHead         转发给gameserver的消息C2SHead
     * @param iTmpOffset    消息偏移
     * @param unTmpDataLen  数据长度
	 * @return          0：成功 1：数据不完整继续接收  其他：错误
	 */
	static int ParseClientStream(const void* pBuff,
                                    unsigned int nRecvAllLen,
                                    C2SHead* pHead,
                                    unsigned int& unTmpOffset,
                                    unsigned int& unTmpDataLen);

    /**
     * 序列化消息发送到gameserver,如果客户端上行数据加密则解密(gateservet==>gameserver),
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     消息长度
     * @param pDataBuff     客户端上行消息数据指针
     * @param unDataLen     客户端上行消息数据长度
     * @param pMsg          反序列化客户端消息组成的CClientMessage
     * @param bEncrypt      是否加密
     * @param pEncrypt      密钥
     * @return               0 成功 其他 失败
     */
	static int ConverToGameStream(const void* pBuff,
                                    unsigned int& unBuffLen,
                                    const void *pDataBuff,
                                    unsigned int& unDataLen,
                                    C2SHead* pHead = NULL,
                                    const unsigned char* pEncrypt = ClientCommEngine::tpKey);

    /**
     * 序列化消息CMessage 发送到gateserver（gameserver==>gateserver）
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     消息长度
     * @param pMsg          反序列化客户端消息组成的CClientMessage
     * @param bEncrypt      是否加密
     * @param pEncrypt      密钥
     * @return              0 成功 其他 失败
     */
	static int ConvertMessageToStream(const void* pBuff,
                                        unsigned int& unBuffLen,
                                        S2CHead* pHead,
                                        CMessage* pMsg);

    /**
     * @param pBuff         存放转换后地址指针
     * @param unBuffLen     消息长度
     * @param pHead         反序列化后的消息头
     * @param message       反序列化后的消息
     * @return              0 成功 其他:失败错误码
     */
	static int ConvertStreamToMessage(const void* pBuff,
                                    unsigned short unBuffLen,
                                    C2SHead* pHead,
                                    Message* pMessage,
                                    CFactory* pMsgFactory = NULL);  

    /**
	 * @param pBuff         数据指针
	 * @param unBuffLen     数据长度
	 * @param pHead         S2CHead
     * @param iTmpOffset    消息偏移
	 * @return              0 成功 其他:失败错误码
     */
	static int ConvertStreamToMessage(const void* pBuff,
                                    unsigned int nRecvAllLen,
                                    S2CHead* pHead,
                                    unsigned int& unTmpOffset);
};

#endif /* CLIENT_COMM_ENGINE_H_ */
