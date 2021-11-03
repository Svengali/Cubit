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
	// TODO HACK Properly find out the layouts
	const auto it = m_filename.find( "VertPosNormalColorUV" );

	if( it < m_filename.size() )
	{
		std::vector<dg::LayoutElement> LayoutElems =
		{
			// Attribute 0 - vertex position
			dg::LayoutElement{0, 0, 3, dg::VT_FLOAT32, dg::False},
			// Attribute 1 - normal
			dg::LayoutElement{1, 0, 3, dg::VT_FLOAT32, dg::False},
			// Attribute 2 - color
			dg::LayoutElement{2, 0, 4, dg::VT_FLOAT32, dg::False},
			// Attribute 3 - texture coordinates
			dg::LayoutElement{3, 0, 2, dg::VT_FLOAT32, dg::False}
		};

		createRaw( m_vs, m_ps, LayoutElems );

	}
	else
	{
		std::vector<dg::LayoutElement> LayoutElems =
		{
			// Attribute 0 - vertex position
			dg::LayoutElement{0, 0, 3, dg::VT_FLOAT32, dg::False},
			// Attribute 1 - texture coordinates
			dg::LayoutElement{1, 0, 2, dg::VT_FLOAT32, dg::False}
		};

		createRaw( m_vs, m_ps, LayoutElems );
	}




}

static i32 s_count = 128;

void ResDGPipelineState::createRaw( 
	const ResDGVertexShaderPtr &vs, 
	const ResDGPixelShaderPtr &ps, 
	const std::vector<dg::LayoutElement> &layout
	)
{

	dg::GraphicsPipelineStateCreateInfo PSOCreateInfo;
	dg::PipelineStateDesc &PSODesc = PSOCreateInfo.PSODesc;
	dg::PipelineResourceLayoutDesc &ResourceLayout = PSODesc.ResourceLayout;
	dg::GraphicsPipelineDesc &GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;

	char buffer[256];

	snprintf( buffer, 256, "PSO_%i", s_count++ );

	//TODO CONFIG
	PSODesc.Name = buffer;

	// This is a graphics pipeline
	//TODO CONFIG
	PSODesc.PipelineType = dg::PIPELINE_TYPE_GRAPHICS;


	//TODO CONFIG
	PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;

	GraphicsPipeline.RTVFormats[0]                = PhamApp::Info().SwapChain()->GetDesc().ColorBufferFormat;
	
	GraphicsPipeline.DSVFormat                    = PhamApp::Info().SwapChain()->GetDesc().DepthBufferFormat;

	//TODO CONFIG	
	GraphicsPipeline.PrimitiveTopology            = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	
	//TODO CONFIG	
	GraphicsPipeline.RasterizerDesc.CullMode      = dg::CULL_MODE_BACK;
	
	//TODO CONFIG	
	GraphicsPipeline.DepthStencilDesc.DepthEnable = dg::True;
	
	PSOCreateInfo.pVS = vs->m_vs;
	PSOCreateInfo.pPS = ps->PS();

	//TODO CONFIG
	//dg::CreateUniformBuffer( PhamApp::Info().Device(), sizeof( dg::float4x4 ), "VS constants CB", &m_VSConstants );


	//TODO CONFIG	
	GraphicsPipeline.InputLayout.LayoutElements = layout.data();
	GraphicsPipeline.InputLayout.NumElements    = (u32)layout.size();

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
	dg::ImmutableSamplerDesc ImtblSamplers[] =
	{
			{dg::SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
	};

	//TODO CONFIG	
	ResourceLayout.ImmutableSamplers = ImtblSamplers;
	ResourceLayout.NumImmutableSamplers = _countof( ImtblSamplers );


	PhamApp::Info().Device()->CreateGraphicsPipelineState( PSOCreateInfo, &m_pso );

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

