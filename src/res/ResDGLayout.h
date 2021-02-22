//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "res/Resource.h"

PtrFwd( ResDGLayout );


template< typename T >
inline dg::LayoutElement LayoutElementFromType( const u32 count, const T &val )
{
	// If youre getting an error here about  must return a value
	// You need to add the type below 
}

template<>
inline dg::LayoutElement LayoutElementFromType( const u32 count, const cb::Vec3 &val )
{
	return dg::LayoutElement{ count, 0, 3, dg::VT_FLOAT32, dg::False };
}

template<>
inline dg::LayoutElement LayoutElementFromType( const u32 count, const cb::Vec2 &val )
{
	return dg::LayoutElement{ count, 0, 2, dg::VT_FLOAT32, dg::False };
}



class StructConverter
{

public:

	std::vector<dg::LayoutElement> *const m_pElements;

	StructConverter( std::vector<dg::LayoutElement> * const pElements )
		:
		m_pElements( pElements )
	{
	}



	template < typename T >
	void operator() ( const char *name, T &val )
	{
		const u32 index = (u32)m_pElements->size();

		const dg::LayoutElement le = LayoutElementFromType( index, val );

		m_pElements->push_back( le );
	}


};

template< typename T >
void StructToLayout( const T &t, std::vector<dg::LayoutElement> *const pLayout );



struct VertexBase
{
public:
	STRUCT( VertexBase )

	void Reflection( StructConverter &converter ) { Reflection<StructConverter>( converter ); }

	REFLECT_BEGIN_ROOT( VertexBase )
	REFLECT_END()

};

struct VertPosUV: public VertexBase
{
	STRUCT( VertPosUV, VertexBase );

	REFLECT_BEGIN( VertPosUV, VertexBase )
		REFLECT( pos );
		REFLECT( uv );
	REFLECT_END();

	VertPosUV() = default;

	VertPosUV( const cb::Vec3 _pos, const cb::Vec2 _uv )
		:
		pos( _pos ),
		uv ( _uv  )
		{}

	cb::Vec3 pos;
	cb::Vec2 uv;
};

struct VertPosNormalUV: public VertexBase
{
	STRUCT( VertPosUV, VertexBase );

	REFLECT_BEGIN( VertPosUV, VertexBase )
		REFLECT( pos );
		REFLECT( pos );
		REFLECT( uv );
	REFLECT_END();


	cb::Vec3 pos;
	cb::Vec3 normal;
	cb::Vec2 uv;
};



//General buffer
class ResDGLayout: public Resource
{
public:
	CLASS( ResDGLayout, Resource );


	static ResDGLayoutPtr create( const std::vector<dg::LayoutElement> &layout );


	ResDGLayout( const std::vector<dg::LayoutElement> &layout );
	virtual ~ResDGLayout();


	virtual void load( const char *const pFilename );


	REFLECT_BEGIN( ResDGLayout, Resource )
	REFLECT_END();

	const std::vector<dg::LayoutElement> &Layout()
	{
		return m_layout;
	}


private:
	std::vector<dg::LayoutElement> m_layout;
};

PtrDef( ResDGLayout );







template< typename T >
void StructToLayout( T &t, std::vector<dg::LayoutElement> *const pLayout )
{
	StructConverter converter( pLayout );

	t.Reflection( converter );

}
