//
// 2019-11-01, jjuiddong
// visual programming language global header
//
// 2021-07-09
//	- change name vprog -> vpl (visual programming language)
//
#pragma once


namespace vpl {

	using common::StrId;
	using common::StrPath;

	DECLARE_ENUM(ePinType,
		Flow,
		Bool,
		Int,
		Float,
		Enums,
		String,
		Object,
		Function,
		Delegate,
		Any,//NotDef,
		None,
		Array,
		Map
	);

	DECLARE_ENUM(ePinKind,
		Output,
		Input
	);

	DECLARE_ENUM(eNodeType,
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
	);


	// Convert ePinType to Value Type
	VARTYPE GetPin2VarType(const ePinType::Enum type);

	// Is Variable Type? int, float .. etc
	bool IsPrimitiveType(const ePinType::Enum type);

}
