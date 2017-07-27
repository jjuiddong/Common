#pragma once


namespace framework
{

	class cEvent
	{
	public:
		cEvent();
		cEvent(EVENT::TYPE type, int id);
		cEvent(EVENT::TYPE type, int id, int param);
		virtual ~cEvent() {}
		void SetType(EVENT::TYPE type);
		EVENT::TYPE GetType() const;
		int GetParam() const;
		int GetId() const;
		void Skip();
		bool IsSkip() const;


	private:
		EVENT::TYPE m_type;
		int m_id; // node id
		int m_param;
		bool m_isSkip;
	};


	inline void	cEvent::SetType(EVENT::TYPE type) { m_type = type; }
	inline EVENT::TYPE cEvent::GetType() const { return m_type; }
	inline int cEvent::GetParam() const { return m_param; }
	inline int cEvent::GetId() const { return m_id; }
	inline void	cEvent::Skip() { m_isSkip = true; }
	inline bool cEvent::IsSkip() const { return m_isSkip; }
}
