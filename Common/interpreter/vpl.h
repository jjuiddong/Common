//
// 2019-11-01, jjuiddong
// visual programming language global header
//
// 2021-07-09
//	- change name vprog -> vpl (visual programming language)
//
// 2023-08-13
//	- refactoring DECLARE_ENUM -> enum class
//
#pragma once


namespace vpl {

	using common::StrId;
	using common::StrPath;

	enum class ePinType
	{
		Flow,
		Bool,
		Int,
		Float,
		Enums,
		String,
		Object,
		Function,
		Delegate,
		Any, // NotDef
		None,
		Array,
		Map,
		Blank,
		COUNT
	};

	enum class ePinKind
	{
		Output,
		Input
	};

	enum class eNodeType
	{
		None,
		Event,
		Function,
		Control,
		Operator,
		Macro,
		Variable,
		Comment,
		Define,
		Tree,
		Array,
		Widget
	};

	
	string PinTypeToStr(const ePinType type);
	ePinType StrToPinType(const string& str);
	// Convert ePinType to Value Type
	VARTYPE GetPin2VarType(const ePinType type);
	// Is Variable Type? int, float .. etc
	bool IsPrimitiveType(const ePinType type);

	string PinKindToStr(const ePinKind kind);
	ePinKind StrToPinKind(const string& str);

	string NodeTypeToStr(const eNodeType type);
	eNodeType StrToNodeType(const string& str);

}
