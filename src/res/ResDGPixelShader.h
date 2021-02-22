//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2017 Marc Hernandez

#pragma once

#include "res/Resource.h"

PtrFwd( ResDGPixelShader );


class ResDGPixelShader: public Resource
{
public:
	CLASS( ResDGPixelShader, Resource );

	static ResDGPixelShaderPtr create( const char * const pFilename, const util::Symbol &type );

	ResDGPixelShader( const dg::ShaderCreateInfo &shaderCI, const dg::RefCntAutoPtr<dg::IShader> &ps );
	virtual ~ResDGPixelShader();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGPixelShader, Resource )
	REFLECT_END();
	
	dg::RefCntAutoPtr<dg::IShader> &PS() { return m_ps; }

private:
	dg::ShaderCreateInfo m_shaderCI;
	dg::RefCntAutoPtr<dg::IShader> m_ps;

};

PtrDef( ResDGPixelShader );