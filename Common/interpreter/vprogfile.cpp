
#include "stdafx.h"
#include "vprogfile.h"

using namespace vprog;


cVProgFile::cVProgFile()
{
}

cVProgFile::~cVProgFile()
{
	Clear();
}


bool cVProgFile::Read(const StrPath &fileName)
{
	using namespace std;
	ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		return false;

	sNode node = { 0, };
	sPin pin = { 0, "", ePinType::Flow };

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
				node = { 0, };
			}
			else if (toks[0] == "symbol")
			{
				state = 4;
				node = { 0, };
			}
			break;

		case 1: // state parsing
			if ((toks[0] == "type") && (toks.size() >= 2))
			{
				node.type = eNodeType::FromString(toks[1]);
			}
			else if ((toks[0] == "id") && (toks.size() >= 2))
			{
				node.id = atoi(toks[1].c_str());
			}
			else if ((toks[0] == "name") && (toks.size() >= 2))
			{
				node.name = toks[1];
			}
			else if ((toks[0] == "varname") && (toks.size() >= 2))
			{
				node.varName = toks[1];
			}
			else if ((toks[0] == "rect") && (toks.size() >= 5))
			{
				// no parsing
			}
			else if ((toks[0] == "color") && (toks.size() >= 5))
			{
				// no parsing
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
						const int toPinId = atoi(toks[i].c_str());
						pin.links.push_back(toPinId);
						m_links.push_back({ pin.id, toPinId });
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

				if (!node.name.empty())
				{
					m_nodes.push_back(node);
					node = { 0, };
				}

				if (toks[0] == "node")
					state = 1;
				else if (toks[0] == "symbol")
					state = 4;
			}
			break;

		case 4: // symbol
		{
			//if ((toks[0] == "id") && (toks.size() >= 2))
			//{
			//	pin.name = "@symbol@";
			//	pin.id = atoi(toks[1].c_str());
			//}
			//else if ((toks[0] == "value") && (toks.size() >= 2))
			//{
			//	if (!m_symbTable.AddSymbolStr(pin, toks[1]))
			//	{
			//		// error occurred
			//		assert(!"nodefile::Read() symbol parse error!!");
			//	}
			//	pin.name.clear();
			//}
		}
		break;
		}
	}

	if (!pin.name.empty() && (pin.name != "@symbol@"))
		AddPin(state, node, pin);

	//if (!node.m_name.empty())
	//{
	//	m_nodes.push_back(node);
	//	node.Clear();
	//}

	return true;
}


bool cVProgFile::Write(const StrPath &fileName)
{
	// nothing~
	return true;
}


// generate script intermediate code
bool cVProgFile::GenerateIntermediateCode(OUT common::script::cIntermediateCode &out)
{
	RETV(m_nodes.empty(), false);

	m_visit.clear(); // avoid duplicate execution

	for (auto &node : m_nodes)
	{
		if (eNodeType::Event == node.type)
		{
			//if (m_visit.find(node.id) != m_visit.end())
			//	continue;
			//m_visit.insert(node.id);
			GenerateCode_Event(node, out);
		}
	}

	return true;
}


// generate intermediate code, event node
bool cVProgFile::GenerateCode_Event(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Event, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	namespace script = common::script;
	string labelName = node.name.c_str();
	labelName += ":";
	out.m_codes.push_back({ script::eCommand::label, labelName });

	for (auto &pin : node.outputs)
	{
		if ((ePinType::Flow == pin.type) && (!pin.links.empty()))
		{
			if (pin.links.size() >= 2)
				common::dbg::Logc(3, "Error!! cVProgFile::Generate intermediate code, flow link too many setting \n");

			sNode *next = nullptr;
			sPin *np = nullptr;
			std::tie(next, np) = FindContainPin(pin.links.front());
			if (!next)
				continue; // error occurred!!
			GenerateCode_Node(*next, out);
		}
	}

	return true;
}


// generate intermediate code, node
bool cVProgFile::GenerateCode_Node(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	switch (node.type)
	{
	case eNodeType::Function:
	case eNodeType::Macro: 
		GenerateCode_Function(node, out); 
		break;

	case eNodeType::Control:
	case eNodeType::Operator:
	case eNodeType::Variable:
		break;
	default:
		return false; // nothing generate this type
	}

	return true;
}


// generate intermediate code, function node type
bool cVProgFile::GenerateCode_Function(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	namespace script = common::script;
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	uint reg = 0;
	for (auto &pin : node.inputs)
	{
		switch (pin.type)
		{
		case ePinType::Flow:
		case ePinType::Function:
		case ePinType::Delegate:
			continue;
		}

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		std::tie(prev, pp) = FindContainPin(pin.links.front());
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(*prev, out);

			GenerateCode_Pin(*prev, *pp, reg, out); // get data from output pin
			GenerateCode_Pin(node, pin, reg, out); // set data to input pin
			++reg;
		}
		else
		{
			// load temporal variable
			GenerateCode_TemporalPin(node, pin, reg, out);
			GenerateCode_Pin(node, pin, reg, out); // set data to input pin
			++reg;
		}
	}
	return true;
}


// generate intermediate code, pin
bool cVProgFile::GenerateCode_Pin(const sNode &node, const sPin &pin, const uint reg
	, OUT common::script::cIntermediateCode &out)
{
	namespace script = common::script;

	if (ePinKind::Input == pin.kind)
	{
		script::sCommandSet code;
		switch (pin.type)
		{
		case ePinType::Bool: code.cmd = script::eCommand::setb; break;
		case ePinType::Int: code.cmd = script::eCommand::seti; break;
		case ePinType::Float: code.cmd = script::eCommand::setf; break;
		case ePinType::String: code.cmd = script::eCommand::sets; break;
		default:
			return false;
		}

		code.str1 = node.name.c_str(); // make scope name
		code.str2 = pin.name.c_str();
		code.reg1 = reg;
		out.m_codes.push_back(code);
	}
	else // output
	{
		script::sCommandSet code;
		switch (pin.type)
		{
		case ePinType::Bool: code.cmd = script::eCommand::getb; break;
		case ePinType::Int: code.cmd = script::eCommand::geti; break;
		case ePinType::Float: code.cmd = script::eCommand::getf; break;
		case ePinType::String: code.cmd = script::eCommand::gets; break;
		default:
			return false;
		}

		code.str1 = node.name.c_str(); // make scope name
		code.str2 = pin.name.c_str();
		code.reg1 = reg;
		out.m_codes.push_back(code);
	}

	return true;
}


// generate intermedate code, load temporal value to register
bool cVProgFile::GenerateCode_TemporalPin(const sNode &node, const sPin &pin
	, const uint reg, OUT common::script::cIntermediateCode &out)
{
	namespace script = common::script;

	if (ePinKind::Input == pin.kind)
	{
		script::sCommandSet code;
		switch (pin.type)
		{
		case ePinType::Bool: 
			code.cmd = script::eCommand::ldbc;
			code.var1.vt = VT_BOOL;
			code.var1.boolVal = false;
			code.reg1 = reg;
			break;
		case ePinType::Int: 
			code.cmd = script::eCommand::ldic; 
			code.var1.vt = VT_I4;
			code.var1.intVal = 0;
			code.reg1 = reg;
			break;
		case ePinType::Float: 
			code.cmd = script::eCommand::ldfc; 
			code.var1.vt = VT_R4;
			code.var1.fltVal = 0.f;
			code.reg1 = reg;
			break;
		case ePinType::String: 
			code.cmd = script::eCommand::ldsc; 
			code.var1.vt = VT_BSTR;
			code.var1.bstrVal = (BSTR)"";
			code.reg1 = reg;
			break;
		default:
			return false;
		}
		out.m_codes.push_back(code);
	}
	else // output
	{
		// nothing~
	}

	return true;
}


bool cVProgFile::AddPin(const int parseState, sNode &node, const sPin &pin)
{
	if (parseState == 2) // input
		node.inputs.push_back(pin);
	else if (parseState == 3) // output
		node.outputs.push_back(pin);
	return true;
}


// return pin contain node
std::pair<cVProgFile::sNode*, cVProgFile::sPin*> 
	cVProgFile::FindContainPin(const int pinId)
{
	for (auto &node : m_nodes)
	{
		for (auto &pin : node.inputs)
			if (pin.id == pinId)
				return std::make_pair(&node, &pin);
		for (auto &pin : node.outputs)
			if (pin.id == pinId)
				return std::make_pair(&node, &pin);
	}
	return std::make_pair(nullptr, nullptr);
}


void cVProgFile::Clear()
{
	m_nodes.clear();
	m_links.clear();
}
