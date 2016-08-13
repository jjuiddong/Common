
#include "stdafx.h"
#include "errorcorrectcompare.h"
#include "dictionary.h"


using namespace tess;

cErrCorrectCompare::cErrCorrectCompare()
{
	ZeroMemory(&m_result, sizeof(m_result));
}

cErrCorrectCompare::~cErrCorrectCompare()
{
}


bool cErrCorrectCompare::Compare(const cDictionary &dict, char *src, char *dict_word)
{
	CompareSub2Type(dict, src, dict_word, m_result);
	return true;
}


bool cErrCorrectCompare::CompareSub2Type(const cDictionary &dict, char *src, char *dict_word, OUT sInfo &info)
{
	sInfo info1;
	ZeroMemory(&info1, sizeof(info1));
	CompareSub(dict, src, dict_word, info1); // 원본 단어를 바꿔가면서 비교

	sInfo info2;
	ZeroMemory(&info2, sizeof(info2));
	info2.flags = 1;
	CompareSub(dict, src, dict_word, info2); // 사전 단어를 바꿔가면서 비교

	// 더 비슷한 결과를 저장하고 리턴한다.
	info = (info1.tot < info2.tot) ? info2 : info1;
	return true;
}


bool cErrCorrectCompare::CompareSub(const cDictionary &dict, char *src, char *dict_word, OUT sInfo &info)
{
	const int MAX_ERROR = 15;
	const int MAX_WORD_ERROR = 3;

	sInfo maxFitness;
	ZeroMemory(&maxFitness, sizeof(maxFitness));
	maxFitness.tot = -1000;

	int nextCount = 0;
	char *storePtr = NULL;

	while (*src)
	{
		while (*dict_word)
		{
			if ((*src < 0) || (*dict_word < 0)) // 유니코드 작업 필요.
				break;
			if ((*src == '\n') || (*src == '\r')) // 개행문자면, 종료.
				break;

			if (*src == *dict_word)
			{
				nextCount = 0;
				++info.hit;
				++src;
				++dict_word;
			}
			else if (
				(0 != dict.m_ambiguousTable[*src]) &&
				(dict.m_ambiguousTable[ *src] == dict.m_ambiguousTable[ *dict_word]))
			{
				// 모호한 문장으로 동일할 때, 동일한 것으로 간주
				nextCount = 0;
				++info.rep;
				++src;
				++dict_word;
			}
			else
			{
				++info.err;

				if (0 == nextCount)
					storePtr = (info.flags==0)? src : dict_word;

				if (info.flags ==0)
					++src; // 인식한 단어를 증가시킨다. 문자 인식이 잘못되서, 여러 단어가 같이 들어오는 경우가 많다.
				else
					++dict_word; // 사전 단어 인덱스만 증가시킨다. 인식한 문장에서 단어가 빠지는 경우가 많기 때문.

				++nextCount;

				// 연속적으로 매칭이 안된경우, src or dict_word 를 증가시킨다.
				// 매칭이 안된 곳부터 재귀적으로 다시 검사한다.
				if (nextCount > MAX_WORD_ERROR)
				{
					info.err -= (nextCount - 1);

					sInfo tmp = info;
					CompareSub2Type(dict, src, dict_word, tmp);
					if (tmp.tot > maxFitness.tot)
						maxFitness = tmp;

					if (0 == info.flags)
					{
						src = storePtr + 1;
						++dict_word;
					}
					else
					{
						dict_word = storePtr + 1;
						++src;
					}

					nextCount = 0;
				}
			}

			if (!*src || !*dict_word)
				break;
			if (info.err >= MAX_ERROR)
				break;
		}

		if (!*src || !*dict_word)
			break;
		if ((*src == '\n') || (*src == '\r')) // 개행문자면, 종료.
			break;
		if (info.err >= MAX_ERROR)
			break;
		if ((*src < 0) || (*dict_word < 0)) // 유니코드 작업 필요.
			break;
	}

	// 남은 여분은 문자만큼 에러 발생
	while (*dict_word++)
		++info.err;
	while (*src++)
		++info.err;

	info.tot = info.hit - info.err;

	if (maxFitness.tot > info.tot)
		info = maxFitness;

	return false;
}
