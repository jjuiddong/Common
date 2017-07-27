
#include "stdafx.h"
#include "eventhandler.h"
#include "event.h"


using namespace framework;


bool cEventHandler::sEventEntry::operator==(const sEventEntry &rhs)
{
	if (rhs.type == EVENT::EVT_NULL)
		return (handler == rhs.handler);
	else
		return ((type == rhs.type) && (id == rhs.id) && (handler == rhs.handler));
}


/**
 @brief Add Event Table
 */
bool cEventHandler::EventConnect(cEventHandler *handler, EVENT::TYPE type, int id, EventFunction fn)
{
	sEventEntry entry;
	entry.handler = handler;
	entry.type = type;
	entry.id = id;
	entry.fn = fn;
	m_EventTable.push_back(entry);
	return true;
}


/**
 @brief Remove Event Handler
 @pram type : if NULL remove all handler element
 */
bool cEventHandler::EventDisconnect(cEventHandler *handler, EVENT::TYPE type, int id)
{
	if (EVENT::EVT_NULL == type)
	{
		for (int i=m_EventTable.size()-1; i >= 0; --i)
		{
			if (m_EventTable[ i].handler == handler)
			{
				common::removevector(m_EventTable, m_EventTable[ i]);
			}
		}
	}
	else
	{
		sEventEntry item;
		item.handler = handler;
		item.type =  type;
		item.id = id;
		common::removevector(m_EventTable, item);
	}
	return true;
}


/**
 @brief call event handler
 */
bool cEventHandler::SearchEventTable( cEvent &event )
{
	for each (auto &entry in m_EventTable)
	{
		if ((event.GetType() == entry.type) && (event.GetId() == entry.id))
		{
			if (entry.handler)
			{
				cEventHandler *handler = entry.handler;
				(handler->*entry.fn)( event );
			}
			else
			{
				// not yet
				//EvtHandler_Fn fn = (EvtHandler_Fn)entry.fn;
				//(*fn)(event);
			}
		}
	}

	return true;
}

