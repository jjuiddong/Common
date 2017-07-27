
#include "stdafx.h"
#include "event.h"

using namespace framework;


cEvent::cEvent() 
{ 
	m_type = EVENT::EVT_NULL; 
	m_isSkip = false; 
	m_id = 0;
}

cEvent::cEvent(EVENT::TYPE type, int id) : 
	m_type(type)
,	m_id(id)
,	m_isSkip(false) 
{
}

cEvent::cEvent(EVENT::TYPE type, int id, int param) : 
	m_type(type)
,	m_id(id)
,	m_param(param)
,	m_isSkip(false) 
{
}

