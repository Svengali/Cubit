//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

#include "./ResDGVertexShader.h"

ResDGVertexShaderPtr ResDGVertexShader::create( const char *const pFilename, const util::Symbol &type )
{

	dg::ShaderCreateInfo ShaderCI;
	// Tell the system that the shader source code is in HLSL.
	// For OpenGL, the engine will convert this into GLSL under the hood.


	//TODO RES CONFIG
	ShaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;

	// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
	//TODO RES CONFIG
	ShaderCI.UseCombinedTextureSamplers = true;

	// In this tutorial, we will load shaders from file. To be able to do that,
	// we need to create a shader source stream factory
	dg::RefCntAutoPtr<dg::IShaderSourceInputStreamFactory> pShaderSourceFactory;

	dg::App::Info().EngineFactory()->CreateDefaultShaderSourceStreamFactory( nullptr, &pShaderSourceFactory );

	ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
	// Create a vertex shader
	dg::RefCntAutoPtr<dg::IShader> pVS;

	//This can be implied
	ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_VERTEX;

	//TODO RES CONFIG
	ShaderCI.EntryPoint      = "main";

	//TODO RES CONFIG
	ShaderCI.Desc.Name       = "Cube VS";

	ShaderCI.FilePath        = pFilename; //"cube.vsh";

	dg::App::Info().Device()->CreateShader( ShaderCI, &pVS );

	return ResDGVertexShaderPtr( new ResDGVertexShader( ShaderCI, pVS ) );
}


ResDGVertexShader::ResDGVertexShader( const dg::ShaderCreateInfo &shaderCI, const dg::RefCntAutoPtr<dg::IShader> &vs )
	:
	m_shaderCI( shaderCI ),
	m_vs( vs )
{
}

ResDGVertexShader::~ResDGVertexShader( void )
{
}

void ResDGVertexShader::load( const char *const pFilename )
{

}

