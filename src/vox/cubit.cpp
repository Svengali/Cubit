//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"


#include "cubit.h"
#include <res/ResDGBuffer.h>
#include <res/ResDGLayout.h>
#include <res/ResDGPipelineState.h>
#include <grx/GeoDiligent.h>
#include <grx/DGRenderer.h>





void vox::CubitArr::set_slow( u8 v, vox::LPos pos )
{

}

u8 vox::CubitArr::get_slow( vox::LPos pos )
{
	return m_arr.m_arr[m_arr.index( pos )];
}

void vox::Cubit::genWorld( Plane<Cubit> *pPlane, const CPos pos )
{

}


//*
void vox::Cubit::genGeo( Plane<Cubit> * pPlane, const CPos pos )
{
}
//*/

static SimplexNoise noise;
static f32 s_fractalMultXY = 1.0f / 256.0f;
static f32 s_fractalMultZ = 1.0f / 64.0f;


void vox::CubitArr::genWorld( Plane<Cubit> *pPlane, const CPos pos )
{
	for( i32 z = 0; z < k_edgeSize.size; ++z )
	{
		const i32 cubeWorldZ = m_gPos.z + z;
		const f32 worldZ = (f32)cubeWorldZ;

		const f32 perlinZ = worldZ * s_fractalMultZ;

		for( i32 y = 0; y < k_edgeSize.size; ++y )
		{
			const i32 cubeWorldY = m_gPos.y + y;
			const f32 worldY = (f32)cubeWorldY;

			const f32 perlinY = worldY * s_fractalMultXY;

			for( i32 x = 0; x < k_edgeSize.size; ++x )
			{
				const i32 cubeWorldX = m_gPos.x + x;
				const f32 worldX = (f32)cubeWorldX;

				const f32 perlinX = worldX * s_fractalMultXY;

				const i32 index = m_arr.index( x, y, z );

				const f32 rawPerlinValue = noise.fractal( 4, perlinX, perlinY, perlinZ );

				const f32 zFade = (worldZ - 32.0f) / 256.0f;

				const f32 perlinValue = zFade + rawPerlinValue + perlinZ - 1;



				m_arr.m_arr[index] = perlinValue < 1.2f;
			}
		}
	}
}


//      (-1,+1,+1)________________(+1,+1,+1)
//               /|              /|
//              / |             / |
//             /  |            /  |
//            /   |           /   |
//(-1,-1,+1) /____|__________/(+1,-1,+1)
//           |    |__________|____|
//           |   /(-1,+1,-1) |    /(+1,+1,-1)
//           |  /            |   /
//           | /             |  /
//           |/              | /
//           /_______________|/
//        (-1,-1,-1)       (+1,-1,-1)
//



const f32 S =  0.5f;
const f32 Z =  0.0F;
const f32 N = -1.0f;
const f32 P =  1.0f;

#if 0
enum Faces
{
	ePX = 1 << 0,
	eNX = 1 << 1,
	ePY = 1 << 2,
	eNY = 1 << 3,
	ePZ = 1 << 4,
	eNZ = 1 << 5,
};
#endif

static VertPosNormalUV CubeVerts[] =
{
		{cb::Vec3( +S,-S,-S ), cb::Vec3( P, Z, Z ), cb::Vec2( 0,1 )},
		{cb::Vec3( +S,-S,+S ), cb::Vec3( P, Z, Z ), cb::Vec2( 1,1 )},
		{cb::Vec3( +S,+S,+S ), cb::Vec3( P, Z, Z ), cb::Vec2( 1,0 )},
		{cb::Vec3( +S,+S,-S ), cb::Vec3( P, Z, Z ), cb::Vec2( 0,0 )},

		{cb::Vec3( -S,+S,-S ), cb::Vec3( N, Z, Z ), cb::Vec2( 1,0 )},
		{cb::Vec3( -S,+S,+S ), cb::Vec3( N, Z, Z ), cb::Vec2( 0,0 )},
		{cb::Vec3( -S,-S,+S ), cb::Vec3( N, Z, Z ), cb::Vec2( 0,1 )},
		{cb::Vec3( -S,-S,-S ), cb::Vec3( N, Z, Z ), cb::Vec2( 1,1 )},

		{cb::Vec3( +S,+S,-S ), cb::Vec3( Z, P, Z ), cb::Vec2( 0,1 )},
		{cb::Vec3( +S,+S,+S ), cb::Vec3( Z, P, Z ), cb::Vec2( 0,0 )},
		{cb::Vec3( -S,+S,+S ), cb::Vec3( Z, P, Z ), cb::Vec2( 1,0 )},
		{cb::Vec3( -S,+S,-S ), cb::Vec3( Z, P, Z ), cb::Vec2( 1,1 )},

		{cb::Vec3( -S,-S,-S ), cb::Vec3( Z, N, Z ), cb::Vec2( 0,1 )},
		{cb::Vec3( -S,-S,+S ), cb::Vec3( Z, N, Z ), cb::Vec2( 0,0 )},
		{cb::Vec3( +S,-S,+S ), cb::Vec3( Z, N, Z ), cb::Vec2( 1,0 )},
		{cb::Vec3( +S,-S,-S ), cb::Vec3( Z, N, Z ), cb::Vec2( 1,1 )},

		{cb::Vec3( +S,-S,+S ), cb::Vec3( Z, Z, P ), cb::Vec2( 0,1 )},
		{cb::Vec3( -S,-S,+S ), cb::Vec3( Z, Z, P ), cb::Vec2( 1,1 )},
		{cb::Vec3( -S,+S,+S ), cb::Vec3( Z, Z, P ), cb::Vec2( 1,0 )},
		{cb::Vec3( +S,+S,+S ), cb::Vec3( Z, Z, P ), cb::Vec2( 0,0 )},

		{cb::Vec3( -S,-S,-S ), cb::Vec3( Z, Z, N ), cb::Vec2( 0,1 )},
		{cb::Vec3( +S,-S,-S ), cb::Vec3( Z, Z, N ), cb::Vec2( 1,1 )},
		{cb::Vec3( +S,+S,-S ), cb::Vec3( Z, Z, N ), cb::Vec2( 1,0 )},
		{cb::Vec3( -S,+S,-S ), cb::Vec3( Z, Z, N ), cb::Vec2( 0,0 )},

};

// clang-format off
static u16 CubeIndices[] =
{
		1,0,2,    2,0,3,
};
// clang-format on



//*

class ChunkMesh
{
public:
	ChunkMesh()
	{
	}

	enum Faces
	{
		ePX = 1 << 0,
		eNX = 1 << 1,
		ePY = 1 << 2,
		eNY = 1 << 3,
		ePZ = 1 << 4,
		eNZ = 1 << 5,
	};

	void pushSingleCube( Faces faces, std::vector<float> *const pPos, std::vector<float> *const pAttr, std::vector<u32> *const pInd, const cb::Vec3 pos )
	{
		static const f32 positions[] = {
			// Right
			P, N, P,
			P, N, N,
			P, P, P,
			P, P, N,

			// Left
			N, N, N,
			N, N, P,
			N, P, N,
			N, P, P,

			// Top
			N, P, P,
			P, P, P,
			N, P, N,
			P, P, N,

			// Bottom
			N, N, N,
			P, N, N,
			N, N, P,
			P, N, P,

			// Near
			N, N, P,
			P, N, P,
			N, P, P,
			P, P, P,

			// Far
			P, N, N,
			N, N, N,
			P, P, N,
			N, P, N,
		};

		static const f32 attr[] = {
			// Right
			P, 0, 0, 0, 0, 0, N, 0, 0, P,
			P, 0, 0, 0, 0, 0, N, 0, P, P,
			P, 0, 0, 0, 0, 0, N, 0, 0, 0,
			P, 0, 0, 0, 0, 0, N, 0, P, 0,

			// Left
			N, 0, 0, 0, 0, 0, P, 0, 0, P,
			N, 0, 0, 0, 0, 0, P, 0, P, P,
			N, 0, 0, 0, 0, 0, P, 0, 0, 0,
			N, 0, 0, 0, 0, 0, P, 0, P, 0,

			// Top
			0, P, 0, 0, P, 0, 0, 0, 0, P,
			0, P, 0, 0, P, 0, 0, 0, P, P,
			0, P, 0, 0, P, 0, 0, 0, 0, 0,
			0, P, 0, 0, P, 0, 0, 0, P, 0,

			// Bottom
			0, N, 0, 0, P, 0, 0, 0, 0, P,
			0, N, 0, 0, P, 0, 0, 0, P, P,
			0, N, 0, 0, P, 0, 0, 0, 0, 0,
			0, N, 0, 0, P, 0, 0, 0, P, 0,

			// Near
			0, 0, P, 0, P, 0, 0, 0, 0, P,
			0, 0, P, 0, P, 0, 0, 0, P, P,
			0, 0, P, 0, P, 0, 0, 0, 0, 0,
			0, 0, P, 0, P, 0, 0, 0, P, 0,

			// Far
			0, 0, N, 0, N, 0, 0, 0, 0, P,
			0, 0, N, 0, N, 0, 0, 0, P, P,
			0, 0, N, 0, N, 0, 0, 0, 0, 0,
			0, 0, N, 0, N, 0, 0, 0, P, 0,
		};

		static const u16 indices[] = {
			0, 1, 2, 3, 2, 1,
			4, 5, 6, 7, 6, 5,
			8, 9, 10, 11, 10, 9,
			12, 13, 14, 15, 14, 13,
			16, 17, 18, 19, 18, 17,
			20, 21, 22, 23, 22, 21,
		};


		u32 curFace = 1 << 0;

		const u32 posStride = 3 * 4;
		const u32 attrStride = 10 * 4;
		const u32 indexStride = 6;

		for( i32 iFace = 0; iFace < 6; ++iFace )
		{
			if( curFace & faces )
			{
				const i32 posStart = posStride * iFace;
				const i32 posEnd = posStart + posStride;

				const i32 attrStart = attrStride * iFace;
				const i32 attrEnd = attrStart + attrStride;

				//const i32 indexStart = indexStride * i;
				//const i32 indexEnd = indexStart + indexStride;


				for( i32 iAttr = attrStart; iAttr < attrEnd; ++iAttr )
				{
					pAttr->push_back( attr[iAttr] );
				}

				const i32 indexStart = (i32)pPos->size() / 3;
				const i32 otherIndexStart = (i32)pInd->size();

				// TODO PORT
				//s_triangleCount += 2;

				for( i32 iIndex = 0; iIndex < 6; ++iIndex )
				{
					pInd->push_back( (u32)(indices[iIndex] + indexStart) );
				}

				for( i32 iPos = posStart; iPos < posEnd; ++iPos )
				{
					pPos->push_back( positions[iPos] + pos[iPos % 3] );
				}
			}

			curFace <<= 1;
		}




	}



	void pushSingleCube( Faces faces, std::vector<VertPosNormalUV> *const pVerts, std::vector<u32> *const pInd, const cb::Vec3 pos )
	{
		u32 curFace = 1 << 0;

		const u32 posStride = 3 * 4;
		const u32 attrStride = 10 * 4;
		const u32 indexStride = 6;

		for( i32 iFace = 0; iFace < 6; ++iFace )
		{
			if( curFace & faces )
			{
				const i32 vertStart = iFace * 4;
				const i32 indicesStart = pVerts->size();

				for( i32 iVert = 0; iVert < 4; ++iVert )
				{
					pVerts->push_back( CubeVerts[vertStart+iVert] );
					pVerts->back().pos += pos;
				}

				for( i32 iIndex = 0; iIndex < 6; ++iIndex )
				{
					pInd->push_back( (u32)( CubeIndices[iIndex] + indicesStart ) );
				}
			}

			curFace <<= 1;
		}




	}

	void cube(
		vox::CubitArr *const pCubit,
		std::vector<VertPosNormalUV> *const pVerts,
//		std::vector<float> *const pAttr,
		std::vector<u32> *const pIndices,
		const vox::LPos pos,
		const cb::Vec3 worldPos,
		const u16 posX,
		const u16 negX,
		const u16 posY,
		const u16 negY,
		const u16 posZ,
		const u16 negZ
	)
	{
		const i32 index = pCubit->m_arr.index( pos );

		const auto cur = pCubit->m_arr.m_arr[index];

		const auto l = cb::Vec3( (f32)pos.x, (f32)pos.y, (f32)pos.z );

		//If its air, skip
		if( !cur ) return;

		Faces faces = (Faces)0;

		faces = cast<Faces>( faces | ( Faces::ePX * cast<int>( !posX ) ) );
		faces = cast<Faces>( faces | ( Faces::eNX * cast<int>( !negX ) ) );

		faces = cast<Faces>( faces | ( Faces::ePY * cast<int>( !posY ) ) );
		faces = cast<Faces>( faces | ( Faces::eNY * cast<int>( !negY ) ) );

		faces = cast<Faces>( faces | ( Faces::ePZ * cast<int>( !posZ ) ) );
		faces = cast<Faces>( faces | ( Faces::eNZ * cast<int>( !negZ ) ) );

		const auto h = l + cb::Vec3( 1, 1, 1 );

		if( faces )
		{
			//const auto startIndex = cast<u16>( pPositions->size() );

			pushSingleCube( faces, pVerts, pIndices, cb::Vec3( worldPos.x + pos.x, worldPos.y + pos.y, worldPos.z + pos.z ) );
		}

	}



	i32 fill( vox::Plane<vox::Cubit> *pPlane, vox::CubitArr *const pCubit, const vox::CPos chunkPos, const cb::Vec3 worldPos )
	{
		std::vector<VertPosNormalUV> verts;
		//std::vector<float> attributes;
		//std::vector<float> attr;
		std::vector<u32> indices;

		//material = gr::StockMaterials::get().get_checkerboard();
		//static_aabb = gr::AABB( gr::vec3( -1000 ) + worldPos, gr::vec3( 1000 ) + worldPos );

		const auto uv00 = cb::Vec2( 0, 0 );
		const auto uv01 = cb::Vec2( 0, 1 );
		const auto uv10 = cb::Vec2( 1, 0 );
		const auto uv11 = cb::Vec2( 1, 1 );

		for( i32 z = 1; z < pCubit->k_edgeSize.size - 1; ++z )
		{
			for( i32 y = 1; y < pCubit->k_edgeSize.size - 1; ++y )
			{
				for( i32 x = 1; x < pCubit->k_edgeSize.size - 1; ++x )
				{
					const auto pos = vox::LPos( x, y, z );

					const u16 posX = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 1, pos.y + 0, pos.z + 0 )];
					const u16 negX = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x - 1, pos.y + 0, pos.z + 0 )];
					const u16 posY = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y + 1, pos.z + 0 )];
					const u16 negY = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y - 1, pos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y + 0, pos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y + 0, pos.z - 1 )];

					cube( pCubit, &verts, &indices, pos, worldPos, posX, negX, posY, negY, posZ, negZ );

				}
			}
		}

		//*
		const i32 zSmall = 0;
		const i32 zBig = pCubit->k_edgeSize.size - 1;

		const auto vPosX = cb::Vec3i( 1, 0, 0 );
		const auto vNegX = cb::Vec3i( -1, 0, 0 );
		const auto vPosY = cb::Vec3i( 0, 1, 0 );
		const auto vNegY = cb::Vec3i( 0, -1, 0 );
		const auto vPosZ = cb::Vec3i( 0, 0, 1 );
		const auto vNegZ = cb::Vec3i( 0, 0, -1 );

		for( i32 y = 0; y < pCubit->k_edgeSize.size; ++y )
		{
			for( i32 x = 0; x < pCubit->k_edgeSize.size; ++x )
			{
				{
					const auto lPos = vox::LPos( x, y, zSmall );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pPlane->get_slow( gPos + vPosZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pPlane->get_slow( gPos + vNegZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, &verts, &indices, lPos, worldPos, posX, negX, posY, negY, posZ, negZ );
				}

				{
					const auto lPos = vox::LPos( x, y, zBig );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pPlane->get_slow( gPos + vPosZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pPlane->get_slow( gPos + vNegZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, &verts, &indices, lPos, worldPos, posX, negX, posY, negY, posZ, negZ );
				}


			}
		}
		//*/

		//*
		const i32 ySmall = 0;
		const i32 yBig = pCubit->k_edgeSize.size - 1;

		for( i32 z = 1; z < pCubit->k_edgeSize.size - 1; ++z )
		{
			for( i32 x = 0; x < pCubit->k_edgeSize.size - 0; ++x )
			{
				{
					const auto lPos = vox::LPos( x, ySmall, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, &verts, &indices, lPos, worldPos, posX, negX, posY, negY, posZ, negZ );
				}

				{
					const auto lPos = vox::LPos( x, yBig, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, &verts, &indices, lPos, worldPos, posX, negX, posY, negY, posZ, negZ );
				}


			}
		}
		//*/

		//*
		const i32 xSmall = 0;
		const i32 xBig = pCubit->k_edgeSize.size - 1;

		for( i32 z = 1; z < pCubit->k_edgeSize.size - 1; ++z )
		{
			for( i32 y = 1; y < pCubit->k_edgeSize.size - 1; ++y )
			{
				{
					const auto lPos = vox::LPos( xSmall, y, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, &verts, &indices, lPos, worldPos, posX, negX, posY, negY, posZ, negZ );
				}

				{
					const auto lPos = vox::LPos( xBig, y, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, &verts, &indices, lPos, worldPos, posX, negX, posY, negY, posZ, negZ );
				}
			}
		}
		//*/

		if( verts.size() == 0 )
			return 0;

		const auto bufVerts = ResDGBufVertex::createRaw( verts.size() * sizeof( VertPosNormalUV ), verts.data() );

		const auto bufIndices = ResDGBufIndex::createRaw( indices.size() * sizeof( u32 ), indices.data() );

		GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/vox.xml" );
		ResourceMgr::RemResource( "config/geo/vox.xml" );

		cfg->m_vertexBuf = bufVerts;
		cfg->m_indexBuf  = bufIndices;


		cb::Mat3 mat( cb::Mat3::eIdentity );

		cb::Vec3 pos( 0, 0, 0 );
		cb::Frame3 frame( mat, pos );


		GeoDiligentPtr geo = GeoDiligentPtr( new GeoDiligent( ent::EntityId::makeNext(), cfg ) );

		DGRenderer::Inst().addStaticGeo( frame, geo );

		return (i32)verts.size();
	}

};

//*/

void vox::CubitArr::genGeo( Plane<Cubit> * pPlane, const CPos pos )
{
	const auto pGeo = new ChunkMesh();

	const auto worldPos = GPos::from( pos );
	const auto wP = cb::Vec3( (f32)worldPos.x, (f32)worldPos.y, (f32)worldPos.z ) ;

	const auto vertCount = pGeo->fill( pPlane, this, pos, wP );

	if( vertCount == 0 )
		return;
}

static i32 s_curX = 1;
static i32 s_curY = 1;
static i32 s_curYExtent = 10;

void vox::CubitPlane::genWorld( const cb::Vec3 pos )
{

	const auto gPos = GPos::from( pos );

	const auto v = CPos::from( gPos );

	const cb::Vec3i startingChunkPos( v.x, v.y, v.z );


	if( s_curX > 50 )
	{
		if( s_curY > 50 )
		{
			return;
		}

		s_curX = 1;
		s_curY += s_curYExtent;
	}


	for( i32 z = 1; z < 14; ++z )
	{
		for( i32 y = s_curY; y < s_curY + s_curYExtent; ++y )
		{
			const i32 x = s_curX;
			//for( i32 x = 1; x < 200; ++x )
			{
				//async::parallel_for( async::irange( 1, 100 ), [ startingChunkPos, z, y, this ]( int x ) {
				auto cbChunkPos = startingChunkPos;

				cbChunkPos += cb::Vec3i( x, y, z );

				const CPos chunkPos( cbChunkPos.x, cbChunkPos.y, cbChunkPos.z );


				//auto chunk = get( chunkPos );


				const auto cubit = TChunk::Ptr( new CubitArr( chunkPos ) );

				cubit->genWorld( this, chunkPos );

				//if( !chunk.has_value() ) chunk = cubit

				const auto hashFn = std::hash<CPos>();
				const auto hash = hashFn( chunkPos );


				m_sparse[chunkPos] = cubit;

				//cubit->genGeo( this, pDev, pScene, chunkPos );
			//} );
			}
		}
	}

	s_curX++;

	//int dummy = 0;

}

//*
void vox::CubitPlane::genGeo( const cb::Vec3 pos )
{

	i32 generated = 0;
	for(auto it = m_sparse.begin(); it != m_sparse.end(); ++it)
	{
		const auto chunk = it->second;

		if( !chunk->m_generated )
		{
			chunk->genGeo( this, it->first );
			chunk->m_generated = true;
			generated++;
		}

		if( generated > 64 )
		{
			return;
		}
	}



}
//*/



