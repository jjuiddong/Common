
#include "stdafx.h"
#include "terrainloader.h"

using namespace graphic;
using boost::property_tree::ptree;


cTerrainLoader::cTerrainLoader(cTerrain *terrain)
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
			const float width = dim.x;
			const float height = dim.z;
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
				WriteModel(p, tree);
				models.push_back(std::make_pair("", tree));
			}
		}

		// Write Etc
		for (auto &node : m_terrain->m_children)
		{
			// Ignore cTile Node
			if (eNodeType::TERRAIN == node->m_type)
				continue;

			ptree tree;
			WriteModel(node, tree);
			models.push_back(std::make_pair("", tree));
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

		// 대부분의 리소스가 로딩되기 전까지, 로딩 쓰레드를 종료시키지 않기 위한, 지연 태스크다.
		cResourceManager::Get()->AddTask(new graphic::cTaskDelay(5));

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
				const StrPath fileNameUTF8 = vt.second.get<string>("filename", ""); // UTF-8
				const StrPath fileName = fileNameUTF8.ansi();

				const float tileSizeW = width;
				const float tileSizeH = height;
				const float tileCellSizeW = width / 16.f;
				const float tileCellSizeH = height / 16.f;
				cTile *tile = new cTile();
				//const float x = pos.x - tileSizeW / 2;
				//const float z = pos.z - tileSizeH / 2;
				const float x = pos.x;
				const float z = pos.z;

				//const sRectf rect(x, z, x + tileSizeW, z + tileSizeH);
				Transform transform;
				//transform.pos = Vector3(x + tileSizeW/2.f, 0, z + tileSizeH/2.f);
				transform.pos = Vector3(x, 0, z);
				transform.scale = Vector3(tileSizeW / 2.f, tileSizeH / 2.f, tileSizeH / 2.f);

				if (!terrainCr)
				{
					terrainCr = true;
					m_terrain->Create(renderer, row, col, tileCellSizeW, tileCellSizeH, tilerow, tilecol);
				}

				tile->Create(renderer
					, common::GenerateId()
					, fileName.GetFileNameExceptExt().c_str()
					, location.x // row
					, location.y // col
					, transform
					, fileName.c_str()
				);

				//tile->m_dbgTile.SetColor(cColor::RED);
				m_terrain->AddTile(tile);
			}
		}

		// 높이맵 로딩
		if (!ReadHeightmap((fileName + ".height").c_str()))
			return false;

		m_terrain->HeightmapNormalize();
		m_terrain->UpdateHeightmapToTile(renderer, &m_terrain->m_tiles[0], m_terrain->m_tiles.size());


		// 모델 로딩
		ptree::assoc_iterator mitor = props.find("models");
		if (props.not_found() != mitor)
		{
			ptree &child_field = props.get_child("models");
			for (ptree::value_type &vt : child_field)
			{
				cNode *model = CreateNode(renderer, vt.second);

				// insert model to most nearest tile
				if (model)
				{
					model->m_name = vt.second.get<string>("name");

					if (eNodeType::VIRTUAL == model->m_type)
						m_terrain->AddChild(model);
					else
						m_terrain->AddModel(model);
				}
			}
		}

	}
	catch (std::exception &e)
	{
		MessageBoxA(NULL, e.what(), "ERROR", MB_OK);
		return false;
	}

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


// 기본적으로 cModel 클래스를 생성하고, 초기화해서 리턴한다.
// 특수한 모델일 경우, 클래스를 상속받아서 메소드를 오버라이딩 한다.
cNode* cTerrainLoader::CreateNode(cRenderer &renderer, const ptree &tree)
{ 
	string typeStr = tree.get<string>("type", "None");
	const eSubType::Enum type = eSubType::FromString(common::upperCase(typeStr).c_str());

	if (type == eSubType::AREA)
	{
		Vector3 pos[4];
		std::stringstream ss1(tree.get<string>("left-top"));
		ss1 >> pos[0].x >> pos[0].y >> pos[0].z;
		std::stringstream ss2(tree.get<string>("right-top"));
		ss2 >> pos[1].x >> pos[1].y >> pos[1].z;
		std::stringstream ss3(tree.get<string>("right-bottom"));
		ss3 >> pos[2].x >> pos[2].y >> pos[2].z;
		std::stringstream ss4(tree.get<string>("left-bottom"));
		ss4 >> pos[3].x >> pos[3].y >> pos[3].z;

		cRect3D *rect3D = new cRect3D();
		rect3D->Create(renderer);
		m_terrain->GetRect3D(renderer, pos[0], pos[2], *rect3D);
		rect3D->m_rectId = tree.get<int>("rect id");
		return rect3D;
	}
	else if (type == eSubType::CUBE)
	{
		Vector4 color;
		std::stringstream ss1(tree.get<string>("color", "1 1 1 1"));
		ss1 >> color.x >> color.y >> color.z >> color.w;

		cCube *cube = new cCube();
		const Transform transform = ParseTransform(tree);
		cBoundingBox bbox;
		//bbox.SetBoundingBox(transform);
		cube->Create(renderer, bbox);
		cube->m_transform = transform;
		cube->m_color.SetColor(color);

		return cube;
	}
	else
	{
		// Load Model File
		StrPath fileName = tree.get<string>("filename", "");
		assert(!fileName.empty());
		cModel *model = new cModel();

		// 모델이 병렬로 로딩되기 때문에, transform, boundingBox는 먼저 로딩한다.
		model->m_transform = ParseTransform(tree);
		model->m_boundingBox = ParseBoundingBox(tree, Transform());
		
		// 모델 병렬 로딩
		model->Create(renderer, common::GenerateId(), fileName.ansi(), true);

		model->m_animationSpeed = tree.get<float>("animation speed", 1.f);
		return model;
	}

	return NULL;
}


bool cTerrainLoader::WriteModel(cNode *p, INOUT boost::property_tree::ptree &tree)
{
	tree.put("name", p->m_name.c_str());
	tree.put("type", eSubType::ToString(p->m_subType));

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

	if (cRect3D *rect3D = dynamic_cast<cRect3D*>(p))
	{
		tree.put("rect id", rect3D->m_rectId);

		tree.put("left-top", format<64>("%f %f %f"
			, rect3D->m_pos[0].x, rect3D->m_pos[0].y, rect3D->m_pos[0].z).c_str());
		tree.put("right-top", format<64>("%f %f %f"
			, rect3D->m_pos[1].x, rect3D->m_pos[1].y, rect3D->m_pos[1].z).c_str());
		tree.put("right-bottom", format<64>("%f %f %f"
			, rect3D->m_pos[2].x, rect3D->m_pos[2].y, rect3D->m_pos[2].z).c_str());
		tree.put("left-bottom", format<64>("%f %f %f"
			, rect3D->m_pos[3].x, rect3D->m_pos[3].y, rect3D->m_pos[3].z).c_str());
	}
	else if (cCube *cube = dynamic_cast<cCube*>(p))
	{
		Vector4 color = cube->m_color.GetColor();
		tree.put("color", format<64>("%f %f %f %f"
			, color.x, color.y, color.z, color.w).c_str());
	}

	WriteNode(p, tree); // Call Customizing Method

	return true;
}


Transform cTerrainLoader::ParseTransform(const boost::property_tree::ptree &tree)
{
	Transform transform;

	std::stringstream ss1(tree.get<string>("pos"));
	ss1 >> transform.pos.x >> transform.pos.y >> transform.pos.z;

	std::stringstream ss2(tree.get<string>("scale"));
	ss2 >> transform.scale.x >> transform.scale.y >> transform.scale.z;

	std::stringstream ss3(tree.get<string>("rot"));
	ss3 >> transform.rot.x >> transform.rot.y >> transform.rot.z >> transform.rot.w;

	return transform;
}


cBoundingBox cTerrainLoader::ParseBoundingBox(const boost::property_tree::ptree &tree
	, const Transform &transform )
{
	cBoundingBox bbox; // world space

	std::stringstream ss(tree.get<string>("boundingbox"));

	Vector3 _min, _max;
	ss >> _min.x >> _min.y >> _min.z >> _max.x >> _max.y >> _max.z;

	bbox.SetBoundingBox(transform.pos
		, Vector3(abs(_max.x - _min.x)*0.5f*transform.scale.x
				, abs(_max.y - _min.y)*0.5f*transform.scale.y
				, abs(_max.z - _min.z)*0.5f*transform.scale.z)
		, transform.rot);

	return bbox;
}
