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

#include "ent/free.h"

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
	m_rsFreefall = RSEntityPtr( new RSEntity() );
	m_rsVoxels = RSEntityPtr( new RSEntity() );
	m_rsCubes = RSEntityPtr( new RSEntity() );
}


DGRenderer::~DGRenderer( void )
{
}

static i32 s_localFrameNum = 0;

void DGRenderer::render( RCDiligent *pContext )
{
	//lprintf( "RENDERING NEW FRAME\n" );

	/*
	dg::IFence *pFence = nullptr;

	dg::FenceDesc fence;

	PhamApp::Info().Device()->CreateFence( fence, &pFence );

	pContext->fences[0] = pFence;
	*/

	//*
	for( i32 i = 0; i < pContext->activeCmdLists.size(); ++i )
	{

	}
	//*/

	const auto timeFreefall = Timer<>::execution( [&]() {

		m_rsFreefall->render( pContext, "Freefall" );

		} );

	if( ( s_localFrameNum & 0xff ) == 0 )
	{
		const auto timeBraceF = (f32)timeFreefall;

		lprintf( "timeFreefall in %.3f ms\n", timeBraceF / 1000.0f );
	}

	const auto timeCubes = Timer<>::execution( [&]() {

		m_rsCubes->render( pContext, "Cubes" );

		} );

	if( ( s_localFrameNum & 0xff ) == 0 )
	{
		const auto timeBraceF = (f32)timeCubes;

		lprintf( "timeCubes in %.3f ms\n", timeBraceF / 1000.0f );
	}

	const auto timeVoxels = Timer<>::execution( [&]() {

		m_rsVoxels->render( pContext, "Voxels" );

		} );

	if( ( s_localFrameNum & 0xff ) == 0 )
	{
		const auto timeBraceF = (f32)timeVoxels;

		lprintf( "timeVoxels in %.3f ms\n", timeBraceF / 1000.0f );
	}

	const auto timeProcess = Timer<>::execution( [&]() {

		processContexts( pContext );

		} );

	if( ( s_localFrameNum & 0xff ) == 0 )
	{
		const auto timeBraceF = (f32)timeProcess;

		lprintf( "timeProcess in %.3f ms\n", timeBraceF / 1000.0f );
	}


	++s_localFrameNum;

	/*
	for( i32 i = 0; i < pContext->cmdLists.size(); ++i )
	{
		auto context = PhamApp::Info().DeferredContexts()[i];
		context->FinishFrame();
	}
	*/

	//pContext->m_devContext->Flush();
	//pContext->m_devContext->FinishFrame();

	//pContext->m_devContext->DeviceWaitForFence( pContext->fences[0].RawPtr(), 0 );
	//pContext->fences[0].Release();


}


/*
void DGRenderer::addStaticGeo( const cb::Frame3 frame, const GeoDiligentPtr &geo )
{
	m_rsStatic->add( frame, geo );
}
*/

struct BlockRow
{
	FreefallData *pData;
};



#if 0
enki::TaskSetPtr render( RCDiligent *pContext, std::vector<FreefallData *> *pFreefalVec )
{
	const auto size = pFreefalVec->size();

	auto renderTarget = pContext->m_renderTarget;
	auto viewProj = pContext->m_viewProj;

	auto *pTask = new enki::TaskSet( size,
		[pContext, pFreefalVec, renderTarget, viewProj]( enki::TaskSetPartition range, uint32_t threadnum ) {

			for( size_t i = range.start; i < range.end; ++i )
			{
				auto *pBlock = pFreefalVec->at( i )->m_com.m_blocks.m_block[0].get();
				auto max = pFreefalVec->at( i )->m_com.m_blocks.m_allocated[0];


				const auto *__restrict const pSrcFrame = pBlock->src<FreefallData::Frame, cb::Frame3>();
				const auto *__restrict const pSrcGeo = pBlock->src<FreefallData::Geometry, GeoDiligentPtr>();

				const i32 id = threadnum;

				pContext->activeCmdLists[id] = true;

				auto context = PhamApp::Info().DeferredContexts()[id];

				RCDiligent newRC;
				newRC.m_devContext = context;
				newRC.m_renderTarget = renderTarget;
				newRC.m_viewProj = pContext->m_viewProj;

				auto swapChain = PhamApp::Info().SwapChain();
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


	auto taskPtr = enki::TaskSetPtr( pTask );
	PhamApp::Info().Task.AddTaskSetToPipe( pTask );
}


}
#endif


void processContexts( RCDiligent *pContext )
{
	auto contextImm = PhamApp::Info().ImmContext();

	//num = 64;

	for( i32 i = 0; i < pContext->cmdLists.size(); ++i )
	{
		auto context = PhamApp::Info().DeferredContexts()[i];

		if( pContext->activeCmdLists[i] )
		{
			if( context )
			{
				//lprintf( "Finishing CL %i\n", i );

				DGCommandListPtr ptr( nullptr );
				dg::ICommandList *pCL = nullptr;
				context->FinishCommandList( &pCL );
				pContext->cmdLists[i].push_back( dg::Ptr<dg::ICommandList>( pCL ) );
			}
			else
			{
				lprinterr( "context def %i is null. Cant finish command list", i );
			}
		}
	}

	for( i32 i = 0; i < pContext->cmdLists.size(); ++i )
	{
		if( pContext->activeCmdLists[i] )
		{
			for( auto cl : pContext->cmdLists[i] )
			{
				//lprintf( "Executing %i\n", i );

				std::array<dg::ICommandList *, 1> list = { cl.RawPtr() };

				contextImm->ExecuteCommandLists( 1, list.data() );
			}
		}
	}

	for( i32 i = 0; i < pContext->cmdLists.size(); ++i )
	{
		// Release command lists now to release all outstanding references
		// In d3d11 mode, command lists hold references to the swap chain's back buffer
		// that cause swap chain resize to fail
		for( auto cl : pContext->cmdLists[i] )
		{
			cl.Release();
		}

		pContext->cmdLists[i].clear();
	}


	for( i32 i = 0; i < pContext->cmdLists.size(); ++i )
	{
		//if( pContext->activeCmdLists[i] )
		{
			auto context = PhamApp::Info().DeferredContexts()[i];

			if( context )
			{
				context->FinishFrame();
				//contextDef->WaitForIdle();
			}
			else
			{
				lprinterr( "context def %i is null. Cant finish frame", i );
			}

			pContext->activeCmdLists[i].store( false );
			pContext->lastUsedPSO[i] = nullptr;
		}
	}

	//contextImm->Flush();

	//dg::ICommandList *pCL;

	//contextImm->FinishCommandList(&pCL);

	//contextImm->FinishFrame();

	//contextImm->Flush();
	//contextImm->FinishFrame();
	
	//contextImm->WaitForIdle();

}



void renderEntities( RCDiligent *pContext, std::vector<FreefallData *> *pFreeFallVec )
{

	for( i32 i = 0; i < pContext->activeCmdLists.size(); ++i )
	{
		if( pContext->activeCmdLists[i].load() )
		{
			//lprinterr( "Context %i is active!\n", i );
		}
	}

	//*
	auto task = enki::TaskSet( (u32)pFreeFallVec->size(),
		[pContext, pFreeFallVec]( enki::TaskSetPartition range, uint32_t threadnum ) {

			const i32 id = threadnum;

			auto context = PhamApp::Info().DeferredContexts()[id];

			RCDiligent newRC;
			newRC.m_devContext = context;
			newRC.m_renderTarget = pContext->m_renderTarget;
			newRC.m_viewProj = pContext->m_viewProj;

			if( !pContext->activeCmdLists[id] )
			{
				pContext->activeCmdLists[id] = true;

				auto context = PhamApp::Info().DeferredContexts()[id];
				context->Begin( 0 );
			}
		/*/
		const auto range = enki::TaskSetPartition{ 0, (u32)pFreeFallVec->size() };
		RCDiligent newRC;
		newRC.m_devContext = pContext->m_devContext;
		newRC.m_renderTarget = pContext->m_renderTarget;
		newRC.m_viewProj = pContext->m_viewProj;
		auto context = pContext->m_devContext;

		//*/


			for( u32 iData = range.start; iData < range.end; ++iData )
			{
				auto *pData = ( *pFreeFallVec )[iData];

				const auto blockCount = pData->m_com.m_blocks.m_block.size();

				for( i32 bIndex = 0; bIndex < blockCount; ++bIndex )
				{
					auto *pBlock = pData->m_com.m_blocks.m_block[bIndex].get();
					auto max = pData->m_com.m_blocks.m_allocated[bIndex];

					const auto *__restrict const pSrcFrame = pBlock->src<FreefallData::Frame, cb::Frame3>();
					const auto *__restrict const pSrcGeo = pBlock->src<FreefallData::Geometry, GeoDiligentPtr>();



					auto swapChain = PhamApp::Info().SwapChain();
					auto *pRTV = swapChain->GetCurrentBackBufferRTV();
					context->SetRenderTargets( 1, &pRTV, swapChain->GetDepthBufferDSV(), dg::RESOURCE_STATE_TRANSITION_MODE_VERIFY );

					for( i32 i = 0; i < (i32)max; ++i )
					{
						const cb::Frame3 &frame = pSrcFrame[i];
						const GeoDiligentPtr &geo = pSrcGeo[i];

						/*
						if( pContext->lastUsedPSO[id] != geo->m_cfg->m_pso )
						{
							if( !pContext->lastUsedPSO[id] )
							{
							}
							else
							{
								//DGCommandListPtr ptr( nullptr );
								//pContext->cmdLists[id].push_back( ptr );
								//pContext->m_devContext->FinishCommandList( &pContext->cmdLists[id].back() );
							}

							pContext->lastUsedPSO[id] = geo->m_cfg->m_pso;
						}
						*/

						geo->renderDiligent( &newRC, frame );

					}
				}
			}


			/*
			dg::ICommandList *pCL = nullptr;
			context->FinishCommandList( &pCL );

			pContext->cmdLists[id].push_back( dg::Ptr<dg::ICommandList>( pCL ) );
			*/


	//*
		} );

	PhamApp::Info().Task.AddTaskSetToPipe( &task );
	PhamApp::Info().Task.WaitforTask( &task );
	//*/

	//processContexts( pContext );
}



void RSEntity::render( RCDiligent *pContext, const char *pDebugRender )
{

	//std::array<std::atomic<bool>, 64> waitList;

	//*
	auto &geos = m_geos;

	const auto size = (int)geos.m_com.m_blocks.m_block.size();

	//typedef decltype( geos.m_com.m_blocks.m_block ) TBlock;

	//*
	enki::TaskSet task( size,
		[&geos, pContext, pDebugRender]( enki::TaskSetPartition range, uint32_t threadnum ) {

			const i32 id = threadnum;

			// TODO MULTITHREADING 
			//lprintf( "Rendering on %i\n", id );

			auto context = PhamApp::Info().DeferredContexts()[id];

			//context->Begin(0);

			if( !pContext->activeCmdLists[id] )
			{
				//lprintf( "Begin on %i\n", id );
				//context->Begin( 0 );
				pContext->activeCmdLists[id] = true;

				context->Begin( 0 );
			}

			RCDiligent newRC;
			newRC.m_devContext = context;
			newRC.m_renderTarget = pContext->m_renderTarget;
			newRC.m_viewProj = pContext->m_viewProj;
			/*/


			const auto range = enki::TaskSetPartition{ 0, (u32)size };
			RCDiligent newRC;
			newRC.m_devContext = pContext->m_devContext;
			newRC.m_renderTarget = pContext->m_renderTarget;
			newRC.m_viewProj = pContext->m_viewProj;
			auto context = pContext->m_devContext;

			//*/

			for( i32 iBlock = range.start; iBlock < (i32)range.end; ++iBlock )
			{
				auto *pBlock = geos.m_com.m_blocks.m_block[iBlock].get();
				auto max = geos.m_com.m_blocks.m_allocated[iBlock];

				const auto *__restrict const pSrcFrame = pBlock->src<GeoBlock::Frame, cb::Frame3>();
				const auto *__restrict const pSrcGeo = pBlock->src<GeoBlock::Geometry, GeoDiligentPtr>();


				auto swapChain = PhamApp::Info().SwapChain();
				auto *pRTV = swapChain->GetCurrentBackBufferRTV();
				context->SetRenderTargets( 1, &pRTV, swapChain->GetDepthBufferDSV(), dg::RESOURCE_STATE_TRANSITION_MODE_VERIFY );

				for( i32 i = 0; i < (i32)max; ++i )
				{
					const cb::Frame3 &frame = pSrcFrame[i];
					const GeoDiligentPtr &geo = pSrcGeo[i];

					/*
					if( pContext->lastUsedPSO[id] != geo->m_cfg->m_pso )
					{
						if( !pContext->lastUsedPSO[id] )
						{
						}
						else
						{
							//lprintf( "Switching to a new command list %i\n", id );

							//DGCommandListPtr ptr( nullptr );
							//pContext->cmdLists[id].push_back( ptr );
							//pContext->m_devContext->FinishCommandList( &pContext->cmdLists[id].back() );
						}

						pContext->lastUsedPSO[id] = geo->m_cfg->m_pso;
					}
					*/


					geo->renderDiligent( &newRC, frame );
				}
			}

			/*
			dg::ICommandList *pCL = nullptr;
			context->FinishCommandList( &pCL );

			pContext->cmdLists[id].push_back( dg::Ptr<dg::ICommandList>( pCL ) );
			*/

		//*

		} );

	PhamApp::Info().Task.AddTaskSetToPipe( &task );
	PhamApp::Info().Task.WaitforTask( &task );
	//*/

	//*/

	if( m_freefall )
	{
		std::vector<FreefallData *> freefallVec;

		for( auto [k, v] : m_freefall->m_sparse )
		{
			v->gatherActiveBlocks( &freefallVec );
		}

		renderEntities( pContext, &freefallVec );
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

