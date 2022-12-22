/**
 * @file Branch.h
 * Common include for users interacting with Branches
 */
#pragma once

#include <exception>
#include <map>
#include <memory>
#include <type_traits>
#include <vector>

#include "hdtree/AbstractBranch.h"
#include "hdtree/Access.h"
#include "hdtree/Constants.h"
#include "hdtree/Reader.h"
#include "hdtree/Writer.h"

// GeneralBranch goes first so that the other
// templates can be partial template specializations of it
#include "hdtree/branch/AtomicBranch.h"
#include "hdtree/branch/GeneralBranch.h"
#include "hdtree/branch/MapBranch.h"
#include "hdtree/branch/VectorBranch.h"
