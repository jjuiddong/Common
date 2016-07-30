//
// 2016-04-16, jjuiddong
//
// 모션 스크립트 파서
//
#pragma once

#include "motionscript.h"


namespace motion
{

	class cMotionScriptParser
	{
	public:
		cMotionScriptParser();
		virtual ~cMotionScriptParser();

		bool Read(const string &fileName);
		bool Parse(const string &str);


	protected:
		// BNF
		// 
		// program -> command_list component_list
		// command_list -> { command }
		// command -> id '=' num | id
		// component_list -> { component }
		// component -> comp_header '{' comp_body '}'
		// comp_header -> :id [id]
		// comp_body -> '{' command_list subcomponent_list '}'
		// subcomponent_list -> { subcomponent }
		// subcomponent ->  protocol | math | modulation | format
		// protocol -> comp_header '{' protocol_body '}'
		// protocol_body  -> protocol script parsing
		// init -> comp_header '{' init_body '}'
		// init_body -> math script parsing
		// math -> comp_header '{' math_body '}'
		// math_body -> math script parsing
		// modulation -> comp_header '{' modulation_body '}'
		// modulation_body -> modulation config parsing
		// format -> comp_header '{' format_body '}'
		// format_body -> ??
	
		sProgram* program(string &src);
		sCommand* command_list(string &src);
		sComponentList* component_list(string &src);
		sComponent* component(string &src);
		sComponentHeader* component_header(string &src);
		sSubComponentList* subcomponent_list(string &src);
		sSubComponent* subcomponent(string &src);
		
		COMPONENT_TYPE::TYPE GetComponentType(const string &str);
		DEVICE_TYPE::TYPE GetDeviceType(const string &str);
		SUBCOMPONENT_TYPE::TYPE GetSubComponentType(const string &str);


	public:
		sProgram *m_program;
	};

}
