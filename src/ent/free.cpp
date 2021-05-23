//////////////////////////////////////////////////////////////////////////////
//
//   P h a m 
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

#include "free.h"


cb::Vec3 vox::FramePlane<Freefall>::m_translation = cb::Vec3( 0.0f, 0.0f, 0.0f );
cb::Vec3 vox::FramePlane<Freefall>::m_scaleFactor = cb::Vec3( 64.0f, 64.0f, 64.0f );
cb::Vec3 vox::FramePlane<Freefall>::m_invScaleFactor = cb::Vec3( 1.0f / 64.0f, 1.0f / 64.0f, 1.0f / 64.0f );



namespace ent
{
//SERIALIZABLE( EntityConfig );
}


void FreefallArr::set_slow( FreefallBlock v, LPos pos )
{
	ASSERT( false && "This creates trash" );
}

FreefallBlock FreefallArr::get_slow( LPos pos )
{
	ASSERT( false && "This creates trash" );
	return FreefallBlock();
}

FreefallBlock *FreefallArr::pGet( LPos pos )
{
	const auto index = m_arr.index( pos );

	return &m_arr.m_arr[index];
}
