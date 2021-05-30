//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez



#include "../stdafx.h"

#include "./GeoDiligent.h"
#include "./RCDiligent.h"


#include "grx/RenderContext.h"

#include "grx/RCDiligent.h"
#include "res/ResDGBuffer.h"
#include "res/ResDGVertexShader.h"
#include "res/ResDGPixelShader.h"
#include "res/ResDGTexture.h"


SERIALIZABLE( GeoDiligentCfg );

SERIALIZABLE( GeoDiligent );

GeoDiligent::GeoDiligent()
{
	//Do Nothing.
}






GeoDiligent::GeoDiligent( const ent::EntityId id, const GeoDiligentCfgPtr &cfg )
	:
	m_id( id ),
	m_cfg( cfg )
{

	//lprintf( "Constants buffer count %i\n", (i32)m_cfg->m_pso->m_namedBuffers.size() );

	auto constants = m_cfg->m_pso->m_namedBuffers.front();

	//lprintf( "First buffer named %s\n", constants.m_name.GetString() );

	// Since we did not explcitly specify the type for 'Constants' variable, default
	 // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
	 // never change and are bound directly through the pipeline state object.

	auto constBuf = m_cfg->m_pso->m_pso->GetStaticVariableByName( dg::SHADER_TYPE_VERTEX, constants.m_name.GetString() );
	if( constBuf )
	{
		constBuf->Set( constants.m_buffer->Buffer() );
	}
	else
	{
		lprinterr( "Constant buffer %s not found.\n", constants.m_name.GetString() );
	}

	if( !m_cfg->m_pso->m_srb )
	{
		// Since we are using mutable variable, we must create a shader resource binding object
		// http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
		m_cfg->m_pso->m_pso->CreateShaderResourceBinding( &m_cfg->m_pso->m_srb, true );

		m_cfg->m_pso->m_srb->GetVariableByName( dg::SHADER_TYPE_PIXEL, "g_Texture" )->Set( m_cfg->m_texture->View() );

		PhamApp::Info().Barrier( dg::StateTransitionDesc( m_cfg->m_texture->Texture().RawPtr(), dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_SHADER_RESOURCE, false ) );

	}
}

GeoDiligent::~GeoDiligent( void )
{
}

void GeoDiligent::load( const char *const pFilename )
{

}

void GeoDiligent::renderDiligent( RCDiligent *pRC, const cb::Frame3 &frame )
{

	const cb::Mat4 mat( frame );

	const dg::float4x4 world = dg::float4x4::MakeMatrix( mat.GetData() );

	const dg::float4x4 fin = world * pRC->m_viewProj;
	const dg::float4x4 finT = fin.Transpose();


	//auto *pRTV = PhamApp::Info().SwapChain()->GetCurrentBackBufferRTV();
	//pRC->m_devContext->SetRenderTargets( 1, &pRTV, PhamApp::Info().SwapChain()->GetDepthBufferDSV(), dg::RESOURCE_STATE_TRANSITION_MODE_VERIFY );



	auto debugName = m_cfg->m_pso->m_namedBuffers.front().m_name;
	auto constants = m_cfg->m_pso->m_namedBuffers.front().m_buffer;
	//*
	{
		// Map the buffer and write current world-view-projection matrix
		dg::MapHelper<dg::float4x4> CBConstants( pRC->m_devContext, constants->Buffer(), dg::MAP_WRITE, dg::MAP_FLAG_DISCARD );
		*CBConstants = finT;
	}
	/*/
	pRC->m_devContext->UpdateBuffer( constants->Buffer(), 0, sizeof( fin ), &finT, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
	//*/


	// Bind vertex and index buffers
	u32   offset = 0;
	dg::IBuffer *pBuffs[] = { m_cfg->m_vertexBuf->Buffer().RawPtr() };
	pRC->m_devContext->SetVertexBuffers( 0, 1, pBuffs, &offset, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::SET_VERTEX_BUFFERS_FLAG_RESET );
	pRC->m_devContext->SetIndexBuffer( m_cfg->m_indexBuf->Buffer().RawPtr(), 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
	

	// Set the pipeline state
	pRC->m_devContext->SetPipelineState( m_cfg->m_pso->m_pso.RawPtr() );

	// Commit shader resources. RESOURCE_STATE_TRANSITION_MODE_TRANSITION mode
	// makes sure that resources are transitioned to required states.
	pRC->m_devContext->CommitShaderResources( m_cfg->m_pso->m_srb.RawPtr(), dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION );


	dg::DrawIndexedAttribs DrawAttrs;     // This is an indexed draw call

	// TODO CONFIG
	DrawAttrs.IndexType = dg::VT_UINT32; 

	// TODO CONFIG
	DrawAttrs.NumIndices = m_cfg->m_indexBuf->m_count;

	// TODO CONFIG
	DrawAttrs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

	pRC->m_devContext->DrawIndexed( DrawAttrs );

}



void GeoDiligent::render( RenderContext *pRC, const cb::Frame3 &frame )
{
	ASSERT( false && "Dont need to run through render, we go directly through renderDiligent" );
}


