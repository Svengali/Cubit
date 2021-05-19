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
#include "res/ResReflect.h"

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

	ResourceMgr::RegisterCreator( "verts", ResDGBufVertex::SClass(), ResDGBufVertex::create );
	ResourceMgr::RegisterCreator( "indices", ResDGBufIndex::SClass(), ResDGBufIndex::create );

	//ResourceMgr::RegisterCreator( "xml", GeoDiligentCfg::SClass(), GeoDiligentCfg::create );

	ResourceMgr::RegisterCreator( "png", ResDGTexture::SClass(), ResDGTexture::create );
	ResourceMgr::RegisterCreator( "jpg", ResDGTexture::SClass(), ResDGTexture::create );

	ResourceMgr::RegisterCreator( "res", Resource::SClass(), &ResReflect::create );

}

void DGRenderer::shutdown()
{
	ResourceMgr::AppStop();

	delete s_renderer;
	s_renderer = nullptr;
}



DGRenderer::DGRenderer()
{
	RSEntityPtr rs = RSEntityPtr( new RSEntity() );
	m_rsStatic = rs;
}


DGRenderer::~DGRenderer( void )
{
}

void DGRenderer::render( RCDiligent *pContext )
{
	m_rsStatic->render( pContext );
}



void DGRenderer::addStaticGeo( const cb::Frame3 frame, const GeoDiligentPtr &geo )
{
	m_rsStatic->add( frame, geo );
}


void RSEntity::render( RCDiligent *pContext )
{

	std::array<bool, 32> activeCmdLists = { false };
	std::array<dg::RefCntAutoPtr<dg::ICommandList>, 32> cmdLists;
	//std::array<std::atomic<bool>, 64> waitList;

	auto &geos = m_geos;

	const auto size = (int)geos.m_com.m_blocks.m_block.size();

	typedef decltype( geos.m_com.m_blocks.m_block ) TBlock;

	enki::TaskSet task( size,
		[&cmdLists, &geos, pContext, &activeCmdLists]( enki::TaskSetPartition range, uint32_t threadnum ) {

			for( i32 iBlock = range.start; iBlock < (i32)range.end; ++iBlock )
			{
				auto *pBlock = geos.m_com.m_blocks.m_block[iBlock].get();
				auto max = geos.m_com.m_blocks.m_allocated[iBlock];

				const auto *__restrict const pSrcFrame = pBlock->src<GeoBlock::Frame, cb::Frame3>();
				const auto *__restrict const pSrcGeo = pBlock->src<GeoBlock::Geometry, GeoDiligentPtr>();

				const i32 id = threadnum;

				activeCmdLists[id] = true;

				auto context = dg::App::Info().DeferredContexts()[id];

				RCDiligent newRC;
				newRC.m_devContext = context;
				newRC.m_renderTarget = pContext->m_renderTarget;
				newRC.m_viewProj = pContext->m_viewProj;

				auto swapChain = dg::App::Info().SwapChain();
				auto *pRTV = swapChain->GetCurrentBackBufferRTV();
				context->SetRenderTargets( 1, &pRTV, swapChain->GetDepthBufferDSV(), dg::RESOURCE_STATE_TRANSITION_MODE_VERIFY );

				for( i32 i = 0; i < (i32)max; ++i )
				{
					const cb::Frame3 &frame = pSrcFrame[i];
					const GeoDiligentPtr &geo = pSrcGeo[i];

					geo->renderDiligent( &newRC, frame );
				}
			}
		} );

	dg::App::Info().Task.AddTaskSetToPipe( &task );
	dg::App::Info().Task.WaitforTask( &task );

	auto contextImm = dg::App::Info().ImmContext();

	//num = 64;

	for( i32 i = 0; i < cmdLists.size(); ++i )
	{
		if( activeCmdLists[i] )
		{
			auto contextDef = dg::App::Info().DeferredContexts()[i];

			if( contextDef )
			{
				contextDef->FinishCommandList( &cmdLists[i] );
			}
			else
			{
				lprinterr( "context def %i is null. Cant finish command list", i );
			}
		}
	}

	for( i32 i = 0; i < cmdLists.size(); ++i )
	{
		if( activeCmdLists[i] )
		{
			contextImm->ExecuteCommandList( cmdLists[i] );

			// Release command lists now to release all outstanding references
			// In d3d11 mode, command lists hold references to the swap chain's back buffer
			// that cause swap chain resize to fail
			cmdLists[i].Release();
		}
	}


	for( i32 i = 0; i < cmdLists.size(); ++i )
	{
		if( activeCmdLists[i] )
		{
			auto contextDef = dg::App::Info().DeferredContexts()[i];

			if( contextDef )
			{
				contextDef->FinishFrame();
			}
			else
			{
				lprinterr( "context def %i is null. Cant finish frame", i );
			}
		}
	}








}

void RSEntity::add( const cb::Frame3 frame, const GeoDiligentPtr &ptr )
{
	const auto id = ptr->m_id;

	m_geos.m_com.insert( id, frame, ptr );


	GeoBlock::Block::TTuple values;

	m_geos.m_com.debug_get( id, &values );
}



void GeoBlock::operate( std::function<void( Block *, const i32 max )> fn )
{
	const auto size = (int)m_com.m_blocks.m_block.size();





	//*
	async::parallel_for( async::irange( 0, size ), [fn, this]( int iBlock ) {
		/*/
		for( i32 iBlock = 0; iBlock < size; ++iBlock ) {
		//*/


		auto &block = m_com.m_blocks.m_block[iBlock];

		const auto max = m_com.m_blocks.m_allocated[iBlock];

		fn( block.get(), max );


		//updateBlock( dtMs, *block.get(), count );
		}
		//*
	);
	/*/
	//*/
}

