
#include "stdafx.h"
#include "vprogfile.h"

using namespace common;
using namespace common::script;


cVProgFile::cVProgFile()
{
}

cVProgFile::~cVProgFile()
{
}


bool cVProgFile::Read(const StrPath &fileName)
{
	//using namespace std;
	//ifstream ifs(fileName.c_str());
	//if (!ifs.is_open())
	//	return false;

	//cNode node(0, "");
	//sPin pin(0, "", ePinType::Flow);

	//int state = 0;
	//string line;
	//while (getline(ifs, line))
	//{
	//	common::trim(line);
	//	vector<string> toks;
	//	common::tokenizer_space(line, toks);
	//	if (toks.empty())
	//		continue;

	//	switch (state)
	//	{
	//	case 0:
	//		if (toks[0] == "node")
	//		{
	//			state = 1;
	//			node.Clear();
	//		}
	//		else if (toks[0] == "symbol")
	//		{
	//			state = 4;
	//			node.Clear();
	//		}
	//		break;

	//	case 1: // state parsing
	//		if ((toks[0] == "type") && (toks.size() >= 2))
	//		{
	//			node.m_type = eNodeType::FromString(toks[1]);
	//		}
	//		else if ((toks[0] == "id") && (toks.size() >= 2))
	//		{
	//			node.m_id = atoi(toks[1].c_str());
	//		}
	//		else if ((toks[0] == "name") && (toks.size() >= 2))
	//		{
	//			node.m_name = toks[1];
	//		}
	//		else if ((toks[0] == "varname") && (toks.size() >= 2))
	//		{
	//			node.m_varName = toks[1];
	//		}
	//		else if ((toks[0] == "rect") && (toks.size() >= 5))
	//		{
	//			const sRectf rect(
	//				(float)atof(toks[1].c_str())
	//				, (float)atof(toks[2].c_str())
	//				, (float)atof(toks[3].c_str())
	//				, (float)atof(toks[4].c_str())
	//			);
	//			node.m_size = ImVec2(rect.Width(), rect.Height());
	//			node.m_pos = ImVec2(rect.left, rect.top);
	//		}
	//		else if ((toks[0] == "color") && (toks.size() >= 5))
	//		{
	//			const Vector4 color(
	//				(float)atof(toks[1].c_str())
	//				, (float)atof(toks[2].c_str())
	//				, (float)atof(toks[3].c_str())
	//				, (float)atof(toks[4].c_str())
	//			);
	//			node.m_color = *(ImColor*)&color;
	//		}
	//		else if (toks[0] == "input")
	//		{
	//			state = 2;
	//			pin.name.clear();
	//		}
	//		else if (toks[0] == "output")
	//		{
	//			state = 3;
	//			pin.name.clear();
	//		}
	//		break;

	//	case 2: // input slot parsing
	//	case 3: // output parsing
	//		if ((toks[0] == "type") && (toks.size() >= 2))
	//		{
	//			pin.type = ePinType::FromString(toks[1]);
	//		}
	//		else if ((toks[0] == "id") && (toks.size() >= 2))
	//		{
	//			pin.id = atoi(toks[1].c_str());
	//		}
	//		else if ((toks[0] == "name") && (toks.size() >= 2))
	//		{
	//			pin.name = toks[1];
	//			if (pin.name.empty())
	//				pin.name = " ";
	//		}
	//		else if ((toks[0] == "value") && (toks.size() >= 2))
	//		{
	//			//pin.value = atoi(toks[1].c_str());
	//		}
	//		else if ((toks[0] == "links") && (toks.size() >= 2))
	//		{
	//			if (state == 3) // add link output pin(from) -> input pin(to)
	//			{
	//				for (uint i = 1; i < toks.size(); ++i)
	//				{
	//					// maybe link id duplicated
	//					const ed::PinId toPinId = atoi(toks[i].c_str());
	//					m_links.push_back(sLink(ed::LinkId(common::GenerateId())
	//						, pin.id, toPinId));
	//				}
	//			}
	//		}
	//		else if (toks[0] == "input")
	//		{
	//			if (!pin.name.empty())
	//				AddPin(state, node, pin);

	//			state = 2;
	//			pin.name.clear();
	//		}
	//		else if (toks[0] == "output")
	//		{
	//			if (!pin.name.empty())
	//				AddPin(state, node, pin);

	//			state = 3;
	//			pin.name.clear();
	//		}
	//		else if ((toks[0] == "node") || (toks[0] == "symbol"))
	//		{
	//			if (!pin.name.empty())
	//				AddPin(state, node, pin);

	//			if (!node.m_name.empty())
	//			{
	//				m_nodes.push_back(node);
	//				node.Clear();
	//			}

	//			if (toks[0] == "node")
	//				state = 1;
	//			else if (toks[0] == "symbol")
	//				state = 4;
	//		}
	//		break;

	//	case 4: // symbol
	//	{
	//		if ((toks[0] == "id") && (toks.size() >= 2))
	//		{
	//			pin.name = "@symbol@";
	//			pin.id = atoi(toks[1].c_str());
	//		}
	//		else if ((toks[0] == "value") && (toks.size() >= 2))
	//		{
	//			if (!m_symbTable.AddSymbolStr(pin, toks[1]))
	//			{
	//				// error occurred
	//				assert(!"nodefile::Read() symbol parse error!!");
	//			}
	//			pin.name.clear();
	//		}
	//	}
	//	break;
	//	}
	//}

	//if (!pin.name.empty() && (pin.name != "@symbol@"))
	//	AddPin(state, node, pin);

	//if (!node.m_name.empty())
	//{
	//	m_nodes.push_back(node);
	//	node.Clear();
	//}

	return true;
}


bool cVProgFile::Write(const StrPath &fileName)
{

	return true;
}

