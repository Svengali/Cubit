//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

#include "./ResDGPipelineState.h"


#include "ResDGBuffer.h"

#include "ResDGVertexShader.h"
#include "ResDGPixelShader.h"

SERIALIZABLE( ResDGPipelineState );

/*
ResDGPipelineStatePtr ResDGPipelineState::create( const char *const pFilename, const util::Symbol &type )
{
	return createRaw( m_vs, m_ps );
}
*/

void ResDGPipelineState::onPostLoad()
{
	createRaw( m_vs, m_ps );
}

void ResDGPipelineState::createRaw( const ResDGVertexShaderPtr &vs, const ResDGPixelShaderPtr &ps )
{

	// Pipeline state object encompasses configuration of all GPU stages
	dg::PipelineStateCreateInfo PSOCreateInfo;
	dg::PipelineStateDesc &PSODesc = PSOCreateInfo.PSODesc;

	//TODO CONFIG
	PSODesc.Name = "Cube PSO";

	// This is a graphics pipeline
	//TODO CONFIG
	PSODesc.PipelineType = dg::PIPELINE_TYPE_GRAPHICS;

	//TODO CONFIG
	PSODesc.GraphicsPipeline.NumRenderTargets             = 1;

	PSODesc.GraphicsPipeline.RTVFormats[0]                = dg::App::Info().SwapChain()->GetDesc().ColorBufferFormat;
	
	PSODesc.GraphicsPipeline.DSVFormat                    = dg::App::Info().SwapChain()->GetDesc().DepthBufferFormat;

	//TODO CONFIG	
	PSODesc.GraphicsPipeline.PrimitiveTopology            = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	
	//TODO CONFIG	
	PSODesc.GraphicsPipeline.RasterizerDesc.CullMode      = dg::CULL_MODE_BACK;
	
	//TODO CONFIG	
	PSODesc.GraphicsPipeline.DepthStencilDesc.DepthEnable = dg::True;
	

	// Define vertex shader input layout
	//TODO CONFIG
	//*
	dg::LayoutElement LayoutElems[] =
	{
			// Attribute 0 - vertex position
			dg::LayoutElement{0, 0, 3, dg::VT_FLOAT32, dg::False},
			// Attribute 1 - texture coordinates
			dg::LayoutElement{1, 0, 2, dg::VT_FLOAT32, dg::False}
	};
	//*/

	PSODesc.GraphicsPipeline.pVS = vs->m_vs;
	PSODesc.GraphicsPipeline.pPS = ps->PS();

	//TODO CONFIG
	//dg::CreateUniformBuffer( dg::App::Info().Device(), sizeof( dg::float4x4 ), "VS constants CB", &m_VSConstants );


	//TODO CONFIG	
	PSODesc.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
	PSODesc.GraphicsPipeline.InputLayout.NumElements    = _countof( LayoutElems );

	// Define variable type that will be used by default
	PSODesc.ResourceLayout.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

	//TODO CONFIG	
	dg::ShaderResourceVariableDesc Vars[] =
	{
			{dg::SHADER_TYPE_PIXEL, "g_Texture", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
	};

	PSODesc.ResourceLayout.Variables    = Vars;
	PSODesc.ResourceLayout.NumVariables = _countof( Vars );

	//TODO CONFIG	
	dg::SamplerDesc SamLinearClampDesc
	{
			dg::FILTER_TYPE_LINEAR, dg::FILTER_TYPE_LINEAR, dg::FILTER_TYPE_LINEAR,
			dg::TEXTURE_ADDRESS_CLAMP, dg::TEXTURE_ADDRESS_CLAMP, dg::TEXTURE_ADDRESS_CLAMP
	};

	//TODO CONFIG
	dg::StaticSamplerDesc StaticSamplers[] =
	{
			{dg::SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
	};

	//TODO CONFIG	
	PSODesc.ResourceLayout.StaticSamplers    = StaticSamplers;
	PSODesc.ResourceLayout.NumStaticSamplers = _countof( StaticSamplers );


	dg::App::Info().Device()->CreatePipelineState( PSOCreateInfo, &m_pso );

	// Since we did not explcitly specify the type for 'Constants' variable, default
	// type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
	// never change and are bound directly through the pipeline state object.
	//pso->GetStaticVariableByName( dg::SHADER_TYPE_VERTEX, "Constants" )->Set( constants->Buffer() );

	/*
	// Since we are using mutable variable, we must create a shader resource binding object
	// http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
	m_pPSO->CreateShaderResourceBinding( &m_SRB, true );
	*/
}



ResDGPipelineState::ResDGPipelineState( const dg::RefCntAutoPtr<dg::IPipelineState> &pso )
	:
	m_pso( pso )
{
}

ResDGPipelineState::~ResDGPipelineState( void )
{
}

void ResDGPipelineState::load( const char *const pFilename )
{

}

