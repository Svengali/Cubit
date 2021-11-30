//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

//Move into stdafx

#include "./ResDGBuffer.h"
#include <grx/Generate.h>

SERIALIZABLE( ResDGBufferCreator );
SERIALIZABLE( ResDGCubeCreator );

template <>
void XMLReader::operator() < ResDGBuffer > ( const TiXmlElement *const pNamedElem, ResDGBuffer &val )
{
}



ResDGBufferPtr ResDGBuffer::create( dg::RefCntAutoPtr<dg::IBuffer> &buffer )
{
	return ResDGBufferPtr( new ResDGBuffer( buffer ) );
}

ResDGBufferPtr ResDGBuffer::create( const i32 size, const std::string &desc )
{
	dg::IBuffer *pBuffer;

	//*
	CreateUniformBuffer( PhamApp::Info().Device(), size, desc.c_str(), &pBuffer );
	/*/
	dg::BufferDesc CBDesc;
	CBDesc.Name = desc.c_str();
	CBDesc.Size = size;
	CBDesc.Usage = dg::USAGE_DYNAMIC;
	CBDesc.BindFlags = dg::BIND_UNIFORM_BUFFER;

	PhamApp::Info().Device()->CreateBuffer( CBDesc, nullptr, &pBuffer );
	//*/

	auto bufferPtr = dg::RefCntAutoPtr( pBuffer );

	PhamApp::Info().Barrier( dg::StateTransitionDesc( pBuffer, dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_CONSTANT_BUFFER ) );

	return ResDGBuffer::create( bufferPtr );
}

ResDGBuffer::ResDGBuffer( const dg::RefCntAutoPtr<dg::IBuffer> &buffer )
	:
	m_buffer( buffer )
{
}

ResDGBuffer::~ResDGBuffer( void )
{
	if( m_buffer )
	{
		//m_buffer->Release();
	}
}

void ResDGBuffer::load( const char *const pFilename )
{

}


// V e r t e x
ResDGBufVertexPtr ResDGBufVertex::createRaw( const u32 dataSize, void *const pData )
{

	dg::BufferDesc VertBuffDesc;
	VertBuffDesc.Name = "Unknown Vertex Buffer";

	// TODO CONFIG
	VertBuffDesc.Usage = dg::USAGE_IMMUTABLE;

	// TODO CONFIG
	VertBuffDesc.BindFlags = dg::BIND_VERTEX_BUFFER;

	VertBuffDesc.Size = dataSize;

	dg::BufferData VBData;
	VBData.pData = pData;
	VBData.DataSize = dataSize;

	dg::RefCntAutoPtr<dg::IBuffer> buffer;
	PhamApp::Info().Device()->CreateBuffer( VertBuffDesc, &VBData, &buffer );

	PhamApp::Info().Barrier( dg::StateTransitionDesc( buffer.RawPtr(), dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_VERTEX_BUFFER ) );

	auto bufVertex = ResDGBufVertexPtr( new ResDGBufVertex( buffer ) );

	return bufVertex;
}

ResDGBufVertex::ResDGBufVertex( const dg::RefCntAutoPtr<dg::IBuffer> &buffer )
	:
	ResDGBuffer( buffer )
{
}

ResDGBufVertex::~ResDGBufVertex( void )
{
}


void ResDGBufVertex::load( const char *const pFilename, const util::Symbol type )
{

}


ResDGBufVertexPtr ResDGBufVertex::create( const char *const pFilename, const util::Symbol &type )
{
	const auto cubeVerts = grx::gen::createCenteredCubeVertices( 0.5f );

	//ResourceMgr::AddResource( "+gen:0.5.verts", cubeVerts );

	return cubeVerts;
}


// I n d e x 
ResDGBufIndexPtr ResDGBufIndex::createRaw( const u32 dataSize, void *const pData )
{

	dg::BufferDesc IndBuffDesc;
	IndBuffDesc.Name = "Unknown Index Buffer";
	IndBuffDesc.Usage = dg::USAGE_IMMUTABLE;
	IndBuffDesc.BindFlags = dg::BIND_INDEX_BUFFER;
	IndBuffDesc.Size = dataSize;
	dg::BufferData IBData;
	IBData.pData = pData;
	IBData.DataSize = dataSize;

	dg::RefCntAutoPtr<dg::IBuffer> buffer;
	PhamApp::Info().Device()->CreateBuffer( IndBuffDesc, &IBData, &buffer );

	PhamApp::Info().Barrier( dg::StateTransitionDesc( buffer.RawPtr(), dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_INDEX_BUFFER ) );

	auto bufIndex = ResDGBufIndexPtr( new ResDGBufIndex( buffer ) );

	bufIndex->m_count = dataSize / sizeof( u32 );

	return bufIndex;
}


ResDGBufIndex::ResDGBufIndex( const dg::RefCntAutoPtr<dg::IBuffer> &buffer )
	:
	ResDGBuffer( buffer )
{
}

ResDGBufIndex::~ResDGBufIndex( void )
{
}


void ResDGBufIndex::load( const char *const pFilename, const util::Symbol type )
{

}

ResDGBufIndexPtr ResDGBufIndex::create( const char *const pFilename, const util::Symbol &type )
{
	const auto cubeIndicies = grx::gen::createCenteredCubeIndicies();

	//ResourceMgr::AddResource( "+gen:0.5.indices", cubeIndicies );

	return cubeIndicies;

}

static i32 s_count = 0;

ResourcePtr ResDGBufferCreator::create() const
{

	dg::IBuffer *pBuffer = nullptr;

	//char buffer[256];

	//snprintf( buffer, 256, "%s_%i", desc.c_str(), s_count++ );

	//*
	//CreateUniformBuffer( PhamApp::Info().Device(), size, desc.c_str(), &pBuffer );

	dg::BufferDesc BuffDesc;
	BuffDesc.Size = sizeof( dg::float4x4 );
	BuffDesc.Usage = dg::USAGE_DYNAMIC;
	BuffDesc.BindFlags = dg::BIND_UNIFORM_BUFFER;
	BuffDesc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
  PhamApp::Info().Device()->CreateBuffer( BuffDesc, nullptr, &pBuffer );


	/*/
	dg::BufferDesc CBDesc;
	CBDesc.Name = desc.c_str();
	CBDesc.Size = size;
	CBDesc.Usage = dg::USAGE_DEFAULT;
	CBDesc.BindFlags = dg::BIND_UNIFORM_BUFFER;
	//CBDesc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;

	PhamApp::Info().Device()->CreateBuffer( CBDesc, nullptr, &pBuffer );
	//*/

	if( pBuffer )
	{

		auto bufferPtr = dg::RefCntAutoPtr( pBuffer );

		PhamApp::Info().Barrier( dg::StateTransitionDesc( pBuffer, dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_CONSTANT_BUFFER ) );

		return ResDGBuffer::create( bufferPtr );
	}
	else
	{
		lprinterr( "Couldn not create buf %s\n", desc.c_str() );
	}

	auto bufferPtr = dg::RefCntAutoPtr( pBuffer );
	return ResDGBuffer::create( bufferPtr );
}



ResourcePtr ResDGCubeCreator::create() const
{

	const auto size = cb::Vec3( length, width, height );
	const auto off  = cb::Vec3( 0, 0, 0 );

	const auto buf = grx::gen::createArbitraryCube( size, off );

	return buf;
}
