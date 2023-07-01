
#include "stdafx.h"
#include "intermediatecode.h"

using namespace common;
using namespace common::script;


cIntermediateCode::cIntermediateCode()
{
	// m_regName: val0 ~ 9
	m_regName[0] = 'v', m_regName[1] = 'a', m_regName[2] = 'l';
	m_regName[4] = '\0';
}

cIntermediateCode::cIntermediateCode(const cIntermediateCode &rhs)
{
	operator=(rhs);
}

cIntermediateCode::~cIntermediateCode()
{
	Clear();
}


// read intermediate code file
// fileName: *.icode file
bool cIntermediateCode::Read(const StrPath &fileName)
{
	Clear();

	m_fileName = fileName.GetFullFileName();

	using namespace std;
	ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		return false;

	int state = 0;
	int lineCnt = 0; // for debugging
	string line;
	while (getline(ifs, line))
	{
		++lineCnt;
		common::trim(line);
		vector<string> toks;
		common::tokenizer_space(line, toks);
		if (toks.empty())
			continue;

		if (trim(toks[0]) == "#")
			continue; // comment line

		sInstruction code;

		if (((toks[0] == "geti")
			|| (toks[0] == "getb")
			|| (toks[0] == "getf")
			|| (toks[0] == "gets")
			|| (toks[0] == "geta")
			|| (toks[0] == "getm")
			|| (toks[0] == "seti")
			|| (toks[0] == "setb")
			|| (toks[0] == "setf")
			|| (toks[0] == "sets")
			|| (toks[0] == "seta")
			|| (toks[0] == "setm")
			|| (toks[0] == "copya")
			|| (toks[0] == "copym")
			)
			&& (toks.size() >= 4))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.str1 = toks[1];
			code.str2 = toks[2];
			code.reg1 = GetRegisterIndex(toks[3]);
		}
		else if (((toks[0] == "ldbc")
			|| (toks[0] == "ldic")
			|| (toks[0] == "ldfc")
			|| (toks[0] == "ldsc"))
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.reg1 = GetRegisterIndex(toks[1]);
			const VARTYPE vt = common::script::GetVarType(code.cmd);
			code.var1 = common::str2variant(vt, toks[2]);
			if (vt == VT_EMPTY)
				dbg::Logc(3, "Error cIntermediateCode::Read() parse error\n");
		}
		else if ((toks[0] == "ldac")
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.reg1 = GetRegisterIndex(toks[1]);
			code.var1.vt = VT_BYREF | VT_INT;
			code.var1.intVal = 0; // tricky code, array type
			//const VARTYPE vt = common::script::GetVarType(code.cmd);
			//code.var1 = common::str2variant(vt, toks[2]);
			//if (vt == VT_EMPTY)
			//	dbg::Logc(3, "Error cIntermediateCode::Read() parse error\n");
		}
		else if ((toks[0] == "ldmc")
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.reg1 = GetRegisterIndex(toks[1]);
			code.var1.vt = VT_BYREF | VT_INT;
			code.var1.intVal = 0; // tricky code, map type
			//const VARTYPE vt = common::script::GetVarType(code.cmd);
			//code.var1 = common::str2variant(vt, toks[2]);
			//if (vt == VT_EMPTY)
			//	dbg::Logc(3, "Error cIntermediateCode::Read() parse error\n");
		}
		else if (((toks[0] == "ldcmp")
			|| (toks[0] == "ldncmp")
			|| (toks[0] == "ldtim"))
			&& (toks.size() >= 2))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.reg1 = GetRegisterIndex(toks[1]);
		}
		else if (((toks[0] == "lesi")
			|| (toks[0] == "lesf")
			|| (toks[0] == "leqi")
			|| (toks[0] == "leqf")
			|| (toks[0] == "eqi")
			|| (toks[0] == "eqf")
			|| (toks[0] == "eqs")
			|| (toks[0] == "gri")
			|| (toks[0] == "grf")
			|| (toks[0] == "greqi")
			|| (toks[0] == "greqf")
			|| (toks[0] == "addi")
			|| (toks[0] == "adds")
			|| (toks[0] == "subi")
			|| (toks[0] == "muli")
			|| (toks[0] == "divi")
			|| (toks[0] == "remi")
			|| (toks[0] == "addf")
			|| (toks[0] == "subf")
			|| (toks[0] == "mulf")
			|| (toks[0] == "divf")
			|| (toks[0] == "remf")
			|| (toks[0] == "opand")
			|| (toks[0] == "opor")
			)
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.reg1 = GetRegisterIndex(toks[1]);
			code.reg2 = GetRegisterIndex(toks[2]);
		}
		else if ((toks[0] == "negate") && (toks.size() >= 2))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.reg1 = GetRegisterIndex(toks[1]);
		}
		else if (((toks[0] == "eqic")
			|| (toks[0] == "eqfc")
			|| (toks[0] == "eqsc"))
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.reg1 = GetRegisterIndex(toks[1]);

			const VARTYPE vt = common::script::GetVarType(code.cmd);
			code.var1 = common::str2variant(vt, toks[2]);
			if (vt == VT_EMPTY)
				dbg::Logc(3, "Error cIntermediateCode::Read() parse error\n");
		}
		else if ((toks[0] == "call")
			&& (toks.size() >= 2))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.str1 = toks[1];
		}
		else if ((toks[0] == "jnz")
			&& (toks.size() >= 2))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.str1 = toks[1];
		}
		else if ((toks[0] == "jmp")
			&& (toks.size() >= 2))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.str1 = toks[1];
		}
		else if ((toks[0] == "pushic") && (toks.size() >= 2))
		{
			code.cmd = eCommand::FromString(toks[0]);
			const VARTYPE vt = common::script::GetVarType(code.cmd);
			code.var1 = common::str2variant(vt, toks[1]);
			if (vt == VT_EMPTY)
				dbg::Logc(3, "Error cIntermediateCode::Read() parse error\n");
		}
		else if (((toks[0] == "sret") 
			|| (toks[0] == "pop")
			|| (toks[0] == "cstack"))
			&& (toks.size() >= 1))
		{
			code.cmd = eCommand::FromString(toks[0]);
		}
		else if (((toks[0] == "symbolb")
			|| (toks[0] == "symboli")
			|| (toks[0] == "symbolf")
			|| (toks[0] == "symbols"))
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.str1 = toks[1];
			code.str2 = toks[2];
			const VARTYPE vt = common::script::GetVarType(code.cmd);
			code.var1 = common::str2variant(vt
				, (toks.size()>=4)? toks[3] : "");
		}
		else if (
			(
				(toks[0] == "symbolab")
				|| (toks[0] == "symbolai")
				|| (toks[0] == "symbolaf")
				|| (toks[0] == "symbolas")
				|| (toks[0] == "symbolmb")
				|| (toks[0] == "symbolmi")
				|| (toks[0] == "symbolmf")
				|| (toks[0] == "symbolms")
				|| (toks[0] == "symbolma")
			)
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.str1 = toks[1];
			code.str2 = toks[2];
			if (toks.size() >= 4)
				code.str3 = toks[3]; // array, map type string
		}
		else if ((toks[0] == "#comment")
			&& (toks.size() >= 4))
		{
			code.cmd = eCommand::cmt;
			code.str1 = toks[1];
			code.reg1 = atoi(toks[2].c_str());
			code.reg2 = atoi(toks[3].c_str());
		}
		else if (toks[0] == "delay")
		{
			code.cmd = eCommand::FromString(toks[0]);
		}
		else if (((toks[0] == "timer1")
			|| (toks[0] == "timer2"))
			&& (toks.size() >= 3))
		{
			code.cmd = eCommand::FromString(toks[0]);
			code.str1 = toks[1];
			const VARTYPE vt = common::script::GetVarType(code.cmd);
			code.var1 = common::str2variant(vt, toks[2]);
		}
		else if (toks[0] == "nop")
		{
			code.cmd = eCommand::FromString(toks[0]);
		}
		else
		{
			if (toks.size() == 2)
			{
				if (toks[1] == ":")
				{
					// jump label
					code.cmd = eCommand::label;
					code.str1 = toks[0];
				}
				else
				{
					dbg::Logc(3, "Error cIntermediateCode::Read() parse error\n");
				}
			}
			else
			{
				dbg::Logc(3, "Error cIntermediateCode::Read() parse error\n");
			}
		}

		if (code.cmd != eCommand::none)
			m_codes.push_back(code);
	}

	InitOptimizeInfo();
	return true;
}


// write intermediate code file
bool cIntermediateCode::Write(const StrPath &fileName)
{
	using namespace std;
	ofstream ofs(fileName.c_str());
	if (!ofs.is_open())
		return false;

	for (auto &code : m_codes)
	{
		switch (code.cmd)
		{
		case eCommand::ldbc:
		case eCommand::ldic:
		case eCommand::ldfc:
		case eCommand::ldsc:
		case eCommand::ldac:
		case eCommand::ldmc:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << GetRegisterName(code.reg1);
			ofs << ", " << common::variant2str(code.var1, true);
			break;

		case eCommand::ldcmp:
		case eCommand::ldncmp:
		case eCommand::ldtim:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << GetRegisterName(code.reg1);
			break;

		case eCommand::getb:
		case eCommand::geti:
		case eCommand::getf:
		case eCommand::gets:
		case eCommand::geta:
		case eCommand::getm:
		case eCommand::setb:
		case eCommand::seti:
		case eCommand::setf:
		case eCommand::sets:
		case eCommand::seta:
		case eCommand::setm:
		case eCommand::copya:
		case eCommand::copym:
			ofs << eCommand::ToString(code.cmd);
			ofs << " \"" << code.str1 << "\"";
			ofs << ", \"" << code.str2 << "\"";
			ofs << ", " << GetRegisterName(code.reg1);
			break;

		case eCommand::eqi:
		case eCommand::eqf:
		case eCommand::eqs:
		case eCommand::lesi:
		case eCommand::lesf:
		case eCommand::leqi:
		case eCommand::leqf:
		case eCommand::gri:
		case eCommand::grf:
		case eCommand::greqi:
		case eCommand::greqf:
		case eCommand::addi:
		case eCommand::adds:
		case eCommand::subi:
		case eCommand::muli:
		case eCommand::divi:
		case eCommand::remi:
		case eCommand::addf:
		case eCommand::subf:
		case eCommand::mulf:
		case eCommand::divf:
		case eCommand::remf:
		case eCommand::opand:
		case eCommand::opor:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << GetRegisterName(code.reg1);
			ofs << ", " << GetRegisterName(code.reg2);
			break;

		case eCommand::negate:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << GetRegisterName(code.reg1);
			break;

		case eCommand::eqic:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << GetRegisterName(code.reg1);
			ofs << ", " << (int)code.var1;
			break;

		case eCommand::eqfc:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << GetRegisterName(code.reg1);
			ofs << ", " << (float)code.var1;
			break;

		case eCommand::eqsc:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << GetRegisterName(code.reg1);
			ofs << ", \"" << (bstr_t)code.var1 << "\"";
			break;

		case eCommand::call:
		case eCommand::jnz:
		case eCommand::jmp:
			ofs << eCommand::ToString(code.cmd);
			ofs << " \"" << code.str1 << "\"";
			break;

		case eCommand::nop:
			ofs << eCommand::ToString(code.cmd);
			break;

		case eCommand::label:
			ofs << "\"" << code.str1 << "\":";
			break;

		case eCommand::pushic:
			ofs << eCommand::ToString(code.cmd);
			ofs << " " << common::variant2str(code.var1, true);
			break;

		case eCommand::sret:
		case eCommand::cstack:
		case eCommand::pop:
			ofs << eCommand::ToString(code.cmd);
			break;

		case eCommand::symbolb:
		case eCommand::symboli:
		case eCommand::symbolf:
		case eCommand::symbols:
		case eCommand::symbolab:
		case eCommand::symbolai:
		case eCommand::symbolaf:
		case eCommand::symbolas:
		case eCommand::symbolmb:
		case eCommand::symbolmi:
		case eCommand::symbolmf:
		case eCommand::symbolms:
		case eCommand::symbolma:
			ofs << eCommand::ToString(code.cmd);
			ofs << " \"" << code.str1 << "\"";
			ofs << ", \"" << code.str2 << "\"";
			ofs << ", " << common::variant2str(code.var1, true);
			if (!code.str3.empty())
				ofs << ", \"" << code.str3 << "\"";
			break;

		case eCommand::cmt:
			ofs << "#comment";
			ofs << " \"" << code.str1 << "\"";
			ofs << ", " << code.reg1;
			ofs << ", " << code.reg2;
			break;

		case eCommand::delay:
			ofs << "delay";
			break;

		case eCommand::timer1:
		case eCommand::timer2:
			ofs << eCommand::ToString(code.cmd);
			ofs << " \"" << code.str1 << "\"";
			ofs << ", " << common::variant2str(code.var1, true);
			break;

		default:
			dbg::Logc(3, "Error cIntermediateCode::Write()\n");
			break;
		}

		ofs << endl;
	}

	return true;
}


// initialize jump map, symbol table, timer event
// to fast search
bool cIntermediateCode::InitOptimizeInfo()
{
	// make jump map
	for (uint i = 0; i < m_codes.size(); ++i)
	{
		auto &code = m_codes[i];
		if (eCommand::label == code.cmd)
			m_jmpMap.insert({ code.str1, i });
	}

	// setup initial symbol table
	for (uint i = 0; i < m_codes.size(); ++i)
	{
		auto &code = m_codes[i];
		switch (code.cmd)
		{
		case eCommand::symbolb:
		case eCommand::symboli:
		case eCommand::symbolf:
		case eCommand::symbols:
			m_variables.Set(code.str1, code.str2, code.var1);
			break;

		// skip this code, array, map already initialize
		// 
		//case eCommand::symbolab:
		//	m_variables.InitArray(code.str1, code.str2, "array<bool>");
		//	break;
		//case eCommand::symbolai:
		//	m_variables.InitArray(code.str1, code.str2, "array<int>");
		//	break;
		//case eCommand::symbolaf:
		//	m_variables.InitArray(code.str1, code.str2, "array<float>");
		//	break;
		//case eCommand::symbolas:
		//	m_variables.InitArray(code.str1, code.str2, "array<string>");
		//	break;
		//
		//case eCommand::symbolmb:
		//	m_variables.InitMap(code.str1, code.str2, "map<string,bool>");
		//	break;
		//case eCommand::symbolmi:
		//	m_variables.InitMap(code.str1, code.str2, "map<string,int>");
		//	break;
		//case eCommand::symbolmf:
		//	m_variables.InitMap(code.str1, code.str2, "map<string,float>");
		//	break;
		//case eCommand::symbolms:
		//	m_variables.InitMap(code.str1, code.str2, "map<string,string>");
		//	break;
		//case eCommand::symbolma:
		//	m_variables.InitMap(code.str1, code.str2, code.str3);
		//	break;
		}
	}

	// make timer1 event list
	for (uint i = 0; i < m_codes.size(); ++i)
	{
		auto &code = m_codes[i];
		if (eCommand::timer1 == code.cmd)
			m_timer1Events.push_back({ code.str1, (int)code.var1 });
		//else if (eCommand::timer2 == code.cmd)
		//	m_timer2Events.push_back({ code.str1, 0 });
	}

	return true;
}


uint cIntermediateCode::FindJumpAddress(const StrId &label)
{
	auto it = m_jmpMap.find(label);
	if (m_jmpMap.end() == it)
		return UINT_MAX;
	return it->second;
}


bool cIntermediateCode::IsLoaded()
{
	return !m_codes.empty();
}


uint cIntermediateCode::GetRegisterIndex(const string &regName)
{
	if (regName.length() < 4)
		return 0;
	if ((regName[0] == 'v')
		&& (regName[1] == 'a')
		&& (regName[2] == 'l'))
		return (uint)min(9, max(0, (regName[3] - '0')));
	else
		return 0;
}


const char* cIntermediateCode::GetRegisterName(const uint regIdx)
{
	m_regName[3] = (char)('0' + min((uint)9, regIdx));
	return m_regName;
}


cIntermediateCode& cIntermediateCode::operator=(const cIntermediateCode &rhs)
{
	if (this != &rhs)
	{
		m_fileName = rhs.m_fileName;
		m_codes = rhs.m_codes;
		m_variables = rhs.m_variables;
		m_jmpMap = rhs.m_jmpMap;
		m_timer1Events = rhs.m_timer1Events;
		//m_timer2Events = rhs.m_timer2Events;
	}
	return *this;
}


void cIntermediateCode::Clear()
{
	m_codes.clear();
	m_jmpMap.clear();
	m_timer1Events.clear();
	//m_timer2Events.clear();
	m_variables.Clear();
	m_fileName.clear();
}
