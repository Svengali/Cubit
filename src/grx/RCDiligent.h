//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "grx/RenderContext.h"

PtrFwd( ResDGPipelineState );

//Base class for everything that can render diligent geometry
class RCDiligent : public RenderContext
{

public:
	CLASS( RCDiligent, RenderContext );
	
	RCDiligent() = default;


	RCDiligent( dg::float4x4 viewProj, DGViewPtr renderTarget, DGContextPtr devContext )
		:
		m_viewProj( viewProj ),
		m_renderTarget( renderTarget ),
		m_devContext( devContext )
	{

	}
	
	virtual ~RCDiligent() = default;


	REFLECT_BEGIN( RCDiligent, RenderContext );
	REFLECT_END();

	std::atomic_int32_t deferred = 0;

	i32 getDef()
	{
		
	}

	dg::float4x4 m_viewProj;

	DGViewPtr m_renderTarget;

	DGContextPtr m_devContext;

	static const i32 k_maxThreads = 30;

	std::array<std::atomic_bool, k_maxThreads> activeCmdLists = { false };
	std::array<ResDGPipelineStatePtr, k_maxThreads> lastUsedPSO;
	std::array<std::vector<dg::RefCntAutoPtr<dg::ICommandList>>, k_maxThreads> cmdLists;
	std::array<dg::Ptr<dg::IFence>, k_maxThreads> fences;


};

//PtrDef( RCDiligent );

