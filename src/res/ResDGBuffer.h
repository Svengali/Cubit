//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2017 Marc Hernandez

#pragma once

#include "res/Resource.h"

PtrFwd( ResDGBuffer );
PtrFwd( ResDGBufVertex );
PtrFwd( ResDGBufIndex );

//General buffer
class ResDGBuffer: public Resource
{
public:
	CLASS( ResDGBuffer, Resource );

	static ResDGBufferPtr create( dg::RefCntAutoPtr<dg::IBuffer> &buffer );

	ResDGBuffer( const dg::RefCntAutoPtr<dg::IBuffer> &buffer );
	virtual ~ResDGBuffer();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGBuffer, Resource )
		REFLECT_END();

	dg::RefCntAutoPtr<dg::IBuffer> &Buffer() { return m_buffer; }

private:
	dg::RefCntAutoPtr<dg::IBuffer> m_buffer;

};

PtrDef( ResDGBuffer );


class ResDGBufVertex: public ResDGBuffer
{
public:
	CLASS( ResDGBufVertex, ResDGBuffer );

	static ResDGBufVertexPtr create( const u32 dataSize, void *const pData );

	ResDGBufVertex( const dg::RefCntAutoPtr<dg::IBuffer> &buffer );
	virtual ~ResDGBufVertex();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGBufVertex, ResDGBuffer )
		REFLECT_END();


private:

};


class ResDGBufIndex: public ResDGBuffer
{
public:
	CLASS( ResDGBufIndex, ResDGBuffer );

	static ResDGBufIndexPtr create( const u32 dataSize, void *const pData );

	ResDGBufIndex( const dg::RefCntAutoPtr<dg::IBuffer> &buffer );
	virtual ~ResDGBufIndex();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGBufIndex, ResDGBuffer )
		REFLECT_END();


private:

};








PtrDef( ResDGBufVertex );
PtrDef( ResDGBufIndex );
