
#include "stdafx.h"
#include "messagemanager.h"


using namespace ai;


cMessageManager::cMessageManager()
	: m_defaultReceiver(NULL)
{
}

cMessageManager::~cMessageManager()
{
}


void cMessageManager::PostMsg(const sMsg &msg)
{
	m_msgs.push(msg);
}


void cMessageManager::SendMsg(const sMsg &msg)
{
	ProcessMsg(msg);
}


void cMessageManager::DispatchMsg()
{
	while (!m_msgs.empty())
	{
		ProcessMsg(m_msgs.front());
		m_msgs.pop();
	}	
}


void cMessageManager::Clear()
{
	while (!m_msgs.empty())
		m_msgs.pop();
}


void cMessageManager::ProcessMsg(const sMsg &msg)
{
	if (msg.receiver)
	{
		msg.receiver->DispatchMessage(msg);
	}
	else
	{
		if (m_defaultReceiver)
			m_defaultReceiver->DispatchMessage(msg);
	}
}

