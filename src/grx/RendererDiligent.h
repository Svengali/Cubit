//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez

#pragma once

#include "grx/Renderer.h"



//Base class for everything that can render diligent geometry
class RendererDiligent : public Renderer
{

public:
	CLASS( RendererDiligent, Renderer );
	
	RendererDiligent();
	
	virtual ~RendererDiligent();


	REFLECT_BEGIN( RendererDiligent, Renderer );
	REFLECT_END();

};

PtrDef( RendererDiligent );

