//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "res/Resource.h"



PtrFwd( ResDGVertexShader );



class ResDGVertexShaderCfg: public Config
{
public:

	CLASS( ResDGVertexShaderCfg, Config );

	REFLECT_BEGIN( ResDGVertexShaderCfg, Config );
	REFLECT_END();


	std::string m_entryPoint = "main";
	std::string m_desc = "Unknown";

	ResDGVertexShaderPtr m_vertexShader;


};


PtrDef( ResDGVertexShaderCfg );





class ResDGVertexShader: public Resource
{
public:
	CLASS( ResDGVertexShader, Resource );

	static ResDGVertexShaderPtr create( const char * const pFilename, const util::Symbol &type );

	ResDGVertexShader( const dg::ShaderCreateInfo &shaderCI, const dg::RefCntAutoPtr<dg::IShader> &vs );
	virtual ~ResDGVertexShader();

	virtual void load( const char *const pFilename );

	REFLECT_BEGIN( ResDGVertexShader, Resource )
		REFLECT( m_cfg );
		//REFLECT( m_shaderCI );
		//REFLECT( m_vs );
	REFLECT_END();

	dg::RefCntAutoPtr<dg::IShader> VS()
	{
		return m_vs;
	}

	ResDGVertexShaderCfgPtr m_cfg;
	dg::ShaderCreateInfo m_shaderCI;
	dg::RefCntAutoPtr<dg::IShader> m_vs;


private:

};

PtrDef( ResDGVertexShader );
