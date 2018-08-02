//
// 2017-06-25, jjuiddong
// text manager
//
// 2017-08-24
//	- Upgrade DX11
//
// 2018-08-01
//	- Delay Generate Text Texture
//
#pragma once


namespace graphic
{
	using namespace common;

	class cTextManager
	{
	public:
		enum { TEXTURE_SIZEX = 256, TEXTURE_SIZEY = 32, MAX_STR = 64};

		cTextManager();
		virtual ~cTextManager();

		void Create(const u_int maxTextCount = 128
			, const int textureSizeX= TEXTURE_SIZEX, const int textureSizeY = TEXTURE_SIZEY);
		void NewFrame();
		
		void AddTextRender(cRenderer &renderer
			, const __int64 id
			, const wchar_t *str 
			, const cColor &color = cColor::WHITE
			, const cColor &outlineColor = cColor::BLACK
			, BILLBOARD_TYPE::TYPE type = BILLBOARD_TYPE::Y_AXIS
			, const Transform &tm = Transform::Identity
			, const bool isDepthNone=false
			, const int width=16
			, const int height=1
			, const float dynScaleMin = 0.5f
			, const float dynScaleMax = 200.5f
		);

		void Render(cRenderer &renderer, const bool isSort=false);
		void ProcessTextCmd(cRenderer &renderer);
		void DelayGenerateText(cRenderer &renderer);
		void Sorting();
		void Clear();


	public:
		struct sText
		{
			__int64 id;
			bool used;
			bool gen;
			bool depthNone;
			int initTime;
			sAlphaBlendSpace *space;
			cText3d3 text;
		};

		struct sCommand
		{
			__int64 id;
			String<wchar_t, MAX_STR> str;
			BILLBOARD_TYPE::TYPE type;
			cColor color;
			cColor outlineColor;
			Transform tm;
			bool depthNone;
			sAlphaBlendSpace *space;
			int width;
			int height;
			float dynScaleMin;
			float dynScaleMax;
		};

		struct sDelayGenerateText
		{
			__int64 id;
			String<wchar_t, MAX_STR> str;
			BILLBOARD_TYPE::TYPE type;
			cColor color;
			cColor outlineColor;
			Transform tm;
			sText *stext;
		};

		sText* GetCacheText(const __int64 id);
		void SetCommand2Text(cRenderer &renderer, sText *text, const sCommand &cmd);
		void GarbageCollection();


	public:
		u_int m_maxTextCount;
		vector<sText*> m_renders; // reference m_buffer
		vector<sText*> m_buffer; // m_renders, m_buffer chainning system, has original memory
		vector<sCommand> m_cmds;
		vector<sDelayGenerateText> m_delayGens;
		map<__int64, sText*> m_renderMap; // reference m_buffer
		map<__int64, sText*> m_bufferMap; // reference m_buffer
		map<__int64, sText*> m_cacheMap; // reference m_buffer
		set<__int64> m_delayGenSet; // check duplicate m_delayGens

		int m_textureSizeX;
		int m_textureSizeY;
		cTimer m_timer;
		double m_timePrevSecond;
		double m_timeLoadBalance;

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
