//
// 2017-06-25, jjuiddong
// text manager
//
// 2017-08-24
//	- Upgrade DX11
//
#pragma once


namespace graphic
{
	using namespace common;

	class cTextManager
	{
	public:
		cTextManager();
		virtual ~cTextManager();

		void Create(const u_int maxTextCount = 100, const int textureSizeX=256, const int textureSizeY = 32);
		void NewFrame();
		
		void AddTextRender(cRenderer &renderer
			, const int id
			, const wchar_t *str 
			, const cColor &color = cColor::WHITE
			, const cColor &outlineColor = cColor::BLACK
			, BILLBOARD_TYPE::TYPE type = BILLBOARD_TYPE::Y_AXIS
			, const Transform &tm = Transform::Identity
			, const bool isDepthNone=false
			, const int width=8, const int height=1);

		void Render(cRenderer &renderer, const bool isSort=false);
		void ProcessTextCmd(cRenderer &renderer);
		void Sorting();
		void Clear();


	public:
		struct sText
		{
			int id;
			bool used;
			bool depthNone;
			int initTime;
			sAlphaBlendSpace *space;
			cText3d3 text;
		};

		struct sCommand
		{
			enum {MAX_STR=64};

			int id;
			String<wchar_t, MAX_STR> str;
			BILLBOARD_TYPE::TYPE type;
			cColor color;
			cColor outlineColor;
			Transform tm;
			bool depthNone;
			sAlphaBlendSpace *space;
			int width;
			int height;
		};

		sText* GetCacheText(const int id);
		void SetCommand2Text(cRenderer &renderer, sText *text, const sCommand &cmd);
		void GarbageCollection();


	public:
		enum {TEXTURE_SIZEX=256, TEXTURE_SIZEY = 32};

		u_int m_maxTextCount;
		vector<sText*> m_renders; // reference m_buffer
		vector<sText*> m_buffer; // m_renders, m_buffer chainning system, has original memory
		vector<sCommand> m_cmds;
		map<int, sText*> m_renderMap; // reference m_buffer
		map<int, sText*> m_bufferMap; // reference m_buffer
		map<int, sText*> m_cacheMap; // reference m_buffer

		int m_textureSizeX;
		int m_textureSizeY;

		// GdiPlus Buffer
		std::shared_ptr<Gdiplus::Bitmap> m_graphicBmp;
		std::shared_ptr<Gdiplus::Bitmap> m_textBmp;
		std::shared_ptr<Gdiplus::Graphics> m_graphic;
		std::shared_ptr<Gdiplus::Graphics> m_graphicText;

		// Debugging
		int m_generateCount;
		int m_cacheCount;
	};

}
