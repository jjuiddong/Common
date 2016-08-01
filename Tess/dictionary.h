//
// 2016-07-31, jjuiddong
// 사전 기능
//
#pragma once


namespace tess
{

	class cDictionary
	{
	public:
		cDictionary();
		virtual ~cDictionary();

		bool Init(const string &fileName);
		string FastSearch(const string &word, OUT vector<string> &out);
		string ErrorCorrection(const string &word);
		void Clear();


	protected:
		void GenerateCharTable();


	public:
		struct sSentence
		{
			string src; // 원본 단어
			string lower; // 소문자 단어
		};

		vector<sSentence> m_sentences;
		map<string, int> m_sentenceLookUp; // sentence (lower case), sentence id
		map<string, int> m_wordLookup; // all separate word (lower case), int=m_words index
		vector<string> m_words; // lower case words
		char m_ambiguousTable[256]; // alphabet + number + special char
		int m_ambigId;

		struct sCharTable
		{
			vector<uint> words;
		};

		enum {
			MAX_CHAR=256,
			MAX_LEN = 30,
			MAX_WORD = 1000, // 최대 단어 갯수
			MIN_WORD_LEN=3,
		};
		sCharTable m_charTable[MAX_CHAR][MAX_LEN]; // character table
		bool m_useChar[MAX_CHAR]; // 사용되고 있는 문자라면 true 가된다. GenerateCharTable() 에서 초기화

		set<int> m_wordSet[MAX_WORD]; // 단어가 포함된 사전의 문자 id 를 저장한다.
	};

}
