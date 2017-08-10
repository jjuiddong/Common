//
// 2017-05-03, jjuiddong
// load resource parallel
//
#pragma once


namespace graphic
{

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

		virtual eRunResult::Enum Run() 
		{
			cAssimpModel *model = NULL;
			const StrPath resourcePath = cResourceManager::Get()->GetResourceFilePath(m_fileName);
			if (resourcePath.empty())
				goto error;

			model = new cAssimpModel;
			if (!model->Create(*m_renderer, resourcePath))
				goto error;

			cResourceManager::Get()->InsertAssimpModel(m_fileName, model);
			return eRunResult::END; 


		error:
			dbg::ErrLog("Error cTaskAssimpLoader %s \n", m_fileName.c_str());
			SAFE_DELETE(model);
			return eRunResult::END;
		}

		StrPath m_fileName;
		cRenderer *m_renderer;
	};


	////-----------------------------------------------------------------------------------------------
	//class cTaskXFileLoader : public common::cTask
	//{
	//public:
	//	cTaskXFileLoader(int id, cRenderer *renderer, const StrPath &fileName)
	//		: cTask(id, "cTaskXFileLoader")
	//		, m_renderer(renderer)
	//		, m_fileName(fileName) {
	//	}
	//	virtual ~cTaskXFileLoader() {
	//	}

	//	virtual eRunResult::Enum Run()
	//	{
	//		cXFileMesh *mesh = NULL;
	//		const StrPath resourcePath = cResourceManager::Get()->GetResourceFilePath(m_fileName);
	//		if (resourcePath.empty())
	//			goto error;

	//		mesh = new cXFileMesh;
	//		if (!mesh->Create(*m_renderer, resourcePath, false, true))
	//			goto error;

	//		cResourceManager::Get()->InsertXFileModel(m_fileName, mesh);
	//		return eRunResult::END;


	//	error:
	//		dbg::ErrLog("Error cTaskXFileLoader %s \n", m_fileName.c_str());
	//		SAFE_DELETE(mesh);
	//		return eRunResult::END;
	//	}

	//	StrPath m_fileName;
	//	cRenderer *m_renderer;
	//};


	////-----------------------------------------------------------------------------------------------
	//class cTaskShadowLoader : public common::cTask
	//{
	//public:
	//	cTaskShadowLoader(int id, cRenderer *renderer
	//		, const StrPath &fileName
	//		, cColladaModel *colladaModel
	//		, cXFileMesh *xfile
	//	)
	//		: cTask(id, "cTaskShadowLoader")
	//		, m_renderer(renderer)
	//		, m_fileName(fileName)
	//		, m_colladaModel(colladaModel)
	//		, m_xfile(xfile) {
	//	}
	//	virtual ~cTaskShadowLoader() {
	//	}

	//	virtual eRunResult::Enum Run()
	//	{
	//		cShadowVolume *shadow = NULL;

	//		if (m_colladaModel)
	//		{
	//			shadow = new cShadowVolume();
	//			if (!m_colladaModel->m_meshes.empty())
	//			{
	//				cMesh2 *mesh = m_colladaModel->m_meshes[0];
	//				if (!shadow->Create(*m_renderer, mesh->m_buffers->m_vtxBuff, mesh->m_buffers->m_idxBuff))
	//				{
	//					SAFE_DELETE(shadow);
	//				}
	//			}
	//		}
	//		else if (m_xfile)
	//		{
	//			shadow = new cShadowVolume();
	//			if (!shadow->Create(*m_renderer, m_xfile->m_mesh, true))
	//			{
	//				SAFE_DELETE(shadow);
	//			}
	//		}

	//		if (shadow)
	//			cResourceManager::Get()->InsertShadow(m_fileName, shadow);

	//		return eRunResult::END;
	//	}

	//	StrPath m_fileName;
	//	cRenderer *m_renderer;
	//	cColladaModel *m_colladaModel;
	//	cXFileMesh *m_xfile;
	//};


	//-----------------------------------------------------------------------------------------------
	class cTaskTextureLoader : public common::cTask
	{
	public:
		cTaskTextureLoader(int id, cRenderer *renderer, const StrPath &fileName)
			: cTask(id, "cTaskTextureLoader")
			, m_renderer(renderer)
			, m_fileName(fileName) {
		}
		virtual ~cTaskTextureLoader() {
		}

		virtual eRunResult::Enum Run()
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
				cResourceManager::Get()->InsertTexture(m_fileName, texture);
				return eRunResult::END;
			}
			else
			{
				cResourceManager::Get()->InsertTexture(m_fileName,
					cResourceManager::Get()->LoadTexture(*m_renderer, g_defaultTexture, false));
			}


		error:
			dbg::ErrLog("Error cTaskTextureLoader %s \n", m_fileName.c_str());
			SAFE_DELETE(texture);
			return eRunResult::END;
		}

		StrPath m_fileName;
		cRenderer *m_renderer;
	};


}
