//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
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

static DGRenderer s_renderer;

DGRenderer &DGRenderer::Inst()
{
	return s_renderer;
}

void DGRenderer::startup()
{
	Renderer::startup( &s_renderer );

	//void RegisterCreator( const char *const pExtension, const util::Symbol &type, FnCreator func );

	ResourceMgr::RegisterCreator( "xml", Config::SClass(), &Config::create );

	ResourceMgr::RegisterCreator( "psh", ResDGPixelShader::SClass(), ResDGPixelShader::create );
	ResourceMgr::RegisterCreator( "vsh", ResDGVertexShader::SClass(), ResDGVertexShader::create );

	ResourceMgr::RegisterCreator( "verts", ResDGPixelShader::SClass(), ResDGPixelShader::create );
	ResourceMgr::RegisterCreator( "indices", ResDGVertexShader::SClass(), ResDGVertexShader::create );

	//ResourceMgr::RegisterCreator( "xml", GeoDiligentCfg::SClass(), GeoDiligentCfg::create );

	ResourceMgr::RegisterCreator( "png", ResDGTexture::SClass(), ResDGTexture::create );
	ResourceMgr::RegisterCreator( "jpg", ResDGTexture::SClass(), ResDGTexture::create );
}

void DGRenderer::shutdown()
{

}



DGRenderer::DGRenderer()
{
}


DGRenderer::~DGRenderer(void)
{
}

