//
// 2019-11-01, jjuiddong
// visual programming global header
//
#pragma once


namespace vprog {

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
		Delegate
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
		Tree
	);


	// convert ePinType to Value Type
	VARTYPE GetPin2VarType(const ePinType::Enum type);

}
