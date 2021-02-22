//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "grx/GeoDiligent.h"


//Render context for the diligent engine
//PtrFwd( RCDiligent );

//Base class for everything that can render diligent geometry
class GDGLTF : public GeoDiligent
{

public:
	CLASS( GDGLTF, GeoDiligent );
	
	GDGLTF();
	
	GDGLTF( const char * const pMesh );
	
	virtual ~GDGLTF();

	virtual void load( const char *const pFilename );

	
	virtual void renderDiligent( const RCDiligentPtr &rcon, const cb::Frame3 &frame ) ;

	REFLECT_BEGIN( GeoDiligent, GeoDiligent );
	REFLECT_END();

private:
};

PtrDef( GeoDiligent );

