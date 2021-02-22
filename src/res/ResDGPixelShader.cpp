//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

//Move into stdafx

#include "./ResDGPixelShader.h"


ResDGPixelShaderPtr ResDGPixelShader::create( const char *const pFilename, const util::Symbol &type )
{

  dg::ShaderCreateInfo ShaderCI;
  // Tell the system that the shader source code is in HLSL.
  // For OpenGL, the engine will convert this into GLSL under the hood.
  ShaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;

  // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
  ShaderCI.UseCombinedTextureSamplers = true;

  // Create a shader source stream factory to load shaders from files.
  dg::RefCntAutoPtr<dg::IShaderSourceInputStreamFactory> pShaderSourceFactory;
  dg::App::Info().EngineFactory()->CreateDefaultShaderSourceStreamFactory( nullptr, &pShaderSourceFactory );
  ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
  // Create a vertex shader


  dg::RefCntAutoPtr<dg::IShader> pPS;
  ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
  ShaderCI.EntryPoint      = "main";
  ShaderCI.Desc.Name       = "Cube PS";
  ShaderCI.FilePath        = pFilename; //"cube.psh";
  dg::App::Info().Device()->CreateShader( ShaderCI, &pPS );

	return ResDGPixelShaderPtr( new ResDGPixelShader( ShaderCI, pPS ) );
}



ResDGPixelShader::ResDGPixelShader( const dg::ShaderCreateInfo &shaderCI, const dg::RefCntAutoPtr<dg::IShader> &ps )
  :
  m_shaderCI( shaderCI ),
  m_ps( ps )
{
}

ResDGPixelShader::~ResDGPixelShader( void )
{
}

void ResDGPixelShader::load( const char *const pFilename )
{
  
}

