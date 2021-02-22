//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

//Move into stdafx

#include "./ResDGLayout.h"


ResDGLayoutPtr ResDGLayout::create( const std::vector<dg::LayoutElement> &layout )
{
	return ResDGLayoutPtr( new ResDGLayout( layout ) );
}



ResDGLayout::ResDGLayout( const std::vector<dg::LayoutElement> &layout )
	:
	m_layout( layout )
{
}

ResDGLayout::~ResDGLayout( void )
{
}

void ResDGLayout::load( const char *const pFilename )
{

}
