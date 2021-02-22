//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "grx/Renderer.h"



//Class for everything that can render diligent geometry
class DGRenderer : public Renderer
{

public:
	CLASS( DGRenderer, Renderer );
	
	static DGRenderer &Inst();

	static void startup();
	static void shutdown();

	DGRenderer();
	
	virtual ~DGRenderer();


	REFLECT_BEGIN( DGRenderer, Renderer );
	REFLECT_END();







};

PtrDef( DGRenderer );

