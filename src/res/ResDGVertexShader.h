//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2017 Marc Hernandez

#pragma once

#include "res/Resource.h"

PtrFwd( ResDGVertexShader );


class ResDGVertexShader: public Resource
{
public:
	CLASS( ResDGVertexShader, Resource );

	static ResDGVertexShaderPtr create( const char * const pFilename, const util::Symbol &type );

	ResDGVertexShader( const dg::ShaderCreateInfo &shaderCI, const dg::RefCntAutoPtr<dg::IShader> &vs );
	virtual ~ResDGVertexShader();

	virtual void load( const char *const pFilename );

	REFLECT_BEGIN( ResDGVertexShader, Resource )
		//REFLECT( m_shaderCI );
		//REFLECT( m_vs );
	REFLECT_END();

	dg::RefCntAutoPtr<dg::IShader> VS()
	{
		return m_vs;
	}


private:
	dg::ShaderCreateInfo m_shaderCI;
	dg::RefCntAutoPtr<dg::IShader> m_vs;

};

PtrDef( ResDGVertexShader );
