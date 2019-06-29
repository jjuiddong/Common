//
// 2018-01-09, jjuiddong
// fileloader, multithread
//	- parallel load
//
// 2019-05-18
//	- thread pool loading
//
#pragma once


namespace graphic
{
	using namespace common;

	//-----------------------------------------------------------------------
	// Declare cFileLoad 
	//-----------------------------------------------------------------------
	struct sFileLoaderArg {
		int dummy;
	};

	struct sFileCompareData {
		int dummy;
	};


	template<class T
		, size_t MAX
		, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData
		, size_t TP_MAX = 1>
	class cFileLoader
	{
	public:
		enum STATE { NONE, LOADING, COMPLETE };
		struct sChunk
		{
			double accessTime;
			STATE state; // 0 = loading, 1 = load complete
			T *data;
		};


	public:
		cFileLoader();
		virtual ~cFileLoader();

		T* Load(cRenderer &renderer, const char *fileName);
		T* Load(cRenderer &renderer, const T *src, const char *fileName, const Args &args);
		std::pair<bool, T*> LoadParallel(cRenderer &renderer, const char *fileName, int *outFlag, void **outPtr
			, const bool isTopPriority = false
			, const CompareData &compData = {}
			);
		std::pair<bool, T*> LoadParallel(cRenderer &renderer, const T *src, const char *fileName
			, const Args &args, int *outFlag, void **outPtr
			, const bool isTopPriority = false
			, const CompareData &compData = {});
		std::tuple<bool, int, T*> Find(const char *fileName);
		bool Insert(const char *fileName, T *data);
		bool FailLoad(const char *fileName);
		bool Remove(const char *fileName);
		bool Remove(T *data);
		void Clear();


	public:
		map<hashcode, sChunk> m_files; //key = fileName hashcode
		map<hashcode, std::set< std::pair<void**,int*>>> m_updatePtrs; // update pointers
		CriticalSection m_cs;
		cTPSemaphore m_tpLoader;
	};


	//-----------------------------------------------------------------------
	// Declare cTaskFileLoad 
	//-----------------------------------------------------------------------
	template<class T
		, size_t MAX
		, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData
		, size_t TP_MAX = 1>
	class cTaskFileLoader : public common::cTask
						, public common::cMemoryPool4<cTaskFileLoader<T, MAX, Args, CompareData, TP_MAX>>
	{
	public:
		cTaskFileLoader(cFileLoader<T, MAX, Args, CompareData, TP_MAX> *fileLoader, cRenderer *renderer, const char *fileName
			, const bool isTopPriority = false
			, const CompareData &compData = {})
			: cTask(common::GenerateId(), "cTaskFileLoader", isTopPriority)
			, m_renderer(renderer)
			, m_fileName(fileName)
			, m_fileLoader(fileLoader)
			, m_loadType(0)
			, m_compare(compData) {
			m_name = fileName;
		}
		cTaskFileLoader(cFileLoader<T, MAX, Args, CompareData, TP_MAX> *fileLoader, cRenderer *renderer, const T *src, const char *fileName
			, const Args &args, const bool isTopPriority = false
			, const CompareData &compData = {})
			: cTask(common::GenerateId(), "cTaskFileLoader", isTopPriority)
			, m_renderer(renderer)
			, m_fileName(fileName)
			, m_fileLoader(fileLoader)
			, m_src(src)
			, m_args(args)
			, m_loadType(1)
			, m_compare(compData) {
			m_name = fileName;
		}
		virtual ~cTaskFileLoader() {
		}

		virtual eRunResult::Enum Run(const double deltaSeconds)
		{
			T *data = NULL;
			try 
			{
				if (0 == m_loadType)
				{
					data = new T(*m_renderer, m_fileName.c_str());
					if (m_fileLoader->Insert(m_fileName.c_str(), data))
					{
						//dbg::Logp("Success Parallel Load1 File %s\n", m_fileName.c_str());
					}
					else
					{
						dbg::Logp("Error Parallel Load1 Insert %s\n", m_fileName.c_str());
						goto error;
					}
				}
				else if (1 == m_loadType)
				{
					data = new T(*m_renderer, m_src, m_fileName.c_str(), m_args);
					if (m_fileLoader->Insert(m_fileName.c_str(), data))
					{
						//dbg::Logp("Success Parallel Load2 File %s\n", m_fileName.c_str());
					}
					else
					{
						dbg::Logp("Error Parallel Load2 Insert %s\n", m_fileName.c_str());
						goto error;
					}
				}

			}
			catch (...)
			{
				goto error;
			}

			return eRunResult::END;

		error:
			dbg::ErrLogp("Error cTaskFileLoader %d, %s\n", m_loadType, m_fileName.c_str());
			m_fileLoader->FailLoad(m_fileName.c_str());
			SAFE_DELETE(data);
			return eRunResult::END;
		}

		int m_loadType; // 0 or 1
		StrPath m_fileName;
		cRenderer *m_renderer; // reference
		cFileLoader<T, MAX, Args, CompareData, TP_MAX> *m_fileLoader; // reference
		const T *m_src; // reference
		Args m_args;
		CompareData m_compare;
	};

	
	//-----------------------------------------------------------------------
	// Implements cFileLoader 
	//-----------------------------------------------------------------------
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline cFileLoader<T, MAX, Args, CompareData, TP_MAX>::cFileLoader() {
	}

	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>

	inline cFileLoader<T, MAX, Args, CompareData, TP_MAX>::~cFileLoader() {
		Clear();
	}


	// Load
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline T* cFileLoader<T, MAX, Args, CompareData, TP_MAX>::Load(graphic::cRenderer &renderer, const char *fileName)
	{
		auto result = Find(fileName);
		if (std::get<0>(result))
			return std::get<2>(result);

		if (m_files.size() >= MAX)
		{
			dbg::Logp("Error cFileLoader::Load(), Over Maximum Load Count\n");
			return NULL;
		}

		T *data = NULL;
		try 
		{
			data = new T(renderer, fileName);
			if (Insert(fileName, data))
			{
				//dbg::Logp("Success Load File %s\n", fileName);
			}
			else
			{
				dbg::ErrLogp("Error cFileLoader Insertion %s \n", fileName);
				goto error;
			}
		}
		catch (...)
		{
			dbg::ErrLogp("Error cFileLoader %s \n", fileName);
			goto error;
		}

		return data;

	error:
		SAFE_DELETE(data);
		return NULL;
	}


	// Load
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline T* cFileLoader<T, MAX, Args, CompareData, TP_MAX>::Load(cRenderer &renderer, const T *src, const char *fileName, const Args &args)
	{
		auto result = Find(fileName);
		if (std::get<0>(result))
			return std::get<2>(result);

		if (m_files.size() >= MAX)
		{
			dbg::Logp("Error cFileLoader::Load(), Over Maximum Load Count\n");
			return NULL;
		}

		T *data = NULL;
		try
		{
			data = new T(renderer, src, fileName, args);
			if (Insert(fileName, data))
			{
				//dbg::Logp("Success Load File %s\n", fileName);
			}
			else
			{
				dbg::ErrLogp("Error cFileLoader Insertion %s \n", fileName);
				goto error;
			}
		}
		catch (...)
		{
			dbg::ErrLogp("Error cFileLoader %s %s\n", fileName);
			goto error;
		}
		return data;

	error:
		SAFE_DELETE(data);
		return NULL;
	}


	// LoadParallel
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	std::pair<bool, T*> cFileLoader<T, MAX, Args, CompareData, TP_MAX>::LoadParallel(
		graphic::cRenderer &renderer
		, const char *fileName
		, int *outFlag, void **outPtr
		, const bool isTopPriority //= false
		, const CompareData &compData //={}
	)
	{
		{
			AutoCSLock cs(m_cs);

			// is exist?
			StrPath path(fileName);
			auto it = m_files.find(path.GetHashCode());
			const bool isFind = (m_files.end() != it);
			const STATE state = (m_files.end() != it) ? it->second.state : NONE;
			T *ptr = it->second.data;
			if (isFind)
			{
				// 로딩 중이라면, 업데이트될 포인터를 등록한다.
				if (LOADING == state)
				{
					if (outFlag)
						*outFlag = 1;
					const StrPath path(fileName);
					m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
				}
				else if (COMPLETE == state)
				{
					if (outPtr)
						*outPtr = ptr;
					if (outFlag)
						*outFlag = 0;
				}
				else
					assert(0);

				return std::make_pair(isFind, ptr);
			}
			else // // Not Found, loading
			{				
				if (m_files.size() >= MAX)
				{
					dbg::Logp("Error cFileLoader::LoadParallel(), Over Maximum Load Count\n");

					if (outFlag)
						*outFlag = 0;
					return{ false, NULL };
				}

				// 중복로딩을 막기위해, 미리 NULL로 저장한다.
				if (outFlag)
					*outFlag = 1;
				StrPath path(fileName);
				m_files[path.GetHashCode()] = { 0, LOADING, NULL };
				m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
			}
		}

		if (!m_tpLoader.IsInit())
			m_tpLoader.Init(TP_MAX);
		m_tpLoader.PushTask(new cTaskFileLoader<T, MAX, Args, CompareData, TP_MAX>(this, &renderer, fileName, isTopPriority, compData));

		return { true, NULL };
	}


	// LoadParallel
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	std::pair<bool, T*> cFileLoader<T, MAX, Args, CompareData, TP_MAX>::LoadParallel(graphic::cRenderer &renderer, const T *src
		, const char *fileName, const Args &args, int *outFlag, void **outPtr
		, const bool isTopPriority //= false
		, const CompareData &compData //={}
	)
	{
		{
			AutoCSLock cs(m_cs);

			// is exist?
			StrPath path(fileName);
			auto it = m_files.find(path.GetHashCode());
			const bool isFind = (m_files.end() != it);
			const STATE state = (m_files.end() != it) ? it->second.state : NONE;
			T *ptr = it->second.data;
			if (isFind)
			{
				// 로딩 중이라면, 업데이트될 포인터를 등록한다.
				if (LOADING == state)
				{
					if (outFlag)
						*outFlag = 1;
					const StrPath path(fileName);
					m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
				}
				else if (COMPLETE == state)
				{
					if (outPtr)
						*outPtr = ptr;
					if (outFlag)
						*outFlag = 0;
				}
				else
					assert(0);

				return std::make_pair(true, ptr);
			}
			else // Not Found, Loading
			{
				if (m_files.size() >= MAX)
				{
					dbg::Logp("Error cFileLoader::LoadParallel(), Over Maximum Load Count\n");

					if (outFlag)
						*outFlag = 0;
					return{ false, NULL };
				}

				// 중복로딩을 막기위해, 미리 NULL로 저장한다.
				if (outFlag)
					*outFlag = 1;
				StrPath path(fileName);
				m_files[path.GetHashCode()] = { 0, LOADING, NULL };
				m_updatePtrs[path.GetHashCode()].insert({ outPtr, outFlag });
			}
		}

		if (!m_tpLoader.IsInit())
			m_tpLoader.Init(TP_MAX);
		m_tpLoader.PushTask(new cTaskFileLoader<T, MAX, Args, CompareData, TP_MAX>(this, &renderer, src, fileName, args, isTopPriority, compData));

		return{ true, NULL };
	}


	// Find
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline std::tuple<bool, int, T*> cFileLoader<T, MAX, Args, CompareData, TP_MAX>::Find(const char *fileName)
	{
		StrPath path(fileName);
		AutoCSLock cs(m_cs);
		auto it = m_files.find(path.GetHashCode());
		if (m_files.end() == it)
			return{ false, NONE, NULL }; // not exist		
		return std::make_tuple(true, it->second.state, it->second.data);
	}

	
	// Insert
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline bool cFileLoader<T, MAX, Args, CompareData, TP_MAX>::Insert(const char *fileName, T *data)
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
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline bool cFileLoader<T, MAX, Args, CompareData, TP_MAX>::FailLoad(const char *fileName)
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
					*outFlag = 0; // complete flag
			}
			updatePtrs.clear();

			m_files[path.GetHashCode()] = { 0, COMPLETE, NULL};
		}

		return true;
	}


	// Remove
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline bool cFileLoader<T, MAX, Args, CompareData, TP_MAX>::Remove(const char *fileName)
	{
		StrPath path(fileName);

		AutoCSLock cs(m_cs);
		auto it = m_files.find(path.GetHashCode());
		m_updatePtrs[path.GetHashCode()].clear();

		if (m_files.end() == it)
		{
			return false; // not exist
		}

		delete it->second.data;
		m_files.erase(it);
		m_tpLoader.RemoveTask(fileName);
		return true;
	}

	
	// Remove
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline bool cFileLoader<T, MAX, Args, CompareData, TP_MAX>::Remove(T *data)
	{
		AutoCSLock cs(m_cs);
		for (auto &kv : m_files)
		{
			if (data == kv.second.data)
			{
				delete kv.second.data;
				m_files.erase(kv.first);
				m_updatePtrs[kv.first].clear();
				//m_tpLoader.RemoveTask(fileName); // todo: remove task
				break;
			}
		}
		return true;
	}


	// Clear
	template<class T, size_t MAX, class Args = sFileLoaderArg
		, class CompareData = sFileCompareData, size_t TP_MAX = 1>
	inline void cFileLoader<T, MAX, Args, CompareData, TP_MAX>::Clear()
	{
		m_tpLoader.Clear();
		common::cMemoryPool4<cTaskFileLoader<T, MAX, Args, CompareData, TP_MAX>>::Clear();

		{
			AutoCSLock cs(m_cs);
			for (auto &kv : m_files)
				delete kv.second.data;
			m_files.clear();
			m_updatePtrs.clear();
		}

	}

}
