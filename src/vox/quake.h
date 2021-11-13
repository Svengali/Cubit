//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

struct VertPosNormalColorUV;

namespace vox
{

	
	const static f32 c_sizeQuake = ( 1.0f / 256.0f );
	const static f32 c_sizeQuakeInv = 1.0f / c_sizeQuake;




	class Quake: public Chunk<Quake, u8, 32>
	{
	public:

		using Chunk::Chunk;

		virtual bool genWorld( Plane<Quake> *pPlane, const CPos pos );
		virtual void genGeo( Plane<Quake> * pPlane, const CPos pos, std::vector<VertPosNormalColorUV> *pVerts, std::vector<u32> *pIndices );

		bool m_generated = false;

		//cb::Vec3 m_translation = cb::Vec3( 0.0f, 0.0f, 0.0f );
		//cb::Vec3 m_scaleFactor = cb::Vec3( 1.0f, 1.0f, 1.0f );

	protected:
		//gr::AbstractRenderableHandle m_geo;
	};

	class QuakeArr: public Quake
	{
	public:

		using Quake::Quake;

		virtual void set_slow( u8 v, LPos pos ) override;
		virtual u8 get_slow( LPos pos ) override;

		virtual bool genWorld( Plane<Quake> *pPlane, CPos pos ) override;
		virtual void genGeo( Plane<Quake> * pPlane, const CPos pos, std::vector<VertPosNormalColorUV> *pVerts, std::vector<u32> *pIndices ) override;

		virtual void genCollision( Plane<Quake> *pPlane, CPos pos );

		StorageArr<Quake> m_arr;

		// TODO Fix this hack.  This works for a 1:1 situation, but we need a general case solution.
		StorageArr<Quake> m_faces;

		// TODO STATIC COLLISION
		//minx sorted collision boxes
		//std::vector<cb::AxialBox> m_collisionBoxes;

	private:

		friend class DGChunkMesh;

	};

	class QuakePlane: public FramePlane<Quake>
	{
	public:
		virtual bool genWorld( const cb::Vec3 pos );
		virtual bool genGeo( const cb::Vec3 pos );
	
		void collide( const cb::Segment &seg, cb::SegmentResults *pRes );

		void genAxialBoxes( Quake::Ptr chunk, const i32 iz, const Quake::LPos lMin, const Quake::LPos lMax, std::vector<cb::AxialBox> *pBoxes );

		template< size_t size >
		void findOverlapping( const Quake::CPos min, const Quake::CPos max, std::array< Quake::Ptr, size > *pArr, i32 *pCount );

	private:
		std::hash_map<typename Quake::CPos, typename Quake::Ptr, PosHash<Quake>> m_generateGeo;

		// TODO STATIC COLLISION
		//std::hash_map<typename Quake::CPos, typename Quake::Ptr, PosHash<Quake>> m_genColl;
	};

	PtrDef(QuakePlane);

}

