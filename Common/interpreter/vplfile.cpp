
#include "stdafx.h"
#include "vplfile.h"

using namespace vpl;
namespace script = common::script;

const static cVplFile::sNode nullNode = { 0, };


cVplFile::cVplFile()
	: m_jmpLabelSeedId(0)
	, m_syncId(1)
{
}

cVplFile::~cVplFile()
{
	Clear();
}


// make *.vpl parsine rule
void cVplFile::MakeParsingRule(OUT vector<common::cSimpleData2::sRule> &out)
{
	out.push_back({ 0, "node", 1, -1 });
	out.push_back({ 1, "output", 2, -1 });
	out.push_back({ 2, "output", 2, 1 });
	out.push_back({ 3, "output", 2, 1 });
	out.push_back({ 1, "input", 3, -1 });
	out.push_back({ 3, "input", 3, 1 });
	out.push_back({ 2, "input", 3, 1 });
	out.push_back({ 1, "node", 1, 0 });
	out.push_back({ 2, "node", 1, 0 });
	out.push_back({ 3, "node", 1, 0 });
	out.push_back({ 4, "node", 1, 0 });
	out.push_back({ 5, "node", 1, 0 });
	out.push_back({ 7, "node", 1, 0 });
	out.push_back({ 1, "symbol", 4, 0 });
	out.push_back({ 2, "symbol", 4, 0 });
	out.push_back({ 3, "symbol", 4, 0 });
	out.push_back({ 4, "symbol", 4, 0 });
	out.push_back({ 5, "symbol", 4, 0 });
	out.push_back({ 7, "symbol", 4, 0 });
	out.push_back({ 0, "define", 5, 0 });
	out.push_back({ 1, "define", 5, 0 });
	out.push_back({ 2, "define", 5, 0 });
	out.push_back({ 3, "define", 5, 0 });
	out.push_back({ 4, "define", 5, 0 });
	out.push_back({ 5, "define", 5, 0 });
	out.push_back({ 7, "define", 5, 0 });
	out.push_back({ 5, "attr", 6, -1 });
	out.push_back({ 6, "attr", 6, 5 });
	out.push_back({ 6, "node", 1, 0 });
	out.push_back({ 6, "symbol", 4, 0 });
	out.push_back({ 6, "define", 5, 0 });
	out.push_back({ 6, "initvar", 7, 0 });
	out.push_back({ 1, "initvar", 7, 0 });
	out.push_back({ 2, "initvar", 7, 0 });
	out.push_back({ 3, "initvar", 7, 0 });
	out.push_back({ 4, "initvar", 7, 0 });
	out.push_back({ 5, "initvar", 7, 0 });
	out.push_back({ 7, "initvar", 7, 0 });
}


// read *.vpl file
bool cVplFile::Read(const StrPath &fileName)
{
	Clear();

	vector<common::cSimpleData2::sRule> rules;
	MakeParsingRule(rules);
	common::cSimpleData2 sdata(rules);
	if (!sdata.Read(fileName))
		return false;

	m_fileName = fileName.GetFullFileName();
	return Load(sdata);
}


// load from input stream
bool cVplFile::Load(std::istream &is)
{
	Clear();

	vector<common::cSimpleData2::sRule> rules;
	MakeParsingRule(rules);
	common::cSimpleData2 sdata(rules);
	if (!sdata.Read(is))
		return false;

	m_fileName.clear();
	return Load(sdata);
}


// load from simpledata2
bool cVplFile::Load(common::cSimpleData2 &sdata)
{
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
			assert(!"cVplFile::Read() Error, not defined node type");
			break;
		}
	} //~for nodes, type, symbol
	return true;
}


// add node from parsed *.vpl file
bool cVplFile::AddNode(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	sNode node;
	node.type = vpl::StrToNodeType(sdata.Get<string>(p, "type", "Event"));
	node.id = sdata.Get<int>(p, "id", 0);
	node.name = sdata.Get<string>(p, "name", "name");
	node.typeStr = sdata.Get<string>(p, "typeStr", node.name);
	node.labelName = sdata.Get<string>(p, "labelname", "");

	for (auto &c : p->children)
	{
		if ((c->name == "output") || (c->name == "input"))
		{
			sPin pin;

			const string type = sdata.Get<string>(c, "type", " ");
			const string typeStr = sdata.Get<string>(c, "typeStr", "");
			pin.type = vpl::StrToPinType(type);
			pin.typeStr = typeStr;
			common::script::ParseTypeString(typeStr, pin.typeValues);

			if (pin.type == ePinType::None) // not found type, check enum type?
			{
				if (m_variables.FindSymbol(type))
				{
					pin.type = ePinType::Enums;
				}
				else
				{
					assert(!"cVplFile::Read() Error!, not defined type name");
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

	// check function, macro
	// if no flow node, macro type
	if (eNodeType::Function == node.type)
	{
		bool hasFlow = false;
		for (auto &pin : node.inputs)
		{
			if (ePinType::Flow == pin.type)
			{
				hasFlow = true;
				break;
			}
		}
		if (!hasFlow) // no flow input? macro type, no function type
			node.type = eNodeType::Macro;
	}

	m_nodes.push_back(node);
	return true;
}


// add variable from parsed *.vpl file
bool cVplFile::AddVariable(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	const string scopeName = sdata.Get<string>(p, "scopename", "");
	const string name = sdata.Get<string>(p, "name", "");
	const string typeStr = sdata.Get<string>(p, "typeStr", "");
	return AddVariable2(scopeName, name, typeStr, sdata, p);
}


// add symbole
bool cVplFile::AddSymbol(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	sPin pin;
	pin.name = "@symbol@";
	pin.id = sdata.Get<int>(p, "id", 0);

	sNode *n = nullptr;
	sPin *pp = nullptr;
	std::tie(n, pp) = FindContainPin(pin.id);
	if (!n || !pp)
	{
		common::dbg::Logc(1, "Error!! cVplFile::AddSymbol() symbol parse error!!\n");
		return false;
	}

	const string scopeName = MakeScopeName(*n);
	const string varName = pp->name.c_str();
	return AddVariable2(scopeName, varName, pp->typeStr, sdata, p);
}


// add variable
bool cVplFile::AddVariable2(const string &scopeName, const string &name
	, const string &typeStr
	, common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
{
	if (scopeName.empty() || name.empty())
	{
		common::dbg::Logc(1, "Error!! cVplFile::AddVariable2() symbol parse error!! 1\n");
		return false; // error
	}

	//ePinType::Enum pinType;
	using namespace common::script;
	vector<eSymbolType> typeValues;
	if (!ParseTypeString(typeStr, typeValues)) 
	{
		common::dbg::Logc(1
			, "Error!! cVplFile::AddVariable2() symbol parse error!! 1-1, typeStr = %s\n"
			, typeStr.c_str());
		return false;
	}
	const common::script::eSymbolType symbType = typeValues.front();

	variant_t val;
	switch (symbType)
	{
	case eSymbolType::Bool: val = sdata.Get<bool>(p, "value", false); break;
	case eSymbolType::Enums:
	case eSymbolType::Int: val = sdata.Get<int>(p, "value", 0); break;
	case eSymbolType::Float: val = sdata.Get<float>(p, "value", 0.f); break;
	case eSymbolType::String:
	{
		// replace <p> => \n to avoid multiline
		string str = sdata.Get<string>(p, "value", "");
		common::replaceAll(str, "<p>", "\n");
		val = common::str2variant(VT_BSTR, str);
	}
	break;
	case eSymbolType::Array:
	{
		if (!m_variables.InitArray(scopeName, name, typeStr))
		{
			assert(!"cNodefile::AddVariable2() symbol parse error!! 3");
			return false;
		}
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
		common::dbg::Logc(1, "Error!! cVplFile::AddVariable2() symbol parse error!! 2\n");
		return false;
	}

	// initialize array
	if (symbType == eSymbolType::Array)
	{
		string str = sdata.Get<string>(p, "value", "");
		common::replaceAll(str, "<p>", ""); // remove <p> character
		vector<string> toks;
		common::tokenizer(str, ",", "", toks);

		sVariable *var = m_variables.FindVarInfo(scopeName, name);
		if (var && (typeValues.size() > 1))
		{
			const eSymbolType itemType = typeValues[1]; // array item type
			for (auto& tok : toks)
			{
				common::replaceAll(tok, "'", ""); // remove \' character
				switch (itemType)
				{
				case eSymbolType::Bool: val = (bool)atoi(tok.c_str()); break;
				case eSymbolType::Enums:
				case eSymbolType::Int: val = (int)atoi(tok.c_str()); break;
				case eSymbolType::Float: val = (float)atof(tok.c_str()); break;
				case eSymbolType::String: val = common::str2variant(VT_BSTR, tok); break;
				default: continue;
				}
				var->PushArrayElement(val);
			}
		}
	}
	// update value bool, int, float, string, enum
	else if ((symbType != eSymbolType::Map))
	{
		if (!m_variables.Set(scopeName, name, val, typeStr))
		{
			assert(!"cNodefile::AddVariable2() symbol parse error!! 5");
			return false;
		}
	}

	return true;
}


// add temporal initialize variable
bool cVplFile::AddVariable3(const string &scopeName, const string &name
	, const string &typeStr
	, const int flags //= 0
)
{
	using namespace common::script;

	if (scopeName.empty() || name.empty())
	{
		common::dbg::Logc(1, "Error!! cVplFile::AddVariable3() symbol parse error!! 1\n");
		return false; // error
	}

	vector<eSymbolType> typeValues;
	if (!ParseTypeString(typeStr, typeValues))
	{
		common::dbg::Logc(1
			, "Error!! cVplFile::AddVariable3() symbol parse error!! 1-1, typeStr = %s\n"
			, typeStr.c_str());
		return false;
	}
	const eSymbolType symbType = typeValues.front();

	variant_t val;
	switch (symbType)
	{
	case eSymbolType::Bool: val = (bool)false; break;
	case eSymbolType::Enums:
	case eSymbolType::Int: val = (int)0; break;
	case eSymbolType::Float: val = (float)0; break;
	case eSymbolType::String:
		val = common::str2variant(VT_BSTR, "");
		break;
	case eSymbolType::Array:
		if (!m_variables.InitArray(scopeName, name, typeStr))
		{
			assert(!"cNodefile::AddVariable3() symbol parse error!! 3");
			return false;
		}
		break;
	case eSymbolType::Map:
		if (!m_variables.InitMap(scopeName, name, typeStr))
		{
			assert(!"cNodefile::AddVariable3() symbol parse error!! 4");
			return false;
		}
		break;
	case eSymbolType::Any:
		return true; // nothing, anytype ignore
	default:
		common::dbg::Logc(1, "Error!! cVplFile::AddVariable3() symbol parse error!! 2\n");
		return false;
	}

	// update value bool, int, float, string, enum (array, map already setting)
	if ((symbType != eSymbolType::Array) && (symbType != eSymbolType::Map))
	{
		if (!m_variables.Set(scopeName, name, val, typeStr))
		{
			assert(!"cNodefile::AddVariable3() symbol parse error!! 5");
			return false;
		}
	}

	m_variables.SetVarFlag(scopeName, name, flags);
	return true;
}


// add define from parsed *.vpl file
bool cVplFile::AddDefine(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p)
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
		assert(!"cVplFile::Read() Error, not defined type parse");
	}

	m_variables.AddSymbol(symbol);

	return true;
}


// write *.vpl file
bool cVplFile::Write(const StrPath &fileName)
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
bool cVplFile::Write_Node(std::ostream &ofs, sNode &node)
{
	using namespace std;

	ofs << "node" << endl;
	ofs << "\t" << "type " << vpl::NodeTypeToStr(node.type) << endl;
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
		ofs << "\t\t" << "type " << vpl::PinTypeToStr(pin.type) << endl;
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
		ofs << "\t\t" << "type " << vpl::PinTypeToStr(pin.type) << endl;
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
bool cVplFile::Write_Define(std::ostream &ofs, sNode &node)
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
bool cVplFile::GenerateIntermediateCode(OUT common::script::cIntermediateCode &out)
{
	using namespace common::script;

	RETV(m_nodes.empty(), false);

	m_visit.clear(); // avoid duplicate execution
	m_genNodes.clear();  // avoid duplicate execution

	// make main entry
	{
		out.m_codes.push_back({ script::eCommand::nop }); // no operation
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = "main";
		out.m_codes.push_back(code);

		// initialize SyncOrder
		for (auto& node : m_nodes)
		{
			if ((eNodeType::Control == node.type) && (node.name == "SyncOrder"))
				SyncOrderInit_GenCode(node, out);
		}

		out.m_codes.push_back({ script::eCommand::nop });
	}

	// make intermediate code from event, widget
	for (auto &node : m_nodes)
	{
		if (eNodeType::Event == node.type)
			Event_GenCode(node, out);
		if (eNodeType::Widget == node.type)
			Widget_GenCode(node, out);
		if (eNodeType::Function == node.type)
			Callback_GenCode(node, out);
	}

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

	// make initial variable in node output pin
	// to avoid no symbol assertion 
	for (auto &node : m_nodes)
	{
		for (auto &pin : node.outputs)
		{
			if ((ePinType::Flow == pin.type) || (ePinType::Blank == pin.type))
				continue;

			string scopeName;
			// event scopename no has id because event is unique
			// except 'tick event'
			if (eNodeType::Event == node.type)
			{
				scopeName = (node.name == "Tick Event") ?
					script::cSymbolTable::MakeScopeName(node.name, node.id)
					: node.name;
			}
			else
			{
				scopeName = script::cSymbolTable::MakeScopeName(node.name, node.id);
			}

			// check input/output pin same name
			// this pin determine input pin
			auto it = std::find_if(node.inputs.begin(), node.inputs.end()
				, [&](const auto& a) { return a.name == pin.name; });
			// ouput pin flag to check function node input variable
			const int flags = (node.inputs.end() == it) ? 0x02 : 0x00;

			script::sVariable *var = m_variables.FindVarInfo(scopeName, pin.name);
			if (var) 
			{
				m_variables.SetVarFlag(scopeName, pin.name, flags);
				continue; // already exist, ignore
			}
			switch (pin.type)
			{
			case ePinType::Bool: AddVariable3(scopeName, pin.name, pin.typeStr, flags); break;
			case ePinType::Enums:
			case ePinType::Int: AddVariable3(scopeName, pin.name, pin.typeStr, flags); break;
			case ePinType::Float: AddVariable3(scopeName, pin.name, pin.typeStr, flags); break;
			case ePinType::String: AddVariable3(scopeName, pin.name, pin.typeStr, flags); break;
			case ePinType::Array: AddVariable3(scopeName, pin.name, pin.typeStr, flags); break;
			case ePinType::Map: AddVariable3(scopeName, pin.name, pin.typeStr, flags); break;
			default: break;
			}
		}
	}

	// make initial symbol (register in symboltable)
	for (auto &kv1 : m_variables.m_vars) // loop all scope
	{
		for (auto &kv2 : kv1.second) // loop scope variable
		{
			const sVariable &var = kv2.second;
			Symbol_GenCode(kv1.first, kv2.first, var, out);
		}
	}

	// make register tick event
	for (auto &node : m_nodes)
	{
		if (eNodeType::Event == node.type)
		{
			if (node.name == "Tick Event")
			{
				const string name = script::cSymbolTable::MakeScopeName(node.name, node.id);
				script::sVariable *var = m_variables.FindVarInfo(name, "interval");
				if (var)
				{
					script::sInstruction code;
					code.cmd = script::eCommand::timer1;
					code.str1 = name;
					code.var1 = var->var;
					out.m_codes.push_back(code);
				}
			}
		}
	}

	out.m_variables = m_variables;
	out.m_fileName = m_fileName.GetFileNameExceptExt2();
	out.m_fileName += ".icode";
	out.InitOptimizeInfo();

	return true;
}


// generate symbol initialize code
bool cVplFile::Symbol_GenCode(const string &scopeName, const string &varName
	, const common::script::sVariable &var
	, OUT common::script::cIntermediateCode &out)
{
	using namespace common::script;

	script::sInstruction code;
	string typeStr; // array, map type string

	const eSymbolType symbType = var.typeValues[0];
	switch (symbType)
	{
	case eSymbolType::Bool: code.cmd = script::eCommand::symbolb; break;
	case eSymbolType::Int: code.cmd = script::eCommand::symboli; break;
	case eSymbolType::Float: code.cmd = script::eCommand::symbolf; break;
	case eSymbolType::String: code.cmd = script::eCommand::symbols; break;
	case eSymbolType::Array:
	{
		typeStr = var.type;
		const eSymbolType elementType = var.typeValues[1]; //array<type>
		switch (elementType)
		{
		case eSymbolType::Bool: code.cmd = script::eCommand::symbolab; break;
		case eSymbolType::Int: code.cmd = script::eCommand::symbolai; break;
		case eSymbolType::Float: code.cmd = script::eCommand::symbolaf; break;
		case eSymbolType::String: code.cmd = script::eCommand::symbolas; break;
		case eSymbolType::None: code.cmd = script::eCommand::none; break;
		default: 
			common::dbg::Logc(3,
				"Error!! cVplFile::Symbol_GenCode(), invalid symbol type2\n");
			break;
		}
	}
	break;
	case eSymbolType::Map:
	{
		typeStr = var.type;
		const eSymbolType valueType = var.typeValues[2]; //map<string,type>
		switch (valueType)
		{
		case eSymbolType::Bool: code.cmd = script::eCommand::symbolmb; break;
		case eSymbolType::Int: code.cmd = script::eCommand::symbolmi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::symbolmf; break;
		case eSymbolType::String: code.cmd = script::eCommand::symbolms; break;
		case eSymbolType::Array: code.cmd = script::eCommand::symbolma; break;
		case eSymbolType::None: code.cmd = script::eCommand::none; break;
		default:
			common::dbg::Logc(3,
				"Error!! cVplFile::Symbol_GenCode(), invalid symbol type3\n");
			break;
		}
	}
	break;
	default:
		common::dbg::Logc(3,
			"Error!! cVplFile::Symbol_GenCode(), invalid symbol type1\n");
		break;
	}

	if (script::eCommand::none != code.cmd)
	{
		code.str1 = scopeName; // scopename
		code.str2 = varName; // varname
		code.str3 = typeStr; // array, map type string
		code.var1 = var.var;
		code.reg1 = var.flags;
		out.m_codes.push_back(code);
	}
	return true;
}


// generate intermediate code, event node
bool cVplFile::Event_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Event, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	out.m_codes.push_back({ script::eCommand::nop });

	// labelName: node.name '_' node.id	 
	// make label name, if need unique event label name, update from node.labelName
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
				common::dbg::Logc(3, "Error!! cVplFile::Event_GenCode code, flow link too many setting \n");

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


// generate intermediate code, widget node
bool cVplFile::Widget_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Widget, false);
	RETV(node.name == "Comment", false); // comment widget ignore
	if (node.inputs.size() > 0) // listbox widget
		return Widget_GenCode2(node, out);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// button widget
	out.m_codes.push_back({ script::eCommand::nop });

	// labelName: node.name '_' node.id	 
	// make label name, if need unique event label name, update from node.labelName
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
				common::dbg::Logc(3, "Error!! cVplFile::Widget_GenCode, flow link too many setting \n");

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


// generate intermediate code, listbox widget 
bool cVplFile::Widget_GenCode2(const sNode& node
	, OUT common::script::cIntermediateCode& out)
{
	RETV(node.type != eNodeType::Widget, false);
	RETV(node.name == "Comment", false); // comment widget ignore
	RETV(node.inputs.size() == 0, false); // only output event widget allow
	RETV(node.outputs.size() < 2, false);

	for (auto& pin : node.outputs)
	{
		if (ePinType::Flow != pin.type)
			continue;
		if (pin.links.empty())
			continue;
		if (common::trim2(pin.name).empty())
			continue; // ignore blank name output widget (main output flow pin)
		
		if (pin.links.size() >= 2)
			common::dbg::Logc(3, "Error!! cVplFile::Widget_GenCode2, flow link too many setting \n");

		out.m_codes.push_back({ script::eCommand::nop });

		// labelName: node.name '_' node.id	 
		// make label name, if need unique event label name, update from node.labelName
		const string labelName = 
			script::cSymbolTable::MakeScopeName2(node.name, node.id, pin.name);
		out.m_codes.push_back({ script::eCommand::label, labelName });

		// clear stack
		out.m_codes.push_back({ script::eCommand::cstack });

		sNode* next = nullptr;
		sPin* np = nullptr;
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(next, np) = FindContainPin(linkId);
		if (!next)
			continue; // error occurred!!

		Node_GenCode(node, *next, pin, out);

		// return, if has jump address
		out.m_codes.push_back({ script::eCommand::sret });

		// finish, stop execute
		out.m_codes.push_back({ script::eCommand::nop });
	}

	return true;
}


// generate intermediate code, node
bool cVplFile::Node_GenCode(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	switch (node.type)
	{
	case eNodeType::Widget:
	case eNodeType::Function:
	case eNodeType::Macro: Function_GenCode(prevNode, node, fromPin, out); break;
	case eNodeType::Control: Control_GenCode(prevNode, node, fromPin, out); break;
	case eNodeType::Operator: Operator_GenCode(node, out); break;
	case eNodeType::Variable: Variable_GenCode(node, out); break;
	case eNodeType::Event: break;
	default:
		common::dbg::Logc(1
			, "Error!! cVplFile::Node_GenCode(), node type is invalid %d\n"
			, node.type);
		return false; // nothing generate this type
	}
	return true;
}


// generate intermediate code, macro, operator node code only
bool cVplFile::Node_GenCode2(const sNode &prevNode, const sNode &node
	, const sPin &fromPin, OUT common::script::cIntermediateCode &out)
{
	switch (node.type)
	{
	case eNodeType::Macro: Function_GenCode(prevNode, node, fromPin, out); break;
	case eNodeType::Operator: Operator_GenCode(node, out); break;
	case eNodeType::Function:
	case eNodeType::Control:
	case eNodeType::Variable: 
	case eNodeType::Event: 
	case eNodeType::Widget: break;
	default:
		common::dbg::Logc(1
			, "Error!! cVplFile::Node_GenCode2(), %s, node type is invalid %d\n"
			, m_fileName.c_str(), node.type);
		return false; // nothing generate this type
	}
	return true;
}


// generate intermediate code, function node type
bool cVplFile::Function_GenCode(const sNode &prevNode, const sNode &node
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
			case ePinType::Array: code.cmd = script::eCommand::seta; break;
			case ePinType::Map: code.cmd = script::eCommand::setm; break;
			default:
				return false;
			}

			// 2020-11-23, variable name data from desc
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
				, "Error!! cVplFile::Function_GenCode(), set node, no input pin, %s\n"
				, m_fileName.c_str());
		}

		// Set node has out pint?
		for (auto &pin : node.outputs)
			if (ePinType::Flow != pin.type)
				Pin2_GenCode(ePinKind::Input, node, pin, reg, out);
	}
	else if (node.name == "Delay")
	{
		{
			// call 'Delay' function to notify timeout
			script::sInstruction code;
			code.cmd = script::eCommand::call;
			code.str1 = MakeScopeName(node);
			out.m_codes.push_back(code);
		}
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

	// next flow pin
	for (auto &pin : node.outputs)
	{
		if ((ePinType::Flow == pin.type) && common::trim2(pin.name).empty())
		{
			sNode *next = nullptr; // next node
			sPin *np = nullptr; // next pin
			const int linkId = pin.links.empty() ? -1 : pin.links.front();
			std::tie(next, np) = FindContainPin(linkId);
			if (!next)
				continue;

			Node_GenCode(node, *next, pin, out);
			NodeEscape_GenCode(node, out);
			break;
		}
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate intermediate code, control node
bool cVplFile::Control_GenCode(const sNode &prevNode, const sNode &node
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
	else if (node.name == "Sync")
		Sync_GenCode(prevNode, node, fromPin, out);
	else if (node.name == "SyncOrder")
		SyncOrder_GenCode(prevNode, node, fromPin, out);

	return true;
}


// generate intermediate code, control node
bool cVplFile::Branch_GenCode(const sNode &prevNode, const sNode &node
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
			//common::dbg::Logc(1, "cVplFile::Branch_GenCode, no branch label\n");
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
bool cVplFile::Switch_GenCode(const sNode &prevNode, const sNode &node
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
	if (vpl::ePinType::Any == selPin->type)
	{
		// empty input slot, nothing to do
		NodeEscape_GenCode(node, out);
		return true;
	}

	common::script::cSymbolTable::sSymbol *symbol = nullptr;
	if (ePinType::Enums == selPin->type)
		symbol = m_variables.FindSymbol(selPin->typeStr.c_str());

	for (auto &pin : node.outputs)
	{
		if (ePinType::Flow != pin.type)
			continue;
		if ((pin.name == "Default") || (pin.name == "default"))
			continue; // not yet

		script::eCommand cmd;
		variant_t value;
		if (symbol)
		{
			auto it = std::find_if(symbol->enums.begin(), symbol->enums.end()
				, [&](const auto &a) {return a.name == pin.name; });
			if (symbol->enums.end() == it)
			{
				assert(!"cVplFile::GenerateCode_Switch() error");
				continue; // error occurred!!
			}
			value = (int)it->value;
			cmd = script::eCommand::eqic;
		}
		else
		{
			// int selection type, name is value
			switch (selPin->type) {
			case vpl::ePinType::Bool:
				value = ((pin.name == "true") || (pin.name == "True")) ? 1 : 0;
				cmd = script::eCommand::eqic;
				break;
			case vpl::ePinType::Int:
				value = atoi(pin.name.c_str());
				cmd = script::eCommand::eqic;
				break;
			case vpl::ePinType::Float:
				value = (float)atof(pin.name.c_str());
				cmd = script::eCommand::eqfc;
				break;
			case vpl::ePinType::String:
				value = pin.name.c_str();
				cmd = script::eCommand::eqsc;
				break;
			default:
				common::dbg::Logc(3, "cVplFile::Switch_GenCode() error");
				continue; // error occurred!!, ignore this code
			}
		}

		string jumpLabel;
		sNode *next = nullptr; // next node
		sPin *np = nullptr; // next pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(next, np) = FindContainPin(linkId);
		if (!next)
		{
			// no link, finish node
			jumpLabel = MakeScopeName(node) + "-Blank";
		}
		else
		{
			jumpLabel = MakeScopeName(*next);
		}

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
			// no default link? jump blank case
			//NodeEscape_GenCode(node, out);
			{
				script::sInstruction code;
				code.cmd = script::eCommand::jmp;
				code.str1 = MakeScopeName(node) + "-Blank";
				out.m_codes.push_back(code);
			}
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

	// insert blank case jump label
	{
		script::sInstruction code;
		code.cmd = script::eCommand::label;
		code.str1 = MakeScopeName(node) + "-Blank";
		out.m_codes.push_back(code);
	}

	NodeEscape_GenCode(node, out);
	return true;
}


// generate while syntax code
bool cVplFile::While_GenCode(const sNode &prevNode, const sNode &node
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
			//common::dbg::Logc(1, "cVplFile::While_GenCode, no branch label\n");
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
bool cVplFile::ForLoop_GenCode(const sNode &prevNode, const sNode &node
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
			//common::dbg::Logc(1, "cVplFile::ForLoop_GenCode, no branch label\n");
		}
		else
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = exitLabel;
			out.m_codes.push_back(code);
		}
	}

	// loop flow node (generate Loop branch node)
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
bool cVplFile::Sequence_GenCode(const sNode &prevNode, const sNode &node
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


// generate sync code
bool cVplFile::Sync_GenCode(const sNode& prevNode, const sNode& node
	, const sPin& fromPin, OUT common::script::cIntermediateCode& out)
{
	RETV(eNodeType::Control != node.type, false);
	RETV(fromPin.links.empty(), false);

	int inputPinIdx = -1;
	for (uint i = 0; i < node.inputs.size(); ++i)
	{
		const sPin& pin = node.inputs[i];
		if (fromPin.links[0] == pin.id)
		{
			inputPinIdx = (int)i;
			break;
		}
	}
	if (inputPinIdx < 0)
		return false;

	const int syncId = node.id;

	// initialize sync?
	if (common::trim2(node.inputs[inputPinIdx].name).empty())
	{
		if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
			return true;

		// load timeout value to register0
		uint condReg = 0; // condition register
		NodeInput_GenCode(node, 0, false, out);

		// initialize sync 
		// 1. timeout, synct
		{
			script::sInstruction code;
			code.cmd = script::eCommand::synct;
			code.reg1 = 0;
			code.var1 = syncId;
			out.m_codes.push_back(code);
		}

		// 2. link input pin count, synci
		{
			script::sInstruction code;
			code.cmd = script::eCommand::ldic;
			code.reg1 = 0;
			code.var1 = syncId;
			out.m_codes.push_back(code);

			int count = 0;
			for (auto& pin : node.inputs)
			{
				if ((ePinType::Flow == pin.type)
					&& !common::trim2(pin.name).empty())
					++count;
			}
			{
				script::sInstruction code;
				code.cmd = script::eCommand::synci;
				code.reg1 = 0; // reg1:syncId
				code.var1 = count;
				out.m_codes.push_back(code);
			}
		}

		// next flow node
		for (auto& pin : node.outputs)
		{
			if ((ePinType::Flow == pin.type)
				&& common::trim2(pin.name).empty())
			{
				sNode* next = nullptr; // next node
				sPin* np = nullptr; // next pin
				const int linkId = pin.links.empty() ? -1 : pin.links.front();
				std::tie(next, np) = FindContainPin(linkId);
				if (next)
					Node_GenCode(node, *next, pin, out);
				break;
			}
		}

		NodeEscape_GenCode(node, out);

		// generate Sync output pin
		for (auto& pin : node.outputs)
		{
			if (ePinType::Flow == pin.type)
			{
				sNode* next = nullptr; // next node
				sPin* np = nullptr; // next pin
				const int linkId = pin.links.empty() ? -1 : pin.links.front();
				std::tie(next, np) = FindContainPin(linkId);
				if (next && (pin.name == "Sync"))
				{
					// insert jump label
					{
						script::sInstruction code;
						code.cmd = script::eCommand::label;
						code.str1 = MakeScopeName(node) + "-Sync";
						out.m_codes.push_back(code);
					}

					Node_GenCode(node, *next, pin, out);
					break;
				}
			}
		}

		// generate TimeOut output pin
		for (auto& pin : node.outputs)
		{
			if (ePinType::Flow == pin.type)
			{
				sNode* next = nullptr; // next node
				sPin* np = nullptr; // next pin
				const int linkId = pin.links.empty() ? -1 : pin.links.front();
				std::tie(next, np) = FindContainPin(linkId);
				if (next && (pin.name == "TimeOut"))
				{
					// insert jump label
					{
						script::sInstruction code;
						code.cmd = script::eCommand::label;
						code.str1 = MakeScopeName(node) + "-TimeOut_event";
						out.m_codes.push_back(code);
					}

					Node_GenCode(node, *next, pin, out);
					break;
				}
			}
		}
	}
	else
	{
		DebugInfo_GenCode(prevNode, node, fromPin, out);

		// check sync, sync
		{
			script::sInstruction code;
			code.cmd = script::eCommand::ldic;
			code.reg1 = 0;
			code.var1 = syncId;
			out.m_codes.push_back(code);
		}
		{
			int syncPinIdx = 0; // sync pin index
			for (uint i = 0; i < node.inputs.size(); ++i)
			{
				const sPin& pin = node.inputs[i];
				if ((ePinType::Flow == pin.type)
					&& !common::trim2(pin.name).empty())
				{
					if (fromPin.links[0] == pin.id)
						break;
					++syncPinIdx;
				}
			}

			script::sInstruction code;
			code.cmd = script::eCommand::sync;
			code.reg1 = 0; // reg1:syncId
			code.var1 = syncPinIdx;
			out.m_codes.push_back(code);
		}
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = MakeScopeName(node) + "-Sync";
			out.m_codes.push_back(code);
		}
	}
	
	NodeEscape_GenCode(node, out);
	return true;
}


// generate SyncOrder code
bool cVplFile::SyncOrder_GenCode(const sNode& prevNode, const sNode& node
	, const sPin& fromPin, OUT common::script::cIntermediateCode& out)
{
	RETV(eNodeType::Control != node.type, false);
	RETV(fromPin.links.empty(), false);

	int inputPinIdx = -1;
	for (uint i = 0; i < node.inputs.size(); ++i)
	{
		const sPin& pin = node.inputs[i];
		if (fromPin.links[0] == pin.id)
		{
			inputPinIdx = (int)i;
			break;
		}
	}
	if (inputPinIdx < 0)
		return false;

	const int syncId = node.id;

	// initialize sync?
	if (node.inputs[inputPinIdx].name == "initialize")
	{
		if (!NodeEnter_GenCode(prevNode, node, fromPin, out))
			return true;

		SyncOrderInit_GenCode(node, out);

		NodeEscape_GenCode(node, out);
	}
	else
	{
		DebugInfo_GenCode(prevNode, node, fromPin, out);

		// check sync, sync
		{
			script::sInstruction code;
			code.cmd = script::eCommand::ldic;
			code.reg1 = 0;
			code.var1 = syncId;
			out.m_codes.push_back(code);
		}

		int syncPinIdx = 0; // sync pin index
		{
			for (uint i = 0; i < node.inputs.size(); ++i)
			{
				const sPin& pin = node.inputs[i];
				if ((ePinType::Flow == pin.type) && (pin.name != "initialize"))
				{
					if (fromPin.links[0] == pin.id)
						break;
					++syncPinIdx;
				}
			}

			script::sInstruction code;
			code.cmd = script::eCommand::sync;
			code.reg1 = 0; // reg1:syncId
			code.var1 = syncPinIdx;
			out.m_codes.push_back(code);
		}

		const string jmpLabels[4] = { "First Enter1", "Last Enter1"
			, "First Enter2", "Last Enter2" };

		const string firstEnter = jmpLabels[(syncPinIdx * 2)];
		const string lastEnter = jmpLabels[(syncPinIdx * 2) + 1];

		// has output pin?
		bool hasPin1 = false;
		bool hasPin2 = false;
		for (auto& pin : node.outputs)
		{
			if ((ePinType::Flow == pin.type) && (pin.name == firstEnter)
				&& (pin.links.size() > 0))
				hasPin1 = true;
			else if ((ePinType::Flow == pin.type) && (pin.name == lastEnter)
				&& (pin.links.size() > 0))
				hasPin2 = true;
		}

		{
			script::sInstruction code;
			code.cmd = script::eCommand::jnz;
			code.str1 = hasPin1? (MakeScopeName(node) + "-" + firstEnter) : "blank";
			out.m_codes.push_back(code);
		}
		{
			script::sInstruction code;
			code.cmd = script::eCommand::jmp;
			code.str1 = hasPin2 ? (MakeScopeName(node) + "-" + lastEnter) : "blank";
			out.m_codes.push_back(code);
		}

		NodeEscape_GenCode(node, out);

		// generate 'First Enter1, Last Enter1, First Enter2, Last Enter2' output flow
		if (m_genNodes.end() == m_genNodes.find(node.id))
		{
			m_genNodes.insert(node.id); // only once generate

			out.m_codes.push_back({ script::eCommand::nop }); // nop

			for (auto& pin : node.outputs)
			{
				if (ePinType::Flow == pin.type)
				{
					sNode* next = nullptr; // next node
					sPin* np = nullptr; // next pin
					const int linkId = pin.links.empty() ? -1 : pin.links.front();
					std::tie(next, np) = FindContainPin(linkId);
					if (next)
					{
						// insert jump label
						{
							script::sInstruction code;
							code.cmd = script::eCommand::label;
							code.str1 = MakeScopeName(node) + "-" + pin.name;
							out.m_codes.push_back(code);
						}

						Node_GenCode(node, *next, pin, out);
					}
				}
			}//~for
		}

	}

	return true;
}


// generate intermediate code, SyncOrder node
// initialize SyncOrder command
bool cVplFile::SyncOrderInit_GenCode(const sNode& node
	, OUT common::script::cIntermediateCode& out)
{
	const int syncId = node.id;

	// 1. link input pin count, synco
	{
		script::sInstruction code;
		code.cmd = script::eCommand::ldic;
		code.reg1 = 0;
		code.var1 = syncId;
		out.m_codes.push_back(code);

		int count = 0;
		for (auto& pin : node.inputs)
		{
			if ((ePinType::Flow == pin.type) && (pin.name != "initialize"))
				++count;
		}

		{
			script::sInstruction code;
			code.cmd = script::eCommand::synco;
			code.reg1 = 0; // reg1:syncId
			code.var1 = count;
			out.m_codes.push_back(code);
		}

		{
			script::sInstruction code;
			code.cmd = script::eCommand::seti;
			code.str1 = MakeScopeName(node);
			code.str2 = "id";
			code.reg1 = 0; // reg1:syncId
			code.var1 = count;
			out.m_codes.push_back(code);
		}
	}
	return true;
}


// generate intermediate code, operator node
bool cVplFile::Operator_GenCode(const sNode &node
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
	eSymbolType symbType = eSymbolType::None;
	for (auto &pin : node.inputs)
	{
		if (pin.typeValues.empty())
			continue;

		symbType = pin.typeValues[0];
		if (eSymbolType::None != symbType)
			break;
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
				, "Error!! cVplFile::Operator_GenCode(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
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
				, "Error!! cVplFile::Operator_GenCode(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
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
				, "Error!! cVplFile::Operator_GenCode(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
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
				, "Error!! cVplFile::Operator_GenCode(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
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
				, "Error!! cVplFile::Operator_GenCode(), compare type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
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
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
		out.m_codes.push_back(code);
		MathInput_GenCode(node, code, out);
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
				, "Error!! cVplFile::Operator_GenCode(), arithmatic type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
		out.m_codes.push_back(code);
		MathInput_GenCode(node, code, out);
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
				, "Error!! cVplFile::Operator_GenCode(), arithmatic type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
		out.m_codes.push_back(code);
		MathInput_GenCode(node, code, out);
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
				, "Error!! cVplFile::Operator_GenCode(), arithmatic type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
		out.m_codes.push_back(code);
		MathInput_GenCode(node, code, out);
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
				, "Error!! cVplFile::Operator_GenCode(), arithmatic type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
		out.m_codes.push_back(code);
		MathInput_GenCode(node, code, out);
		opType = 0;
	}
	else if (node.name == "%")
	{
		script::sInstruction code;
		switch (symbType)
		{
		case eSymbolType::Bool:
		case eSymbolType::Int: code.cmd = script::eCommand::remi; break;
		case eSymbolType::Float: code.cmd = script::eCommand::remf; break;
		default:
			common::dbg::Logc(1
				, "Error!! cVplFile::Operator_GenCode(), arithmatic type invalid\n");
			break;
		}
		code.reg1 = 8; // reg8
		code.reg2 = 9; // reg9
		out.m_codes.push_back(code);
		MathInput_GenCode(node, code, out);
		opType = 0;
	}
	else if ((node.name == "!") || (node.name == "not"))
	{
		script::sInstruction code;
		code.cmd = script::eCommand::negate;
		code.reg1 = 8; // reg8
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
					, "Error!! cVplFile::Operator_GenCode(), compare type invalid\n");
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
bool cVplFile::Variable_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	RETV(node.type != eNodeType::Operator, false);
	RETV(m_visit.find(node.id) != m_visit.end(), false);
	m_visit.insert(node.id);

	// nothing to operation

	return true;
}


// generate intermediate code, function node, callback flow
bool cVplFile::Callback_GenCode(const sNode& node
	, OUT common::script::cIntermediateCode& out)
{
	RETV(node.type != eNodeType::Function, false);
	RETV(node.outputs.empty(), false);

	for (auto& pin : node.outputs)
	{
		if ((ePinType::Flow != pin.type) || (pin.links.empty()))
			continue;
		if (common::trim2(pin.name).empty())
			continue; // ignore blank name output widget (main output flow pin)

		if (pin.links.size() >= 2)
			common::dbg::Logc(3, "Error!! cVplFile::Callback_GenCode, flow link too many setting \n");

		out.m_codes.push_back({ script::eCommand::nop });

		// labelName: node.name '_' node.id	 
		// make label name, if need unique event label name, update from node.labelName
		const string labelName =
			script::cSymbolTable::MakeScopeName2(node.name, node.id, pin.name);
		out.m_codes.push_back({ script::eCommand::label, labelName });

		// clear stack
		out.m_codes.push_back({ script::eCommand::cstack });

		sNode* next = nullptr;
		sPin* np = nullptr;
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(next, np) = FindContainPin(linkId);
		if (!next)
			continue; // error occurred!!

		Node_GenCode(node, *next, pin, out);

		// return, if has jump address
		out.m_codes.push_back({ script::eCommand::sret });

		// finish, stop execute
		out.m_codes.push_back({ script::eCommand::nop });
	}

	return true;
}


// generate intermediate code, pin
bool cVplFile::Pin_GenCode(const sNode &node, const sPin &pin, const uint reg
	, OUT common::script::cIntermediateCode &out)
{
	return Pin2_GenCode(pin.kind, node, pin, reg, out);
}


// generate intermediate code, pin
bool cVplFile::Pin2_GenCode(const ePinKind kind
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
bool cVplFile::TemporalPin_GenCode(const sNode &node, const sPin &pin
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
				// empty array
				code.cmd = script::eCommand::ldac;
				code.var1.vt = VT_BYREF | VT_INT; // map tricky code, no memory allocate
				code.var1.intVal = 0;
				break;
			case ePinType::Map:
				// empty map
				code.cmd = script::eCommand::ldmc;
				code.var1.vt = VT_BYREF | VT_INT; // map tricky code, no memory allocate
				code.var1.intVal = 0;
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
bool cVplFile::DebugInfo_GenCode(const sPin &from, const sPin &to
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
bool cVplFile::DebugInfo_GenCode(const sNode &from, const sNode &to
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
bool cVplFile::NodeEnter_GenCode(const sNode &prevNode, const sNode &node
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
bool cVplFile::NodeEscape_GenCode(const sNode &node
	, OUT common::script::cIntermediateCode &out)
{
	// check macro function
	// no input flow slot? macro function!
	uint inputFlowCnt = 0, outputFlowCnt = 0;
	for (auto& pin : node.inputs)
		if ((ePinType::Flow == pin.type) && (common::trim2(pin.name).empty()))
			++inputFlowCnt;
	for (auto& pin : node.outputs)
		if ((ePinType::Flow == pin.type) && (common::trim2(pin.name).empty()))
			++outputFlowCnt;
	const bool isMacro = (eNodeType::Control != node.type) 
		&& (0 == inputFlowCnt) && (0 == outputFlowCnt);
	
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
bool cVplFile::NodeInput_GenCode(const sNode &node, const uint reg
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
			Node_GenCode2(nullNode, *prev, pin, out);
	}

	// load register prev node output pin
	// isUpdateInputPin == true? store node input symboltable
	uint r = reg; // start register number
	for (auto &pin : node.inputs)
	{
		if (IsIgnoreInputPin(pin.type))
			continue;
		if (r >= 10) // check max register count
			break;

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


// arithmatic, logic operator generation code
bool cVplFile::MathInput_GenCode(const sNode &node
	, const script::sInstruction &opCode
	, OUT common::script::cIntermediateCode &out)
{
	if (node.inputs.size() <= 2)
		return false; // nothing to do

	// reg[9] = reg[9] op reg[8]
	script::sInstruction code = opCode;
	code.reg1 = 9;
	code.reg2 = 8;

	// start third input pin
	for (uint i = 2; i < node.inputs.size(); ++i)
	{
		const sPin &pin = node.inputs[i];
		if (IsIgnoreInputPin(pin.type))
			continue;

		sNode *prev = nullptr; // prev node
		sPin *pp = nullptr; // prev pin
		const int linkId = pin.links.empty() ? -1 : pin.links.front();
		std::tie(prev, pp) = FindContainPin(linkId);
		if (prev)
		{
			Pin_GenCode(*prev, *pp, 8, out); // load register from prev output pin
			DebugInfo_GenCode(*pp, pin, out); // insert debuginfo
		}
		else
		{
			// load temporal variable
			TemporalPin_GenCode(node, pin, 8, out);
		}
		out.m_codes.push_back(code);
	}
	return true;
}


bool cVplFile::AddPin(const int parseState, sNode &node, const sPin &pin)
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
std::pair<cVplFile::sNode*, cVplFile::sPin*> 
	cVplFile::FindContainPin(const int pinId)
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
string cVplFile::MakeScopeName(const sNode &node
	, const int uniqueId // =-1
)
{
	string scopeName;
	if (eNodeType::Event == node.type)
	{
		// 'Tick Event' is no unique node, (labelName empty)
		if (!node.labelName.empty())
			scopeName = node.labelName.c_str();
	}

	if (scopeName.empty())
	{
		const string sname = 
			script::cSymbolTable::MakeScopeName(node.name.c_str(), node.id).c_str();
		if (uniqueId >= 0)
			scopeName = common::format("%s-%d", sname.c_str(), uniqueId);
		else
			scopeName = sname;
	}

	return scopeName;
}


// return Input Flow links count
uint cVplFile::GetInputFlowCount(const sNode &node)
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
const cVplFile::sPin* cVplFile::GetInputPin(const sNode &node, const vector<string> &names)
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
bool cVplFile::IsIgnoreInputPin(const ePinType type)
{
	switch (type)
	{
	case ePinType::Flow:
	case ePinType::Function:
	case ePinType::Delegate:
	case ePinType::Blank:
		return true;
	}
	return false;
}


void cVplFile::Clear()
{
	m_syncId = 1;
	m_nodes.clear();
	m_variables.Clear();
}
