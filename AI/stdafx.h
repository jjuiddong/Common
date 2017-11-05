// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include "../Common/common.h"
using namespace common;


// 부스트 메모리 풀 이용.
#include <boost/pool/object_pool.hpp>


#include "aidef.h"
#include "control/object.h"
#include "control/messagemanager.h"
#include "control/aimain.h"

#include "object/actorinterface.h"
#include "action/action.h"
#include "action/rootaction.h"
#include "object/aiactor.h"

#include "action/move.h"

#include "path\pathfinder.h"

