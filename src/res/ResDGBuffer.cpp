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
	dg::IBuffer *buffer;

	CreateUniformBuffer( dg::App::Info().Device(), size, desc.c_str(), &buffer );

	auto bufferPtr = dg::RefCntAutoPtr( buffer );

	dg::App::Info().Barriers.emplace_back( buffer, dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_CONSTANT_BUFFER, true );


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
	VertBuffDesc.Usage = dg::USAGE_STATIC;

	// TODO CONFIG
	VertBuffDesc.BindFlags = dg::BIND_VERTEX_BUFFER;

	VertBuffDesc.uiSizeInBytes = dataSize;

	dg::BufferData VBData;
	VBData.pData = pData;
	VBData.DataSize = dataSize;

	dg::RefCntAutoPtr<dg::IBuffer> buffer;
	dg::App::Info().Device()->CreateBuffer( VertBuffDesc, &VBData, &buffer );

	dg::App::Info().Barriers.emplace_back( buffer, dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_VERTEX_BUFFER, true );

	return ResDGBufVertexPtr( new ResDGBufVertex( buffer ) );
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
	IndBuffDesc.Usage = dg::USAGE_STATIC;
	IndBuffDesc.BindFlags = dg::BIND_INDEX_BUFFER;
	IndBuffDesc.uiSizeInBytes = dataSize;
	dg::BufferData IBData;
	IBData.pData = pData;
	IBData.DataSize = dataSize;

	dg::RefCntAutoPtr<dg::IBuffer> buffer;
	dg::App::Info().Device()->CreateBuffer( IndBuffDesc, &IBData, &buffer );

	dg::App::Info().Barriers.emplace_back( buffer, dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_INDEX_BUFFER, true );

	return ResDGBufIndexPtr( new ResDGBufIndex( buffer ) );
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

	char buffer[256];

	snprintf( buffer, 256, "%s_%i", desc.c_str(), s_count++ );

	CreateUniformBuffer( dg::App::Info().Device(), size, buffer, &pBuffer );

	auto bufferPtr = dg::RefCntAutoPtr( pBuffer );

	dg::App::Info().Barriers.emplace_back( pBuffer, dg::RESOURCE_STATE_UNKNOWN, dg::RESOURCE_STATE_CONSTANT_BUFFER, true );


	return ResDGBuffer::create( bufferPtr );
}
