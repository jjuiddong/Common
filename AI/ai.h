// 
// ai class
//
#pragma once


#include "../Common/common.h"
//using namespace common;

// boost memory pool
#include <boost/pool/object_pool.hpp>


#include "aidef.h"
#include "control/object.h"
#include "control/messagemanager.h"
#include "control/aimain.h"

#include "object/agent.h"
#include "action/action.h"
#include "action/rootaction.h"
#include "object/brain.h"

#include "action/move.h"

#include "path/pathfinder.h"
#include "path/pathfinder2.h"
#include "path/pathfinder2d.h"
#include "path/navigationmesh.h"

#include "nn/neuralnet.h"
#include "nn/genetic.h"
