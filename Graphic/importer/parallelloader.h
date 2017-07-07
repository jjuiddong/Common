//
// 2017-05-08, jjuiddong
// parallel loader
//
#pragma once


namespace graphic
{

	class cParallelLoader
	{
	public:
		struct eType {
			enum Enum {
				NONE
				, COLLADA
				, XFILE
				, SHADOW_VOLUME
				, TEXTURE				
			};
		};

		cParallelLoader() 
			: m_type(eType::NONE)
			, m_ptr(NULL)
			, m_incTime(0)
		{
		}

		cParallelLoader(const eType::Enum type, const StrPath &key, void **ptr)
			: m_type(type)
			, m_key(key)
			, m_ptr(ptr) // this memory pointer is only use heap pointer, not stack pointer
			, m_incTime(0)
		{
		}

		virtual ~cParallelLoader() {
		}

		bool Update(const float deltaSeconds) {
			m_incTime += deltaSeconds;
			if (m_incTime < 0.5f)
				return false;
			m_incTime = 0;

			DWORD ptr = 0;
			switch (m_type)
			{
			case eType::COLLADA:
			{
				auto result = cResourceManager::Get()->FindColladaModel(m_key);
				if (result.second)
					ptr = (DWORD)result.second;
			}
			break;

			case eType::XFILE:
				break;

			case eType::SHADOW_VOLUME:
				break;

			case eType::TEXTURE:
			{
				auto result = cResourceManager::Get()->FindTexture(m_key);
				if (result.second)
					ptr = (DWORD)result.second;
			}
			break;

			default: assert(0);
			}

			if (ptr)
			{
				*(DWORD*)m_ptr = ptr;
				m_type = eType::NONE;
				return true;
			}

			return false;
		}


	public:
		eType::Enum m_type;
		StrPath m_key;
		void **m_ptr;
		float m_incTime;
	};

}
