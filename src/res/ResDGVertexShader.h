//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "res/Resource.h"



PtrFwd( ResDGVertexShader );




class ResDGVertexShader: public Resource
{
public:
	CLASS( ResDGVertexShader, Resource );

	static ResDGVertexShaderPtr create( const char * const pFilename, const util::Symbol &type );

	// REFLECTION
	ResDGVertexShader(){}

	ResDGVertexShader( const dg::ShaderCreateInfo &shaderCI, const dg::RefCntAutoPtr<dg::IShader> &vs );
	virtual ~ResDGVertexShader();

	virtual void load( const char *const pFilename );

	REFLECT_BEGIN( ResDGVertexShader, Resource )
	REFLECT_END();

	dg::ShaderCreateInfo m_shaderCI;
	dg::RefCntAutoPtr<dg::IShader> m_vs;


private:

};

PtrDef( ResDGVertexShader );
