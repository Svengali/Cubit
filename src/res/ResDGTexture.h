//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2017 Marc Hernandez

#pragma once

#include "res/Resource.h"

PtrFwd( ResDGTexture );

//General buffer
class ResDGTexture: public Resource
{
public:
	CLASS( ResDGTexture, Resource );

	static ResDGTexturePtr create( const char *const pFilename, const util::Symbol &type );

	ResDGTexture( const dg::RefCntAutoPtr<dg::ITexture> &texture );
	virtual ~ResDGTexture();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGTexture, Resource );
	REFLECT_END();

	dg::RefCntAutoPtr<dg::ITexture> &Texture() { return m_texture; }
	dg::RefCntAutoPtr<dg::ITextureView> &View() { return m_texView; }

private:
	dg::RefCntAutoPtr<dg::ITexture> m_texture;
	dg::RefCntAutoPtr<dg::ITextureView> m_texView;
};

PtrDef( ResDGTexture );

