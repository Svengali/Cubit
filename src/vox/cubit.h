//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

struct VertPosNormalUV;

namespace vox
{
	class Cubit: public Chunk<Cubit, u8, 32>
	{
	public:

		using Chunk::Chunk;

		virtual bool genWorld( Plane<Cubit> *pPlane, const CPos pos );
		virtual void genGeo( Plane<Cubit> * pPlane, const CPos pos, std::vector<VertPosNormalUV> *pVerts, std::vector<u32> *pIndices );

		bool m_generated = false;

		cb::Vec3 m_translation = cb::Vec3( 0.0f, 0.0f, 0.0f );
		cb::Vec3 m_scaleFactor = cb::Vec3( 1.0f, 1.0f, 1.0f );

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
		virtual void genGeo( Plane<Cubit> * pPlane, const CPos pos, std::vector<VertPosNormalUV> *pVerts, std::vector<u32> *pIndices ) override;

		StorageArr<Cubit> m_arr;

	private:

		friend class DGChunkMesh;

	};

	class CubitPlane: public Plane<Cubit>
	{
	public:
		virtual bool genWorld( const cb::Vec3 pos );
		virtual void genGeo( const cb::Vec3 pos );

		cb::Vec3 m_translation = cb::Vec3( 0.0f, 0.0f, 0.0f );
		cb::Vec3 m_scaleFactor = cb::Vec3( 1.0f, 1.0f, 1.0f );
	private:
	};

	PtrDef(CubitPlane);

}

