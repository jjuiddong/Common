
#include "stdafx.h"
#include "vprogfile.h"

using namespace vprog;
namespace script = common::script;

cVProgFile::cVProgFile()
{
}

cVProgFile::~cVProgFile()
{
	Clear();
}


bool cVProgFile::Read(const StrPath &fileName)
{
	Clear();

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
				pin = {};
			}
			else if (toks[0] == "output")
			{
				state = 3;
				pin = {};
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
				for (uint i = 1; i < toks.size(); ++i)
				{
					// maybe link id duplicated
					const int toPinId = atoi(toks[i].c_str());
					pin.links.push_back(toPinId);
				}
			}
			else if (toks[0] == "input")
			{
				if (!pin.name.empty())
					AddPin(state, node, pin);

				state = 2;
				pin = {};
			}
			else if (toks[0] == "output")
			{
				if (!pin.name.empty())
					AddPin(state, node, pin);

				state = 3;
				pin = {};
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
			if ((toks[0] == "id") && (toks.size() >= 2))
			{
				pin.name = "@symbol@";
				pin.id = atoi(toks[1].c_str());
			}
			else if ((toks[0] == "value") && (toks.size() >= 2))
			{
				// add variable table
				sNode *n = nullptr;
				sPin *p = nullptr;
				std::tie(n, p) = FindContainPin(pin.id);
				if (n && p)
				{
					string scopeName = MakeScopeName(*n);
					string varName = p->name.c_str();
					VARTYPE vt = VT_VOID;
					switch (p->type)
					{
					case ePinType::Bool: vt = VT_BOOL; break;
					case ePinType::Int: vt = VT_INT; break;
					case ePinType::Float: vt = VT_R4; break;
					case ePinType::String: vt = VT_BSTR; break;
					default:
						common::dbg::Logc(1
							, "Error!! cVProgFile::Read() symbol parse error!!\n");
						break;
					}

					variant_t val = common::str2variant(vt, toks[1]);
					if (!m_variables.Set(scopeName, varName, val))
					{
						common::dbg::Logc(1
							, "Error!! cVProgFile::Read() symbol parse error!!\n");
					}
				}
				else
				{
					common::dbg::Logc(1
						, "Error!! cVProgFile::Read() symbol parse error!!\n");
				}

				pin = {};
			}
		}
		break;
		}
	}

	if (!pin.name.empty() && (pin.name != "@symbol@"))
		AddPin(state, node, pin);

	if (!node.name.empty())
	{
		m_nodes.push_back(node);
		node = { 0, };
	}

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

	{
		out.m_codes.push_back({ script::eCommand::nop });
		script::sCommandSet code;
		code.cmd = script::eCommand::label;
		code.str1 = "main";
		out.m_codes.push_back(code);
		out.m_codes.push_back({ script::eCommand::nop });
	}

	// make intermediate code
	for (auto &node : m_nodes)
		if (eNodeType::Event == node.type)
			GenerateCode_Event(node, out);

	// make intermediate code, insert initial symbol
	for (auto &kv1 : m_variables.m_symbols)
	{
		for (auto &kv2 : kv1.second)
		{
			script::sCommandSet code;
			switch (kv2.second.vt)
			{
			case VT_BOOL: code.cmd = script::eCommand::symbolb; break;
			case VT_INT: code.cmd = script::eCommand::symboli; break;
			case VT_R4: code.cmd = script::eCommand::symbolf; break;
			case VT_BSTR: code.cmd = script::eCommand::symbols; break;				
			default:
				common::dbg::Logc(3, "Error!! cVProgFile::GenerateIntermediateCode(), invalid symbol type\n");
				break;
			}
			code.str1 = kv1.first;
			code.str2 = kv2.first;
			code.var1 = kv2.second;
			out.m_codes.push_back(code);
		}
	}

	out.m_variables = m_variables;

	return true;
}


// generate intermediate code, event node
bool cVProgFile::GenerateCode_Event(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Event, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	out.m_codes.push_back({ script::eCommand::nop });
	const string labelName = node.name.c_str();
	out.m_codes.push_back({ script::eCommand::label, labelName });

	for (auto &pin : node.outputs)
	{
		if ((ePinType::Flow == pin.type) && (!pin.links.empty()))
		{
			if (pin.links.size() >= 2)
				common::dbg::Logc(3, "Error!! cVProgFile::Generate intermediate code, flow link too many setting \n");

			sNode *next = nullptr;
			sPin *np = nullptr;
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (!next)
				continue; // error occurred!!
			GenerateCode_Node(*next, out);
		}
	}

	out.m_codes.push_back({ script::eCommand::nop });
	return true;
}


// generate intermediate code, node
bool cVProgFile::GenerateCode_Node(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	switch (node.type)
	{
	case eNodeType::Function:
	case eNodeType::Macro: GenerateCode_Function(node, out); break;
	case eNodeType::Control: GenerateCode_Branch(node, out); break;
	case eNodeType::Operator: GenerateCode_Operator(node, out); break;
	case eNodeType::Variable: GenerateCode_Variable(node, out); break;
		break;
	default:
		common::dbg::Logc(1
			, "Error!! cVProgFile::GenerateCode_Node(), node type is invalid\n");
		return false; // nothing generate this type
	}

	return true;
}


// generate intermediate code, function node type
bool cVProgFile::GenerateCode_Function(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	// function, macro type
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// get input variable
	const uint reg = 0;
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
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(*prev, out);

			GenerateCode_Pin(*prev, *pp, reg, out); // get data from prev output pin
			GenerateCode_Pin(node, pin, reg, out); // set data to input pin
		}
		else
		{
			// load temporal variable
			GenerateCode_TemporalPin(node, pin, reg, out);
			GenerateCode_Pin(node, pin, reg, out); // set data to input pin
		}
	}

	// call function
	script::sCommandSet code;
	code.cmd = script::eCommand::call;
	code.str1 = MakeScopeName(node);
	out.m_codes.push_back(code);

	// next flow node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next)
			{
				GenerateCode_Node(*next, out);
			}
		}
	}

	return true;
}


// generate intermediate code, control node
bool cVProgFile::GenerateCode_Branch(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// get input variable
	uint reg = 0;
	uint condReg = 0; // condition register
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
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(*prev, out);

			GenerateCode_Pin(*prev, *pp, reg, out); // get data from prev output pin
			++reg;
		}
		else
		{
			// load temporal variable
			GenerateCode_TemporalPin(node, pin, reg, out);
			++reg;
		}

		if (pin.name == "Condition")
			condReg = reg - 1;
	}

	// insert branch code
	// compare condition is zero?
	{
		script::sCommandSet code;
		code.cmd = script::eCommand::eqic;
		code.reg1 = condReg;
		code.var1 = variant_t((int)0);
		out.m_codes.push_back(code);
	}
	{
		// jump if true
		// find False branch pin
		string jumpLabel;
		for (auto &pin : node.outputs)
		{
			if ((ePinType::Flow == pin.type)
				&& (pin.name == "False"))
			{
				sNode *next = nullptr; // next node
				sPin *np = nullptr; // next pin
				const int linkId = pin.links.empty() ? -1 : pin.links.front();
				std::tie(next, np) = FindContainPin(linkId);
				if (next)
					jumpLabel = MakeScopeName(*next);
			}
		}

		if (jumpLabel.empty())
		{
			// no branch node, nop
			script::sCommandSet code;
			code.cmd = script::eCommand::nop;
			out.m_codes.push_back(code);
			common::dbg::Logc(1, "cVProgFile::GenerateCode_Control, no branch label\n");
		}
		else
		{
			script::sCommandSet code;
			code.cmd = script::eCommand::jnz;
			code.str1 = jumpLabel;
			out.m_codes.push_back(code);
		}
	}

	// next flow node (generate True branch node)
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "True"))
			{
				GenerateCode_Node(*next, out);
			}
		}
	}

	// generate False branch node
	// seperate from this node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "False"))
			{
				// insert nop
				{
					script::sCommandSet code;
					code.cmd = script::eCommand::nop;
					out.m_codes.push_back(code);
				}
				// insert jump label
				{
					script::sCommandSet code;
					code.cmd = script::eCommand::label;
					code.str1 = MakeScopeName(*next);
					out.m_codes.push_back(code);
				}

				GenerateCode_Node(*next, out);
				break;
			}
		}
	}

	return true;
}


// generate intermediate code, operator node
bool cVProgFile::GenerateCode_Operator(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Operator, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// get input variable
	uint reg = 8;
	VARTYPE vt = VT_VOID;
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
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			if (m_visit.end() == m_visit.find(prev->id))
				GenerateCode_Node(*prev, out);

			GenerateCode_Pin(*prev, *pp, reg, out); // get data from prev output pin
			++reg;
		}
		else
		{
			// load temporal variable
			GenerateCode_TemporalPin(node, pin, reg, out);
			++reg;
		}
		
		if (VT_VOID == vt)
		{
			switch (pin.type)
			{
			case ePinType::Bool: vt = VT_BOOL; break;
			case ePinType::Int: vt = VT_INT; break;
			case ePinType::Float: vt = VT_R4; break;
			case ePinType::String: vt = VT_BSTR; break;
			default: break;
			}
		}
	}

	// insert compare code
	if (node.name == "<")
	{
		script::sCommandSet code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::lesi; break;
		case VT_R4: code.cmd = script::eCommand::lesf; break;
		default: 
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}
	else if (node.name == "<=")
	{
		script::sCommandSet code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::leqi; break;
		case VT_R4: code.cmd = script::eCommand::leqf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}
	else if (node.name == ">")
	{
		script::sCommandSet code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::gri; break;
		case VT_R4: code.cmd = script::eCommand::grf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}
	else if (node.name == ">=")
	{
		script::sCommandSet code;
		switch (vt)
		{
		case VT_INT: code.cmd = script::eCommand::greqi; break;
		case VT_R4: code.cmd = script::eCommand::greqf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
	}

	// load compare flag to register
	{
		script::sCommandSet code;
		code.cmd = script::eCommand::ldcmp;
		code.reg1 = 9;
		out.m_codes.push_back(code);
	}

	// insert compare data to symboltable
	{
		script::sCommandSet code;
		code.cmd = script::eCommand::setb;
		code.str1 = MakeScopeName(node);
		code.str2 = "O";
		code.reg1 = 9;
		out.m_codes.push_back(code);
	}

	return true;
}


// generate intermediate code, variable node
bool cVProgFile::GenerateCode_Variable(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Operator, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// nothing to operation

	return true;
}


// generate intermediate code, pin
bool cVProgFile::GenerateCode_Pin(const sNode &node, const sPin &pin, const uint reg
	, OUT common::script::cIntermediateCode &out)
{

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

		code.str1 = MakeScopeName(node);
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

		code.str1 = MakeScopeName(node);
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
			code.var1.vt = VT_INT;
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
	sPin p = pin;
	p.kind = (parseState == 2) ? ePinKind::Input : ePinKind::Output;

	if (parseState == 2) // input
		node.inputs.push_back(p);
	else if (parseState == 3) // output
		node.outputs.push_back(p);
	return true;
}


// return pin contain node
std::pair<cVProgFile::sNode*, cVProgFile::sPin*> 
	cVProgFile::FindContainPin(const int pinId)
{
	if (pinId < 0)
		return std::make_pair(nullptr, nullptr);

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


string cVProgFile::MakeScopeName(const sNode &node)
{
	if (eNodeType::Event == node.type)
		return node.name.c_str();
	else
		return script::cSymbolTable::MakeScopeName(node.name.c_str(), node.id).c_str();
}


void cVProgFile::Clear()
{
	m_nodes.clear();
	m_variables.Clear();
}
