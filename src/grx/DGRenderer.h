//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "grx/Renderer.h"
#include "RCDiligent.h"
#include "ent/controller.h"

PtrFwd( GeoDiligent );
PtrFwd( FreefallPlane );


const i32 k_geoBlockSize = 4 * 1024;

class GeoBlock
{
public:


	enum EDynamicSlots
	{
		Status		= 0,
		EntityId	= 1,
		Frame			= 2,
		Geometry	= 3,
	};

public:


	typedef df::ComBlocks<k_geoBlockSize, GeoBlock::EDynamicSlots, cb::Frame3, GeoDiligentPtr> TCom;
	typedef TCom::AllBlocks::TBlock Block;

	TCom m_com;

public:

	void operate( std::function<void( TCom::AllBlocks::TBlock *, const i32 )> );
};




// These are meant to be major systems 
class DGRenderableSystem
{
public:

	CLASS( DGRenderableSystem );

	virtual void add( const cb::Frame3 frame, const GeoDiligentPtr &ptr ) = 0;

	virtual void render( RCDiligent *pContext, const char *pDebugRender ) = 0;

};

PtrDef( DGRenderableSystem );

class RSEntity: public DGRenderableSystem
{
public:

	CLASS( RSEntity, DGRenderableSystem );

	virtual void render( RCDiligent *pContext, const char *pDebugRender ) override;

	// Inherited via DGRenderableSystem
	virtual void add( const cb::Frame3 frame, const GeoDiligentPtr &ptr ) override;

	GeoBlock m_geos;

	FreefallPlanePtr m_freefall;


private:
	//std::vector<GeoDiligentPtr> m_geos;

};

PtrDef( RSEntity );

struct DebugGeo
{
	const cb::Frame3 frame;
	const GeoDiligentPtr geo;
};

//Class for everything that can render diligent geometry
class DGRenderer: public Renderer
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

	//void addStaticGeo( const cb::Frame3 frame, const GeoDiligentPtr &geo );



	void render( RCDiligent *pContext );


	RSEntityPtr m_rsFreefall;
	RSEntityPtr m_rsVoxels;
	RSEntityPtr m_rsCubes;

	// TODO HACK just setting the players info here. 
	//GeoDiligentPtr m_player;
	//cb::Frame3     m_playerPos;


	//std::vector<DebugGeo> m_debugGeos;

	FreefallData m_debugGeos;

	//This is silly, but it is what it is
	ent::PlayerController m_players;

private:


};

PtrDef( DGRenderer );

void processContexts( RCDiligent *pContext );
