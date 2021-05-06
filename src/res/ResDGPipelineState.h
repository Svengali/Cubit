//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "res/Resource.h"
#include "res/ResDGBuffer.h"
#include "res/ResDGVertexShader.h"
#include "res/ResDGPixelShader.h"


PtrFwd( ResDGPipelineState );


struct NamedBuffer
{
public:

	CLASS( NamedBuffer );

	REFLECT_BEGIN_ROOT( NamedBuffer );
	REFLECT( m_name );
	REFLECT( m_buffer );
	REFLECT_END();

	util::Symbol		m_name = util::Symbol( util::Symbol::eEmpty );
	ResDGBufferPtr	m_buffer;

};

template<>
struct cb::TypeTraits<NamedBuffer>
{
	BoolAsType_True  hasReflection;
	BoolAsType_False isPrimitive;
	BoolAsType_False ioBytes;
};



class ResDGPipelineState: public Resource
{
public:
	CLASS( ResDGPipelineState, Resource );


	static ResDGPipelineStatePtr create( const char *const pFilename, const util::Symbol &type );

	void createRaw( const ResDGVertexShaderPtr &vs, const ResDGPixelShaderPtr &ps );

	// REFLECTION
	ResDGPipelineState() {}

	virtual ~ResDGPipelineState();


	virtual void load( const char *const pFilename );
	virtual void onPostLoad() override;


	REFLECT_BEGIN( ResDGPipelineState, Resource )
		REFLECT( m_vs );
		REFLECT( m_ps );
		REFLECT( m_namedBuffers );
	REFLECT_END();

	ResDGVertexShaderPtr m_vs;
	ResDGPixelShaderPtr  m_ps;
	std::vector<NamedBuffer> m_namedBuffers;
	dg::RefCntAutoPtr<dg::IPipelineState> m_pso;

private:
	ResDGPipelineState( const dg::RefCntAutoPtr<dg::IPipelineState> &pso );

};

PtrDef( ResDGPipelineState );
