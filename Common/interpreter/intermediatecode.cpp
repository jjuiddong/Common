
#include "stdafx.h"
#include "intermediatecode.h"

using namespace common;
using namespace common::script;


cIntermediateCode::cIntermediateCode()
{
}

cIntermediateCode::~cIntermediateCode()
{
	Clear();
}


// read intermediate code file
bool cIntermediateCode::Read(const StrPath &fileName)
{
	Clear();

	using namespace std;
	ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		return false;

	int state = 0;
	string line;
	while (getline(ifs, line))
	{
		common::trim(line);
		vector<string> toks;
		common::tokenizer_space(line, toks);
		if (toks.empty())
			continue;

		if (trim(toks[0]) == "#")
			continue; // comment line

		sCommandSet cmd;

		if (((toks[0] == "geti") 
			|| (toks[0] == "getb")
			|| (toks[0] == "getf")
			|| (toks[0] == "gets"))
			&& (toks.size() >= 4))
		{
			cmd.cmd = eCommand::FromString(toks[0]);
			cmd.str1 = toks[1];
			cmd.str2 = toks[2];
			cmd.reg1 = GetRegisterIndex(toks[3]);
		}
		else if (((toks[0] == "seti")
			|| (toks[0] == "setb")
			|| (toks[0] == "setf")
			|| (toks[0] == "sets"))
			&& (toks.size() >= 4))
		{
			cmd.cmd = eCommand::FromString(toks[0]);
			cmd.str1 = toks[1];
			cmd.str2 = toks[2];
			cmd.reg1 = GetRegisterIndex(toks[3]);
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
			|| (toks[0] == "greqf"))
			&& (toks.size() >= 3))
		{
			cmd.cmd = eCommand::FromString(toks[0]);
			cmd.reg1 = GetRegisterIndex(toks[1]);
			cmd.reg2 = GetRegisterIndex(toks[2]);
		}
		else if (((toks[0] == "eqic")
			|| (toks[0] == "eqfc")
			|| (toks[0] == "eqsc"))
			&& (toks.size() >= 3))
		{
			cmd.cmd = eCommand::FromString(toks[0]);
			cmd.reg1 = GetRegisterIndex(toks[1]);

			variant_t type;
			switch (cmd.cmd)
			{
			case eCommand::eqic:
				type.vt = VT_I4;
				cmd.var1 = common::str2variant(type, toks[2]);
				break;
			case eCommand::eqfc:
				type.vt = VT_R4;
				cmd.var1 = common::str2variant(type, toks[2]);
				break;
			case eCommand::eqsc:
				cmd.str1 = toks[2];
				break;
			default:
				assert(!"Error!! cIntermediateCode::Read() parse error");
				break;
			}
		}
		else if ((toks[0] == "call")
			&& (toks.size() >= 2))
		{
			cmd.cmd = eCommand::FromString(toks[0]);
			cmd.str1 = toks[1];
		}
		else if ((toks[0] == "jnz")
			&& (toks.size() >= 2))
		{
			cmd.cmd = eCommand::FromString(toks[0]);
			cmd.str1 = toks[1];
		}
		else if ((toks[0] == "jmp")
			&& (toks.size() >= 2))
		{
			cmd.cmd = eCommand::FromString(toks[0]);
			cmd.str1 = toks[1];
		}
		else if (toks[0] == "end")
		{
			cmd.cmd = eCommand::FromString(toks[0]);
		}
		else
		{
			if (toks.size() == 1)
			{
				size_t n = toks[0].find(":");
				if (n != string::npos)
				{
					// jump label
					cmd.cmd = eCommand::label;
					cmd.str1 = toks[0].substr(0, n);
				}
				else
				{
					assert(!"Error!! cIntermediateCode::Read() parse error");
				}
			}
			else
			{
				assert(!"Error!! cIntermediateCode::Read() parse error");
			}
		}

		if (cmd.cmd != eCommand::none)
			m_cmds.push_back(cmd);
	}

	for (uint i=0; i < m_cmds.size(); ++i)
	{
		auto &cmd = m_cmds[i];
		if (eCommand::label == cmd.cmd)
			m_jmpMap.insert({ cmd.str1, i });
	}

	return true;
}


// write intermediate code file
bool cIntermediateCode::Write(const StrPath &fileName)
{
	using namespace std;
	ofstream ofs(fileName.c_str());
	if (!ofs.is_open())
		return false;

	for (auto &cmd : m_cmds)
	{
		switch (cmd.cmd)
		{
		case eCommand::getb:
		case eCommand::geti:
		case eCommand::getf:
		case eCommand::gets:
		case eCommand::setb:
		case eCommand::seti:
		case eCommand::setf:
		case eCommand::sets:
			ofs << eCommand::ToString(cmd.cmd);
			ofs << " " << cmd.str1;
			ofs << ", " << cmd.str2;
			ofs << ", " << GetRegisterName(cmd.reg1);
			break;

		case eCommand::eqi:
		case eCommand::eqf:
		case eCommand::eqs:
			ofs << eCommand::ToString(cmd.cmd);
			ofs << " " << GetRegisterName(cmd.reg1);
			ofs << ", " << GetRegisterName(cmd.reg2);
			break;

		case eCommand::eqic:
			ofs << eCommand::ToString(cmd.cmd);
			ofs << " " << GetRegisterName(cmd.reg1);
			ofs << ", " << cmd.var1.intVal;
			break;

		case eCommand::eqfc:
			ofs << eCommand::ToString(cmd.cmd);
			ofs << " " << GetRegisterName(cmd.reg1);
			ofs << ", " << cmd.var1.fltVal;
			break;

		case eCommand::eqsc:
			ofs << eCommand::ToString(cmd.cmd);
			ofs << " " << GetRegisterName(cmd.reg1);
			ofs << ", " << cmd.str1;
			break;

		case eCommand::lesi:
		case eCommand::lesf:
		case eCommand::leqi:
		case eCommand::leqf:
		case eCommand::gri:
		case eCommand::grf:
		case eCommand::greqi:
		case eCommand::greqf:
			ofs << eCommand::ToString(cmd.cmd);
			ofs << " " << GetRegisterName(cmd.reg1);
			ofs << ", " << GetRegisterName(cmd.reg2);
			break;

		case eCommand::call:
		case eCommand::jnz:
		case eCommand::jmp:
			ofs << eCommand::ToString(cmd.cmd);
			ofs << " " << cmd.str1;
			break;

		case eCommand::end:
			ofs << eCommand::ToString(cmd.cmd);
			break;

		case eCommand::label:
			ofs << cmd.str1 << ":";
			break;

		default:
			assert(!"Error cIntermediateCode::Write()");
			break;
		}

		ofs << endl;
	}

	return true;
}


bool cIntermediateCode::IsLoaded()
{
	return !m_cmds.empty();
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
	static char regName[5] = "val0";
	regName[3] = (char)('0' + min((uint)9, regIdx));
	return regName;
}


void cIntermediateCode::Clear()
{
	m_cmds.clear();
	m_jmpMap.clear();
}
