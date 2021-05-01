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
	
	RCDiligent();
	
	virtual ~RCDiligent();


	REFLECT_BEGIN( RCDiligent, RenderContext );
	REFLECT_END();

	dg::float4x4 m_viewProj;

	dg::RefCntAutoPtr<dg::IDeviceContext> m_devContext;


};

//PtrDef( RCDiligent );

