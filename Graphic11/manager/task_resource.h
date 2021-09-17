//
// 2017-05-03, jjuiddong
// load resource parallel
//
#pragma once

#include <mmsystem.h>

namespace graphic
{
	using namespace common;

	//-----------------------------------------------------------------------------------------------
	class cTaskAssimpLoader : public common::cTask
	{
	public:
		cTaskAssimpLoader(int id, cRenderer *renderer, const StrPath &fileName)
			: cTask(id, "cTaskAssimpLoader")
			, m_renderer(renderer)
			, m_fileName(fileName) {
		}
		virtual ~cTaskAssimpLoader() {
		}

		virtual eRunResult Run(const double deltaSeconds)
		{
			cAssimpModel *model = NULL;
			const StrPath resourcePath = cResourceManager::Get()->GetResourceFilePath(m_fileName);
			if (resourcePath.empty())
				goto error;

			model = new cAssimpModel;
			if (!model->Create(*m_renderer, resourcePath))
				goto error;

			cResourceManager::Get()->InsertAssimpModel(m_fileName, model);
			return eRunResult::End;


		error:
			dbg::ErrLog("Error cTaskAssimpLoader %s \n", m_fileName.c_str());
			SAFE_DELETE(model);
			return eRunResult::End;
		}

		StrPath m_fileName;
		cRenderer *m_renderer;
	};



	//-----------------------------------------------------------------------------------------------
	class cTaskTextureLoader : public common::cTask
	{
	public:
		cTaskTextureLoader(int id, cRenderer *renderer, const StrPath &key, const StrPath &fileName)
			: cTask(id, "cTaskTextureLoader")
			, m_renderer(renderer)
			, m_key(key)
			, m_fileName(fileName) {
		}
		virtual ~cTaskTextureLoader() {
		}

		virtual eRunResult Run(const double deltaSeconds)
		{
			cTexture *texture = NULL;
			const StrPath resourcePath = cResourceManager::Get()->GetResourceFilePath(m_fileName);
			if (resourcePath.empty())
				goto error;

			texture = new cTexture();
			if (!texture->Create(*m_renderer, resourcePath))
			{
				if (resourcePath == g_defaultTexture) // this file must loaded
				{
					assert(0);
					goto error;
				}
			}

			if (texture && texture->IsLoaded())
			{
				//dbg::Log("Success Load Texture %s\n", m_key.c_str());
				cResourceManager::Get()->InsertTexture(m_key, texture);
				return eRunResult::End;
			}
			else
			{
				cResourceManager::Get()->InsertTexture(m_key,
					cResourceManager::Get()->LoadTexture(*m_renderer, g_defaultTexture, false));
			}


		error:
			dbg::ErrLog("Error cTaskTextureLoader %s \n", m_fileName.c_str());
			SAFE_DELETE(texture);
			return eRunResult::End;
		}

		StrPath m_key;
		StrPath m_fileName;
		cRenderer *m_renderer;
	};


	// 일정한 시간 동안 쓰레드가 종료되지 않게 하기위한 더미 태스크다.
	class cTaskDelay : public common::cTask
	{
	public:
		cTaskDelay(const float delaySeconds)
			: cTask(common::GenerateId(), "cTaskDelay")
			, m_delaySeconds(delaySeconds)
			, m_oldTime(0) {
		}
		~cTaskDelay() {
		}

		virtual eRunResult Run(const double deltaSeconds) override
		{
			const int curT = timeGetTime();
			if (m_oldTime == 0)
				m_oldTime = curT;
			const int elapseT = curT - m_oldTime;
			if (elapseT < 30)
				return eRunResult::Continue;
			m_oldTime = curT;
			m_delaySeconds -= (elapseT * 0.001f);
			return (m_delaySeconds < 0) ? eRunResult::End : eRunResult::Continue;
		}

		float m_delaySeconds;
		int m_oldTime;
	};

}
