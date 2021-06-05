//////////////////////////////////////////////////////////////////////////////
//
//   P h a m 
//
// copyright 2005-2021 Marc Hernandez
#pragma once

#include <com/Component.h>

PtrFwd( GeoDiligent );

const i32 k_freefallBlockSize = 64;

class FreefallData
{
public:


	enum ESlots
	{
		Status		= 0,
		EntityId	= 1,
		Geometry	= 2,
		Frame			= 3,
		Velocity	= 4,
		Accel     = 5,
		Force			= 6,
	};

public:


	typedef df::ComBlocks<k_freefallBlockSize, FreefallData::ESlots, GeoDiligentPtr, cb::Frame3, cb::Vec3, cb::Vec3, cb::Vec3> TCom;
	typedef TCom::AllBlocks::TBlock Block;

	TCom m_com;

public:

	void operate( std::function<void( TCom::AllBlocks::TBlock *, const i32 )> );
};

class RCDiligent;


class FreefallBlock
{
public:

	template<typename... Args>
	void insert( ent::EntityId id, Args... args )
	{
		++m_count;
		m_data.m_com.insert( id, args... );
	}

	void remove( ent::EntityId id )
	{
		--m_count;
		m_data.m_com.remove( id );
	}

	i32						m_count = 0;
	FreefallData	m_data;

};


class Freefall: public vox::Chunk<Freefall, FreefallBlock, 8>
{
public:
	using Chunk::Chunk;

	virtual FreefallBlock *pGet( LPos pos ) = 0;

	virtual void gatherActiveBlocks( std::vector<FreefallData *> *pFreefalVec ) = 0;

protected:
};

class FreefallArr: public Freefall
{
public:

	using Freefall::Freefall;


	virtual void set_slow( FreefallBlock v, LPos pos ) override;
	virtual FreefallBlock get_slow( LPos pos ) override;

	FreefallBlock *pGet( LPos pos );


	virtual void gatherActiveBlocks( std::vector<FreefallData *> *pFreefallVec ) override;

	//virtual void render( RCDiligent *pContext )


	vox::StorageArr<Freefall> m_arr;

private:

	friend class DGChunkMesh;

};


class FreefallPlane: public vox::FramePlane<Freefall>
{
public:


	typename TChunk::Opt get( const cb::Vec3 world )
	{
		const auto gpos = from( world );

		const auto cpos = TChunk::CPos::from( gpos );

		const auto chunkOpt = vox::FramePlane<Freefall>::get( cpos );

		return chunkOpt;
	}

	template<typename... Args>
	void add( const cb::Vec3 world, ent::EntityId id, Args... args )
	{
		TChunk::Opt chunkOpt = get( world );

		const auto gpos = from( world );

		const auto cpos = TChunk::CPos::from( gpos );

		const auto lpos = TChunk::LPos::from( cpos, gpos );

		if( !chunkOpt.has_value() )
		{
			const auto ptr =TChunk::Ptr( new FreefallArr( cpos ) );

			chunkOpt = ptr;

			vox::FramePlane<Freefall>::set( cpos, ptr );
		}

		auto *pBlock = (*chunkOpt)->pGet( lpos );

		pBlock->insert( id, args...);
	}


	void updateBlocks( double dt );


private:
};

PtrDef( FreefallPlane );


