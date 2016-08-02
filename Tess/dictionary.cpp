
#include "stdafx.h"
#include "dictionary.h"
#include "scanner.h"
#include "errorcorrectcompare.h"


using namespace tess;

cDictionary::cDictionary()
	: m_ambigId(1)
{
}

cDictionary::~cDictionary()
{
}


// 사전 스크립트를 읽어서, 단어 사전을 만든다.
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

		if (0 == state ) // word list command
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
			vector<string> words;
			tokenizer(lower, " ", "", words);
			for each (auto word in words)
			{
				trim(word);
				if (word.length() < MIN_WORD_LEN)
					continue; // 너무 짧은 단어는 제외한다.

				int wordId = 0;
				const auto it = m_wordLookup.find(word);
				if (it == m_wordLookup.end())
				{
					m_words.push_back(word);
					wordId = m_words.size() - 1;
					m_wordLookup[word] = m_words.size()-1;
				}
				else
				{
					wordId = it->second;
				}

				if (wordId < MAX_WORD)
					m_sentenceWordSet[wordId].insert(sentenceId);
			}

		}
		else if (1 == state) // ambiguous command
		{
			// format = c1 - c2 - c3
			string tmp = line;
			const string lower = lowerCase(tmp);

			vector<string> words;
			tokenizer(lower, "-", "", words);
			for each (auto word in words)
			{
				trim(word);
				if (word.empty())
					continue;
				if (word.length() > 1) // allow only one character
					continue;
				m_ambiguousTable[word[0]] = m_ambigId; // 같은 ID를 부여한다.
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
// ErrorCorrectionSearch() 보다 빠르다.
string cDictionary::FastSearch(const string &sentence, OUT vector<string> &out)
{
	cScanner scanner(*this, sentence);// 필요없는 문자 제외
	if (scanner.IsEmpty())
		return "";

	stringstream ss;
	set<int> sentenceSet; // words 단어에 포함된 문장 id의 집합

	// 문장을 단어 단위로 비교한다.
	vector<string> words;
	tokenizer(scanner.m_str, " ", "", words);
	for each (auto word in words)
	{
		trim(word);
		if (word.length() < MIN_WORD_LEN)
			continue; // 너무 짧은 단어는 제외한다.
		
		auto it = m_wordLookup.find(word);
		if (it == m_wordLookup.end())
			continue;

		ss << (out.empty()? word : string(" ")+ word);
		out.push_back(word); // add maching word

		const int wordId = it->second;
		if (wordId >= MAX_WORD)
			continue;

		if (sentenceSet.empty())
		{
			sentenceSet = m_sentenceWordSet[wordId];
		}
		else
		{
			// 두 단어가 포함된 문장들끼리의 교집합을 구한다.
			vector<int> tmp;
			std::set_intersection(sentenceSet.begin(), sentenceSet.end(),
				m_sentenceWordSet[wordId].begin(), m_sentenceWordSet[wordId].end(),
				std::back_inserter(tmp));
				
			sentenceSet.clear();
			for each (auto id in tmp)
				sentenceSet.insert(id);
		}
	}
	
	if (sentenceSet.size() == 1)
		return m_sentences[*sentenceSet.begin()].src;

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
			m_charTable[i][k].sentenceIds.clear();

	for (uint i = 0; i < MAX_WORD; ++i)
		m_sentenceWordSet[i].clear();
}


void cDictionary::GenerateCharTable()
{
	for (uint i = 0; i < m_sentences.size(); ++i)
	{
		const uint sentenceId = i;
		const sSentence &sentence = m_sentences[i];
		for (uint k=0; k < sentence.lower.length(); ++k)
		{
			if (k >= MAX_LEN)
				break;
 			const char c = sentence.lower[k];
			m_charTable[c][k].sentenceIds.insert(sentenceId);
			m_useChar[c] = true;
		}
	}
}


// 에러를 보정하면서, 문장을 사전에서 찾는다.
// FastSearch() 보다 느리다.
string cDictionary::ErrorCorrectionSearch(const string &sentence)
{
	cScanner scanner(*this, sentence);// 필요없는 문자 제외
	if (scanner.IsEmpty())
		return "";

	vector<bool> procSentenceIds(m_sentences.size(), false);

	int maxSentenceId = -1;
	int maxTotCount = -100;

	string src = scanner.m_str;
	for (uint i = 0; i < src.size(); ++i)
	{
		const uchar c = src[i];
		for (uint k = 0; k < MAX_LEN; ++k)
		{
			for each (auto sentenceId in m_charTable[c][k].sentenceIds)
			{
				if (procSentenceIds[sentenceId])
					continue;
				procSentenceIds[sentenceId] = true;

				cErrCorrectCompare cmp;
				string tmp = src;
				cmp.Compare(*this, (char*)&tmp[i], (char*)&m_sentences[sentenceId].lower[k] );
				if (cmp.m_result.tot > maxTotCount)
				{
					maxTotCount = cmp.m_result.tot;
					maxSentenceId = sentenceId;
				}
			}
		}
	}

	if (maxSentenceId >= 0)
		return m_sentences[maxSentenceId].src;

	return "";
}


// FastSearch() + ErrorCorrectionSearch()
string cDictionary::Search(const string &sentence, OUT vector<string> &out)
{
	string result = FastSearch(sentence, out);
	if (result.empty())
		result = ErrorCorrectionSearch(sentence);
	return result;
}
