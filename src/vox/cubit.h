//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

struct VertPosNormalColorUV;

namespace vox
{

	




	class Cubit: public Chunk<Cubit, u8, 32>
	{
	public:

		using Chunk::Chunk;

		virtual bool genWorld( Plane<Cubit> *pPlane, const CPos pos );
		virtual void genGeo( Plane<Cubit> * pPlane, const CPos pos, std::vector<VertPosNormalColorUV> *pVerts, std::vector<u32> *pIndices );

		bool m_generated = false;

		//cb::Vec3 m_translation = cb::Vec3( 0.0f, 0.0f, 0.0f );
		//cb::Vec3 m_scaleFactor = cb::Vec3( 1.0f, 1.0f, 1.0f );

	protected:
		//gr::AbstractRenderableHandle m_geo;
	};

	class CubitArr: public Cubit
	{
	public:

		using Cubit::Cubit;

		virtual void set_slow( u8 v, LPos pos ) override;
		virtual u8 get_slow( LPos pos ) override;

		virtual bool genWorld( Plane<Cubit> *pPlane, CPos pos ) override;
		virtual void genGeo( Plane<Cubit> * pPlane, const CPos pos, std::vector<VertPosNormalColorUV> *pVerts, std::vector<u32> *pIndices ) override;

		virtual void genCollision( Plane<Cubit> *pPlane, CPos pos );


		u8 genPoint( const f32 threshold, const f32 worldX, const f32 worldY, const f32 worldZ, const f32 perlinX, const f32 perlinY, const f32 perlinZ );


		StorageArr<Cubit> m_arr;

		// TODO Fix this hack.  This works for a 1:1 situation, but we need a general case solution.
		StorageArr<Cubit> m_faces;

		// TODO STATIC COLLISION
		//minx sorted collision boxes
		//std::vector<cb::AxialBox> m_collisionBoxes;

	private:

		friend class DGChunkMesh;

	};

	class CubitPlane: public FramePlane<Cubit>
	{
	public:
		virtual bool genWorld( const cb::Vec3 pos );
		virtual bool genGeo( const cb::Vec3 pos );
	
		void collide( const cb::Segment &seg, cb::SegmentResults *pRes );

		void genAxialBoxes( Cubit::Ptr chunk, const i32 iz, const Cubit::LPos lMin, const Cubit::LPos lMax, std::vector<cb::AxialBox> *pBoxes );

		template< size_t size >
		void findOverlapping( const Cubit::CPos min, const Cubit::CPos max, std::array< Cubit::Ptr, size > *pArr, i32 *pCount );

	private:
		std::hash_map<typename Cubit::CPos, typename Cubit::Ptr, PosHash<Cubit>> m_generateGeo;

		// TODO STATIC COLLISION
		//std::hash_map<typename Cubit::CPos, typename Cubit::Ptr, PosHash<Cubit>> m_genColl;
	};

	PtrDef(CubitPlane);

}

