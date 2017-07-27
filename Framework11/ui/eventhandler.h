#pragma once


namespace framework
{

	class cEvent;
	class cEventHandler;
	typedef void (cEventHandler::*EventFunction)(cEvent &);


	class cEventHandler
	{
	public:
		cEventHandler() {}
		virtual ~cEventHandler() {}

		bool EventConnect(cEventHandler *handler, EVENT::TYPE type, int id, EventFunction fn);
		bool EventDisconnect(cEventHandler *handler, EVENT::TYPE type, int id);


	protected:
		virtual bool SearchEventTable( cEvent &event );


	private:
		struct sEventEntry
		{
			EVENT::TYPE type;
			int id;
			cEventHandler *handler;
			EventFunction fn;
			bool operator==(const sEventEntry &rhs);
		};

		std::vector<sEventEntry> m_EventTable;
	};

}
