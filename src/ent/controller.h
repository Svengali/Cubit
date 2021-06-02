//////////////////////////////////////////////////////////////////////////////
//
//   P h a m 
//
// copyright 2005-2021 Marc Hernandez
#pragma once

//#include <com/Component.h>


namespace ent
{


	class PlayerController
	{
	public:


		enum ESlots
		{
			Status   = 0,
			EntityId = 1,
			Geometry = 2,
			Frame    = 3,
			Velocity = 4,
			Latitude = 5,
			Longitude= 6,
		};

	public:


		typedef df::ComBlocks<k_freefallBlockSize, PlayerController::ESlots, GeoDiligentPtr, cb::Frame3, cb::Vec3, f32, f32> TCom;
		typedef TCom::AllBlocks::TBlock Block;

		TCom m_com;

	public:

	};



}


