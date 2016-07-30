
#include "stdafx.h"
#include "qrquadrant.h"


namespace ar
{
	//QRCode 4사분면 탐색 노드
	struct sQRQuadrant
	{
		int axisId;
		int nextNode1; // dot>=0 일 때, 다음단계 탐색 노드 인덱스, -1 이면 터미널노드, -2 이면 탐색실패
		int nextNode2; // dot<0 일 때, 다음단계 탐색 노드 인덱스, -1 이면 터미널노드, -2 이면 탐색실패
		int quadrant; // -1:탐색실패, 0:1사분면, 1:2사분면, 2:3사분면, 3:4사분면
	};

	sQRQuadrant quadrant[10];
}


using namespace ar;



//----------------------------------------------------------------------------------------------------
// quadrarant 검색 트리 생성
//
void ar::Init()
{

	// 4
	quadrant[0].axisId = 4;
	quadrant[0].nextNode1 = 1;
	quadrant[0].nextNode2 = 2;

	// 4 -> 2
	quadrant[1].axisId = 2;
	quadrant[1].nextNode1 = 3;
	quadrant[1].nextNode2 = 4;

	// 4 -> 2 -> 3
	quadrant[3].axisId = 3;
	quadrant[3].nextNode1 = -1;
	quadrant[3].nextNode2 = -2;
	quadrant[3].quadrant = 0; // 1사분면

	// 4 -> 2 -> 6
	quadrant[4].axisId = 6;
	quadrant[4].nextNode1 = 5;
	quadrant[4].nextNode2 = -2;

	// 4 -> 2 -> 6 -> 5
	quadrant[5].axisId = 5;
	quadrant[5].nextNode1 = -1;
	quadrant[5].nextNode2 = -2;
	quadrant[5].quadrant = 3;  // 4사분면

	// 4 -> 0
	quadrant[2].axisId = 0;
	quadrant[2].nextNode1 = 6;
	quadrant[2].nextNode2 = -2;

	// 4 -> 0 -> 2
	quadrant[6].axisId = 2;
	quadrant[6].nextNode1 = 7;
	quadrant[6].nextNode2 = 8;

	// 4 -> 0 -> 2 -> 1
	quadrant[7].axisId = 1;
	quadrant[7].nextNode1 = -1;
	quadrant[7].nextNode2 = -2;
	quadrant[7].quadrant = 1; // 2사분면

	// 4 -> 0 -> 2 -> 6
	quadrant[8].axisId = 6;
	quadrant[8].nextNode1 = 9;
	quadrant[8].nextNode2 = -2;

	// 4 -> 0 -> 2 -> 6 -> 7
	quadrant[9].axisId = 7;
	quadrant[9].nextNode1 = -1;
	quadrant[9].nextNode2 = -2;
	quadrant[9].quadrant = 2; // 3사분면


}



//----------------------------------------------------------------------------------------------------
// nearId를 중심으로 가장 가까운 4점을 찾는다.
// 찾으면 true를 리턴한다.
//
// 1 ----  2 ---- 3 
// |         |         |
// |         |         |
// 0 ---- * ---- 4
// |         |         |
// |         |         |
// 7 ---- 6 ---- 5 
//
bool ar::FindQuadrant(const int tableW, const int tableH, const int nearId, const ar::sQRPos *qrMap, OUT int *nearQuad)
{
	const int startId = 0;

	const int centerId = nearId;

	// left, lefttop, top, righttop, right, rightbottom, bottom, leftbottom
	const int surroundIds[] = { centerId - 1, centerId - tableW - 1, centerId - tableW, centerId - tableW + 1,
		centerId + 1, centerId + tableW + 1, centerId + tableW, centerId + tableW - 1 };

	//
	// leftTop ---- top ---- rightTop
	// |                   |              |
	// |                   |              |
	// left ---- center ---- right
	// |                   |              |
	// |                   |              |
	// leftBottom ---- bottom ---- rightBottom
	//

	vector<Vector3> directions(8);
	const Vector3 gridCenterPos(qrMap[nearId].pos.x, qrMap[nearId].pos.y, 0);

	for (int i = 0; i < 9; ++i)
	{
		const int id = surroundIds[i];
		if (id < 0)
			continue;
		if (id >= (tableH*tableW))
			continue;
		if (!qrMap[id].used)
			continue;

		Vector3 p1(qrMap[id].pos.x, qrMap[id].pos.y, 0);
		Vector3 dir = p1 - gridCenterPos;
		dir.Normalize();
		directions[i] = dir;
	}
	


	// * ---- * ---- * 
	// |         |         |
	// |   2    |   1    |
	// * ---- * ---- *
	// |         |         |
	// |   3    |   4    |
	// * ---- * ---- * 
	ZeroMemory(nearQuad, sizeof(int)*4);

	const Vector3 centerPos(640 / 2, 480 / 2, 0);
	Vector3 v1 = centerPos - gridCenterPos;
	v1.Normalize();

	// 1,2,3,4사 분면 검색.. 
	int quadrantId = -1;
	int nodeIndex = 0;
	while (nodeIndex >= 0)
	{
		const int axisId = quadrant[nodeIndex].axisId;
		if (directions[axisId].IsEmpty())
		{
			nodeIndex = quadrant[nodeIndex].nextNode2;
			continue;
		}

		const float dot = directions[axisId].DotProduct(v1);
		const int nextNode = (dot >= 0) ? quadrant[nodeIndex].nextNode1 : quadrant[nodeIndex].nextNode2;

		if (-1 == nextNode) // terminal node
		{
			quadrantId = quadrant[nodeIndex].quadrant;
			break;
		}
		else if (-2 == nextNode) // fail
		{
			break;
		}
		else // go to next node
		{
			nodeIndex = nextNode;
		}
	}

	switch (quadrantId)
	{
	case 0:
		// 1사 분면
		nearQuad[0] = surroundIds[2];
		nearQuad[1] = surroundIds[3];
		nearQuad[2] = nearId;
		nearQuad[3] = surroundIds[4];
		break;

	case 1:
		// 2사 분면
		nearQuad[0] = surroundIds[1];
		nearQuad[1] = surroundIds[2];
		nearQuad[2] = surroundIds[0];
		nearQuad[3] = nearId;
		break;

	case 2:
		// 3사 분면
		nearQuad[0] = surroundIds[0];
		nearQuad[1] = nearId;
		nearQuad[2] = surroundIds[7];
		nearQuad[3] = surroundIds[6];
		break;

	case 3:
		// 4사 분면
		nearQuad[0] = nearId;
		nearQuad[1] = surroundIds[4];
		nearQuad[2] = surroundIds[6];
		nearQuad[3] = surroundIds[5];
		break;

	default:
		return false;
	}

	return true;
}
