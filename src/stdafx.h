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

#include "stb_image/stb_image.h"

#include "ImGui.h"

// TODO: reference additional headers your program requires here
#include <memory>
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>
#include <optional>

#include "TaskScheduler.h"


//Basic Diligent includes
#include "EngineFactory.h"
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "DeviceContext.h"
//#include "InputController.hpp"
#include "BasicMath.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include "MapHelper.hpp"

//Mostly frozen diligent things
#include "app/SampleApp.hpp"


#include "cblib_includes.h"

#include "pham_core.h"

namespace enki
{
PtrDef( TaskSet );
}


namespace dg = Diligent;

namespace Diligent
{
template< typename T >
using Ptr = RefCntAutoPtr<T>;
}

typedef dg::Ptr<dg::IBuffer>				DGBufferPtr;
typedef dg::Ptr<dg::ITextureView>		DGViewPtr;
typedef dg::Ptr<dg::ISampler>				DGSamplerPtr;
typedef dg::Ptr<dg::IPipelineState> DGPipelinePtr;
typedef dg::Ptr<dg::IDeviceContext> DGContextPtr;
typedef dg::Ptr<dg::ICommandList>		DGCommandListPtr;

typedef dg::Ptr<dg::IShaderResourceBinding> DGShaderResPtr;


#include "app/PhamApp.h"
