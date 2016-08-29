//
// 2016-08-01, jjuiddong
// 사전에 등록된 문장과, 입력으로 들어온 문장을 비교해서 검사한다.
// 오류를 보정하면서 검사한다.
//
#pragma once


namespace tess
{
	class cDictionary;

	class cErrCorrectCompare
	{
	public:
		struct sInfo
		{
			int tot;
			int hit;
			int err;
			int rep;
			int flags;
		};

		cErrCorrectCompare();
		virtual ~cErrCorrectCompare();
		bool Compare(const cDictionary &dict, char *src, char *dict_word);


	protected:
		bool CompareSub(const cDictionary &dict, char *src, char *dict_word, OUT sInfo &info);
		bool CompareSub2Type(const cDictionary &dict, char *src, char *dict_word, OUT sInfo &info);


	public:
		sInfo m_result;
	};
}

