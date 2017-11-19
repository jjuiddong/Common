//
// 2017-07-10, jjuiddong
// json format 
//
// 2017-08-21
//	- DX11 Upgrade
//
// 2017-11-08
//	- Add Overriding Method
//		- WriteNode, CreateNode
//
#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


namespace graphic
{

	class cTerrainLoader
	{
	public:
		cTerrainLoader(cTerrain *terrain);
		virtual ~cTerrainLoader();

		bool Write(const StrPath &fileName);
		bool Read(cRenderer &renderer, const StrPath &fileName);


	protected:
		bool WriteHeightmap(const char *fileName);
		bool ReadHeightmap(const char *fileName);
		bool WriteModel(cNode *model, INOUT boost::property_tree::ptree &tree);
		Transform ParseTransform(const boost::property_tree::ptree &tree);
		cBoundingBox ParseBoundingBox(const boost::property_tree::ptree &tree
			, const Transform &transform);

		virtual bool WriteNode(cNode *node, INOUT boost::property_tree::ptree &tree) { return false; }
		virtual cNode* CreateNode(cRenderer &renderer, const boost::property_tree::ptree &tree);


	public:
		cTerrain *m_terrain; // reference
	};
}

