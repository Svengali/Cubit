//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "res/Resource.h"



PtrFwd( ResDGBuffer );

//General buffer
class ResDGBuffer: public Resource
{
public:
	CLASS( ResDGBuffer, Resource );

	static std::shared_ptr<ResDGBuffer> create( dg::RefCntAutoPtr<dg::IBuffer> &buffer );
	static ResDGBufferPtr create( const i32 size, const std::string &desc );

	ResDGBuffer() {}


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


class ResDGBufferCreator : public ResCreator
{
public:
	i32 size;
	std::string desc;

	REFLECT_BEGIN_ROOT( ResDGBufferCreator )
		REFLECT( size );
		REFLECT( desc );
	REFLECT_END();

	//static ResDGBufferPtr create( const ResDGBufferCreator &info ) { return nullptr; };


	// Inherited via ResCreator
	virtual ResourcePtr create() const override;

};

template<>
struct cb::TypeTraits<ResDGBufferCreator>
{
	BoolAsType_True  hasReflection;
	BoolAsType_False isPrimitive;
	BoolAsType_False ioBytes;
};


template <>
void XMLReader::operator()<ResDGBuffer>( const TiXmlElement *const pNamedElem, ResDGBuffer &val );


template <>
struct CreatorTraits<ResDGBuffer>
{
	CreatorBoolAsType_True hasACustomCreator;
	typedef ResDGBufferCreator Creator;
};



PtrFwd( ResDGBufVertex );

class ResDGBufVertex: public ResDGBuffer
{
public:
	CLASS( ResDGBufVertex, ResDGBuffer );

	static ResDGBufVertexPtr create( const char *const pFilename, const util::Symbol &type );
	static ResDGBufVertexPtr createRaw( const u32 dataSize, void *const pData );

	ResDGBufVertex( const dg::RefCntAutoPtr<dg::IBuffer> &buffer );
	virtual ~ResDGBufVertex();


	virtual void load( const char *const pFilename, const util::Symbol type );


	REFLECT_BEGIN( ResDGBufVertex, ResDGBuffer )
	REFLECT_END();


private:

};


template<>
inline void fillInRes( ResDGBufVertex *pT )
{
	lprintf( "Fill in ResDGBufVertex resource.\n" );
}





class ResDGCubeCreator: public ResCreator
{
public:
	CLASS( ResDGCubeCreator, ResCreator );

	std::string name = "<unknown>";

	f32 length = 0.5f;
	f32 width  = 0.5f;
	f32 height = 0.5f;

	REFLECT_BEGIN_ROOT( ResDGCubeCreator )
		REFLECT( length );
		REFLECT( width );
		REFLECT( height );
	REFLECT_END();

	//static ResDGBufferPtr create( const ResDGBufferCreator &info ) { return nullptr; };


	// Inherited via ResCreator
	virtual ResourcePtr create() const override;

};

template<>
struct cb::TypeTraits<ResDGCubeCreator>
{
	BoolAsType_True  hasReflection;
	BoolAsType_False isPrimitive;
	BoolAsType_False ioBytes;
};

template <>
struct CreatorTraits<ResDGBufVertex>
{
	CreatorBoolAsType_True hasACustomCreator;
	typedef ResDGCubeCreator Creator;
};





PtrFwd( ResDGBufIndex );


class ResDGBufIndex: public ResDGBuffer
{
public:
	CLASS( ResDGBufIndex, ResDGBuffer );

	static ResDGBufIndexPtr create( const char *const pFilename, const util::Symbol &type );
	static ResDGBufIndexPtr createRaw( const u32 dataSize, void *const pData );

	ResDGBufIndex( const dg::RefCntAutoPtr<dg::IBuffer> &buffer );
	virtual ~ResDGBufIndex();


	virtual void load( const char *const pFilename, const util::Symbol type );


	REFLECT_BEGIN( ResDGBufIndex, ResDGBuffer )
	REFLECT_END();

	i32 m_count = 0;

private:

};


PtrDef( ResDGBufVertex );
PtrDef( ResDGBufIndex );


