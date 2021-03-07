//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "grx/Renderer.h"


// These are meant to be major systems 
class DGRenderableSystem
{
public:

	CLASS( DGRenderableSystem );


	virtual void render() = 0;

};

PtrDef( DGRenderableSystem );

class RSEntity : public DGRenderableSystem
{
public:

	CLASS( RSEntity, DGRenderableSystem );


	virtual void render();

};

PtrDef( RSEntity );



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


	void addRenderableSystem( const DGRenderableSystemPtr &rs );


	void render();




private:
	std::vector< DGRenderableSystemPtr > m_rs;


};

PtrDef( DGRenderer );

