
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


bool cErrCorrectCompare::Compare(cDictionary *dict,
	char *src, char *dict_word, const int src_idx, const int dict_index)
{
	sInfo info1;
	ZeroMemory(&info1, sizeof(info1));	
	CompareSub(dict, src, dict_word, src_idx, dict_index, info1); // 원본 단어를 바꿔가면서 비교

	sInfo info2;
	ZeroMemory(&info2, sizeof(info2));
	info2.flags = 1;
	CompareSub(dict, src, dict_word, src_idx, dict_index, info2); // 사전 단어를 바꿔가면서 비교

	// 더 비슷한 결과를 저장하고 리턴한다.
	m_result = (info1.tot < info2.tot) ? info2 : info1;
	return true;
}


bool cErrCorrectCompare::CompareSub(cDictionary *dict, 
	char *src, char *dict_word, const int src_idx, const int dict_index, 
	sInfo &info)
{
	const int MAX_GAP = 20;
	const int MAX_ERROR = 15;
	const int MAX_WORD_ERROR = 3;

	sInfo maxFitness;
	ZeroMemory(&maxFitness, sizeof(maxFitness));
	maxFitness.tot = -1000;

	int i = src_idx;
	int k = dict_index;
	int nextCount = 0;
	int storeIdx = 0;

	while (src[i])
	{
		while (dict_word[k])
		{
			if (src[i] == dict_word[k])
			{
				nextCount = 0;
				++info.hit;
				++i;
				++k;
			}
			else if (
				(0 != dict->m_ambiguousTable[src[i]]) &&
				(dict->m_ambiguousTable[ src[i]] == dict->m_ambiguousTable[ dict_word[i]]))
			{
				// 모호한 문장으로 동일할 때, 동일한 것으로 간주
				nextCount = 0;
				++info.rep;
				++i;
				++k;
			}
			else
			{
				++info.err;

				if (0 == nextCount)
					storeIdx = (info.flags==0)? i : k;

				if (info.flags ==0)
					++i; // 인식한 단어를 증가시킨다. 문자 인식이 잘못되서, 여러 단어가 같이 들어오는 경우가 많다.
				else
					++k; // 사전 단어 인덱스만 증가시킨다. 인식한 문장에서 단어가 빠지는 경우가 많기 때문.

				++nextCount;

				// 연속적으로 매칭이 안된경우, src index or dict index를 증가시킨다.
				// 매칭이 안된 곳부터 재귀적으로 다시 검사한다.
				if (nextCount > MAX_WORD_ERROR)
				{
					info.err -= (nextCount - 1);

					sInfo tmp = info;
					CompareSub(dict, src, dict_word, i, k, tmp);
					if (tmp.tot > maxFitness.tot)
						maxFitness = tmp;

					if (0 == info.flags)
					{
						i = storeIdx + 1;
						++k;
					}
					else
					{
						k = storeIdx + 1;
						++i;
					}

					nextCount = 0;
				}
			}

			if (!src[i] || !dict_word[k])
				break;
			if (abs(i - k) > MAX_GAP) // 두 문장의 간격이 너무크면 종료.
				break;
			if (info.err >= MAX_ERROR)
				break;
		}

		if (!src[i] || !dict_word[k])
			break;
		if (abs(i - k) > MAX_GAP) // 두 문장의 간격이 너무크면 종료.
			break;
		if (info.err >= MAX_ERROR)
			break;
	}

	// 남은 여분은 단어만큼 에러 발생
	while (dict_word[k++])
		++info.err;
	while (src[i++])
		++info.err;

	info.tot = info.hit - info.err;

	if (maxFitness.tot > info.tot)
		info = maxFitness;

	return false;
}
