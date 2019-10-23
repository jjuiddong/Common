//
// 2019-04-12, jjuiddong
// circular queue dynamic memory
//
#pragma once


namespace common
{

	template<class T>
	struct cCircularQueue2
	{
	public:
		cCircularQueue2(const int maxSize);
		virtual ~cCircularQueue2();

		void push(const T &t);
		uint push(const T *t, const uint size);
		bool pop();
		bool pop(const uint size);
		bool pop(T *dst, const uint size);
		T& front();
		bool frontCopy(T *dst, const uint size);
		T* frontPtr();
		T& back();
		T* backPtr();
		bool empty() const;
		uint size() const;
		void clear();


	public:
		T *m_datas;
		int m_front;
		int m_rear;
		const int SIZE;
	};


	template<class T>
	cCircularQueue2<T>::cCircularQueue2(const int maxSize)
		: m_front(0)
		, m_rear(0)
		, m_datas(new T[maxSize])
		, SIZE(maxSize)
	{
	}


	template<class T>
	cCircularQueue2<T>::~cCircularQueue2()
	{
		clear();
		delete[] m_datas;
		m_datas = NULL;
	}


	template<class T>
	void cCircularQueue2<T>::push(const T &t)
	{
		m_datas[m_rear] = t;
		m_rear = (m_rear + 1) % SIZE;
		if (m_rear == m_front)
			m_front = (m_front + 1) % SIZE;
	}


	// return total copy size
	template<class T>
	uint cCircularQueue2<T>::push(const T *t, const uint size0)
	{
		const uint remainSize = SIZE - size();
		if (remainSize < size0)
			return 0;

		uint totalCpSize = 0;
		const uint maxSize = (m_front > m_rear) ? (m_front - m_rear - 1) : (SIZE - m_rear);
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
			m_rear %= SIZE;
		}
		else
		{
			m_rear += cpSize;
			m_rear %= SIZE;
		}

		return totalCpSize;
	}


	template<class T>
	bool cCircularQueue2<T>::pop()
	{
		if (empty())
			return false;
		m_front = (m_front + 1) % SIZE;
		return true;
	}


	template<class T>
	bool cCircularQueue2<T>::pop(const uint size0)
	{
		if (empty())
			return false;
		const uint curSize = size();
		if (curSize < size0)
			return false;

		if (m_front < m_rear)
		{
			const uint popSize = min(curSize, size0);
			m_front += popSize;
			m_front %= SIZE;
			return popSize == size0;
		}
		else
		{
			const uint popSize = min((uint)(SIZE - m_front), size0);
			m_front += popSize;
			m_front %= SIZE;

			if (popSize < size0)
			{
				const uint popRemainSize = min((uint)m_rear, size0 - popSize);
				m_front = popRemainSize;
				m_front %= SIZE;
				return size0 == (popSize + popRemainSize);
			}
		}
		return true;
	}


	// copy front size0, pop size0
	template<class T>
	bool cCircularQueue2<T>::pop(T *dst, const uint size0)
	{
		if (empty())
			return false;
		const uint curSize = size();
		if (curSize < size0)
			return false;

		if (m_front < m_rear)
		{
			const uint popSize = min(curSize, size0);
			memcpy(dst, &m_datas[m_front], popSize);
			m_front += popSize;
			m_front %= SIZE;
			return popSize == size0;
		}
		else
		{
			const uint popSize = min((uint)(SIZE - m_front), size0);
			memcpy(dst, &m_datas[m_front], popSize);
			m_front += popSize;
			m_front %= SIZE;

			if (popSize < size0)
			{
				const uint popRemainSize = min((uint)m_rear, size0 - popSize);
				memcpy(dst + popSize, &m_datas[0], popRemainSize);
				m_front = popRemainSize;
				m_front %= SIZE;
				return size0 == (popSize + popRemainSize);
			}
		}

		return true;
	}


	template<class T>
	T& cCircularQueue2<T>::front()
	{
		return m_datas[m_front];
	}


	// copy front, size0, no remove
	template<class T>
	bool cCircularQueue2<T>::frontCopy(T *dst, const uint size0)
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
			const uint cpSize = min((uint)(SIZE - m_front), (int)size0);
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


	template<class T>
	T* cCircularQueue2<T>::frontPtr()
	{
		return &m_datas[m_front];
	}


	template<class T>
	T& cCircularQueue2<T>::back()
	{
		return m_datas[(m_rear - 1 + SIZE) % SIZE];
	}


	template<class T>
	T* cCircularQueue2<T>::backPtr()
	{
		return &m_datas[(m_rear - 1 + SIZE) % SIZE];
	}


	template<class T>
	bool cCircularQueue2<T>::empty() const
	{
		return m_front == m_rear;
	}


	template<class T>
	uint cCircularQueue2<T>::size() const
	{
		if (m_front > m_rear)
		{
			return (uint)((SIZE - m_front) + m_rear);
		}
		return (uint)(m_rear - m_front);
	}


	template<class T>
	void cCircularQueue2<T>::clear()
	{
		m_front = m_rear = 0;
	}

}
