//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez



#include "../stdafx.h"

#include "./DGRenderer.h"

#include "res/ResDGPixelShader.h"
#include "res/ResDGVertexShader.h"
#include "res/ResDGTexture.h"
#include "grx/GeoDiligent.h"

#include "res/ResourceMgr.h"

SERIALIZABLE( DGRenderer );

static DGRenderer *s_renderer = nullptr;

DGRenderer &DGRenderer::Inst()
{
	return *s_renderer;
}

ResourcePtr nullCreate( const char *const pFilename, const util::Symbol &type )
{
	return nullptr;
}


void DGRenderer::startup()
{
	s_renderer = new DGRenderer();

	Renderer::startup( s_renderer );

	//void RegisterCreator( const char *const pExtension, const util::Symbol &type, FnCreator func );

	ResourceMgr::AppStart();

	ResourceMgr::RegisterCreator( "xml", Config::SClass(), &Config::create );

	ResourceMgr::RegisterCreator( "psh", ResDGPixelShader::SClass(), ResDGPixelShader::create );
	ResourceMgr::RegisterCreator( "vsh", ResDGVertexShader::SClass(), ResDGVertexShader::create );

	ResourceMgr::RegisterCreator( "verts", ResDGPixelShader::SClass(), nullCreate );
	ResourceMgr::RegisterCreator( "indices", ResDGVertexShader::SClass(), nullCreate );

	//ResourceMgr::RegisterCreator( "xml", GeoDiligentCfg::SClass(), GeoDiligentCfg::create );

	ResourceMgr::RegisterCreator( "png", ResDGTexture::SClass(), ResDGTexture::create );
	ResourceMgr::RegisterCreator( "jpg", ResDGTexture::SClass(), ResDGTexture::create );


	RSEntityPtr rs = RSEntityPtr( new RSEntity() );

	s_renderer->addRenderableSystem( rs );


}

void DGRenderer::shutdown()
{
	ResourceMgr::AppStop();

	delete s_renderer;
	s_renderer = nullptr;
}



DGRenderer::DGRenderer()
{
}


DGRenderer::~DGRenderer( void )
{
}

void DGRenderer::addRenderableSystem( const DGRenderableSystemPtr &rs )
{
	m_rs.push_back( rs );
}


void DGRenderer::render( RCDiligent *pContext )
{


	for( auto rs : m_rs )
	{
		rs->render( pContext );
	}

}



void DGRenderer::addGeo( const cb::Frame3 frame, const GeoDiligentPtr &geo )
{
	m_rs.front()->add( frame, geo );
}


void RSEntity::render( RCDiligent *pContext )
{

	m_geos.operate( [pContext]( GeoBlock::Block *pBlock, const i32 max ){

		const auto *__restrict const pSrcFrame	= pBlock->src<GeoBlock::Frame, cb::Frame3>();
		const auto *__restrict const pSrcGeo		= pBlock->src<GeoBlock::Geometry, GeoDiligentPtr>();

		for( i32 i = 0; i < max; ++i )
		{
			const cb::Frame3 &frame = pSrcFrame[i];
			const GeoDiligentPtr &geo = pSrcGeo[i];

			geo->renderDiligent( pContext, frame );
		}


		} );



}

void RSEntity::add( const cb::Frame3 frame, const GeoDiligentPtr &ptr )
{
	const auto id = ptr->m_id;

	m_geos.m_com.insert( id, frame, ptr );


	GeoBlock::Block::TIndividualTuple values;

	m_geos.m_com.debug_get( id, &values );
}



/*
void GeoBlock::updateBlock( const uint64_t dtMs, TCom::AllBlocks::TBlock &block, const i32 count )
{
	const auto *__restrict const pSrcFrame = block.src<Frame, cb::Frame3>();
	const auto *__restrict const pSrcGeo   = block.src<Geometry, GeoDiligentPtr>();

	for( i32 i = 0; i < count; ++i )
	{
		const cb::Frame3 &frame = pSrcFrame[i];
		const GeoDiligentPtr &geo = pSrcGeo[i];

		geo->renderDiligent( nullptr, frame );


	}
}
*/

void GeoBlock::operate( std::function<void( Block *, const i32 max )> fn )
{
	const auto size = (int)m_com.m_blocks.m_block.size();

	async::parallel_for( async::irange( 0, size ), [fn, this]( int iBlock ) {
		auto &block = m_com.m_blocks.m_block[iBlock];

		const auto max = m_com.m_blocks.m_allocated[iBlock];

		fn( block.get(), max );


		//updateBlock( dtMs, *block.get(), count );
		} );
}

