//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "res/Resource.h"
#include "res/ResDGBuffer.h"

PtrFwd( ResDGPipelineState );
PtrFwd( ResDGVertexShader );
PtrFwd( ResDGPixelShader );


class ResDGPipelineState: public Resource
{
public:
	CLASS( ResDGPipelineState, Resource );

	static ResDGPipelineStatePtr create( const ResDGVertexShaderPtr &vs, const ResDGPixelShaderPtr &ps, const ResDGBufferPtr &constants );

	ResDGPipelineState( const dg::RefCntAutoPtr<dg::IPipelineState> &pso );
	virtual ~ResDGPipelineState();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGPipelineState, Resource )
	REFLECT_END();


	dg::RefCntAutoPtr<dg::IPipelineState> &PSO()
	{
		return m_pso;
	}


private:
	dg::RefCntAutoPtr<dg::IPipelineState> m_pso;

};

PtrDef( ResDGPipelineState );
