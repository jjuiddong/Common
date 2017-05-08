//
// 2017-05-03, jjuiddong
// load resource parallel
//
#pragma once


namespace graphic
{

	//-----------------------------------------------------------------------------------------------
	class cTaskColladaLoader : public common::cTask
	{
	public:
		cTaskColladaLoader(int id, cRenderer *renderer, const std::string &fileName)
			: cTask(id, "cTaskColladaLoader")
			, m_renderer(renderer)
			, m_fileName(fileName) {
		}
		virtual ~cTaskColladaLoader() {
		}

		virtual eRunResult::Enum Run() 
		{
			const string resourcePath = cResourceManager::Get()->GetResourceFilePath(m_fileName);
			if (resourcePath.empty())
				goto error;

			cColladaModel *model = new cColladaModel;
			if (!model->Create(*m_renderer, resourcePath))
				goto error;

			cResourceManager::Get()->InsertColladaModel(m_fileName, model);
			return eRunResult::END; 


		error:
			dbg::ErrLog("Error cTaskColladaLoader %s \n", m_fileName.c_str());
			SAFE_DELETE(model);
			return eRunResult::END;
		}

		string m_fileName;
		cRenderer *m_renderer;
	};


	//-----------------------------------------------------------------------------------------------
	class cTaskXFileLoader : public common::cTask
	{
	public:
		cTaskXFileLoader(int id, cRenderer *renderer, const std::string &fileName)
			: cTask(id, "cTaskXFileLoader")
			, m_renderer(renderer)
			, m_fileName(fileName) {
		}
		virtual ~cTaskXFileLoader() {
		}

		virtual eRunResult::Enum Run()
		{
			const string resourcePath = cResourceManager::Get()->GetResourceFilePath(m_fileName);
			if (resourcePath.empty())
				goto error;

			cXFileMesh *mesh = new cXFileMesh;
			if (!mesh->Create(*m_renderer, resourcePath, false, true))
				goto error;

			cResourceManager::Get()->InsertXFileModel(m_fileName, mesh);
			return eRunResult::END;


		error:
			dbg::ErrLog("Error cTaskXFileLoader %s \n", m_fileName.c_str());
			SAFE_DELETE(mesh);
			return eRunResult::END;
		}

		string m_fileName;
		cRenderer *m_renderer;
	};


	//-----------------------------------------------------------------------------------------------
	class cTaskShadowLoader : public common::cTask
	{
	public:
		cTaskShadowLoader(int id, cRenderer *renderer
			, const std::string &fileName
			, cColladaModel *colladaModel
			, cXFileMesh *xfile
		)
			: cTask(id, "cTaskShadowLoader")
			, m_renderer(renderer)
			, m_fileName(fileName)
			, m_colladaModel(colladaModel)
			, m_xfile(xfile) {
		}
		virtual ~cTaskShadowLoader() {
		}

		virtual eRunResult::Enum Run()
		{
			cShadowVolume *shadow = NULL;

			if (m_colladaModel)
			{
				shadow = new cShadowVolume();
				if (!m_colladaModel->m_meshes.empty())
				{
					cMesh2 *mesh = m_colladaModel->m_meshes[0];
					if (!shadow->Create(*m_renderer, mesh->m_buffers->m_vtxBuff, mesh->m_buffers->m_idxBuff))
					{
						SAFE_DELETE(shadow);
					}
				}
			}
			else if (m_xfile)
			{
				shadow = new cShadowVolume();
				if (!shadow->Create(*m_renderer, m_xfile->m_mesh, true))
				{
					SAFE_DELETE(shadow);
				}
			}

			if (shadow)
				cResourceManager::Get()->InsertShadow(m_fileName, shadow);

			return eRunResult::END;
		}

		string m_fileName;
		cRenderer *m_renderer;
		cColladaModel *m_colladaModel;
		cXFileMesh *m_xfile;
	};


	//-----------------------------------------------------------------------------------------------
	class cTaskTextureLoader : public common::cTask
	{
	public:
		cTaskTextureLoader(int id, cRenderer *renderer, const std::string &fileName, const bool isSizePow2)
			: cTask(id, "cTaskTextureLoader")
			, m_renderer(renderer)
			, m_isSizePow2(isSizePow2)
			, m_fileName(fileName) {
		}
		virtual ~cTaskTextureLoader() {
		}

		virtual eRunResult::Enum Run()
		{
			const string resourcePath = cResourceManager::Get()->GetResourceFilePath(m_fileName);
			if (resourcePath.empty())
				goto error;

			cTexture *texture = new cTexture();
			if (!texture->Create(*m_renderer, resourcePath, m_isSizePow2))
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
					cResourceManager::Get()->LoadTexture(*m_renderer, g_defaultTexture, m_isSizePow2, false));
			}


		error:
			dbg::ErrLog("Error cTaskTextureLoader %s \n", m_fileName.c_str());
			SAFE_DELETE(texture);
			return eRunResult::END;
		}

		string m_fileName;
		bool m_isSizePow2;
		cRenderer *m_renderer;
	};


}
