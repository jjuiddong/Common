//
// 2016-04-16, jjuiddong
//
#pragma once


namespace motion
{

	struct sCommand
	{
		map<string, string> values;
	};

	struct sComponentHeader
	{
		string id;
		string filename;
	};

	namespace SUBCOMPONENT_TYPE {
		enum TYPE {
			NONE, PROTOCOL, INIT, MATH, MODULATION, FORMAT,
		};		
	};

	struct sSubComponent
	{
		SUBCOMPONENT_TYPE::TYPE type;
		sComponentHeader *header;
		string source;
	};

	struct sSubComponentList
	{
		sSubComponent *subComp;
		sSubComponentList *next;
	};

	namespace COMPONENT_TYPE {
		enum TYPE {
			NONE, INPUT, MIXER, OUTPUT, MODULE, AMPLITUDE_MODULE,
		};
	}
	namespace DEVICE_TYPE {
		enum TYPE {
			NONE, UDP, TCP, SERIAL, SHAREDMEM, JOYSTICK, CUSTOM,
		};
	}

	struct sComponent
	{
		COMPONENT_TYPE::TYPE type;
		DEVICE_TYPE::TYPE devType;
		sComponentHeader *header;
		sCommand *cmd;
		sSubComponentList *subCompList;
	};

	struct sComponentList
	{
		sComponent *comp;
		sComponentList *next;
	};

	struct sProgram
	{
		sCommand *cmd;
		sComponentList *compList;
	};

	void rm_program(sProgram*p);
	void rm_command(sCommand *p);
	void rm_componentlist(sComponentList *p);
	void rm_component(sComponent *p);
	void rm_componentheader(sComponentHeader *p);
	void rm_subcomponentlist(sSubComponentList *p);
	void rm_subcomponent(sSubComponent *p);

}
