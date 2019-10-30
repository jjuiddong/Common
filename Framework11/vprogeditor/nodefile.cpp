
#include "stdafx.h"
#include "nodefile.h"

using namespace framework;
using namespace framework::vprog;


cNodeFile::cNodeFile()
{
}

cNodeFile::~cNodeFile()
{
}


bool cNodeFile::Read(const StrPath &fileName)
{
	using namespace std;
	ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		return false;

	cNode node(0,"");
	sPin pin(0, "", ePinType::Flow);

	int state = 0;
	string line;
	while (getline(ifs, line))
	{
		common::trim(line);
		vector<string> toks;
		common::tokenizer_space(line, toks);
		if (toks.empty())
			continue;

		switch (state)
		{
		case 0:
			if (toks[0] == "node")
			{
				state = 1;
				node.Clear();
			}
			else if (toks[0] == "symbol")
			{
				state = 4;
				node.Clear();
			}
			break;

		case 1: // state parsing
			if ((toks[0] == "type") && (toks.size() >= 2))
			{
				node.m_type = eNodeType::FromString(toks[1]);
			}
			else if ((toks[0] == "id") && (toks.size() >= 2))
			{
				node.m_id = atoi(toks[1].c_str());
			}
			else if ((toks[0] == "name") && (toks.size() >= 2))
			{
				node.m_name = toks[1];
			}
			else if ((toks[0] == "varname") && (toks.size() >= 2))
			{
				node.m_varName = toks[1];
			}
			else if ((toks[0] == "rect") && (toks.size() >= 5))
			{
				const sRectf rect(
					(float)atof(toks[1].c_str())
					, (float)atof(toks[2].c_str())
					, (float)atof(toks[3].c_str())
					, (float)atof(toks[4].c_str())
				);
				node.m_size = ImVec2(rect.Width(), rect.Height());
				node.m_pos = ImVec2(rect.left, rect.top);
			}
			else if ((toks[0] == "color") && (toks.size() >= 5))
			{
				const Vector4 color(
					(float)atof(toks[1].c_str())
					, (float)atof(toks[2].c_str())
					, (float)atof(toks[3].c_str())
					, (float)atof(toks[4].c_str())
				);
				node.m_color = *(ImColor*)&color;
			}
			else if (toks[0] == "input")
			{
				state = 2;
				pin.name.clear();
			}
			else if (toks[0] == "output")
			{
				state = 3;
				pin.name.clear();
			}
			break;

		case 2: // input slot parsing
		case 3: // output parsing
			if ((toks[0] == "type") && (toks.size() >= 2))
			{
				pin.type = ePinType::FromString(toks[1]);
			}
			else if ((toks[0] == "id") && (toks.size() >= 2))
			{
				pin.id = atoi(toks[1].c_str());
			}
			else if ((toks[0] == "name") && (toks.size() >= 2))
			{
				pin.name = toks[1];
				if (pin.name.empty())
					pin.name = " ";
			}
			else if ((toks[0] == "value") && (toks.size() >= 2))
			{
				//pin.value = atoi(toks[1].c_str());
			}
			else if ((toks[0] == "links") && (toks.size() >= 2))
			{
				if (state == 3) // add link output pin(from) -> input pin(to)
				{
					for (uint i = 1; i < toks.size(); ++i)
					{
						// maybe link id duplicated
						const ed::PinId toPinId = atoi(toks[i].c_str());
						m_links.push_back(sLink(ed::LinkId(common::GenerateId())
							, pin.id, toPinId));
					}
				}
			}
			else if (toks[0] == "input")
			{
				if (!pin.name.empty())
					AddPin(state, node, pin);

				state = 2;
				pin.name.clear();
			}
			else if (toks[0] == "output")
			{
				if (!pin.name.empty())
					AddPin(state, node, pin);

				state = 3;
				pin.name.clear();
			}
			else if ((toks[0] == "node") || (toks[0] == "symbol"))
			{
				if (!pin.name.empty())
					AddPin(state, node, pin);

				if (!node.m_name.empty())
				{
					m_nodes.push_back(node);
					node.Clear();
				}

				if (toks[0] == "node")
					state = 1;
				else if (toks[0] == "symbol")
					state = 4;
			}
			break;

		case 4: // symbol
		{
			if ((toks[0] == "id") && (toks.size() >= 2))
			{
				pin.name = "@symbol@";
				pin.id = atoi(toks[1].c_str());
			}
			else if ((toks[0] == "value") && (toks.size() >= 2))
			{
				if (!m_symbTable.AddSymbolStr(pin, toks[1]))
				{
					// error occurred
					assert(!"nodefile::Read() symbol parse error!!");
				}
				pin.name.clear();
			}
		}
		break;
		}
	}

	if (!pin.name.empty() && (pin.name != "@symbol@"))
		AddPin(state, node, pin);

	if (!node.m_name.empty())
	{
		m_nodes.push_back(node);
		node.Clear();
	}

	return true;
}


bool cNodeFile::Write(const StrPath &fileName)
{
	using namespace std;
	ofstream ofs(fileName.c_str());
	if (!ofs.is_open())
		return false;

	for (cNode &node : m_nodes)
	{
		ofs << "node" << endl;
		ofs << "\t" << "type " << eNodeType::ToString(node.m_type) << endl;
		ofs << "\t" << "id " << node.m_id.Get() << endl;
		if (node.m_name.empty())
			ofs << "\t" << "name \" \"" << endl; // blank name
		else
			ofs << "\t" << "name \"" << node.m_name.c_str() << "\"" << endl;
		if (eNodeType::Variable == node.m_type)
			ofs << "\t" << "varname \"" << node.m_varName.c_str() << "\"" << endl;
		ofs << "\t" << "rect " << node.m_pos.x << " "
			<< node.m_pos.y << " "
			<< node.m_pos.x + node.m_size.x << " "
			<< node.m_pos.y + node.m_size.y << endl;

		const Vector4 color = *(Vector4*)&node.m_color.Value;
		ofs << "\t" << "color " << color.x << " "
			<< color.y << " "
			<< color.z << " "
			<< color.w << endl;

		for (auto &pin : node.m_inputs)
		{
			ofs << "\tinput" << endl;
			ofs << "\t\t" << "type " << ePinType::ToString(pin.type) << endl;
			ofs << "\t\t" << "id " << pin.id.Get() << endl;
			if (pin.name.empty())
				ofs << "\t\t" << "name \" \"" << endl; // blank name
			else
				ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;

			//ofs << "\t\t" << "value " << pin.value << endl;
			ofs << "\t\t" << "links ";
			for (auto &link : m_links)
				if (link.toId == pin.id)
					ofs << link.toId.Get() << " ";
			ofs << endl;
		}

		for (auto &pin : node.m_outputs)
		{
			ofs << "\toutput" << endl;
			ofs << "\t\t" << "type " << ePinType::ToString(pin.type) << endl;
			ofs << "\t\t" << "id " << pin.id.Get() << endl;
			if (pin.name.empty())
				ofs << "\t\t" << "name \" \"" << endl; // blank name
			else
				ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;
			//ofs << "\t\t" << "value " << pin.value << endl;
			ofs << "\t\t" << "links ";
			for (auto &link : m_links)
				if (link.fromId == pin.id)
					ofs << link.toId.Get() << " ";
			ofs << endl;
		}
	}

	for (auto &kv : m_symbTable.m_symbols)
	{
		ofs << "symbol" << endl;
		ofs << "\tid " << kv.first << endl;

		const variant_t var = kv.second.var;
		string str = common::variant2str(var);
		if (var.vt == VT_BSTR)
			ofs << "\tvalue \"" << kv.second.str << "\"" << endl;
		else
			ofs << "\tvalue " << str << endl;
	}

	return true;
}


// find pin in m_nodes
// if not exist, return null
sPin* cNodeFile::FindPin(const ed::PinId id)
{
	for (auto &node : m_nodes)
	{
		for (auto &pin : node.m_inputs)
			if (pin.id == id)
				return &pin;
		for (auto &pin : node.m_outputs)
			if (pin.id == id)
				return &pin;
	}
	return nullptr;
}


// parsing 된 pin data를 node에 저장한다.
bool cNodeFile::AddPin(const int parseState, cNode &node, const sPin &pin)
{
	if (parseState == 2) // input
	{
		node.m_inputs.push_back(pin);
	}
	else if (parseState == 3) // output
	{
		node.m_outputs.push_back(pin);
	}
	return true;
}
