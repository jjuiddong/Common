
#include "stdafx.h"
#include "vpljsonfile.h"
#define BOOST_BIND_GLOBAL_PLACEHOLDERS // ignore warning boost bind placehoder
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace vpl;
using namespace common;


cVplJsonFile::cVplJsonFile()
{
}

cVplJsonFile::~cVplJsonFile()
{
	Clear();
}


// read json file
bool cVplJsonFile::ReadJson(const StrPath &fileName)
{
	assert(0); // not implements
	return true;
}


// read json string data
bool cVplJsonFile::ReadJsonRaw(const string &rawStr)
{
	using namespace common::script;

	Clear();

	try
	{
		using boost::property_tree::ptree;
		
		ptree props;
		boost::property_tree::read_json(stringstream(rawStr), props);

		const string ver = props.get<string>("version");
		const string title = props.get<string>("title");
		Vector2 offset;
		offset.x = props.get<float>("offsetX", 0.f);
		offset.y = props.get<float>("offsetY", 0.f);
		const float scale = props.get<float>("scale", 0.f);

		ptree::assoc_iterator nitor = props.find("nodes");
		if (props.not_found() != nitor)
		{
			ptree &children = props.get_child("nodes");
			for (ptree::value_type &vt : children)
			{
				sNode node;
				node.id = vt.second.get<int>("id", -1);
				node.name = vt.second.get<string>("name", "");
				const int type = vt.second.get<int>("type", 0);
				node.type = (eNodeType)type;

				ptree::assoc_iterator iitor = vt.second.find("inputs");
				if (vt.second.not_found() != iitor)
				{
					ptree &inputs = vt.second.get_child("inputs");
					for (ptree::value_type &vt : inputs)
					{
						sPin pin;
						pin.id = vt.second.get<int>("id", -1);
						pin.name = vt.second.get<string>("name", "");
						const int kind = vt.second.get<int>("kind", 0);//input/output
						const int type = vt.second.get<int>("type", 0);//flow/bool/int~
						const string typeStr = vt.second.get<string>("typeStr", "");//flow/bool/int~
						pin.kind = ePinKind::Input;
						pin.type = (ePinType)type;
						pin.typeStr = typeStr;
						ParseTypeString(typeStr, pin.typeValues);
						node.inputs.push_back(pin);
					}
				}

				ptree::assoc_iterator oitor = vt.second.find("outputs");
				if (vt.second.not_found() != oitor)
				{
					ptree &outputs = vt.second.get_child("outputs");
					for (ptree::value_type &vt : outputs)
					{
						sPin pin;
						pin.id = vt.second.get<int>("id", -1);
						pin.name = vt.second.get<string>("name", "");
						const int kind = vt.second.get<int>("kind", 0);//input/output
						const int type = vt.second.get<int>("type", 0);//flow/bool/int~
						const string typeStr = vt.second.get<string>("typeStr", "");//flow/bool/int~
						pin.kind = ePinKind::Output;
						pin.type = (ePinType)type;
						pin.typeStr = typeStr;
						ParseTypeString(typeStr, pin.typeValues);
						node.outputs.push_back(pin);
					}
				}
				m_nodes.push_back(node);
			}//~nodes
		}//~nodes


		ptree::assoc_iterator litor = props.find("links");
		if (props.not_found() != litor)
		{
			ptree &children = props.get_child("links");
			for (ptree::value_type &vt : children)
			{
				sLink link;
				link.from = vt.second.get<int>("from", -1);
				link.to = vt.second.get<int>("to", -1);
				m_links.push_back(link);
			}

			// link proc
			for (auto &link : m_links)
			{
				sNode *node = nullptr;
				sPin *pin = nullptr;

				std::tie(node, pin) = FindContainPin(link.from);
				if (node && pin)
					pin->links.push_back(link.to);

				std::tie(node, pin) = FindContainPin(link.to);
				if (node && pin)
					pin->links.push_back(link.from);
			}
		}//links
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	return true;
}


// write json file
bool cVplJsonFile::WriteJson(const StrPath &fileName)
{
	assert(0); // not implements
	return true;
}


void cVplJsonFile::Clear()
{
	__super::Clear();
}
