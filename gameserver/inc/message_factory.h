/*
 * messagefactory.h
 *
 *  Created on  :  2017年8月8日
 *  Author      : DGuco
 *  c++ placement new 操作符在指定空间macMessageBuff生成消息
 */

#ifndef SERVER_MESSAGE_FACTORY_H_
#define SERVER_MESSAGE_FACTORY_H_

#include "message_interface.h"

#define CASE_NEW_MSG(msgname, msgid)  \
    case msgid:                       \
    {                                 \
        pTmpMessage = new msgname;    \
        break;                        \
    }

class CMessageFactory: public CFactory, public CSingleton<CMessageFactory>
{
public:
	Message *CreateMessage(unsigned int uiMessageID);

private:
	Message *CreateClientMessage(unsigned int uiMessageID);
	Message *CreateServerMessage(unsigned int uiMessageID);
};


#endif /* MESSAGE_FACTORY_H_ */
