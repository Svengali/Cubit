//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

//Move into stdafx

#include "./ResDGTexture.h"


ResDGTexturePtr ResDGTexture::create( const char *const pFilename, const util::Symbol &type )
{

	dg::TextureLoadInfo loadInfo;
	loadInfo.IsSRGB = true;

	dg::ITexture *pTexture = nullptr;

	dg::CreateTextureFromFile( pFilename, loadInfo, PhamApp::Info().Device(), &pTexture );

	dg::Ptr<dg::ITexture> tex( pTexture );

	// Get shader resource view from the texture
	//m_TextureSRV = Tex->GetDefaultView( dg::TEXTURE_VIEW_SHADER_RESOURCE );


	return ResDGTexturePtr( new ResDGTexture( tex ) );
}



ResDGTexture::ResDGTexture( const dg::RefCntAutoPtr<dg::ITexture> &texture )
	:
	m_texture( texture )
{
	m_texView = m_texture->GetDefaultView( dg::TEXTURE_VIEW_SHADER_RESOURCE );

}

ResDGTexture::~ResDGTexture( void )
{
}

void ResDGTexture::load( const char *const pFilename )
{

}
