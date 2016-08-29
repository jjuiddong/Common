//
// 2016-07-31, jjuiddong
// 사전 기능
// 사전에는 소문자로 저장되고, 소문자로 매칭한다.
// 한 칸 이상의 공백은 한 칸으로 간주한다.
// 사용되지 않는 문자나 특수문자는 매칭에서 제외 된다.
//
// 2016-08-22
//	- 문자열 검색을 levenshtein_distance 로 수정
// 
//
#pragma once


namespace tess
{

	class cDictionary2
	{
	public:
		cDictionary2();
		virtual ~cDictionary2();

		bool Init(const string &fileName);
		string FastSearch(const string &sentence, OUT vector<string> &out);
		string ErrorCorrectionSearch(const string &sentence, OUT float &maxFitness);
		string Search(const string &sentence, OUT vector<string> &out, OUT float &maxFitness);
		void Clear();


	protected:
		unsigned int cDictionary2::levenshtein_distance(const std::string& s1, const std::string& s2);


	public:
		struct sSentence
		{
			string src; // 원본 단어
			string lower; // 소문자 단어
			string output; // 매칭이되고 리턴될 문자열, { ~~ } 로 설정한다., 기본값은 원본 단어
		};

		vector<sSentence> m_sentences;
		vector<string> m_words; // lower case words
		map<string, int> m_sentenceLookUp; // sentence (lower case), sentence id (m_sentences index)
		map<string, int> m_wordLookup; // all separate word (lower case), word id (m_words index)
		std::vector<unsigned int> col, prevCol; // for levenshtein_distance function
		char m_ambiguousTable[256]; // alphabet + number + special char
		int m_ambigId;

		enum {
			MAX_CHAR=256,
			MAX_WORD = 1000, // 최대 단어 갯수
			MIN_WORD_LEN=3,
		};

		set<int> m_sentenceWordSet[MAX_WORD]; // 해당하는 단어가 포함된 문장 id의 집합, index = m_words index
	};

}
