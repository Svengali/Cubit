//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "grx/Geometry.h"


//Render context for the diligent engine
/*
PtrFwd( RCDiligent );
PtrFwd( ResDGBufVertex );
PtrFwd( ResDGBufIndex );
PtrFwd( ResDGVertexShader );
PtrFwd( ResDGPixelShader );
PtrFwd( ResDGTexture );
*/

#include "grx/RCDiligent.h"
#include "res/ResDGBuffer.h"
#include "res/ResDGVertexShader.h"
#include "res/ResDGPixelShader.h"
#include "res/ResDGTexture.h"
#include "res/ResDGPipelineState.h"


class GeoDiligentCfg: public Config
{
public:

	CLASS( GeoDiligentCfg, Config );

	REFLECT_BEGIN( GeoDiligent, Config );
		REFLECT( m_pso );
		REFLECT( m_vertexBuf );
		REFLECT( m_indexBuf );
		REFLECT( m_texture );
	REFLECT_END();


	virtual void DoReflection( XMLReader &reader ) override { GeoDiligentCfg::Reflection<XMLReader>( reader ); }


	ResDGPipelineStatePtr m_pso;

	//ResDGVertexShaderPtr	m_vertexShader;
	//ResDGPixelShaderPtr		m_pixelShader;

	ResDGBufVertexPtr	m_vertexBuf;
	ResDGBufIndexPtr	m_indexBuf;

	ResDGTexturePtr	m_texture;


};


PtrDef( GeoDiligentCfg );



//Base class for everything that can render diligent geometry
class GeoDiligent: public Geometry
{

public:
	CLASS( GeoDiligent, Geometry );

	GeoDiligent();

	GeoDiligent( const ent::EntityId id, const GeoDiligentCfgPtr &cfg );

	virtual ~GeoDiligent();

	virtual void load( const char *const pFilename );


	virtual void renderDiligent( RCDiligent *pRC, const cb::Frame3 &frame );

	//virtual void DoReflection( XMLReader &reader ) override { Reflection<XMLReader>( reader ); }
	REFLECT_BEGIN( GeoDiligent, Geometry );
		REFLECT( m_cfg );
		REFLECT( m_pso );
	REFLECT_END();

	ent::EntityId			m_id;
	GeoDiligentCfgPtr m_cfg;

	ResDGPipelineStatePtr m_pso;

private:
	virtual void render( RenderContext *pRC, const cb::Frame3 &frame );
};

PtrDef( GeoDiligent );

