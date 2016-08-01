
#include "stdafx.h"
#include "dictionary.h"
#include "scanner.h"


using namespace tess;

cDictionary::cDictionary()
	: m_ambigId(1)
{
}

cDictionary::~cDictionary()
{
}


bool cDictionary::Init(const string &fileName)
{
	using namespace std;

	Clear();

	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;
	
	int state = 0; // 0:word, 1:ambiguous
	string line;
	while (getline(ifs, line))
	{
		trim(line);
		if (line.empty())
			continue;

		if (line == "word:")
		{
			state = 0;
			continue;
		}
		else if (line == "ambiguous:")
		{
			state = 1;
			continue;
		}

		if (0 == state )
		{
			string tmp = line;
			const string lower = lowerCase(tmp);
			if (m_sentenceLookUp.end() == m_sentenceLookUp.find(lower))
			{// 사전에 추가
				m_sentences.push_back({line, lower});
				m_sentenceLookUp[lower] = m_sentences.size() - 1; // word index
			}
			else
			{ // 사전에 이미 있으니 넘어감
				dbg::ErrLog("error already exist word = [%s]\n", line.c_str());
			}

			// 문장을 공백을 기준으로 나눠 단어로 만든후 저장.
			const int sentenceId = m_sentences.size() - 1;
			vector<string> strs;
			tokenizer(lower, " ", "", strs);
			for each (auto str in strs)
			{
				trim(str);
				if (str.length() < MIN_WORD_LEN)
					continue; // 너무 짧은 단어는 제외한다.

				int wordId = 0;
				const auto it = m_wordLookup.find(str);
				if (it == m_wordLookup.end())
				{
					m_words.push_back(str);
					wordId = m_words.size() - 1;
					m_wordLookup[str] = m_words.size()-1;
				}
				else
				{
					wordId = it->second;
				}

				if (wordId < MAX_WORD)
					m_wordSet[wordId].insert(sentenceId);
			}

		}
		else if (1 == state)
		{
			// format = c1 - c2 - c3
			vector<string> strs;
			tokenizer(line, "-", "", strs);
			for each (auto str in strs)
			{
				trim(str);
				if (str.empty())
					continue;
				if (str.length() > 1) // allow only one character
					continue;
				m_ambiguousTable[str[0]] = m_ambigId; // 같은 ID를 부여한다.
			}
			++m_ambigId;
		}
	}

	// dictionary script 를 모두 읽은 후, 테이블을 생성한다.
	GenerateCharTable();
	
	return true;
}


// word 가 사전에 있는 단어로만 구성되어 있다면, true를 리턴한다.
// 일치하는 단어를 out에 저장해 리턴한다.
string cDictionary::FastSearch(const string &word, OUT vector<string> &out)
{
	cScanner scanner(*this, word);
	if (scanner.IsEmpty())
		return "";

	stringstream ss;
	set<int> sentences;

	vector<string> strs;
	tokenizer(scanner.m_str, " ", "", strs);
	for each (auto str in strs)
	{
		trim(str);
		if (str.length() < MIN_WORD_LEN)
			continue; // 너무 짧은 단어는 제외한다.
		
		auto it = m_wordLookup.find(str);
		if (it != m_wordLookup.end())
		{
			ss << (out.empty()? str : string(" ")+ str);
			out.push_back(str); // add maching word

			const int wordId = it->second;
			if (wordId >= MAX_WORD)
				continue;

			if (sentences.empty())
			{
				sentences = m_wordSet[wordId];
			}
			else
			{
				vector<int> tmp;
				std::set_intersection(sentences.begin(), sentences.end(),
					m_wordSet[wordId].begin(), m_wordSet[wordId].end(),
					std::back_inserter(tmp));
				
				sentences.clear();
				for each (auto id in tmp)
					sentences.insert(id);
			}
		}
	}
	
	if (sentences.empty())
		return "";

	const string lastSentence = ss.str();
	for each (int id in sentences)
	{
		if (lastSentence == m_sentences[id].lower)
		{
			// find in dictionary
			return m_sentences[id].src;
		}
	}

	return "";
}


void cDictionary::Clear()
{
	m_ambigId = 0;
	m_sentences.clear();
	m_words.clear();
	m_sentenceLookUp.clear();
	ZeroMemory(m_useChar, sizeof(m_useChar));

	for (uint i = 0; i < MAX_CHAR; ++i)
		for (uint k = 0; k < MAX_LEN; ++k)
			m_charTable[i][k].words.clear();

	for (uint i = 0; i < MAX_WORD; ++i)
		m_wordSet[i].clear();
}


void cDictionary::GenerateCharTable()
{
	for (uint i = 0; i < m_sentences.size(); ++i)
	{
		const uint wordId = i;
		const sSentence &word = m_sentences[i];
		for (uint k=0; k < word.src.length(); ++k)
		{
			if (k >= MAX_LEN)
				break;
 			const char c = word.src[k];
			m_charTable[c][k].words.push_back(wordId);
			m_useChar[c] = true;
		}
	}
}


string cDictionary::ErrorCorrection(const string &word)
{


	return "";
}
