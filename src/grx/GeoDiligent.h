//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez

#pragma once

#include "grx/Geometry.h"


//Render context for the diligent engine
PtrFwd( RCDiligent );








//Base class for everything that can render diligent geometry
class GeoDiligent : public Geometry
{

public:
	CLASS( GeoDiligent, Geometry );
	
	GeoDiligent();
	
	GeoDiligent( const char * const pMesh );
	
	virtual ~GeoDiligent();

	virtual void load( const char *const pFilename );

	
	virtual void renderDiligent( const RCDiligentPtr &rcon, const cb::Frame3 &frame ) 
	{};

	REFLECT_BEGIN( GeoDiligent, Geometry );
	REFLECT_END();

private:
	virtual void render( const RenderContextPtr &rcon, const cb::Frame3 &frame );
};

PtrDef( GeoDiligent );

