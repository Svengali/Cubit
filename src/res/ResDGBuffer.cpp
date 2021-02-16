//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2017 Marc Hernandez

#include "../stdafx.h"

//Move into stdafx

#include "./ResDGBuffer.h"

/*
RefCntAutoPtr<IBuffer> buffer;
CreateUniformBuffer( dg::App::Info().Device(), sizeof( float4x4 ), "VS constants CB", &buffer );
*/


ResDGBufferPtr ResDGBuffer::create( dg::RefCntAutoPtr<dg::IBuffer> &buffer )
{
	return ResDGBufferPtr( new ResDGBuffer( buffer ) );
}



ResDGBuffer::ResDGBuffer( const dg::RefCntAutoPtr<dg::IBuffer> &buffer )
	:
	m_buffer( buffer )
{
}

ResDGBuffer::~ResDGBuffer( void )
{
}

void ResDGBuffer::load( const char *const pFilename )
{

}


ResDGBufVertexPtr ResDGBufVertex::create( const u32 dataSize, void * const pData )
{

	dg::BufferDesc VertBuffDesc;
	VertBuffDesc.Name          = "Unknown Vertex Buffer";

	// TODO CONFIG
	VertBuffDesc.Usage         = dg::USAGE_STATIC;

	// TODO CONFIG
	VertBuffDesc.BindFlags     = dg::BIND_VERTEX_BUFFER;

	VertBuffDesc.uiSizeInBytes = dataSize;

	dg::BufferData VBData;
	VBData.pData    = pData;
	VBData.DataSize = dataSize;

	dg::RefCntAutoPtr<dg::IBuffer> buffer;
	dg::App::Info().Device()->CreateBuffer( VertBuffDesc, &VBData, &buffer );


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


void ResDGBufVertex::load( const char *const pFilename )
{

}


ResDGBufIndexPtr ResDGBufIndex::create( const u32 dataSize, void *const pData )
{

	dg::BufferDesc IndBuffDesc;
	IndBuffDesc.Name          = "Unknown Index Buffer";
	IndBuffDesc.Usage         = dg::USAGE_STATIC;
	IndBuffDesc.BindFlags     = dg::BIND_INDEX_BUFFER;
	IndBuffDesc.uiSizeInBytes = dataSize;
	dg::BufferData IBData;
	IBData.pData    = pData;
	IBData.DataSize = dataSize;

	dg::RefCntAutoPtr<dg::IBuffer> buffer;
	dg::App::Info().Device()->CreateBuffer( IndBuffDesc, &IBData, &buffer );

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


void ResDGBufIndex::load( const char *const pFilename )
{

}
