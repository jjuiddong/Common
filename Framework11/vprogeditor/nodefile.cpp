
#include "stdafx.h"
#include "nodefile.h"

using namespace framework;
using namespace vprog;


cNodeFile::cNodeFile()
{
}

cNodeFile::~cNodeFile()
{
	Clear();
}


bool cNodeFile::Read(const StrPath &fileName)
{
	Clear();

	vector<common::cSimpleData2::sRule> rules;
	rules.push_back({ 0, "node", 1, -1 });
	rules.push_back({ 1, "output", 2, -1 });
	rules.push_back({ 2, "output", 2, 1 });
	rules.push_back({ 3, "output", 2, 1 });
	rules.push_back({ 1, "input", 3, -1 });
	rules.push_back({ 3, "input", 3, 1 });
	rules.push_back({ 2, "input", 3, 1 });
	rules.push_back({ 1, "node", 1, 0 });
	rules.push_back({ 2, "node", 1, 0 });
	rules.push_back({ 3, "node", 1, 0 });
	rules.push_back({ 4, "node", 1, 0 });
	rules.push_back({ 5, "node", 1, 0 });
	rules.push_back({ 7, "node", 1, 0 });
	rules.push_back({ 1, "symbol", 4, 0 });
	rules.push_back({ 2, "symbol", 4, 0 });
	rules.push_back({ 3, "symbol", 4, 0 });
	rules.push_back({ 4, "symbol", 4, 0 });
	rules.push_back({ 5, "symbol", 4, 0 });
	rules.push_back({ 7, "symbol", 4, 0 });
	rules.push_back({ 0, "define", 5, 0 });
	rules.push_back({ 1, "define", 5, 0 });
	rules.push_back({ 2, "define", 5, 0 });
	rules.push_back({ 3, "define", 5, 0 });
	rules.push_back({ 4, "define", 5, 0 });
	rules.push_back({ 5, "define", 5, 0 });
	rules.push_back({ 7, "define", 5, 0 });
	rules.push_back({ 5, "attr", 6, -1 });
	rules.push_back({ 6, "attr", 6, 5 });
	rules.push_back({ 6, "node", 1, 0 });
	rules.push_back({ 6, "symbol", 4, 0 });
	rules.push_back({ 6, "define", 5, 0 });
	rules.push_back({ 6, "initvar", 7, 0 });
	rules.push_back({ 1, "initvar", 7, 0 });
	rules.push_back({ 2, "initvar", 7, 0 });
	rules.push_back({ 3, "initvar", 7, 0 });
	rules.push_back({ 4, "initvar", 7, 0 });
	rules.push_back({ 5, "initvar", 7, 0 });
	rules.push_back({ 7, "initvar", 7, 0 });


	common::cSimpleData2 sdata(rules);
	sdata.Read(fileName);
	RETV(!sdata.m_root, false);

	for (auto &p : sdata.m_root->children)
	{
		if (p->name == "node")
		{
			cNode node(0,"");
			node.m_type = eNodeType::FromString(sdata.Get<string>(p, "type", "Event"));
			node.m_id = sdata.Get<int>(p, "id", 0);
			node.m_name = sdata.Get<string>(p, "name", "name");
			node.m_desc = sdata.Get<string>(p, "desc", node.m_name.c_str());

			const auto &rectAr = sdata.GetArray(p, "rect");
			if (rectAr.size() >= 5)
			{
				const sRectf rect(
					(float)atof(rectAr[1].c_str())
					, (float)atof(rectAr[2].c_str())
					, (float)atof(rectAr[3].c_str())
					, (float)atof(rectAr[4].c_str())
				);
				node.m_size = ImVec2(rect.Width(), rect.Height());
				node.m_pos = ImVec2(rect.left, rect.top);
			}

			const auto &colorAr = sdata.GetArray(p, "color");
			if (colorAr.size() >= 5)
			{
				const Vector4 color(
					(float)atof(colorAr[1].c_str())
					, (float)atof(colorAr[2].c_str())
					, (float)atof(colorAr[3].c_str())
					, (float)atof(colorAr[4].c_str())
				);
				node.m_color = *(ImColor*)&color;
			}

			for (auto &c : p->children)
			{
				if ((c->name == "output")
					|| (c->name == "input"))
				{
					sPin pin(0, "", ePinType::Flow);

					const string typeStr = sdata.Get<string>(c, "type", " ");
					pin.typeStr = typeStr;
					pin.type = ePinType::FromString(typeStr);
					if (pin.type == ePinType::COUNT) // not found type, check enum type
					{
						if (m_symbTable.FindSymbol(typeStr))
						{
							pin.type = ePinType::Enums;
						}
						else
						{
							assert(!"cNodeFile::Read() Error!, not defined type name");
							pin.type = ePinType::Int;
						}
					}

					pin.id = sdata.Get<int>(c, "id", 0);
					pin.name = sdata.Get<string>(c, "name", "name");
					if (pin.name.empty())
						pin.name = " ";

					if (c->name == "output")
					{
						// add link output pin(from) -> input pin(to)
						auto &ar = sdata.GetArray(c, "links");
						for (uint i = 1; i < ar.size(); ++i)
						{
							// maybe link id duplicated
							const ed::PinId toPinId = atoi(ar[i].c_str());
							m_links.push_back(sLink(ed::LinkId(common::GenerateId())
								, pin.id, toPinId));
						}

						pin.kind = ePinKind::Output;
						node.m_outputs.push_back(pin);
					}
					else // input
					{
						pin.kind = ePinKind::Input;
						node.m_inputs.push_back(pin);
					}
				}
			}
			m_nodes.push_back(node);
		}
		else if (p->name == "symbol")
		{
			sPin pin(sdata.Get<int>(p, "id", 0), "@symbol@");
			cNode *n = nullptr;
			sPin *pp = nullptr;
			std::tie(n, pp) = FindContainPin(pin.id);
			if (n && pp)
			{
				const string name = pp->name.c_str();
				const string typeStr = pp->typeStr.c_str();
				const string scopeName = common::script::cSymbolTable::MakeScopeName(
					n->m_name.c_str(), n->m_id.Get());
				const string value = sdata.Get<string>(p, "value", "0");

				_variant_t var;
				GetPinVar(*pp, value, var);

				if (!m_symbTable.Set(scopeName, name, var, typeStr))
				{
					// error occurred
					assert(!"cNodefile::Read() symbol parse error!!");
				}
			}
		}
		else if (p->name == "initvar")
		{
			const string scopeName = sdata.Get<string>(p, "scopename", "");
			const string name = sdata.Get<string>(p, "name", "");
			const string valueStr = sdata.Get<string>(p, "value", "");
			const string typeStr = sdata.Get<string>(p, "type", "");

			if (!scopeName.empty() && !name.empty())
			{
				if (!m_symbTable.Set(scopeName, name, valueStr, typeStr))
				{
					// error occurred
					assert(!"cNodefile::Read() symbol parse error!!");
				}
			}
		}
		else if (p->name == "define")
		{
			common::script::cSymbolTable::sSymbol type;
			const string typeStr = sdata.Get<string>(p, "type", "Enum");
			type.type = (typeStr == "Enum") ? 
				common::script::cSymbolTable::eType::Enum 
				: common::script::cSymbolTable::eType::None;
			type.name = sdata.Get<string>(p, "name", " ");

			if (type.type == common::script::cSymbolTable::eType::Enum)
			{
				for (auto &c : p->children)
				{
					if (c->name == "attr")
					{
						common::script::cSymbolTable::sEnum e;
						e.name = sdata.Get<string>(c, "name", " ");
						e.value = sdata.Get<int>(c, "value", 0);
						type.enums.push_back(e);
					}
				}
			}
			else
			{
				assert(!"cNodeFile::Read() Error, not defined type parse");
			}

			m_symbTable.AddSymbol(type);
		}
		else
		{
			assert(!"cNodeFile::Read() Error, not defined node type");
			break;
		}
	} //~for nodes, type, symbol

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
		if (eNodeType::Define == node.m_type)
			Write_Define(ofs, node);
		else
			Write_Node(ofs, node);
	}

	for (auto &kv : m_symbTable.m_vars)
	{
		const string scopeName = kv.first;
		for (auto &kv2 : kv.second)
		{
			const string name = kv2.first;
			string valueStr = common::variant2str(kv2.second.var);
			ofs << "initvar" << endl;
			ofs << "\tscopename " << scopeName << endl;
			ofs << "\tname " << name << endl;
			ofs << "\tvalue \"" << valueStr << "\"" << endl;
			ofs << "\ttype " << kv2.second.type << endl;
		}
	}

	return true;
}


// write node data
bool cNodeFile::Write_Node(std::ostream &ofs, cNode &node)
{
	using namespace std;

	ofs << "node" << endl;
	ofs << "\t" << "type " << eNodeType::ToString(node.m_type) << endl;
	ofs << "\t" << "id " << node.m_id.Get() << endl;
	if (node.m_name.empty())
		ofs << "\t" << "name \" \"" << endl; // blank name
	else
		ofs << "\t" << "name \"" << node.m_name.c_str() << "\"" << endl;
	//if (eNodeType::Variable == node.m_type)
	ofs << "\t" << "desc \"" << node.m_desc.c_str() << "\"" << endl;
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
		//ofs << "\t\t" << "type " << ePinType::ToString(pin.type) << endl;
		ofs << "\t\t" << "type " << pin.typeStr.c_str() << endl;
		ofs << "\t\t" << "id " << pin.id.Get() << endl;
		if (pin.name.empty())
			ofs << "\t\t" << "name \" \"" << endl; // blank name
		else
			ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;

		//ofs << "\t\t" << "value " << pin.value << endl;
		ofs << "\t\t" << "links ";
		for (auto &link : m_links)
			if (link.toId == pin.id)
				ofs << link.fromId.Get() << " ";
		ofs << endl;
	}

	for (auto &pin : node.m_outputs)
	{
		ofs << "\toutput" << endl;
		//ofs << "\t\t" << "type " << ePinType::ToString(pin.type) << endl;
		ofs << "\t\t" << "type " << pin.typeStr.c_str() << endl;
		ofs << "\t\t" << "id " << pin.id.Get() << endl;
		if (pin.name.empty())
			ofs << "\t\t" << "name \" \"" << endl; // blank name
		else
			ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;
		ofs << "\t\t" << "links ";
		for (auto &link : m_links)
			if (link.fromId == pin.id)
				ofs << link.toId.Get() << " ";
		ofs << endl;
	}

	return true;
}


// write define type node data
bool cNodeFile::Write_Define(std::ostream &ofs, cNode &node)
{
	using namespace std;

	ofs << "define" << endl;
	ofs << "\t" << "type " << node.m_desc.c_str() << endl;
	ofs << "\t" << "name \"" << node.m_name.c_str() << "\"" << endl;
	for (auto &pin : node.m_outputs)
	{
		ofs << "\tattr" << endl;
		ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;
		ofs << "\t\t" << "value " << pin.value << endl;
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


// return pin contain node
std::pair<vprog::cNode*, vprog::sPin*>
 cNodeFile::FindContainPin(const ed::PinId pinId)
{
	for (auto &node : m_nodes)
	{
		for (auto &pin : node.m_inputs)
			if (pin.id == pinId)
				return std::make_pair(&node, &pin);
		for (auto &pin : node.m_outputs)
			if (pin.id == pinId)
				return std::make_pair(&node, &pin);
	}
	return std::make_pair(nullptr, nullptr);
}


// convert string value to _variant_t type
bool cNodeFile::GetPinVar(const sPin &pin, const string &value
	, OUT _variant_t &out)
{
	const VARTYPE vt = GetPin2VarType(pin.type);
	out = common::str2variant(vt, value);
	return true;
}


void cNodeFile::Clear()
{
	m_nodes.clear();
	m_links.clear();
	m_symbTable.Clear();
}
