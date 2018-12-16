//
// 2018-12-03, jjuiddong
// circular queue
//
#pragma once


namespace common
{

	template<class T, size_t MAX_SIZE>
	class cCircularQueue
	{
	public:
		cCircularQueue();
		virtual ~cCircularQueue();

		void Push(const T &t);
		void Pop();
		T& Front();
		T& Back();
		bool IsEmpty() const;
		void Clear();


	public:
		const int SIZE = MAX_SIZE;
		T m_datas[MAX_SIZE];
		int m_front;
		int m_rear;
	};


	template<class T, size_t MAX_SIZE>
	cCircularQueue<T, MAX_SIZE>::cCircularQueue()
		: m_front(0)
		, m_rear(0)
	{
	}


	template<class T, size_t MAX_SIZE>
	cCircularQueue<T, MAX_SIZE>::~cCircularQueue()
	{
		Clear();
	}


	template<class T, size_t MAX_SIZE>
	void cCircularQueue<T, MAX_SIZE>::Push(const T &t)
	{
		m_datas[m_rear] = t;
		m_rear = (m_rear + 1) % MAX_SIZE;
		if (m_rear == m_front)
			m_front = (m_front + 1) % MAX_SIZE;
	}


	template<class T, size_t MAX_SIZE>
	void cCircularQueue<T, MAX_SIZE>::Pop()
	{
		if (IsEmpty())
			return;
		m_front = (m_front + 1) % MAX_SIZE;
	}


	template<class T, size_t MAX_SIZE>
	T& cCircularQueue<T, MAX_SIZE>::Front()
	{
		return m_datas[m_front];
	}


	template<class T, size_t MAX_SIZE>
	T& cCircularQueue<T, MAX_SIZE>::Back()
	{
		return m_datas[(m_rear - 1 + MAX_SIZE) % MAX_SIZE];
	}


	template<class T, size_t MAX_SIZE>
	bool cCircularQueue<T, MAX_SIZE>::IsEmpty() const
	{
		return m_front == m_rear;
	}


	template<class T, size_t MAX_SIZE>
	void cCircularQueue<T, MAX_SIZE>::Clear()
	{
		m_front = m_rear = 0;
	}

}
