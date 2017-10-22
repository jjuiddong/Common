
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
		props.add("tilerows", m_terrain->m_tileRows);
		props.add("tilecols", m_terrain->m_tileCols);

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

			const Vector3 dim = tile->m_boundingBox.GetDimension();
			const float width = dim.x;// abs(tile->m_boundingBox.m_min.x - tile->m_boundingBox.m_max.x);
			const float height = dim.z;// abs(tile->m_boundingBox.m_min.z - tile->m_boundingBox.m_max.z);
			tree.put("width", width);
			tree.put("height", height);
			tree.put("filename", tile->m_ground->m_texture->m_fileName.utf8().c_str()); // Save UTF-8

			tiles.push_back(std::make_pair("", tree));
		}
		props.add_child("tiles", tiles);

		// Write Model
		ptree models;
		for (auto &tile: m_terrain->m_tiles)
		{
			for (auto &p : tile->m_children)
			{
				if (p->m_name == "grid")
					continue;

				ptree tree;

				tree.put("name", p->m_name.c_str());

				const Vector3 worldPos = p->GetWorldMatrix().GetPosition();
				tree.put("pos", format<64>("%f %f %f"
					, worldPos.x, worldPos.y, worldPos.z).c_str());
				tree.put("scale", format<64>("%f %f %f"
					, p->m_transform.scale.x, p->m_transform.scale.y, p->m_transform.scale.z).c_str());
				tree.put("rot", format<64>("%f %f %f %f"
					, p->m_transform.rot.x, p->m_transform.rot.y, p->m_transform.rot.z, p->m_transform.rot.w).c_str());

				if (cModel *mod = dynamic_cast<cModel*>(p))
					tree.put("animation speed", format<64>("%f", mod->m_animationSpeed).c_str());

				const Vector3 dim = p->m_boundingBox.GetDimension();
				const Vector3 center = *(Vector3*)&p->m_boundingBox.m_bbox.Center;
				const Vector3 _min = center - (dim / 2);
				const Vector3 _max = center + (dim / 2);
				tree.put("boundingbox", format<128>("%f %f %f %f %f %f"
					, _min.x, _min.y, _min.z
					, _max.x, _max.y, _max.z).c_str());

				if (eNodeType::MODEL == p->m_type)
				{
					if (cModel *model = dynamic_cast<cModel*>(p))
						tree.put("filename", model->m_fileName.utf8().c_str());// Save UTF-8
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

	if (!WriteHeightmap((fileName + ".height").c_str()))
		return false;

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

		const int row = props.get<int>("rows");
		const int col = props.get<int>("cols");
		const int tilerow = props.get<int>("tilerows");
		const int tilecol = props.get<int>("tilecols");
		bool terrainCr = false;

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
				const StrPath fileNameUTF8 = vt.second.get<string>("filename"); // UTF-8
				const StrPath fileName = fileNameUTF8.ansi();

				const float tileSize = width;
				cTile *tile = new cTile();
				const float x = pos.x - tileSize / 2;
				const float z = pos.z - tileSize / 2;

				const sRectf rect(x, z, x + tileSize, z + tileSize);

				if (!terrainCr)
				{
					terrainCr = true;
					m_terrain->Create(renderer, row, col, tileSize/16, tilerow, tilecol);
				}

				tile->Create(renderer 
					, common::GenerateId()
					, fileName.GetFileNameExceptExt().c_str()
					, location.x // row
					, location.y // col
					, rect
					, fileName.c_str()
				);

				//tile->m_dbgTile.SetColor(cColor::RED);
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
				float aniSpeed = 1.f;
				cBoundingBox bbox; // world space
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
					std::stringstream ss(vt.second.get<string>("animation speed", "1"));
					ss >> aniSpeed;
				}

				{
					std::stringstream ss(vt.second.get<string>("boundingbox"));
					Vector3 _min, _max;
					ss >> _min.x >> _min.y >> _min.z >> _max.x >> _max.y >> _max.z;
					bbox.SetBoundingBox(transform.pos
						, Vector3(abs(_max.x - _min.x)*transform.scale.x
								, abs(_max.y - _min.y)*transform.scale.y
								, abs(_max.z - _min.z)*transform.scale.z)
						, transform.rot);
				}

				StrPath fileName = vt.second.get<string>("filename");
				cModel *model = new cModel();
				model->Create(renderer, common::GenerateId()
					, fileName.ansi(), true);
				model->m_name = vt.second.get<string>("name");
				model->m_animationSpeed = aniSpeed;

				// insert model to most nearest tile
				{
					vector<cTile*> candidate;
					for (auto &tile : m_terrain->m_tiles)
					{
						cBoundingBox tbbox = tile->m_boundingBox * tile->GetWorldMatrix();
						if (tbbox.Collision(bbox))
						{
							candidate.push_back(tile);
							//model->m_transform = transform * tile->m_transform.Inverse();
							//tile->AddChild(model);
							//break;
						}
					}

					assert(!candidate.empty());

					float nearLen = FLT_MAX;
					cTile *nearTile = NULL;
					for (auto &tile : candidate)
					{
						const float len = tile->GetWorldMatrix().GetPosition().Distance(transform.pos);
						if (len < nearLen)
						{
							nearLen = len;
							nearTile = tile;
						}
					}

					model->m_transform = transform * nearTile->m_transform.Inverse();
					nearTile->AddChild(model);
				}
			}
		}

	}
	catch (std::exception &e)
	{
		MessageBoxA(NULL, e.what(), "ERROR", MB_OK);
		return false;
	}

	if (!ReadHeightmap((fileName + ".height").c_str()))
		return false;

	m_terrain->HeightmapNormalize();
	m_terrain->UpdateHeightmapToTile(renderer, &m_terrain->m_tiles[0], m_terrain->m_tiles.size());

	return true;
}


// Save Terrain Heightmap
bool cTerrainLoader::WriteHeightmap(const char *fileName)
{
	std::ofstream ofs(fileName, std::ios::binary);
	if (!ofs.is_open())
		return false;

	float *map = new float[m_terrain->m_colVtx * m_terrain->m_rowVtx];
	int idx = 0;
	for (auto &v : m_terrain->m_heightMap)
		map[idx++] = v.p.y;

	ofs.write((const char*)map, sizeof(float) * m_terrain->m_colVtx * m_terrain->m_rowVtx);

	delete[] map;
	return true;
}


// Load Terrain Heightmap
bool cTerrainLoader::ReadHeightmap(const char *fileName)
{
	std::ifstream ifs(fileName, std::ios::binary);
	if (!ifs.is_open())
		return false;

	float *map = new float[m_terrain->m_colVtx * m_terrain->m_rowVtx];
	ifs.read((char*)map, sizeof(float)*m_terrain->m_colVtx * m_terrain->m_rowVtx);

	int idx = 0;
	for (auto &v : m_terrain->m_heightMap)
		v.p.y = map[idx++];

	delete[] map;
	return true;
}
