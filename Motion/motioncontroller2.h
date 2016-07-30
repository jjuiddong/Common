//
// 2016-04-13, jjuiddong
// 모션 컨트롤 전역 클래스
//
#pragma once

#include "motiondef.h"


namespace motion
{
	class cInput;
	class cMixer;
	class cOutput;
	
	class cController2 : public common::cSingleton<cController2>
	{
	public:
		cController2();
		virtual ~cController2();

		bool Init(const HWND hWnd, const string &fileName);
		bool Reload(const string &fileName);
		bool AddInput(cInput *input);
		bool AddOutput(cOutput *output);
		bool AddMixer(cMixer *mixer);
		bool AddModule(cModule *module);
		bool AddPostModule(cModule *module);
		cInput* GetInput(const int index);
		cOutput* GetOutput(const int index);
		cMixer* GetMixer(const int index);
		cModule* GetModule(const int index);
		cModule* GetPostModule(const int index);
		void Clear();

		virtual bool Start();
		virtual bool Stop();
		virtual bool Update(const float deltaSeconds);
		void SetOutputFormat(const int outputIndex, const int formatIndex);


	protected:
		bool MotionScriptInterpreter();
		bool CreateComponent(const sComponent *parentComp);


	public:
		MODULE_STATE::TYPE m_state;
		HWND m_hWnd;
		vector<cInput*> m_inputs;
		vector<cModule*> m_modules;
		vector<cMixer*> m_mixers;
		vector<cModule*> m_postModules;
		vector<cOutput*> m_outputs;
		motion::cMotionScriptParser m_script;
		cShmMap<script::sFieldData> m_shmSymbols;
	};

}
