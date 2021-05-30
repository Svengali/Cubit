// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


typedef  uint8_t u8;
typedef   int8_t i8;
typedef uint16_t u16;
typedef  int16_t i16;
typedef uint32_t u32;
typedef  int32_t i32;
typedef uint64_t u64;
typedef  int64_t i64;
typedef  float   f32;
typedef  double  f64;



//Source level compiled in dependencies
#include "util/sptr_util.h"
#include "tinyxml/tinyxml.h"
#include "util/profile/profile.h"
#include "util/concurrentqueue.h"
#include "task/thread_pool.h"
#include "task/ctpl.h"
//#include "async++.h"
#include "math/units.h"
#include "math/units64.h"
#include "math/SimplexNoise.h"
#include "util/index.h"
#include "util/id.h"
#include "util/expected_lite.h"
//#include "util/optional.h"
#include "util/markable.h"
#include "util/lvalue_ref.h"
#include "util/not_null.h"
#include "util/out_param.h"
#include "util/tagged_bool.h"
#include "util/util.h"

#include "util/String.h"
#include "util/Symbol.h"

#include "util/Reflection.h"
#include "util/XMLReader.h"

#include "util/Clock.h"


#include "util/Serialization.h"
#include "util/Config.h"

#include "vox/vox.h"

#include "res/Resource.h"
#include "res/ResReflect.h"

#include "util/profile/profile.h"


// Mostly frozen project includes
#include "ent/entityId.h"
#include "df/com.h"

typedef  uint8_t u8;
typedef   int8_t i8;
typedef uint16_t u16;
typedef  int16_t i16;
typedef uint32_t u32;
typedef  int32_t i32;
typedef uint64_t u64;
typedef  int64_t i64;
typedef  float   f32;
typedef  double  f64;

#include "async++.h"
