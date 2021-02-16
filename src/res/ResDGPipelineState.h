//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2017 Marc Hernandez

#pragma once

#include "res/Resource.h"

PtrFwd( ResDGPipelineState );


class ResDGPipelineState: public Resource
{
public:
	CLASS( ResDGPipelineState, Resource );

	static ResDGPipelineStatePtr create( const ResDGVertexShaderPtr &vs, const ResDGPixelShaderPtr &ps );

	ResDGPipelineState( const dg::ShaderCreateInfo &shaderCI, const dg::RefCntAutoPtr<dg::IShader> &ps );
	virtual ~ResDGPipelineState();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGPipelineState, Resource )
	REFLECT_END();
	

private:
	dg::ShaderCreateInfo m_shaderCI;
	dg::RefCntAutoPtr<dg::IShader> m_ps;

};

PtrDef( ResDGPipelineState );
