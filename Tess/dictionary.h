//
// 2016-07-31, jjuiddong
// 사전 기능
// 사전에는 소문자로 저장되고, 소문자로 매칭한다.
// 한 칸 이상의 공백은 한 칸으로 간주한다.
// 사용되지 않는 문자나 특수문자는 매칭에서 제외 된다.
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
		string FastSearch(const string &sentence, OUT vector<string> &out);
		string ErrorCorrectionSearch(const string &sentence, OUT float &maxFitness);
		string Search(const string &sentence, OUT vector<string> &out, OUT float &maxFitness);
		void Clear();


	protected:
		void GenerateCharTable();


	public:
		struct sSentence
		{
			string src; // 원본 단어
			string lower; // 소문자 단어
			string output; // 매칭이되고 리턴될 문자열, { ~~ } 로 설정한다.
		};

		vector<sSentence> m_sentences;
		vector<string> m_words; // lower case words
		map<string, int> m_sentenceLookUp; // sentence (lower case), sentence id (m_sentences index)
		map<string, int> m_wordLookup; // all separate word (lower case), word id (m_words index)
		char m_ambiguousTable[256]; // alphabet + number + special char
		int m_ambigId;

		struct sCharTable
		{
			set<uint> sentenceIds;
		};

		enum {
			MAX_CHAR=256,
			MAX_LEN = 30,
			MAX_WORD = 1000, // 최대 단어 갯수
			MIN_WORD_LEN=3,
		};
		sCharTable m_charTable[MAX_CHAR][MAX_LEN]; // character table
		bool m_useChar[MAX_CHAR]; // 사용되고 있는 문자라면 true 가된다. GenerateCharTable() 에서 초기화

		set<int> m_sentenceWordSet[MAX_WORD]; // 해당하는 단어가 포함된 문장 id의 집합, index = m_words index
	};

}
