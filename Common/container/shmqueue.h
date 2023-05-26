//
// 2022-09-19, jjuiddong
// Inter-Process Communication with shared memory + semaphore + mutex
// Message Queue with Shared Memory
// - named semaphore
// - shared memory name: name + '_shm'
// - mutex name: name + '_mutex'
// - circular queue
//
#pragma once


namespace common
{

	template <class T>
	class cShmQueue
	{
	public:
		cShmQueue(const string &name, const uint size = 256);
		virtual ~cShmQueue();

		bool push(const T& t);
		bool pop(OUT T &out);
		bool empty() const;
		bool full() const;
		uint size() const;
		void clear();


	public:
		string m_name;
		cShmmem m_shm; // shared memory
		cSemaphore m_sem;
		cMutex m_mutex;
		const int SIZE;
	};



	//---------------------------------------------------------------------------------
	// implementation
	
	// cShmQueue()
	template <class T> 
	cShmQueue<T>::cShmQueue(const string& name, const uint size)
		: m_name(name) 
		, m_mutex(name + "_mutex")
		, m_sem(0, name)
		, SIZE(size)
	{
		const string shmName = m_name + "_shm";
		const int shmSize = sizeof(T) * size + (sizeof(int)*2); // sizeof(int)*2: front, rear size
		m_shm.Init(shmName, shmSize);
	}


	// ~cShmQueue()
	template <class T>
	cShmQueue<T>::~cShmQueue() 
	{
		// does not clear data
		// because another process access shared memory data
	}


	// push data
	template <class T>
	bool cShmQueue<T>::push(const T& t)
	{
		m_mutex.Lock();
		int &front = *(int*)m_shm.m_memPtr;
		int &rear = *(int*)(m_shm.m_memPtr + sizeof(int));
		const int idx = sizeof(T) * rear + sizeof(int) * 2;

		// check current queue size
		const int curSize = (front > rear) ?
			((SIZE - front) + rear) : (rear - front);
		const int remainSize = SIZE - curSize - 1;
		if (remainSize <= 0) // is full?
		{
			m_mutex.Unlock();
			return false; // full
		}

		*(T*)(&m_shm.m_memPtr[idx]) = t;
		rear = (rear + 1) % SIZE;
		if (rear == front)
			front = (front + 1) % SIZE;

		m_mutex.Unlock();
		m_sem.Signal();
		return true;
	}


	// pop data
	template <class T>
	bool cShmQueue<T>::pop(OUT T& out)
	{
		m_sem.Wait();
		m_mutex.Lock();
		int& front = *(int*)m_shm.m_memPtr;
		int& rear = *(int*)(m_shm.m_memPtr + sizeof(int));
		const int idx = sizeof(T) * front + sizeof(int) * 2;

		if (front == rear) // empty?
		{
			m_mutex.Unlock();
			return false;
		}

		out = *(T*)(&m_shm.m_memPtr[idx]);
		front = (front + 1) % SIZE;
		m_mutex.Unlock();

		return true;
	}


	// empty?
	template <class T>
	bool cShmQueue<T>::empty() const
	{
		m_mutex.Lock();
		const int front = *(int*)m_shm.m_memPtr;
		const int rear = *(int*)(m_shm.m_memPtr + sizeof(int));
		m_mutex.Unlock();

		return front == rear;
	}


	// full?
	template <class T>
	bool cShmQueue<T>::full() const
	{
		const int remainSize = SIZE - (int)size() - 1;
		return (remainSize <= 0);
	}


	// queue size
	template <class T>
	uint cShmQueue<T>::size() const
	{
		m_mutex.Lock();
		const int front = *(int*)m_shm.m_memPtr;
		const int rear = *(int*)(m_shm.m_memPtr + sizeof(int));
		m_mutex.Unlock();

		if (front > rear)
		{
			return (uint)((SIZE - front) + rear);
		}
		return (uint)(rear - front);
	}


	// clear queue
	template <class T>
	void cShmQueue<T>::clear()
	{
		m_mutex.Lock();
		*(int*)m_shm.m_memPtr = 0; // front index
		*(int*)(m_shm.m_memPtr + sizeof(int)) = 0; // rear index
		m_mutex.Unlock();
	}
}
