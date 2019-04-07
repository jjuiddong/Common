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

		void push(const T &t);
		uint push(const T *t, const uint size);
		void pop();
		void pop(const uint size);
		T& front();
		bool frontPop(T *dst, const uint size);
		bool front(T *dst, const uint size);
		T* frontPtr();
		T& back();
		T* backPtr();
		bool empty() const;
		uint size() const;
		void clear();


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
		clear();
	}


	template<class T, size_t MAX_SIZE>
	void cCircularQueue<T, MAX_SIZE>::push(const T &t)
	{
		m_datas[m_rear] = t;
		m_rear = (m_rear + 1) % MAX_SIZE;
		if (m_rear == m_front)
			m_front = (m_front + 1) % MAX_SIZE;
	}


	// return total copy size
	template<class T, size_t MAX_SIZE>
	uint cCircularQueue<T, MAX_SIZE>::push(const T *t, const uint size0)
	{
		const uint remainSize = MAX_SIZE - size();
		if (remainSize < size0)
			return 0;

		uint totalCpSize = 0;
		const uint maxSize = (m_front > m_rear)? (m_front - m_rear - 1) : (MAX_SIZE - m_rear);
		const uint cpSize = min(maxSize, size0);
		memcpy(&m_datas[m_rear], t, cpSize);
		totalCpSize += cpSize;

		if ((cpSize < size0) && (m_rear >= m_front))
		{
			// copy remain data
			const uint remainSize = max(0, m_front - 1);
			const uint remainCpSize = min(remainSize, size0 - cpSize);
			memcpy(&m_datas[0], t + cpSize, remainCpSize);
			totalCpSize += remainCpSize;

			m_rear = remainCpSize;
		}
		else
		{
			m_rear += cpSize;
		}

		return totalCpSize;
	}


	template<class T, size_t MAX_SIZE>
	void cCircularQueue<T, MAX_SIZE>::pop()
	{
		if (empty())
			return;
		m_front = (m_front + 1) % MAX_SIZE;
	}

	
	template<class T, size_t MAX_SIZE>
	void cCircularQueue<T, MAX_SIZE>::pop(const uint size0)
	{
		if (empty())
			return;
		const uint curSize = size();
		if (curSize < size0)
			return;

		const int popSize = min(MAX_SIZE, size0);
		const uint maxSize = (m_rear > m_front) ? (m_rear - m_front) : (MAX_SIZE - m_front);
		const uint rmSize = min(popSize, maxSize);
		
		if ((rmSize < popSize) && (m_front > m_rear))
		{
			// pop remain data
			const uint remainSize = max(0, m_rear);
			const uint remainRmSize = min(remainSize, popSize - rmSize);
			m_front = remainRmSize;
		}
		else
		{
			m_front += rmSize;
		}
	}


	template<class T, size_t MAX_SIZE>
	T& cCircularQueue<T, MAX_SIZE>::front()
	{
		return m_datas[m_front];
	}


	template<class T, size_t MAX_SIZE>
	bool cCircularQueue<T, MAX_SIZE>::front(T *dst, const uint size0)
	{
		if (empty())
			return false;
		const uint curSize = size();
		if (curSize < size0)
			return false;

		if (m_front < m_rear)
		{
			const uint cpSize = min(curSize, size0);
			memcpy(dst, &m_datas[m_front], cpSize);
			return cpSize == size0;
		}
		else
		{
			const uint cpSize = min(MAX_SIZE - m_front, size0);
			memcpy(dst, &m_datas[m_front], cpSize);

			if (cpSize < size0)
			{
				const uint cpRemainSize = min((uint)m_rear, size0 - cpSize);
				memcpy(dst + cpSize, &m_datas[0], cpRemainSize);
				return size0 == (cpSize + cpRemainSize);
			}
		}

		return true;
	}


	template<class T, size_t MAX_SIZE>
	bool cCircularQueue<T, MAX_SIZE>::frontPop(T *dst, const uint size0)
	{
		if (empty())
			return false;
		const uint curSize = size();
		if (curSize < size0)
			return false;

		if (m_front < m_rear)
		{
			const uint cpSize = min(curSize, size0);
			memcpy(dst, &m_datas[m_front], cpSize);
			m_front += cpSize;
			return cpSize == size0;
		}
		else
		{
			const uint cpSize = min(MAX_SIZE - m_front, size0);
			memcpy(dst, &m_datas[m_front], cpSize);
			m_front += cpSize;

			if (cpSize < size0)
			{
				const uint cpRemainSize = min((uint)m_rear, size0 - cpSize);
				memcpy(dst + cpSize, &m_datas[0], cpRemainSize);
				m_front = cpRemainSize;
				return size0 == (cpSize + cpRemainSize);
			}
		}

		return true;
	}


	template<class T, size_t MAX_SIZE>
	T* cCircularQueue<T, MAX_SIZE>::frontPtr()
	{
		return &m_datas[m_front];
	}


	template<class T, size_t MAX_SIZE>
	T& cCircularQueue<T, MAX_SIZE>::back()
	{
		return m_datas[(m_rear - 1 + MAX_SIZE) % MAX_SIZE];
	}


	template<class T, size_t MAX_SIZE>
	T* cCircularQueue<T, MAX_SIZE>::backPtr()
	{
		return &m_datas[(m_rear - 1 + MAX_SIZE) % MAX_SIZE];
	}


	template<class T, size_t MAX_SIZE>
	bool cCircularQueue<T, MAX_SIZE>::empty() const
	{
		return m_front == m_rear;
	}


	template<class T, size_t MAX_SIZE>
	uint cCircularQueue<T, MAX_SIZE>::size() const
	{
		if (m_front > m_rear)
		{
			return (uint)((MAX_SIZE - m_front) + m_rear);
		}
		return (uint)(m_rear - m_front);
	}


	template<class T, size_t MAX_SIZE>
	void cCircularQueue<T, MAX_SIZE>::clear()
	{
		m_front = m_rear = 0;
	}

}
