
#include "stdafx.h"
#include "terrainloader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace graphic;
using boost::property_tree::ptree;


cTerrainLoader::cTerrainLoader(cTerrain2 *terrain)
	: m_terrain(terrain)
{
}

cTerrainLoader::~cTerrainLoader()
{
}


//
// version
// tile row, col count
// tile
//		- pos
//		- width, height
//		- texture
// model
//		- filename
//		- name
//		- pos
//		- scale
//		- rot
//
bool cTerrainLoader::Write(const StrPath &fileName)
{
	RETV(!m_terrain, false);
	RETV(fileName.empty(), false);

	try
	{
		ptree props;
		props.add("version", "1.01");
		props.add("rows", m_terrain->m_rows);
		props.add("cols", m_terrain->m_cols);
	
		// Write Tile
		ptree tiles;
		for (auto &tile : m_terrain->m_tiles)
		{
			ptree tree;

			tree.put("id", tile->m_id);
			tree.put("row", tile->m_location.x);
			tree.put("col", tile->m_location.y);
			const Vector3 pos = tile->m_transform.pos;
			tree.put("pos", format<64>("%f %f %f", pos.x, pos.y, pos.z).c_str());

			const float width = abs(tile->m_boundingBox.m_min.x - tile->m_boundingBox.m_max.x);
			const float height = abs(tile->m_boundingBox.m_min.z - tile->m_boundingBox.m_max.z);
			tree.put("width", width);
			tree.put("height", height);

			tree.put("filename", tile->m_ground.m_tex->m_fileName.c_str());

			tiles.push_back(std::make_pair("", tree));
		}
		props.add_child("tiles", tiles);

		// Write Model
		ptree models;
		for (auto &tile: m_terrain->m_tiles)
		{
			for (auto &p : tile->m_children)
			{
				ptree tree;

				tree.put("name", p->m_name.c_str());

				const Vector3 worldPos = p->GetWorldMatrix().GetPosition();
				tree.put("pos", format<64>("%f %f %f"
					, worldPos.x, worldPos.y, worldPos.z).c_str());
				tree.put("scale", format<64>("%f %f %f"
					, p->m_transform.scale.x, p->m_transform.scale.y, p->m_transform.scale.z).c_str());
				tree.put("rot", format<64>("%f %f %f %f"
					, p->m_transform.rot.x, p->m_transform.rot.y, p->m_transform.rot.z, p->m_transform.rot.w).c_str());
				tree.put("boundingbox", format<128>("%f %f %f %f %f %f"
					, p->m_boundingBox.m_min.x, p->m_boundingBox.m_min.y, p->m_boundingBox.m_min.z
					, p->m_boundingBox.m_max.x, p->m_boundingBox.m_max.y, p->m_boundingBox.m_max.z).c_str());

				if (eNodeType::MODEL == p->m_type)
				{
					if (cModel2 *model = dynamic_cast<cModel2*>(p))
						tree.put("filename", model->m_fileName.c_str());
				}

				models.push_back(std::make_pair("", tree));
			}
		}
		props.add_child("models", models);

		// write backup (copy file)
		if (fileName.IsFileExist())
			::CopyFileA(fileName.c_str(), (fileName + ".bak").c_str(), FALSE);

		boost::property_tree::write_json(fileName.c_str(), props);
	}
	catch (std::exception &e)
	{
		MessageBoxA(NULL, e.what(), "ERROR", MB_OK);
		return false;
	}

	return true;
}


bool cTerrainLoader::Read(cRenderer &renderer, const StrPath &fileName)
{
	RETV(!m_terrain, false);

	try
	{
		ptree props;
		boost::property_tree::read_json(fileName.c_str(), props);

		const string version = props.get<string>("version");

		m_terrain->Clear();
		//m_terrain->Create(renderer, sRectf(0, 0, col*m_tileSize, row*m_tileSize));

		m_terrain->m_rows = props.get<int>("rows");
		m_terrain->m_cols = props.get<int>("cols");

		ptree::assoc_iterator itor = props.find("tiles");
		if (props.not_found() != itor)
		{
			ptree &child_field = props.get_child("tiles");
			for (ptree::value_type &vt : child_field)
			{
				const int id = vt.second.get<int>("id");
				Vector2i location;
				location.x = vt.second.get<int>("row");
				location.y = vt.second.get<int>("col");

				std::stringstream ss(vt.second.get<string>("pos"));
				Vector3 pos;
				ss >> pos.x >> pos.y >> pos.z;
				const float width = vt.second.get<float>("width");
				const float height = vt.second.get<float>("height");
				const StrPath fileName = vt.second.get<string>("filename");

				const float tileSize = width;
				cTile *tile = new cTile();
				const float x = pos.x - tileSize / 2;
				const float z = pos.z - tileSize / 2;

				const sRectf rect(x, z, x + tileSize, z + tileSize);

				tile->Create(renderer 
					, common::GenerateId()
					, fileName.GetFileNameExceptExt().c_str()
					, rect
					, -0.1f
					, 1
					, Vector2(0, 0), Vector2(1, 1)
				);

				tile->m_location = location;
				tile->m_ground.m_tex = cResourceManager::Get()->LoadTextureParallel(renderer, fileName);
				if (!tile->m_ground.m_tex)
					cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
						, fileName, (void**)&tile->m_ground.m_tex));

				tile->m_dbgTile.SetColor((DWORD)cColor(1.f, 0, 0, 1));
				m_terrain->AddTile(tile);
			}
		}


		ptree::assoc_iterator mitor = props.find("models");
		if (props.not_found() != mitor)
		{
			ptree &child_field = props.get_child("models");
			for (ptree::value_type &vt : child_field)
			{
				Transform transform;
				cBoundingBox bbox;
				{
					std::stringstream ss(vt.second.get<string>("pos"));
					ss >> transform.pos.x >> transform.pos.y >> transform.pos.z;
				}
				{
					std::stringstream ss(vt.second.get<string>("scale"));
					ss >> transform.scale.x >> transform.scale.y >> transform.scale.z;
				}
				{
					std::stringstream ss(vt.second.get<string>("rot"));
					ss >> transform.rot.x >> transform.rot.y >> transform.rot.z >> transform.rot.w;
				}

				{
					std::stringstream ss(vt.second.get<string>("boundingbox"));
					Vector3 _min, _max;
					ss >> _min.x >> _min.y >> _min.z >> _max.x >> _max.y >> _max.z;					
					bbox.SetBoundingBox(_min, _max);
				}

				StrPath fileName = vt.second.get<string>("filename");
				cModel2 *model = new cModel2();
				model->Create(renderer, common::GenerateId()
					, fileName, "../Media/shader/xfile.fx", "Scene_NoShadow", true);
				model->m_name = vt.second.get<string>("name");
				model->m_boundingBox = bbox;

				// insert model to tile
				bool isInsertSuccess = false;
				for (auto &tile : m_terrain->m_tiles)
				{
					cBoundingBox bbox = model->m_boundingBox;
					bbox.m_tm = transform.GetMatrix();

					if (tile->m_boundingBox.Collision(bbox, tile->GetWorldMatrix()))
					{
						model->m_transform = transform;
						model->m_transform.pos -= tile->m_transform.pos;
						tile->AddChild(model);

						tile->m_shaders.insert(
							cResourceManager::Get()->LoadShader(renderer, "../Media/shader/xfile.fx"));

						isInsertSuccess = true;
						break;
					}
				}

				assert(isInsertSuccess);
			}

			m_terrain->m_shaders.insert(
				cResourceManager::Get()->LoadShader(renderer, "../Media/shader/xfile.fx"));
		}

	}
	catch (std::exception &e)
	{
		MessageBoxA(NULL, e.what(), "ERROR", MB_OK);
		return false;
	}

	return true;
}

