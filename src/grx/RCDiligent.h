//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "grx/RenderContext.h"



//Base class for everything that can render diligent geometry
class RCDiligent : public RenderContext
{

public:
	CLASS( RCDiligent, RenderContext );
	
	RCDiligent() = default;


	RCDiligent( dg::float4x4 viewProj, DGViewPtr renderTarget, DGContextPrt devContext )
		:
		m_viewProj( viewProj ),
		m_renderTarget( renderTarget ),
		m_devContext( devContext )
	{

	}
	
	virtual ~RCDiligent() = default;


	REFLECT_BEGIN( RCDiligent, RenderContext );
	REFLECT_END();

	dg::float4x4 m_viewProj;

	DGViewPtr m_renderTarget;

	DGContextPrt m_devContext;


};

//PtrDef( RCDiligent );

