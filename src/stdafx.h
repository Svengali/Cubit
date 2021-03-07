// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#pragma warning(disable : 4100)

#include <stdio.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

// Needs to be in front of everything
#include "../../cblib/stl_basics.h"


#include <Windows.h>

#include "ImGui.h"

// TODO: reference additional headers your program requires here
#include <memory>
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>
#include <optional>


#include "cblib_includes.h"

#include "pham_core.h"

//Basic Diligent includes
#include "EngineFactory.h"
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
//#include "InputController.hpp"
#include "BasicMath.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"

//Mostly frozen diligent things
#include "app/SampleApp.hpp"


namespace dg = Diligent;
