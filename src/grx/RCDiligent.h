//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez

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

};

PtrDef( RCDiligent );

