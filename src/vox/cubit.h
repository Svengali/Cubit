//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once


namespace vox
{







	class Cubit: public Chunk<Cubit, u8, 32>
	{
	public:

		using Chunk::Chunk;

		virtual void genWorld( Plane<Cubit> *pPlane, const CPos pos );
		virtual void genGeo( Plane<Cubit> * pPlane, const CPos pos );

		bool m_generated = false;

	protected:
		//gr::AbstractRenderableHandle m_geo;
	};

	class CubitArr: public Cubit
	{
	public:

		using Cubit::Cubit;

		virtual void set_slow( u8 v, LPos pos ) override;
		virtual u8 get_slow( LPos pos ) override;

		virtual void genWorld( Plane<Cubit> *pPlane, CPos pos ) override;
		virtual void genGeo( Plane<Cubit> * pPlane, const CPos pos ) override;


		StorageArr<Cubit> m_arr;


	private:

		friend class DGChunkMesh;

	};

	class CubitPlane: public Plane<Cubit>
	{
	public:



		virtual void genWorld( const cb::Vec3 pos );
		virtual void genGeo( const cb::Vec3 pos );





	private:
	};

	PtrDef(CubitPlane);




}

