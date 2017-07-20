//
// 2017-06-25, jjuiddong
// text manager
//
#pragma once


namespace graphic
{

	class cTextManager
	{
	public:
		cTextManager();
		virtual ~cTextManager();

		void Create(const u_int maxTextCount = 100, const int textureSizeX=128, const int textureSizeY = 64);
		void NewFrame();
		void AddTextRender(cRenderer &renderer, const int id, const Str128 &str 
			, const DWORD color =0
			, BILLBOARD_TYPE::TYPE type = BILLBOARD_TYPE::Y_AXIS
			, const Transform &tm = Transform::Identity
			, const int width=3, const int height=2);
		void Render(cRenderer &renderer);
		void Clear();


	public:
		struct sText
		{
			int id;
			bool used;
			cText3d3 text;
		};

		struct sCommand
		{
			enum {MAX_STR=64};

			int id;
			char str[MAX_STR];
			BILLBOARD_TYPE::TYPE type;
			DWORD color;
			Transform tm;
			int width;
			int height;
		};

		sText* GetCacheText(const int id);
		void SetCommand2Text(sText *text, const sCommand &cmd);
		void GarbageCollection();



	public:
		enum {TEXTURE_SIZEX=128, TEXTURE_SIZEY = 64};

		u_int m_maxTextCount;
		vector<sText*> m_renders; // reference m_buffer
		vector<sText*> m_buffer; // m_renders, m_buffer chainning system, has original memory
		vector<sCommand> m_cmds;
		map<int, sText*> m_renderMap; // reference m_buffer
		map<int, sText*> m_bufferMap; // reference m_buffer
		map<int, sText*> m_cacheMap; // reference m_buffer

		int m_textureSizeX;
		int m_textureSizeY;
	};

}
