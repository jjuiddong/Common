//
// 2020-01-10, jjuiddong
// fileloader2
//	- upgrade fileloader
//	- parallel load
//  - multiple thread
//	- multiple object
//
// 2020-05-07
//	- iParallelLoadObject interface
//	- parallel load object must inherit this interface
//		- to check object type
//
//
#pragma once


namespace graphic
{
	using namespace common;

	interface iParallelLoadObject {
		virtual const char* Type() = 0;
	};

	struct sFileLoaderArg2 {
		int dummy;
	};

	struct sFileCompareData2 {
		int dummy;
	};


	//-----------------------------------------------------------------------
	// Declare cFileLoader2
	//-----------------------------------------------------------------------
	template<size_t MAX // maximum store size
		, size_t TP_MAX = 1 // thread pool size
		, class Args = sFileLoaderArg2
		, class CompareData = sFileCompareData2
	>
	class cFileLoader2
	{
	public:
		enum STATE { NONE, LOADING, COMPLETE, REMOVE_L, REMOVE_C };
		struct sChunk
		{
			double accessTime;
			STATE state; // 0 = loading, 1 = load complete
			iParallelLoadObject *data;
		};


	public:
		cFileLoader2();
		virtual ~cFileLoader2();

		template<class T> 
		T* Load(cRenderer &renderer, const char *fileName);

		template<class T> 
		T* Load(cRenderer &renderer, const T *src
			, const char *fileName, const Args &args);

		template<class T> 
		std::pair<bool, T*> LoadParallel(cRenderer &renderer
			, const char *fileName, int *outFlag, void **outPtr
			, const bool isTopPriority = false
			, const CompareData &compData = {}
		);

		template<class T>
		std::pair<bool, T*> LoadParallel(cRenderer &renderer, const T *src
			, const char *fileName, const Args &args, int *outFlag, void **outPtr
			, const bool isTopPriority = false, const CompareData &compData = {});

		template<class T> std::tuple<bool, int, T*> Find(const char *fileName);
		template<class T> bool Insert(const char *fileName, T *data);
		template<class T> bool Remove(const char *fileName);
		template<class T> bool RemoveParallel(const char *fileName);
		template<class T> bool Remove(T *data);
		template<class T> bool RemoveParallel(T *data);
		template<class T> void ClearFile();

		bool FailLoad(const char *fileName);
		void Clear();


	public:
		map<hashcode, sChunk> m_files; //key = fileName hashcode
		map<hashcode, std::set< std::pair<void**, int*>>> m_updatePtrs; // update pointers
		CriticalSection m_cs;
		cTPSemaphore m_tpLoader; // load thread pool
		cTPSemaphore m_tpRemover; // remove thread pool
	};


	//-----------------------------------------------------------------------
	// Declare cTaskFileLoad 2
	//-----------------------------------------------------------------------
	template<class T
		, size_t MAX
		, size_t TP_MAX = 1
		, class Args = sFileLoaderArg2
		, class CompareData = sFileCompareData2 // not used
		>
	class cTaskFileLoader2 : public common::cTask
					, public common::cMemoryPool4<cTaskFileLoader2<T, MAX, TP_MAX, Args, CompareData>>
	{
	public:
		cTaskFileLoader2(cFileLoader2<MAX, TP_MAX, Args, CompareData> *fileLoader
			, cRenderer *renderer
			, const char *fileName
			, const bool isTopPriority = false
			, const CompareData &compData = {})
			: cTask(common::GenerateId(), "cTaskFileLoader2", isTopPriority)
			, m_renderer(renderer)
			, m_fileName(fileName)
			, m_fileLoader(fileLoader)
			, m_loadType(0)
			, m_compare(compData) {
			m_name = fileName;
		}
		cTaskFileLoader2(cFileLoader2<MAX, TP_MAX, Args, CompareData> *fileLoader
			, cRenderer *renderer
			, const T *src
			, const char *fileName
			, const Args &args
			, const bool isTopPriority = false
			, const CompareData &compData = {})
			: cTask(common::GenerateId(), "cTaskFileLoader2", isTopPriority)
			, m_renderer(renderer)
			, m_fileName(fileName)
			, m_fileLoader(fileLoader)
			, m_src(src)
			, m_args(args)
			, m_loadType(1)
			, m_compare(compData) {
			m_name = fileName;
		}

		virtual ~cTaskFileLoader2() {
		}

		virtual eRunResult::Enum Run(const double deltaSeconds)
		{
			T *data = nullptr;
			const StrPath &path = m_fileName;
			{
				AutoCSLock cs(m_fileLoader->m_cs);
				auto it = m_fileLoader->m_files.find(path.GetHashCode());
				if (m_fileLoader->m_files.end() != it)
				{
					typedef cFileLoader2<MAX, TP_MAX, Args, CompareData> fl;
					if ((fl::REMOVE_L == it->second.state)
						|| (fl::REMOVE_C == it->second.state))
					{
						goto $cancel;
					}
				}
			}

			try
			{
				if (0 == m_loadType)
				{
					data = new T(*m_renderer, m_fileName.c_str());
					if (!m_fileLoader->Insert<T>(m_fileName.c_str(), data))
					{
						dbg::Logp("Error Parallel Load1 Insert %s\n"
							, m_fileName.c_str());
						goto $error;
					}
				}
				else if (1 == m_loadType)
				{
					data = new T(*m_renderer, m_src, m_fileName.c_str(), m_args);
					if (!m_fileLoader->Insert<T>(m_fileName.c_str(), data))
					{
						dbg::Logp("Error Parallel Load2 Insert %s\n"
							, m_fileName.c_str());
						goto $error;
					}
				}
			}
			catch (...)
			{
				goto $error;
			}

			return eRunResult::END;

		$error:
			dbg::ErrLogp("Error cTaskFileLoader2 %d, %s\n"
				, m_loadType, m_fileName.c_str());
			m_fileLoader->FailLoad(m_fileName.c_str());
			SAFE_DELETE(data);
			return eRunResult::END;

		$cancel:
			return eRunResult::END;
		}

		int m_loadType; // 0 or 1
		string m_fileName;
		cRenderer *m_renderer; // reference
		cFileLoader2<MAX, TP_MAX, Args, CompareData> *m_fileLoader; // reference
		const T *m_src; // reference
		Args m_args;
		CompareData m_compare;
	};



	//-----------------------------------------------------------------------
	// Declare cTaskFileRemover2
	//-----------------------------------------------------------------------
	template<class T
		, size_t MAX
		, size_t TP_MAX = 1
		, class Args = sFileLoaderArg2 // not used
		, class CompareData = sFileCompareData2 // not used
	>
	class cTaskFileRemover2 : public common::cTask
		, public common::cMemoryPool4<cTaskFileRemover2<T, MAX, TP_MAX, Args, CompareData>>
	{
	public:
		cTaskFileRemover2(cFileLoader2<MAX, TP_MAX, Args, CompareData> *fileLoader
			, const char *fileName
			, const bool isTopPriority = false)
			: cTask(common::GenerateId(), "cTaskFileRemover2", isTopPriority)
			, m_fileName(fileName)
			, m_fileLoader(fileLoader) {
			m_name = StrId(fileName) + StrId("_remove"); // to prevent remove taskfileloader 
		}

		virtual ~cTaskFileRemover2() {
		}

		virtual eRunResult::Enum Run(const double deltaSeconds)
		{
			T *p = nullptr;
			const StrPath &path = m_fileName;
			{
				AutoCSLock cs(m_fileLoader->m_cs);
				auto it = m_fileLoader->m_files.find(path.GetHashCode());
				m_fileLoader->m_updatePtrs[path.GetHashCode()].clear();
				if (m_fileLoader->m_files.end() != it)
				{
					typedef cFileLoader2<MAX, TP_MAX, Args, CompareData> fl;
					if ((fl::REMOVE_L == it->second.state)
						|| (fl::REMOVE_C == it->second.state))
					{
						p = (T*)it->second.data;
						m_fileLoader->m_files.erase(it);
					}
				}
			}

			if (p)
			{
				p->~T(); // call destructor
				delete p;
			}
			return eRunResult::END;
		}

		string m_fileName;
		cFileLoader2<MAX, TP_MAX, Args, CompareData> *m_fileLoader; // reference
	};



	//-----------------------------------------------------------------------
	// Implements cFileLoader2 
	//-----------------------------------------------------------------------
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	inline cFileLoader2<MAX, TP_MAX, Args, CompareData>::cFileLoader2() {
	}

	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	inline cFileLoader2<MAX, TP_MAX, Args, CompareData>::~cFileLoader2() {
		Clear();
	}


	// Load
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline T* cFileLoader2<MAX, TP_MAX, Args, CompareData>::Load(
		graphic::cRenderer &renderer, const char *fileName)
	{
		auto result = Find<T>(fileName);
		if (std::get<0>(result))
			return std::get<2>(result);

		if (m_files.size() >= MAX)
		{
			dbg::Logp("Error cFileLoader2::Load(), Over Maximum(%d) Load Count\n"
				, MAX);
			return NULL;
		}

		T *data = NULL;
		try
		{
			data = new T(renderer, fileName);
			if (!Insert<T>(fileName, data))
			{
				dbg::ErrLogp("Error cFileLoader2 Insertion %s \n", fileName);
				goto error;
			}
		}
		catch (...)
		{
			dbg::ErrLogp("Error cFileLoader2 %s \n", fileName);
			goto error;
		}

		return data;

	error:
		SAFE_DELETE(data);
		return NULL;
	}


	// Load
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline T* cFileLoader2<MAX, TP_MAX, Args, CompareData>::Load(cRenderer &renderer
		, const T *src, const char *fileName, const Args &args)
	{
		auto result = Find<T>(fileName);
		if (std::get<0>(result))
			return std::get<2>(result);

		if (m_files.size() >= MAX)
		{
			dbg::Logp("Error cFileLoader2::Load(), Over Maximum(%d) Load Count\n"
				, MAX);
			return NULL;
		}

		T *data = NULL;
		try
		{
			data = new T(renderer, src, fileName, args);
			if (!Insert<T>(fileName, data))
			{
				dbg::ErrLogp("Error cFileLoader2 Insertion %s \n", fileName);
				goto error;
			}
		}
		catch (...)
		{
			dbg::ErrLogp("Error cFileLoader2 %s %s\n", fileName);
			goto error;
		}
		return data;

	error:
		SAFE_DELETE(data);
		return NULL;
	}


	// LoadParallel
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	std::pair<bool, T*> cFileLoader2<MAX, TP_MAX, Args, CompareData>::LoadParallel(
		graphic::cRenderer &renderer
		, const char *fileName
		, int *outFlag, void **outPtr
		, const bool isTopPriority //= false
		, const CompareData &compData //={}
	)
	{
		const StrPath path(fileName);
		{
			AutoCSLock cs(m_cs);

			// is exist?
			auto it = m_files.find(path.GetHashCode());
			const bool isFind = (m_files.end() != it);
			const STATE state = (m_files.end() != it) ? it->second.state : NONE;
			if (isFind)
			{
				T *ptr = (T*)it->second.data;

				const STATE oldState = state;
				switch (state)
				{
				case LOADING:
					// if now loading, refresh update ptrs (outPtr, outFlag)
					if (outFlag)
						*outFlag = LOADING; // loading state
					m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
					break;
				case COMPLETE:
					// if complete, load update ptrs
					if (outPtr)
						*outPtr = ptr; // load data pointer
					if (outFlag)
						*outFlag = NONE; // complete state
					break;
				case REMOVE_L:
					it->second.state = LOADING; // recovery loading
					break;
				case REMOVE_C:
					it->second.state = COMPLETE; // recovery store
					break;
				default: assert(0); break;
				}

				if (oldState != REMOVE_L)
					return std::make_pair(isFind, ptr);
			}

			// Not Found or REMOVE_L(LOADING) state, loading
			if ((m_files.size() >= MAX) && (state != LOADING))
			{
				dbg::Logp("Error cFileLoader2::LoadParallel(), Over Maximum(%d) Load Count\n"
					, MAX);

				if (outFlag)
					*outFlag = NONE;
				return{ false, NULL };
			}

			// update outFlag=1 to prevent duplicate loading
			if (outFlag)
				*outFlag = 1; // loading state
			m_files[path.GetHashCode()] = { 0, LOADING, NULL };
			m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
		}

		if (!m_tpLoader.IsInit())
			m_tpLoader.Init(TP_MAX);
		m_tpLoader.PushTask(
			new cTaskFileLoader2<T, MAX, TP_MAX, Args, CompareData>(
				this, &renderer, fileName, isTopPriority, compData)
		);

		return { true, NULL };
	}


	// LoadParallel
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	std::pair<bool, T*> cFileLoader2<MAX, TP_MAX, Args, CompareData>::LoadParallel(
		graphic::cRenderer &renderer
		, const T *src, const char *fileName, const Args &args
		, int *outFlag, void **outPtr
		, const bool isTopPriority //= false
		, const CompareData &compData //={}
	)
	{
		StrPath path(fileName);
		{
			AutoCSLock cs(m_cs);

			// is exist?
			auto it = m_files.find(path.GetHashCode());
			const bool isFind = (m_files.end() != it);
			const STATE state = (m_files.end() != it) ? it->second.state : NONE;
			if (isFind)
			{
				T *ptr = (T*)it->second.data;

				const STATE oldState = state;
				switch (state)
				{
				case LOADING:
				{
					// if now loading, refresh update ptrs (outPtr, outFlag)
					if (outFlag)
						*outFlag = 1; // loading state
					const StrPath path(fileName);
					m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
				}
				break;
				case COMPLETE:
					// if complete, load update ptrs
					if (outPtr)
						*outPtr = ptr; // load data pointer
					if (outFlag)
						*outFlag = 0; // complete state
					break;
				case REMOVE_L:
					it->second.state = LOADING; // recovery loading
					break;
				case REMOVE_C:
					it->second.state = COMPLETE; // recovery story
					break;
				default: assert(0); break;
				}

				if (oldState != REMOVE_L)
					return std::make_pair(true, ptr);
			}
			
			// Not Found or REMOVE_L(LOADING)state, Loading
			if (m_files.size() >= MAX)
			{
				dbg::Logp("Error cFileLoader2::LoadParallel(), Over Maximum(%d) Load Count\n"
					, MAX);

				if (outFlag)
					*outFlag = 0; // error complete state
				return{ false, NULL };
			}

			// update outFlag=1 to prevent duplicate loading
			if (outFlag)
				*outFlag = 1; // loading state
			m_files[path.GetHashCode()] = { 0, LOADING, NULL };
			m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
		}

		if (!m_tpLoader.IsInit())
			m_tpLoader.Init(TP_MAX);
		m_tpLoader.PushTask(
			new cTaskFileLoader2<T, MAX, TP_MAX, Args, CompareData>(
				this, &renderer, src, fileName, args, isTopPriority, compData)
		);

		return{ true, NULL };
	}


	// Find
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline std::tuple<bool, int, T*> cFileLoader2<MAX, TP_MAX, Args, CompareData>::Find(
		const char *fileName)
	{
		StrPath path(fileName);
		AutoCSLock cs(m_cs);
		auto it = m_files.find(path.GetHashCode());
		if (m_files.end() == it)
			return{ false, NONE, NULL }; // not exist		
		return std::make_tuple(true, it->second.state, it->second.data);
	}


	// Insert
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline bool cFileLoader2<MAX, TP_MAX, Args, CompareData>::Insert(
		const char *fileName, T *data)
	{
		AutoCSLock cs(m_cs);

		StrPath path(fileName);
		const hashcode hcode = path.GetHashCode();
		auto it = m_files.find(hcode);

		if (m_files.end() == it) // not found, already remove
		{
			delete data;
			m_updatePtrs[hcode].clear();
		}
		else
		{
			auto &updatePtrs = m_updatePtrs[hcode];
			for (auto ptrs : updatePtrs)
			{
				void **outPtr = ptrs.first;
				int *outFlag = ptrs.second;

				if (outPtr)
					*outPtr = data;
				if (outFlag)
					*outFlag = 0; // complete flag
			}
			updatePtrs.clear();

			m_files[path.GetHashCode()] = { 0, COMPLETE, data };
		}
		return true;
	}


	// FailLoad
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	inline bool cFileLoader2<MAX, TP_MAX, Args, CompareData>::FailLoad(
		const char *fileName)
	{
		AutoCSLock cs(m_cs);

		StrPath path(fileName);
		auto it = m_files.find(path.GetHashCode());

		if (m_files.end() == it) // not found, already remove
		{
			m_updatePtrs[path.GetHashCode()].clear();
		}
		else
		{
			auto &updatePtrs = m_updatePtrs[path.GetHashCode()];
			for (auto ptrs : updatePtrs)
			{
				void **outPtr = ptrs.first;
				int *outFlag = ptrs.second;

				if (outPtr)
					*outPtr = NULL;
				if (outFlag)
					*outFlag = 0; // fail complete flag
			}
			updatePtrs.clear();

			m_files[path.GetHashCode()] = { 0, COMPLETE, NULL };
		}

		return true;
	}


	// Remove
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline bool cFileLoader2<MAX, TP_MAX, Args, CompareData>::Remove(
		const char *fileName)
	{
		StrPath path(fileName);

		AutoCSLock cs(m_cs);
		auto it = m_files.find(path.GetHashCode());
		m_updatePtrs[path.GetHashCode()].clear();

		if (m_files.end() == it)
		{
			return false; // not exist
		}

		if (it->second.data)
		{
			((T*)it->second.data)->~T(); // call destructor
			delete it->second.data;
		}
		m_files.erase(it);
		m_tpLoader.RemoveTask(fileName);
		return true;
	}


	// remove parallel
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline bool cFileLoader2<MAX, TP_MAX, Args, CompareData>::RemoveParallel(
		const char *fileName)
	{
		const StrPath path(fileName);
		{
			AutoCSLock cs(m_cs);
			auto it = m_files.find(path.GetHashCode());
			m_updatePtrs[path.GetHashCode()].clear();

			if (m_files.end() == it)
				return false; // not exist

			switch (it->second.state)
			{
			case LOADING: it->second.state = REMOVE_L; break;
			case COMPLETE: it->second.state = REMOVE_C; break;
			default: break;
			}
		}
		
		m_tpLoader.RemoveTask(fileName); // loading stop

		if (!m_tpLoader.IsInit())
			m_tpLoader.Init(1);
		m_tpLoader.PushTask(
			new cTaskFileRemover2<T, MAX, TP_MAX, Args, CompareData>(
				this, fileName)
		);
		return true;
	}


	// Remove
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline bool cFileLoader2<MAX, TP_MAX, Args, CompareData>::Remove(T *data)
	{
		AutoCSLock cs(m_cs);
		for (auto &kv : m_files)
		{
			if ((void*)data == (void*)kv.second.data)
			{
				if (kv.second.data)
				{
					((T*)kv.second.data)->~T(); // call destructor
					delete kv.second.data;
				}
				m_files.erase(kv.first);
				m_updatePtrs[kv.first].clear();
				//m_tpLoader.RemoveTask(fileName); // todo: remove task
				break;
			}
		}
		return true;
	}


	// remove parallel
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline bool cFileLoader2<MAX, TP_MAX, Args, CompareData>::RemoveParallel(
		T *data)
	{

		return true;
	}


	// Clear
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	template<class T>
	inline void cFileLoader2<MAX, TP_MAX, Args, CompareData>::ClearFile()
	{
		m_tpLoader.Clear();
		common::cMemoryPool4<cTaskFileLoader2<T, MAX, TP_MAX, Args, CompareData>>::Clear();

		{
			AutoCSLock cs(m_cs);
			set<hashcode> rms;
			for (auto &kv : m_files)
			{
				if (!kv.second.data)
					continue;

				if (T *p = dynamic_cast<T*>(kv.second.data))
				{
					((T*)kv.second.data)->~T(); // call destructor
					rms.insert(kv.first);
				}
			}

			for (auto &k : rms)
			{
				m_files.erase(k);
				m_updatePtrs.erase(k);
			}
		}
	}


	// Clear
	template<size_t MAX, size_t TP_MAX, class Args, class CompareData>
	inline void cFileLoader2<MAX, TP_MAX, Args, CompareData>::Clear()
	{
		m_tpLoader.Clear();
		m_tpRemover.Clear();

		{
			//todo: memory leak, must call destuctor
			AutoCSLock cs(m_cs);
			for (auto &kv : m_files)
				delete kv.second.data;
			m_files.clear();
			m_updatePtrs.clear();
		}
	}

}
