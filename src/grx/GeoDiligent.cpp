//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez



#include "../stdafx.h"

#include "./GeoDiligent.h"
#include "./RCDiligent.h"

#include "grx/RenderContext.h"

SERIALIZABLE( GeoDiligent );

GeoDiligent::GeoDiligent()
{
	//Do Nothing.
}


GeoDiligent::GeoDiligent( const char * const pMesh )
{
}

GeoDiligent::~GeoDiligent(void)
{
}

void GeoDiligent::load( const char *const pFilename )
{

}



void GeoDiligent::render( const RenderContextPtr &rcon, const cb::Frame3 &frame )
{
	std::shared_ptr<RCDiligent> rcPtr = std::dynamic_pointer_cast<RCDiligent>(rcon);

	renderDiligent( rcPtr, frame );
}


