
#include "stdafx.h"
#include "vprogfile.h"

using namespace vprog;
namespace script = common::script;

const static cVProgFile::sNode nullNode = { 0, };


cVProgFile::cVProgFile()
	: m_jmpLabelSeedId(0)
{
}

cVProgFile::~cVProgFile()
{
	Clear();
}


// read *.vprog file
bool cVProgFile::Read(const StrPath &fileName)
{
	Clear();

	m_fileName = fileName.GetFullFileName();

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
			AddNode(sdata, p);
		else if (p->name == "symbol")
			AddSymbol(sdata, p);
		else if (p->name == "initvar")
			AddVariable(sdata, p);
		else if (p->name == "define")
			AddDefine(sdata, p);
		else
		{
			assert(!"cVProgFile::Read() Error, not defined node type");
			break;
		}
	} //~for nodes, type, symbol

	return true;
}


// add node from parsed *.vprog file
bool cVProgFile::AddNode(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	sNode node;
	node.type = eNodeType::FromString(sdata.Get<string>(p, "type", "Event"));
	node.id = sdata.Get<int>(p, "id", 0);
	node.name = sdata.Get<string>(p, "name", "name");
	//node.desc = sdata.Get<string>(p, "desc", node.name);
	node.typeStr = sdata.Get<string>(p, "typeStr", node.name);
	node.labelName = sdata.Get<string>(p, "labelname", "");

	for (auto &c : p->children)
	{
		if ((c->name == "output") || (c->name == "input"))
		{
			sPin pin;

			const string type = sdata.Get<string>(c, "type", " ");
			const string typeStr = sdata.Get<string>(c, "typeStr", "");
			pin.type = ePinType::FromString(type);
			pin.typeStr = typeStr;
			common::script::ParseTypeString(typeStr, pin.typeValues);

			if (pin.type == ePinType::COUNT) // not found type, check enum type?
			{
				if (m_variables.FindSymbol(type))
				{
					pin.type = ePinType::Enums;
				}
				else
				{
					assert(!"cVProgFile::Read() Error!, not defined type name");
					pin.type = ePinType::Int;
				}
			}

			pin.id = sdata.Get<int>(c, "id", 0);
			pin.name = sdata.Get<string>(c, "name", "name");
			pin.varName = sdata.Get<string>(c, "varname", "");
			auto &ar = sdata.GetArray(c, "links");
			for (uint i = 1; i < ar.size(); ++i)
			{
				//todo: maybe link id duplicated
				const int toPinId = atoi(ar[i].c_str());
				pin.links.push_back(toPinId);
			}

			if (c->name == "output")
			{
				pin.kind = ePinKind::Output;
				node.outputs.push_back(pin);
			}
			else // input
			{
				pin.kind = ePinKind::Input;
				node.inputs.push_back(pin);
			}
		}
	}
	m_nodes.push_back(node);
	return true;
}


// add variable from parsed *.vprog file
bool cVProgFile::AddVariable(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	const string scopeName = sdata.Get<string>(p, "scopename", "");
	const string name = sdata.Get<string>(p, "name", "");
	const string typeStr = sdata.Get<string>(p, "typeStr", "");
	return AddVariable2(scopeName, name, typeStr, sdata, p);
}


// add symbole
bool cVProgFile::AddSymbol(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	sPin pin;
	pin.name = "@symbol@";
	pin.id = sdata.Get<int>(p, "id", 0);

	sNode *n = nullptr;
	sPin *pp = nullptr;
	std::tie(n, pp) = FindContainPin(pin.id);
	if (!n || !pp)
	{
		common::dbg::Logc(1, "Error!! cVProgFile::AddSymbol() symbol parse error!!\n");
		return false;
	}

	const string scopeName = MakeScopeName(*n);
	const string varName = pp->name.c_str();
	return AddVariable2(scopeName, varName, pp->typeStr, sdata, p);
}


// add variable
bool cVProgFile::AddVariable2(const string &scopeName, const string &name
	, const string &typeStr
	, common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	if (scopeName.empty() || name.empty())
	{
		common::dbg::Logc(1, "Error!! cVProgFile::AddVariable2() symbol parse error!! 1\n");
		return false; // error
	}

	//ePinType::Enum pinType;
	using namespace common::script;
	vector<eSymbolType::Enum> typeValues;
	if (!ParseTypeString(typeStr, typeValues)) 
	{
		common::dbg::Logc(1
			, "Error!! cVProgFile::AddVariable2() symbol parse error!! 1-1, typeStr = %s\n"
			, typeStr.c_str());
		return false;
	}
	const common::script::eSymbolType::Enum symbType = typeValues.front();

	variant_t val;
	switch (symbType)
	{
	case eSymbolType::Bool: val = sdata.Get<bool>(p, "value", false); break;
	case eSymbolType::Enums:
	case eSymbolType::Int: val = sdata.Get<int>(p, "value", 0); break;
	case eSymbolType::Float: val = sdata.Get<float>(p, "value", 0.f); break;
	case eSymbolType::String:
		val = common::str2variant(VT_BSTR, sdata.Get<string>(p, "value", ""));
		break;
	case eSymbolType::Array:
		if (!m_variables.InitArray(scopeName, name, typeStr))
		{
			assert(!"cNodefile::AddVariable2() symbol parse error!! 3");
			return false;
		}
		break;
	case eSymbolType::Map:
		if (!m_variables.InitMap(scopeName, name, typeStr))
		{
			assert(!"cNodefile::AddVariable2() symbol parse error!! 4");
			return false;
		}
		break;
	default:
		common::dbg::Logc(1, "Error!! cVProgFile::AddVariable2() symbol parse error!! 2\n");
		return false;
	}

	// update value bool, int, float, string, enum
	if ((symbType != eSymbolType::Array) && (symbType != eSymbolType::Map))
	{
		if (!m_variables.Set(scopeName, name, val, typeStr))
		{
			assert(!"cNodefile::AddVariable2() symbol parse error!! 5");
			return false;
		}
	}

	return true;
}


// add define from parsed *.vprog file
bool cVProgFile::AddDefine(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	namespace script = common::script;

	script::cSymbolTable::sSymbol symbol;
	const string typeStr = sdata.Get<string>(p, "type", "Enum");
	symbol.type = (typeStr == "Enum") ?
		script::cSymbolTable::eType::Enum : script::cSymbolTable::eType::None;
	symbol.name = sdata.Get<string>(p, "name", " ");

	if (symbol.type == script::cSymbolTable::eType::Enum)
	{
		for (auto &c : p->children)
		{
			if (c->name == "attr")
			{
				script::cSymbolTable::sEnum e;
				e.name = sdata.Get<string>(c, "name", " ");
				e.value = sdata.Get<int>(c, "value", 0);
				symbol.enums.push_back(e);
			}
		}
	}
	else
	{
		assert(!"cVProgFile::Read() Error, not defined type parse");
	}

	m_variables.AddSymbol(symbol);

	return true;
}


// write *.vprog file
bool cVProgFile::Write(const StrPath &fileName)
{
	using namespace std;
	ofstream ofs(fileName.c_str());
	if (!ofs.is_open())
		return false;

	for (sNode &node : m_nodes)
	{
		if (eNodeType::Define == node.type)
			Write_Define(ofs, node);
		else
			Write_Node(ofs, node);
	}

	return true;
}


// write node data
bool cVProgFile::Write_Node(std::ostream &ofs, sNode &node)
{
	using namespace std;

	ofs << "node" << endl;
	ofs << "\t" << "type " << eNodeType::ToString(node.type) << endl;
	ofs << "\t" << "id " << node.id << endl;
	if (node.name.empty())
		ofs << "\t" << "name \" \"" << endl; // blank name
	else
		ofs << "\t" << "name \"" << node.name.c_str() << "\"" << endl;
	//ofs << "\t" << "desc \"" << node.desc.c_str() << "\"" << endl;
	ofs << "\t" << "typeStr \"" << node.typeStr.c_str() << "\"" << endl;

	for (auto &pin : node.inputs)
	{
		ofs << "\tinput" << endl;
		ofs << "\t\t" << "type " << ePinType::ToString(pin.type) << endl;
		ofs << "\t\t" << "id " << pin.id << endl;
		if (pin.name.empty())
			ofs << "\t\t" << "name \" \"" << endl; // blank name
		else
			ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;

		ofs << "\t\t" << "links ";
		for (auto &link : m_links)
			if (link.to == pin.id)
				ofs << link.from << " ";
		ofs << endl;
	}

	for (auto &pin : node.outputs)
	{
		ofs << "\toutput" << endl;
		ofs << "\t\t" << "type " << ePinType::ToString(pin.type) << endl;
		ofs << "\t\t" << "id " << pin.id << endl;
		if (pin.name.empty())
			ofs << "\t\t" << "name \" \"" << endl; // blank name
		else
			ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;
		ofs << "\t\t" << "links ";
		for (auto &link : m_links)
			if (link.from == pin.id)
				ofs << link.to << " ";
		ofs << endl;
	}

	return true;
}


// write define type node data
bool cVProgFile::Write_Define(std::ostream &ofs, sNode &node)
{
	using namespace std;

	ofs << "define" << endl;
	//ofs << "\t" << "type " << node.desc.c_str() << endl;
	ofs << "\t" << "type " << node.typeStr.c_str() << endl;
	ofs << "\t" << "name \"" << node.name.c_str() << "\"" << endl;
	for (auto &pin : node.outputs)
	{
		ofs << "\tattr" << endl;
		ofs << "\t\t" << "name \"" << pin.name.c_str() << "\"" << endl;
		ofs << "\t\t" << "value " << pin.value << endl;
	}
	return true;
}


// generate script intermediate code
bool cVProgFile::GenerateIntermediateCode(OUT common::script::cIntermediateCode &out)
{
	using namespace common::script;

	RETV(m_nodes.empty(), false);

	m_visit.clear(); // avoid duplicate execution

	// make main entry
	{
		out.m_codes.push_back({ script::eCommand::nop });
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = "main";
		out.m_codes.push_back(code);
		out.m_codes.push_back({ script::eCommand::nop });
	}

	// make intermediate code from event
	for (auto &node : m_nodes)
		if (eNodeType::Event == node.type)
			Event_GenCode(node, out);

	// make blank branch
	{
		out.m_codes.push_back({ script::eCommand::nop });
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = "blank";
		out.m_codes.push_back(code);
		out.m_codes.push_back({ script::eCommand::sret });
		out.m_codes.push_back({ script::eCommand::nop });
	}

	// make initial symbol
	for (auto &kv1 : m_variables.m_vars) // loop all scope
	{
		for (auto &kv2 : kv1.second) // loop all scope variable
		{
			const sVariable &var = kv2.second;

			script::sInstruction code;
			string typeStr; // array, map type string

			const eSymbolType::Enum symbType = var.typeValues[0];
			switch (symbType)
			{
			case eSymbolType::Bool: code.cmd = script::eCommand::symbolb; break;
			case eSymbolType::Int: code.cmd = script::eCommand::symboli; break;
			case eSymbolType::Float: code.cmd = script::eCommand::symbolf; break;
			case eSymbolType::String: code.cmd = script::eCommand::symbols; break;
			case eSymbolType::Array:
			{
				typeStr = var.type;
				const eSymbolType::Enum elementType = var.typeValues[1]; //array<type>
				switch (elementType)
				{
				case eSymbolType::Bool: code.cmd = script::eCommand::symbolab; break;
				case eSymbolType::Int: code.cmd = script::eCommand::symbolai; break;
				case eSymbolType::Float: code.cmd = script::eCommand::symbolaf; break;
				case eSymbolType::String: code.cmd = script::eCommand::symbolas; break;
				default:
					common::dbg::Logc(3,
						"Error!! cVProgFile::GenerateIntermediateCode(), invalid symbol type2\n");
					break;
				}
			}
			break;
			case eSymbolType::Map:
			{
				typeStr = var.type;
				const eSymbolType::Enum valueType = var.typeValues[2]; //map<string,type>
				switch (valueType)
				{
				case eSymbolType::Bool: code.cmd = script::eCommand::symbolmb; break;
				case eSymbolType::Int: code.cmd = script::eCommand::symbolmi; break;
				case eSymbolType::Float: code.cmd = script::eCommand::symbolmf; break;
				case eSymbolType::String: code.cmd = script::eCommand::symbolms; break;
				case eSymbolType::Array: code.cmd = script::eCommand::symbolma; break;
				default:
					common::dbg::Logc(3, 
						"Error!! cVProgFile::GenerateIntermediateCode(), invalid symbol type3\n");
					break;
				}
			}
			break;
			default:
				common::dbg::Logc(3, 
					"Error!! cVProgFile::GenerateIntermediateCode(), invalid symbol type1\n");
				break;
			}
			code.str1 = kv1.first; // scopename
			code.str2 = kv2.first; // varname
			code.str3 = typeStr; // array, map type string
			code.var1 = var.var;
			out.m_codes.push_back(code);
		}
	}

	// make register tick event
	for (auto &node : m_nodes)
	{
		if (eNodeType::Event == node.type)
		{
			const string name = script::cSymbolTable::MakeScopeName(node.name, node.id);
			script::sVariable *var = m_variables.FindVarInfo(name, "interval");
			if (var) // register event?
			{
				script::sInstruction code;
				code.cmd = script::eCommand::timer1;
				code.str1 = name;
				code.var1 = var->var;
				out.m_codes.push_back(code);
			}
		}
	}

	out.m_variables = m_variables;
	out.m_fileName = m_fileName.GetFileNameExceptExt2();
	out.m_fileName += ".icode";

	return true;
}


// generate intermediate code, event node
bool cVProgFile::Event_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Event, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	out.m_codes.push_back({ script::eCommand::nop });

	// labelName: node.name '_' node.id	 
	// make label name, if need unique event labe name, update from node.labelName
	const string labelName = node.labelName.empty() ?
		script::cSymbolTable::MakeScopeName(node.name, node.id) : node.labelName;
	out.m_codes.push_back({ script::eCommand::label, labelName });
	
	// clear stack
	out.m_codes.push_back({ script::eCommand::cstack });

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
			Node_GenCode(node, *next, pin, out);
		}
	}

	// return, if has jump address
	out.m_codes.push_back({ script::eCommand::sret });

	// finish, stop execute
	out.m_codes.push_back({ script::eCommand::nop });
	return true;
}


// generate intermediate code, node
bool cVProgFile::Node_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	switch (node.type)
	{
	case eNodeType::Function:
	case eNodeType::Macro: Function_GenCode(prevNode, node, fromPin, out); break;
	case eNodeType::Control: Control_GenCode(prevNode, node, fromPin, out); break;
	case eNodeType::Operator: Operator_GenCode(node, out); break;
	case eNodeType::Variable: Variable_GenCode(node, out); break;
		break;
	default:
		common::dbg::Logc(1
			, "Error!! cVProgFile::GenerateCode_Node(), node type is invalid\n");
		return false; // nothing generate this type
	}

	return true;
}


// generate intermediate code, function node type
bool cVProgFile::Function_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
		return true;

	// get input variable
	const uint reg = 0;
	NodeInput_GenCode(node, reg, true, out);

	// Known function?
	if (node.name == "Set")
	{
		const sPin *inputPin = GetInputPin(node, { "in" });
		if (inputPin)
		{
			script::sInstruction code;
			switch (inputPin->type)
			{
			case ePinType::Bool: code.cmd = script::eCommand::setb; break;
			case ePinType::Enums:
			case ePinType::Int: code.cmd = script::eCommand::seti; break;
			case ePinType::Float: code.cmd = script::eCommand::setf; break;
			case ePinType::String: code.cmd = script::eCommand::sets; break;
			case ePinType::Array: code.cmd = script::eCommand::copya; break;
			case ePinType::Map: code.cmd = script::eCommand::copym; break;
			default:
				return false;
			}

			// 2020-11-23, variable name data from desc
			//const std::size_t found = node.desc.find("varname:");
			const std::size_t found = node.typeStr.find("varname:");
			if (found != string::npos)
			{
				//string scope = node.desc.substr(found+8);
				string scope = node.typeStr.substr(found + 8);
				code.str1 = scope; // variable name (scope name)
				code.str2 = "out";
				code.reg1 = reg;
				out.m_codes.push_back(code);
			} 
			else
			{
				//code.str1 = node.desc; // scope name
				code.str1 = node.typeStr; // scope name
				code.str2 = inputPin->name.c_str();
				code.reg1 = reg;
				out.m_codes.push_back(code);
			}
		}
		else
		{
			// error occurred!!
			common::dbg::Logc(2
				, "Error!! cVProgFile::GenerateCode_Function(), not found input pin\n");
		}

		// Set node has out pint?
		for (auto &pin : node.outputs)
			if (ePinType::Flow != pin.type)
				Pin2_GenCode(ePinKind::Input, node, pin, reg, out);
	}
	else if (node.name == "Delay")
	{
		{
			// delay debug information
			script::sInstruction inst;
			inst.cmd = script::eCommand::cmt;
			inst.str1 = "delay";
			inst.reg1 = node.id;
			inst.reg2 = reg;
			out.m_codes.push_back(inst);
		}
		{
			script::sInstruction code;
			code.cmd = script::eCommand::ldtim;
			code.reg1 = reg;
			out.m_codes.push_back(code);
		}
		{
			script::sInstruction code;
			code.cmd = script::eCommand::delay;
			out.m_codes.push_back(code);
		}
	}
	else
	{
		// Unknown Function? call function
		script::sInstruction code;
		code.cmd = script::eCommand::call;
		code.str1 = MakeScopeName(node);
		out.m_codes.push_back(code);
	}

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
				Node_GenCode(node, *next, pin, out);
			}
		}
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate intermediate code, control node
bool cVProgFile::Control_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	if (node.name == "Branch")
		Branch_GenCode(prevNode, node, fromPin, out);
	else if (node.name == "Switch")
		Switch_GenCode(prevNode, node, fromPin, out);
	else if (node.name == "While")
		While_GenCode(prevNode, node, fromPin, out);
	else if (node.name == "For Loop")
		ForLoop_GenCode(prevNode, node, fromPin, out);
	else if (node.name == "Sequence")
		Sequence_GenCode(prevNode, node, fromPin, out);
	return true;
}


// generate intermediate code, control node
bool cVProgFile::Branch_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
		return true;

	uint condReg = 0; // condition register
	NodeInput_GenCode(node, 0, false, out);

	// insert branch code

	// compare condition is zero?  (false condition)
	{
		script::sInstruction code;
		code.cmd = script::eCommand::eqic;
		code.reg1 = condReg;
		code.var1 = variant_t((int)0);
		out.m_codes.push_back(code);
	}
	
	string jumpLabel_False; // false jump label
	{
		// jump if true, jump false flow
		// find False branch pin
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
					jumpLabel_False = MakeScopeName(node) + "-False";
			}
		}

		if (jumpLabel_False.empty())
		{
			// no branch node, jump to blank
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = "blank";
			out.m_codes.push_back(code);
			//common::dbg::Logc(1, "cVProgFile::GenerateCode_Branch, no branch label\n");
		}
		else
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = jumpLabel_False;
			out.m_codes.push_back(code);
		}
	}

	// next flow node (generate True branch node)
	bool hasTrueBranch = false;
	for (auto &pin : node.outputs)
	{
		if ((ePinType::Flow == pin.type) 
			&& (pin.name == "True"))
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next)
			{
				hasTrueBranch = true;
				Node_GenCode(node, *next, pin, out);
			}
		}
	}

	// no has TRUE branch node?
	if (!hasTrueBranch)
	{
		// no branch node, no operation
		NodeEscape_GenCode(node, out);
	}

	// generate False branch node
	// seperate from this node
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
			{
				// insert jump label
				{
					script::sInstruction code;
					code.cmd = script::eCommand::label;
					code.str1 = jumpLabel_False;
					out.m_codes.push_back(code);
				}

				Node_GenCode(node, *next, pin, out);
				break;
			}
		}
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate intermediate code, switch case node
bool cVProgFile::Switch_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
		return true;

	NodeInput_GenCode(node, 0, false, out);

	// get input variable
	const sPin *selPin = GetInputPin(node, { "in", "Selection" });
	if (!selPin)
		return false; // error occurred!!

	common::script::cSymbolTable::sSymbol *symbol = nullptr;
	if (ePinType::Enums == selPin->type)
		symbol = m_variables.FindSymbol(selPin->typeStr.c_str());

	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow != pin.type)
			continue;
		if ((pin.name == "Default") || (pin.name == "default"))
			continue; // not yet

		script::eCommand::Enum cmd;
		variant_t value;
		if (symbol)
		{
			auto it = std::find_if(symbol->enums.begin(), symbol->enums.end()
				, [&](const auto &a) {return a.name == pin.name; });
			if (symbol->enums.end() == it)
			{
				assert(!"cVProgFile::GenerateCode_Switch() error");
				continue; // error occurred!!
			}
			value = (int)it->value;
			cmd = script::eCommand::eqic;
		}
		else
		{
			// int selection type, name is value
			switch (selPin->type) {
			case vprog::ePinType::Bool:
				value = ((pin.name == "true") || (pin.name == "True")) ? 1 : 0;
				cmd = script::eCommand::eqic;
				break;
			case vprog::ePinType::Int:
				value = atoi(pin.name.c_str());
				cmd = script::eCommand::eqic;
				break;
			case vprog::ePinType::Float:
				value = atof(pin.name.c_str());
				cmd = script::eCommand::eqfc;
				break;
			case vprog::ePinType::String:
				value = pin.name.c_str();
				cmd = script::eCommand::eqsc;
				break;
			default:
				common::dbg::Logc(3, "cVProgFile::GenerateCode_Switch() error");
				continue; // error occurred!!, ignore this code
			}
		}

		sNode *next = nullptr; // next node
		sPin *np = nullptr; // next pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(next, np) = FindContainPin(linkId);
		if (!next)
		{
			// no link, finish node
			NodeEscape_GenCode(node, out);
			continue; // no link
		}

		const string jumpLabel = MakeScopeName(*next);

		// compare reg0, enum value (int type)
		// if result is true, jump correspond flow code
		{
			script::sInstruction code;
			code.cmd = cmd;
			code.reg1 = 0;
			code.var1 = common::copyvariant(value);
			out.m_codes.push_back(code);
		}
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = jumpLabel;
			out.m_codes.push_back(code);
		}

	}//~for

	// generate default case
	auto it = std::find_if(node.outputs.begin(), node.outputs.end()
		, [&](const auto &a) {return (a.name == "Default") || (a.name == "default"); });
	if (node.outputs.end() != it)
	{
		auto &pin = *it;
		if (pin.links.empty())
		{
			// no default link? finish node
			NodeEscape_GenCode(node, out);
		}
		else
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next)
			{
				string jumpLabel = MakeScopeName(*next);

				script::sInstruction code;
				code.cmd = script::eCommand::jmp;
				code.str1 = jumpLabel;
				out.m_codes.push_back(code);
			}
		}
	}

	// generate output flow node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow != pin.type)
			continue;

		sNode *next = nullptr; // next node
		sPin *np = nullptr; // next pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(next, np) = FindContainPin(linkId);
		if (!next)
			continue; // not connect flow

		// insert jump label
		{
			script::sInstruction code;
			code.cmd = script::eCommand::label;
			code.str1 = MakeScopeName(*next);
			out.m_codes.push_back(code);
		}

		Node_GenCode(node, *next, pin, out);
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate while syntax code
bool cVProgFile::While_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
		return true;

	// insert while condition jump label
	{
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = MakeScopeName(node);
		out.m_codes.push_back(code);
	}

	uint condReg = 0; // condition register
	NodeInput_GenCode(node, 0, false, out);

	// insert while code

	// compare condition is zero?  (false condition)
	{
		script::sInstruction code;
		code.cmd = script::eCommand::eqic;
		code.reg1 = condReg;
		code.var1 = variant_t((int)0);
		out.m_codes.push_back(code);
	}
	{
		// jump if true, jump Exit flow
		// find Exit branch pin
		string jumpLabel;
		for (auto &pin : node.outputs)
		{
			if ((ePinType::Flow == pin.type)
				&& (pin.name == "Exit"))
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
			// no branch node, jump blank code
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = "blank";
			out.m_codes.push_back(code);
			//common::dbg::Logc(1, "cVProgFile::GenerateCode_While, no branch label\n");
		}
		else
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = jumpLabel;
			out.m_codes.push_back(code);
		}
	}

	// next flow node (generate Loop branch node)
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "Loop"))
			{
				// push instruction index (not determine)
				const uint addressIdx = out.m_codes.size();
				{
					script::sInstruction code;
					code.cmd = script::eCommand::pushic;
					code.var1 = 0; // not determine return address
					out.m_codes.push_back(code);
				}

				Node_GenCode(node, *next, pin, out);

				// update jump address
				out.m_codes[addressIdx].var1 = (int)out.m_codes.size();
				// pop return jump address
				out.m_codes.push_back({ script::eCommand::pop });
			}
		}
	}

	// Insert Jump While Condition code
	{
		script::sInstruction code;
		code.cmd = script::eCommand::jmp;
		code.str1 = MakeScopeName(node);
		out.m_codes.push_back(code);
	}

	// generate Exit branch node
	// seperate from this node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "Exit"))
			{
				// insert jump label
				{
					script::sInstruction code;
					code.cmd = script::eCommand::label;
					code.str1 = MakeScopeName(*next);
					out.m_codes.push_back(code);
				}

				Node_GenCode(node, *next, pin, out);
				break;
			}
		}
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate for-loop syntax code
bool cVProgFile::ForLoop_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
		return true;

	NodeInput_GenCode(node, 0, true, out);

	// initialize index variable (First Index -> Index)
	{
		script::sInstruction code;
		code.cmd = script::eCommand::geti;
		code.str1 = MakeScopeName(node);
		code.str2 = "First Index";
		code.reg1 = 0;
		out.m_codes.push_back(code);
	}
	{
		script::sInstruction code;
		code.cmd = script::eCommand::seti;
		code.str1 = MakeScopeName(node);
		code.str2 = "Index";
		code.reg1 = 0;
		out.m_codes.push_back(code);
	}

	// insert for-loop condition jump label
	{
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = MakeScopeName(node) + "-cond";
		out.m_codes.push_back(code);
	}

	// load last index
	{
		// load index variable
		script::sInstruction code;
		code.cmd = script::eCommand::geti;
		code.str1 = MakeScopeName(node);
		code.str2 = "Last Index";
		code.reg1 = 0;
		out.m_codes.push_back(code);
	}

	{
		// load index variable
		script::sInstruction code;
		code.cmd = script::eCommand::geti;
		code.str1 = MakeScopeName(node);
		code.str2 = "Index";
		code.reg1 = 1;
		out.m_codes.push_back(code);
	}

	// compare loop condition, last >= index
	{
		script::sInstruction code;
		code.cmd = script::eCommand::lesi; // (last < index) -> jump exit label
		code.reg1 = 0;
		code.reg2 = 1;
		out.m_codes.push_back(code);
	}
	//~insert condition code

	// compare condition is false(zero)?  (false condition)
	{
		// jump Exit flow
		// find Exit branch pin
		string exitLabel;
		for (auto &pin : node.outputs)
		{
			if ((ePinType::Flow == pin.type)
				&& (pin.name == "Exit"))
			{
				sNode *next = nullptr; // next node
				sPin *np = nullptr; // next pin
				const int linkId = pin.links.empty() ? -1 : pin.links.front();
				std::tie(next, np) = FindContainPin(linkId);
				if (next)
					exitLabel = MakeScopeName(*next);
			}
		}

		if (exitLabel.empty())
		{
			// no branch node, jump blank code
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = "blank";
			out.m_codes.push_back(code);
			//common::dbg::Logc(1, "cVProgFile::GenerateCode_ForLoop, no branch label\n");
		}
		else
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = exitLabel;
			out.m_codes.push_back(code);
		}
	}

	// next flow node (generate Loop branch node)
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "Loop"))
			{
				// push instruction index (not determine)
				const uint addressIdx = out.m_codes.size();
				{
					script::sInstruction code;
					code.cmd = script::eCommand::pushic;
					code.var1 = 0; // not determine return address
					out.m_codes.push_back(code);
				}

				Node_GenCode(node, *next, pin, out);

				// update jump address
				out.m_codes[addressIdx].var1 = (int)out.m_codes.size();
				// pop return jump address
				out.m_codes.push_back({ script::eCommand::pop });
			}
		}
	}

	// increment Index Variable
	// load index variable
	{
		script::sInstruction code;
		code.cmd = script::eCommand::geti;
		code.str1 = MakeScopeName(node);
		code.str2 = "Index";
		code.reg1 = 0;
		out.m_codes.push_back(code);
	}
	// load 1 to reg1
	{
		script::sInstruction code;
		code.cmd = script::eCommand::ldic;
		code.reg1 = 1;
		code.var1 = _variant_t((int)1);
		out.m_codes.push_back(code);
	}
	// add reg9 = index + 1
	{
		script::sInstruction code;
		code.cmd = script::eCommand::addi;
		code.reg1 = 0;
		code.reg2 = 1;
		out.m_codes.push_back(code);
	}
	// store index symboltable
	{
		script::sInstruction code;
		code.cmd = script::eCommand::seti;
		code.str1 = MakeScopeName(node);
		code.str2 = "Index";
		code.reg1 = 9;
		out.m_codes.push_back(code);
	}

	// Insert Jump For-Loop Condition code
	{
		script::sInstruction code;
		code.cmd = script::eCommand::jmp;
		code.str1 = MakeScopeName(node) + "-cond";
		out.m_codes.push_back(code);
	}

	// generate Exit branch node
	// seperate from this node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && (pin.name == "Exit"))
			{
				// insert jump label
				{
					script::sInstruction code;
					code.cmd = script::eCommand::label;
					code.str1 = MakeScopeName(*next);
					out.m_codes.push_back(code);
				}

				Node_GenCode(node, *next, pin, out);
				break;
			}
		}
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate sequence code
bool cVProgFile::Sequence_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	RETV(eNodeType::Control != node.type, false);
	if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
		return true;

	// generate output node
	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow == pin.type)
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (next && np)
			{
				// push instruction index (not determine)
				const uint addressIdx = out.m_codes.size();
				{
					script::sInstruction code;
					code.cmd = script::eCommand::pushic;
					code.var1 = 0; // not determine return address
					out.m_codes.push_back(code);
				}

				Node_GenCode(node, *next, pin, out);

				// update jump address
				out.m_codes[addressIdx].var1 = (int)out.m_codes.size();
				// pop return jump address
				out.m_codes.push_back({ script::eCommand::pop });
			}
		}
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate intermediate code, operator node
bool cVProgFile::Operator_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	using namespace common::script;

	// node enter comment, to step debugging, and node break point
	{
		script::sInstruction inst;
		inst.cmd = script::eCommand::cmt;
		inst.str1 = "node enter";
		inst.reg1 = node.id;
		out.m_codes.push_back(inst);
	}

	NodeInput_GenCode(node, 8, false, out);

	// get operator type (by input pin type)
	eSymbolType::Enum symbType = eSymbolType::None;
	for (auto &pin : node.inputs)
	{
		if (pin.typeValues.empty())
			continue;

		symbType = pin.typeValues[0];
		if (eSymbolType::None != symbType)
			break;
		//vt = vprog::GetPin2VarType(pin.type);
		//if (VT_EMPTY != vt)
		//	break;
	}

	// get output pin name
	const string outputName = (node.outputs.size() > 0)? node.outputs[0].name : "O";

	int opType = 0; // mathOp:0, CompareOp:1, Negate:2, LogicOp:3
	bool isNegateCmp = false; // !=

	// insert compare code
	if (node.name == "<")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Int: code.cmd = script::eCommand::lesi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::lesf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 1;
	}
	else if (node.name == "<=")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Int: code.cmd = script::eCommand::leqi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::leqf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 1;
	}
	else if (node.name == ">")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Int: code.cmd = script::eCommand::gri; break;
		case eSymbolType::Float: code.cmd = script::eCommand::grf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 1;
	}
	else if (node.name == ">=")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Int: code.cmd = script::eCommand::greqi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::greqf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 1;
	}
	else if ((node.name == "==") || (node.name == "!="))
	{
		if (node.name == "!=")
			isNegateCmp = true;

		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Bool:
		case eSymbolType::Int: code.cmd = script::eCommand::eqi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::eqf; break;
		case eSymbolType::String: code.cmd = script::eCommand::eqs; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 1;
	}
	else if ((node.name == "&&") 
		|| (node.name == "||")
		|| (node.name == "and")
		|| (node.name == "or")
		)
	{
		script::sInstruction code;
		code.cmd = ((node.name == "&&") || (node.name == "and")) ?
			script::eCommand::opand : script::eCommand::opor;
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 3;
	}
	else if (node.name == "+")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Bool:
		case eSymbolType::Int: code.cmd = script::eCommand::addi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::addf; break;
		case eSymbolType::String: code.cmd = script::eCommand::adds; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 0;
	}
	else if (node.name == "-")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Bool:
		case eSymbolType::Int: code.cmd = script::eCommand::subi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::subf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 0;
	}
	else if ((node.name == "X") || (node.name == "x") || (node.name == "*"))
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Bool:
		case eSymbolType::Int: code.cmd = script::eCommand::muli; break;
		case eSymbolType::Float: code.cmd = script::eCommand::mulf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 0;
	}
	else if (node.name == "/")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Bool:
		case eSymbolType::Int: code.cmd = script::eCommand::divi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::divf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // val8
		code.reg2 = 9; // val9
		out.m_codes.push_back(code);
		opType = 0;
	}
	else if (node.name == "!")
	{
		script::sInstruction code;
		code.cmd = script::eCommand::negate;
		code.reg1 = 8; // val8
		out.m_codes.push_back(code);
		opType = 2;
	}

	// compare operator? load compare flag
	if (0 == opType) // math operator?
	{
		// insert calculate data to symboltable
		{
			script::sInstruction code;
			switch (symbType)
			{
			case eSymbolType::Bool:
			case eSymbolType::Int: code.cmd = script::eCommand::seti; break;
			case eSymbolType::Float: code.cmd = script::eCommand::setf; break;
			case eSymbolType::String: code.cmd = script::eCommand::sets; break;
			default:
				common::dbg::Logc(1
					, "Error!! cVProgFile::Generate_Operator(), compare type invalid\n");
				break;
			}
			code.str1 = MakeScopeName(node);
			code.str2 = outputName;
			code.reg1 = 9;
			out.m_codes.push_back(code);
		}
	}
	else if (1 == opType)// compare operator?
	{
		{
			script::sInstruction code;
			code.cmd = isNegateCmp? script::eCommand::ldncmp : script::eCommand::ldcmp;
			code.reg1 = 9;
			out.m_codes.push_back(code);
		}

		// insert compare data to symboltable
		{
			script::sInstruction code;
			code.cmd = script::eCommand::setb;
			code.str1 = MakeScopeName(node);
			code.str2 = outputName;
			code.reg1 = 9;
			out.m_codes.push_back(code);
		}
	}
	else if (2 == opType) // negate operator?
	{
		script::sInstruction code;
		code.cmd = script::eCommand::setb;
		code.str1 = MakeScopeName(node);
		code.str2 = outputName;
		code.reg1 = 9;
		out.m_codes.push_back(code);
	}
	else if (3 == opType) // logic operator?
	{
		{
			script::sInstruction code;
			code.cmd = script::eCommand::ldcmp;
			code.reg1 = 9;
			out.m_codes.push_back(code);
		}

		// insert compare data to symboltable
		{
			script::sInstruction code;
			code.cmd = script::eCommand::setb;
			code.str1 = MakeScopeName(node);
			code.str2 = outputName;
			code.reg1 = 9;
			out.m_codes.push_back(code);
		}
	}

	return true;
}


// generate intermediate code, variable node
bool cVProgFile::Variable_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Operator, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// nothing to operation

	return true;
}


// generate intermediate code, pin
bool cVProgFile::Pin_GenCode(const sNode &node, const sPin &pin, const uint reg
	, OUT common::script::cIntermediateCode &out)
{
	return Pin2_GenCode(pin.kind, node, pin, reg, out);
}


// generate intermediate code, pin
bool cVProgFile::Pin2_GenCode(const ePinKind::Enum kind
	, const sNode &node, const sPin &pin, const uint reg
	, OUT common::script::cIntermediateCode &out)
{
	if (ePinKind::Input == kind)
	{
		script::sInstruction code;
		switch (pin.type)
		{
		case ePinType::Bool: code.cmd = script::eCommand::setb; break;
		case ePinType::Enums:
		case ePinType::Int: code.cmd = script::eCommand::seti; break;
		case ePinType::Float: code.cmd = script::eCommand::setf; break;
		case ePinType::String: code.cmd = script::eCommand::sets; break;
		case ePinType::Array: code.cmd = script::eCommand::seta; break;
		case ePinType::Map: code.cmd = script::eCommand::setm; break;
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
		script::sInstruction code;
		switch (pin.type)
		{
		case ePinType::Bool: code.cmd = script::eCommand::getb; break;
		case ePinType::Enums:
		case ePinType::Int: code.cmd = script::eCommand::geti; break;
		case ePinType::Float: code.cmd = script::eCommand::getf; break;
		case ePinType::String: code.cmd = script::eCommand::gets; break;
		case ePinType::Array: code.cmd = script::eCommand::geta; break;
		case ePinType::Map: code.cmd = script::eCommand::getm; break;
		default:
			return false;
		}

		code.str1 = pin.varName.empty() ? MakeScopeName(node) : pin.varName;
		code.str2 = pin.name.c_str();
		code.reg1 = reg;
		out.m_codes.push_back(code);
	}
	return true;
}


// generate intermedate code, load temporal value to register
bool cVProgFile::TemporalPin_GenCode(const sNode &node, const sPin &pin
	, const uint reg, OUT common::script::cIntermediateCode &out)
{
	const _bstr_t emptyStr(" "); // avoid crash local variable (1 space string)
	if (ePinKind::Input == pin.kind)
	{
		script::sInstruction code;
		code.reg1 = reg;

		// is exist symboltable?
		// varname = node.name + node.id + pin.name
		const string name = script::cSymbolTable::MakeScopeName(node.name, node.id);
		script::sVariable *var = m_variables.FindVarInfo(name, pin.name);

		if (var)
		{
			switch (pin.type)
			{
			case ePinType::Bool: code.cmd = script::eCommand::ldbc; break;
			case ePinType::Enums:
			case ePinType::Int: code.cmd = script::eCommand::ldic; break;
			case ePinType::Float: code.cmd = script::eCommand::ldfc; break;
			case ePinType::String: code.cmd = script::eCommand::ldsc; break;
			case ePinType::Array: code.cmd = script::eCommand::ldac; break;
			case ePinType::Map: code.cmd = script::eCommand::ldmc; break;
			default: return false;
			}
			code.var1 = var->var;
		}
		else 
		{
			switch (pin.type)
			{
			case ePinType::Bool: 
				code.cmd = script::eCommand::ldbc;
				code.var1 = false;
				break;
			case ePinType::Enums:
			case ePinType::Int: 
				code.cmd = script::eCommand::ldic; 
				code.var1 = (int)0;
				break;
			case ePinType::Float: 
				code.cmd = script::eCommand::ldfc; 
				code.var1 = (float)0.f;
				break;
			case ePinType::String: 
				code.cmd = script::eCommand::ldsc; 
				code.var1 = emptyStr;
				break;
			case ePinType::Array:
				code.cmd = script::eCommand::ldac;
				code.var1 = (int)0;
				break;
			case ePinType::Map:
				code.cmd = script::eCommand::ldmc;
				code.var1 = (int)0;
				break;
			default:
				return false;
			}
		}
		out.m_codes.push_back(code);
	}
	else // output
	{
		// nothing~
	}

	return true;
}


// insert flow debug information
bool cVProgFile::DebugInfo_GenCode(const sPin &from, const sPin &to
	, OUT common::script::cIntermediateCode &out)
{
	script::sInstruction inst;
	inst.cmd = script::eCommand::cmt;
	inst.str1 = "flow";
	inst.reg1 = from.id;
	inst.reg2 = to.id;
	out.m_codes.push_back(inst);
	return true;
}


// insert flow debug information, if from-to node was linked
bool cVProgFile::DebugInfo_GenCode(const sNode &from, const sNode &to
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	RETV(from.id == 0, false);

	int fromId = fromPin.id;
	int toId = -1;
	for (auto &p2 : to.inputs)
	{
		if (p2.type != ePinType::Flow)
			continue;
		for (auto &id : p2.links)
		{
			if (fromPin.id == id)
			{
				// two node link with flow pin
				toId = p2.id;
				break;
			}
		}
	}

	if ((fromId != -1) && (toId != -1))
	{
		script::sInstruction inst;
		inst.cmd = script::eCommand::cmt;
		inst.str1 = "flow";
		inst.reg1 = fromId;
		inst.reg2 = toId;
		out.m_codes.push_back(inst);
	}

	return true;
}

// if return false, multiple enter, no need generate this node code
bool cVProgFile::NodeEnter_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	DebugInfo_GenCode(prevNode, node, fromPin, out);

	// todo: visit node, function call mechanism
	if (m_visit.find(node.id) != m_visit.end())
	{
		// if already generate this code, jump this node
		script::sInstruction code;
		code.cmd = script::eCommand::jmp;
		code.str1 = MakeScopeName(node) + "_Header";
		out.m_codes.push_back(code);
		return false;
	}

	// check macro function
	// no input flow slot? macro function!
	const bool isFunction = (node.inputs.size() > 0) && (node.inputs[0].type == ePinType::Flow);
	if (isFunction)
		m_visit.insert(node.id);

	// if multiple flow link, insert node jump label
	if (GetInputFlowCount(node) >= 2)
	{
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = MakeScopeName(node) + "_Header";
		out.m_codes.push_back(code);
	}

	// node enter comment, to step debugging, and node break point
	{
		script::sInstruction inst;
		inst.cmd = script::eCommand::cmt;
		inst.str1 = "node enter";
		inst.reg1 = node.id;
		out.m_codes.push_back(inst);
	}

	return true;
}


// generate node escape code
bool cVProgFile::NodeEscape_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	// check macro function
	// no input flow slot? macro function!
	const bool isMacro = (node.inputs.size() > 0) && (node.inputs[0].type != ePinType::Flow);
	if (isMacro)
		return true; // macro function no need return

	// check sret/nop duplicate
	bool isAlreadyEscapeCode = false;
	const uint csize = out.m_codes.size();
	if (csize >= 2) 
	{
		isAlreadyEscapeCode = (out.m_codes[csize - 2].cmd == script::eCommand::sret)
			&& (out.m_codes[csize - 1].cmd == script::eCommand::nop);
	}

	if (!isAlreadyEscapeCode)
	{
		out.m_codes.push_back({ script::eCommand::sret });
		out.m_codes.push_back({ script::eCommand::nop });
	}
	return true;
}


// generate node input pin code
// reg: load input pin variable register
// isUpdateInputPin: set input pin symbol table
bool cVProgFile::NodeInput_GenCode(const sNode &node, const uint reg
	, const bool isUpdateInputPin, OUT common::script::cIntermediateCode &out)
{
	// calc input pin node
	for (auto &pin : node.inputs)
	{
		if (IsIgnoreInputPin(pin.type))
			continue;

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (!prev)
			continue;

		if (m_visit.end() == m_visit.find(prev->id))
			Node_GenCode(nullNode, *prev, pin, out);
	}

	// load register prev node output pin
	// isUpdateInputPin == true? store node input symboltable
	uint r = reg; // start register number
	for (auto &pin : node.inputs)
	{
		if (IsIgnoreInputPin(pin.type))
			continue;

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			Pin_GenCode(*prev, *pp, r, out); // load register from prev output pin
			if (isUpdateInputPin)
				Pin_GenCode(node, pin, r, out); // store data to input pin symboltable
			DebugInfo_GenCode(*pp, pin, out); // insert debuginfo
			++r;
		}
		else
		{
			// load temporal variable
			TemporalPin_GenCode(node, pin, r, out);
			if (isUpdateInputPin)
				Pin_GenCode(node, pin, r, out); // store data to input pin symboltable
			++r;
		}
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


// make scope name
// event scopeName: nodeName
// scopeName: nodeName '-' nodeId
// unique scopeName: nodeName '-' nodeId '-' uniqueId
string cVProgFile::MakeScopeName(const sNode &node
	, const int uniqueId // =-1
)
{
	if (eNodeType::Event == node.type)
	{
		return node.name.c_str();
	}
	else
	{
		string scopeName = script::cSymbolTable::MakeScopeName(node.name.c_str(), node.id).c_str();
		if (uniqueId >= 0)
			return common::format("%s-%d", scopeName.c_str(), uniqueId);
		else
			return scopeName;
	}
}


// return Input Flow links count
uint cVProgFile::GetInputFlowCount(const sNode &node)
{
	uint cnt = 0;
	for (auto &pin : node.inputs)
	{
		if (ePinType::Flow == pin.type)
		{
			cnt = pin.links.size();
			break;
		}
	}			
	return cnt;
}


// return matching name in node input pin 
const cVProgFile::sPin* cVProgFile::GetInputPin(const sNode &node, const vector<string> &names)
{
	for (auto &pin : node.inputs)
	{
		auto it = std::find(names.begin(), names.end(), pin.name);
		if (names.end() != it)
			return &pin;
	}
	return nullptr;
}


// is ignore generate code? especially input pin
bool cVProgFile::IsIgnoreInputPin(const ePinType::Enum type)
{
	switch (type)
	{
	case ePinType::Flow:
	case ePinType::Function:
	case ePinType::Delegate:
		return true;
	}
	return false;
}


void cVProgFile::Clear()
{
	m_nodes.clear();
	m_variables.Clear();
}
