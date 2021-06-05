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



namespace cb
{
	inline String autoArgConvert( const vox::Cubit::LPos &pos )
	{
		char buffer[256];
		snprintf( buffer, 256, "%i, %i, %i", pos.x, pos.y, pos.z );

		return String( buffer );
	}

} 


cb::Vec3 vox::FramePlane<vox::Cubit>::m_translation = cb::Vec3( 0.0f, 0.0f, 0.0f );
cb::Vec3 vox::FramePlane<vox::Cubit>::m_scaleFactor = cb::Vec3( 0.25f, 0.25f, 0.25f );
cb::Vec3 vox::FramePlane<vox::Cubit>::m_invScaleFactor = cb::Vec3( 4.0f, 4.0f, 4.0f );

static float s_shiftValue = 1.0f;


void vox::CubitArr::set_slow( u8 v, LPos pos )
{
	ASSERT( false && "Unused right now" );
}

u8 vox::CubitArr::get_slow( LPos pos )
{
	return m_arr.m_arr[m_arr.index( pos )];
}

bool vox::Cubit::genWorld( Plane<Cubit> *pPlane, const CPos pos )
{
	return false;
}


//*
void vox::Cubit::genGeo( Plane<Cubit> *pPlane, const CPos pos, std::vector<VertPosNormalUV> *pVerts, std::vector<u32> *pIndices )
{
}
//*/


void vox::CubitPlane::collide( const cb::Segment &seg, cb::SegmentResults *pRes )
{
	cb::AxialBox segmentBox;
	segmentBox.SetEnclosing( seg );

	//8 at the WORST case for any small segment. 
	//We should likely build a collideLarge or somesuch for huge segments 
	//relative to the 
	std::array< Cubit::Ptr, 16 > overlapping;
	i32 count = 0;

	const auto min = segmentBox.GetMin();
	const auto max = segmentBox.GetMax();

	// TODO Combine these?  Go directly from worldpos to 
	const auto gMin = Cubit::GPos::from( min ) + Cubit::GPos(-1,-1,-1 );
	const auto gMax = Cubit::GPos::from( max ) + Cubit::GPos( 1, 1, 1 );

	const auto cMin = Cubit::CPos::from( gMin );
	const auto cMax = Cubit::CPos::from( gMax );

	findOverlapping( cMin, cMax, &overlapping, &count );

	cb::SegmentResults finalRes;

	for( i32 i = 0; i < count; ++i )
	{
		Cubit::Ptr chunk = overlapping[i];

		const auto lMinRaw = Cubit::LPos::from( chunk->m_cPos, gMin );
		const auto lMaxRaw = Cubit::LPos::from( chunk->m_cPos, gMax );

		const auto lMin = Cubit::LPos::MakeMax( lMinRaw, Cubit::LPos( 0, 0, 0 ) );
		const auto lMax = Cubit::LPos::MakeMin( lMaxRaw, Cubit::LMax - Cubit::LPos( 1, 1, 1 ) );


		std::vector<cb::AxialBox> boxes;

		for( i32 iz = lMax.z; iz >= lMin.z; --iz )
		{
			genAxialBoxes( chunk, iz, lMin, lMax, &boxes );

			if( !boxes.empty() )
			{
				for( auto box : boxes )
				{
					cb::SegmentResults res;
					box.IntersectSurface( seg, &res );

					if( res.time < finalRes.time )
					{
						finalRes = res;
					}
				}

				boxes.clear();
			}
		}

		*pRes = finalRes;

		/*
			for( i32 iy = lMin.y; iy < lMax.y; ++iy )
			{
				for( i32 ix = lMin.x; ix < lMax.x; ++ix )
				{
					const Cubit::LPos pos( ix, iy, iz );


				}
			}
			*/
	}

}

void vox::CubitPlane::genAxialBoxes( Cubit::Ptr chunk, const i32 iz, const Cubit::LPos lMin, const Cubit::LPos lMax, std::vector<cb::AxialBox> *pBoxes )
{
	for( i32 iy = lMin.y; iy <= lMax.y; ++iy )
	{
		for( i32 ix = lMin.x; ix <= lMax.x; ++ix )
		{
			const Cubit::LPos pos( ix, iy, iz );

			const Cubit *pChunk = chunk.get();

			// PORT ISSUE.  Any change to our types will need to take this into account
			const CubitArr *pChunkArr = static_cast<const CubitArr *>( pChunk );

			const auto arrIndex = pChunkArr->m_faces.index( pos );

			const auto hasFaces = !!pChunkArr->m_faces.m_arr[arrIndex];

			const auto hasType = !!chunk->get_slow( pos );

			//These are bools, so convert the occasional 2 branches into a jump
			if( hasFaces & hasType )
			{
				const auto gPos = Cubit::GPos::from( chunk->m_cPos, pos );
				const auto wPos = gPos.toWorld();

				// PORT ISSUE, make 0.25 generic
				cb::Vec3 wMin = wPos - 0.125f;
				cb::Vec3 wMax = wPos + 0.125f;

				cb::AxialBox box( wMin, wMax );

				pBoxes->emplace_back( std::move( box ) );
			}
		}
	}

}


template< size_t size >
void vox::CubitPlane::findOverlapping( const Cubit::CPos min, const Cubit::CPos max, std::array< Cubit::Ptr, size > *pArr, i32 *pCount )
{

	i32 index = 0;

	for( auto [pos, ptr] : m_sparse )
	{
		if( ( pos >= min ) & ( pos <= max ) )
		{
			(*pArr)[index] = ptr;
			++index;

			if( index >= size )
			{
				lprinterr( "Blew out the overlapping list of %i\n", index );
				return;
			}
		}
	}

	*pCount = index;
}



static SimplexNoise noise;
static f32 s_fractalMultXY = 1.0f / 512.0f;
static f32 s_fractalMultZ  = 1.0f / 128.0f;


bool vox::CubitArr::genWorld( Plane<Cubit> *pPlane, const CPos pos )
{

	bool hasValues = false;

	for( i32 z = 0; z < k_edgeSize; ++z )
	{
		const i32 cubeWorldZ = m_gPos.z + z;
		const f32 worldZ = (f32)cubeWorldZ + 100.0f;

		const f32 perlinZ = worldZ * s_fractalMultZ;

		for( i32 y = 0; y < k_edgeSize; ++y )
		{
			const i32 cubeWorldY = m_gPos.y + y;
			const f32 worldY = (f32)cubeWorldY;

			const f32 perlinY = worldY * s_fractalMultXY;

			for( i32 x = 0; x < k_edgeSize; ++x )
			{
				const i32 index = m_arr.index( x, y, z );

				const i32 cubeWorldX = m_gPos.x + x;
				const f32 worldX = (f32)cubeWorldX;

				const f32 perlinX = worldX * s_fractalMultXY + s_shiftValue * 10.0f;

				//*
				const f32 roughnessMapRaw = ( noise.fractal( 3, perlinX + 1000.0f, perlinY + 1000 ) + 1.0f ) * 0.4f;

				const f32 roughnessMap = std::clamp( roughnessMapRaw, 0.0f, 1.0f );

				const size_t octaves = (size_t)( roughnessMap * 8.0f ) + 1;

				const f32 rawPerlinValue = noise.fractal( octaves, perlinX, perlinY, perlinZ );

				const f32 fadeMapRaw = ( noise.fractal( 3, perlinX + 2000.0f, perlinY + 2000 ) + 1.0f ) * 0.4f;

				const f32 fadeMapClamped = 1.0f - std::clamp( fadeMapRaw, 0.0f, 1.0f );

				const f32 fadeMap = fadeMapClamped * fadeMapClamped * fadeMapClamped;

				const f32 fadeMapShifted = fadeMap - 0.5f;

				const f32 zFade = ( (worldZ)*fadeMap ) / ( 256.0f * ( ( fadeMap + 1 ) * 4.0f ) );

				const f32 perlinValue = zFade + rawPerlinValue + perlinZ - 1;
				/*/

				const i32 valX = ( ( ( cubeWorldX ) / 64 ) & 1 );
				const i32 valY = ( ( ( cubeWorldY ) / 64 ) & 1 );

				const f32 val = (f32)(valX ^ valY);

				const auto perlinValue = (f32)(cubeWorldZ / 128.0f) + val * (1.0f / 16.0f);
				//*/



				const auto perlinVal = perlinValue < 0.8f;

				hasValues |= perlinVal;

				m_arr.m_arr[index] = perlinVal;
			}
		}
	}

	return hasValues;
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



const f32 S = 0.5f;
const f32 Z = 0.0F;
const f32 N = -1.0f;
const f32 P = 1.0f;

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

template<typename TCHUNK>
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
					pInd->push_back( (u32)( indices[iIndex] + indexStart ) );
				}

				for( i32 iPos = posStart; iPos < posEnd; ++iPos )
				{
					pPos->push_back( positions[iPos] + pos[iPos % 3] );
				}
			}

			curFace <<= 1;
		}




	}



	void pushSingleCube( Faces faces, std::vector<VertPosNormalUV> *const pVerts, std::vector<u32> *const pInd, const cb::Vec3 pos, const cb::Vec3 scale )
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
				const i32 indicesStart = (i32)pVerts->size();

				for( i32 iVert = 0; iVert < 4; ++iVert )
				{
					const auto rawVert = CubeVerts[vertStart + iVert];
					//const auto scaledVert = cb::Vec3( rawVert.x * scale.x, )

					pVerts->push_back( rawVert );
					pVerts->back().pos.ComponentwiseScale( scale );
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
		const typename TCHUNK::LPos pos,
		const cb::Vec3 worldPos,
		const cb::Vec3 scale,
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

		const auto h = l + cb::Vec3( 1 * scale.x, 1 * scale.y, 1 * scale.z );

		pCubit->m_faces.index( pos );
		pCubit->m_faces.m_arr[index] = faces;

		if( faces )
		{
			//const auto startIndex = cast<u16>( pPositions->size() );

			const auto world = cb::Vec3( worldPos.x + l.x * scale.x, worldPos.y + l.y * scale.y, worldPos.z + l.z * scale.z );

			pushSingleCube( faces, pVerts, pIndices, world, scale );
		}

	}



	i32 fill( vox::Plane<vox::Cubit> *pPlane, std::vector<VertPosNormalUV> *pVerts, std::vector<u32> *pIndices, vox::CubitArr *const pCubit, const typename TCHUNK::CPos chunkPos, const cb::Vec3 worldPos, const cb::Vec3 scale )
	{
		//std::vector<VertPosNormalUV> verts;
		//std::vector<u32> indices;

		//std::vector<float> attributes;
		//std::vector<float> attr;

		//material = gr::StockMaterials::get().get_checkerboard();
		//static_aabb = gr::AABB( gr::vec3( -1000 ) + worldPos, gr::vec3( 1000 ) + worldPos );

		const auto uv00 = cb::Vec2( 0, 0 );
		const auto uv01 = cb::Vec2( 0, 1 );
		const auto uv10 = cb::Vec2( 1, 0 );
		const auto uv11 = cb::Vec2( 1, 1 );

		for( i32 z = 1; z < pCubit->k_edgeSize - 1; ++z )
		{
			for( i32 y = 1; y < pCubit->k_edgeSize - 1; ++y )
			{
				for( i32 x = 1; x < pCubit->k_edgeSize - 1; ++x )
				{
					const auto pos = TCHUNK::LPos( x, y, z );

					const u16 posX = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 1, pos.y + 0, pos.z + 0 )];
					const u16 negX = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x - 1, pos.y + 0, pos.z + 0 )];
					const u16 posY = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y + 1, pos.z + 0 )];
					const u16 negY = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y - 1, pos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y + 0, pos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( pos.x + 0, pos.y + 0, pos.z - 1 )];

					cube( pCubit, pVerts, pIndices, pos, worldPos, scale, posX, negX, posY, negY, posZ, negZ );

				}
			}
		}

		//*
		const i32 zSmall = 0;
		const i32 zBig = pCubit->k_edgeSize - 1;

		const auto vPosX = TCHUNK::GPos( 1, 0, 0 );
		const auto vNegX = TCHUNK::GPos( -1, 0, 0 );
		const auto vPosY = TCHUNK::GPos( 0, 1, 0 );
		const auto vNegY = TCHUNK::GPos( 0, -1, 0 );
		const auto vPosZ = TCHUNK::GPos( 0, 0, 1 );
		const auto vNegZ = TCHUNK::GPos( 0, 0, -1 );

		for( i32 y = 0; y < pCubit->k_edgeSize; ++y )
		{
			for( i32 x = 0; x < pCubit->k_edgeSize; ++x )
			{
				{
					const auto lPos = TCHUNK::LPos( x, y, zSmall );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pPlane->get_slow( gPos + vPosZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pPlane->get_slow( gPos + vNegZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, pVerts, pIndices, lPos, worldPos, scale, posX, negX, posY, negY, posZ, negZ );
				}

				{
					const auto lPos = TCHUNK::LPos( x, y, zBig );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pPlane->get_slow( gPos + vPosZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pPlane->get_slow( gPos + vNegZ ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, pVerts, pIndices, lPos, worldPos, scale, posX, negX, posY, negY, posZ, negZ );
				}


			}
		}
		//*/

		//*
		const i32 ySmall = 0;
		const i32 yBig = pCubit->k_edgeSize - 1;

		for( i32 z = 1; z < pCubit->k_edgeSize - 1; ++z )
		{
			for( i32 x = 0; x < pCubit->k_edgeSize - 0; ++x )
			{
				{
					const auto lPos = TCHUNK::LPos( x, ySmall, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, pVerts, pIndices, lPos, worldPos, scale, posX, negX, posY, negY, posZ, negZ );
				}

				{
					const auto lPos = TCHUNK::LPos( x, yBig, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pPlane->get_slow( gPos + vPosY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pPlane->get_slow( gPos + vNegY ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, pVerts, pIndices, lPos, worldPos, scale, posX, negX, posY, negY, posZ, negZ );
				}


			}
		}
		//*/

		//*
		const i32 xSmall = 0;
		const i32 xBig = pCubit->k_edgeSize - 1;

		for( i32 z = 1; z < pCubit->k_edgeSize - 1; ++z )
		{
			for( i32 y = 1; y < pCubit->k_edgeSize - 1; ++y )
			{
				{
					const auto lPos = TCHUNK::LPos( xSmall, y, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, pVerts, pIndices, lPos, worldPos, scale, posX, negX, posY, negY, posZ, negZ );
				}

				{
					const auto lPos = TCHUNK::LPos( xBig, y, z );
					const auto gPos = chunkPos + lPos;

					const u16 posX = pPlane->get_slow( gPos + vPosX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 1, lPos.y + 0, lPos.z + 0 )];
					const u16 negX = pPlane->get_slow( gPos + vNegX ); //pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x - 1, lPos.y + 0, lPos.z + 0 )];
					const u16 posY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 1, lPos.z + 0 )];
					const u16 negY = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y - 1, lPos.z + 0 )];
					const u16 posZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z + 1 )];
					const u16 negZ = pCubit->m_arr.m_arr[pCubit->m_arr.index( lPos.x + 0, lPos.y + 0, lPos.z - 1 )];

					cube( pCubit, pVerts, pIndices, lPos, worldPos, scale, posX, negX, posY, negY, posZ, negZ );
				}
			}
		}
		//*/

		if( pVerts->size() == 0 )
			return 0;


		/*
		const auto bufVerts = ResDGBufVertex::createRaw( (u32)( verts.size() * sizeof( VertPosNormalUV ) ), verts.data() );

		const auto bufIndices = ResDGBufIndex::createRaw( (u32)( indices.size() * sizeof( u32 ) ), indices.data() );

		GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/vox.xml" );
		ResourceMgr::RemResource( "config/geo/vox.xml" );

		cfg->m_vertexBuf = bufVerts;
		cfg->m_indexBuf = bufIndices;


		cb::Mat3 mat( cb::Mat3::eIdentity );

		cb::Vec3 pos( 0, 0, 0 );
		cb::Frame3 frame( mat, pos );


		GeoDiligentPtr geo = GeoDiligentPtr( new GeoDiligent( ent::EntityId::makeNext(), cfg ) );

		DGRenderer::Inst().addStaticGeo( frame, geo );
		*/

		return (i32)pVerts->size();
	}

};

//*/

void vox::CubitArr::genGeo( Plane<Cubit> *pPlane, const CPos pos, std::vector<VertPosNormalUV> *pVerts, std::vector<u32> *pIndices )
{
	const auto pGeo = new ChunkMesh<Cubit>();

	const auto worldPosInt = GPos::from( pos );

	const auto worldPosRaw = cb::Vec3( (f32)worldPosInt.x, (f32)worldPosInt.y, (f32)worldPosInt.z );

	const auto worldPosScaled = cb::Vec3(
		worldPosRaw.x * FramePlane<Cubit>::m_scaleFactor.x,
		worldPosRaw.y * FramePlane<Cubit>::m_scaleFactor.y,
		worldPosRaw.z * FramePlane<Cubit>::m_scaleFactor.z );

	const auto worldPos = worldPosScaled + FramePlane<Cubit>::m_translation;

	const auto vertCount = pGeo->fill( pPlane, pVerts, pIndices, this, pos, worldPos, FramePlane<Cubit>::m_scaleFactor );

	if( vertCount == 0 )
		return;
}

void vox::CubitArr::genCollision( Plane<Cubit> *pPlane, CPos pos )
{
	//Placeholder.  Do nothing for now
}

struct PosChunk
{
	vox::Cubit::CPos	Pos;
	vox::Cubit::Ptr		Chunk;
};


/*
static i32 s_curX = 1;
static i32 s_curY = 1;

#ifdef DEBUG
static i32 s_curYExtent = 16;
static i32 s_maxX = 50;
static i32 s_maxY = 50;
#else
static i32 s_curYExtent = 30;
static i32 s_maxX = 100;
static i32 s_maxY = 100;
#endif
*/

static std::vector<vox::Cubit::CPos> s_chunksToMake;
#ifdef DEBUG
static i32 s_chunksPerTick = 1;
static i32 s_maxX = 30;
static i32 s_maxY = 30;
#else
static i32 s_chunksPerTick = 30;
static i32 s_maxX = 100;
static i32 s_maxY = 100;
#endif



bool vox::CubitPlane::genWorld( const cb::Vec3 pos )
{

	if( s_chunksPerTick == 0 ) return false;

	const auto gPos = from( pos );

	const auto v = vox::Cubit::CPos::from( gPos );

	const cb::Vec3i startingChunkPos( v.x, v.y, v.z );



	if( !s_chunksToMake.size() )
	{
		const i32 randStart = rand();
		const f32 fShift = (f32)randStart;

		s_shiftValue = fShift;

		for( i32 y = s_maxY; y > 0; --y )
		{
			for( i32 x = s_maxX; x > 0; --x )
			{
				for( i32 z = 12; z > 0; --z )
				{
					auto cbChunkPos = startingChunkPos;

					cbChunkPos += cb::Vec3i( x, y, z );

					const vox::Cubit::CPos chunkPos( cbChunkPos.x, cbChunkPos.y, cbChunkPos.z );

					s_chunksToMake.push_back( chunkPos );
				}
			}
		}
	}

	std::array<std::vector<PosChunk>, 30> chunks;

	for( i32 i = 0; i < s_chunksPerTick; ++i )
	{
		for( i32 iThread = 0; iThread < 30; ++iThread )
		{
			if( !s_chunksToMake.empty() )
			{
				const auto chunkPos = s_chunksToMake.back();
				chunks[iThread].push_back( { chunkPos, nullptr } );
				s_chunksToMake.pop_back();
			}
		}
	}

	if( s_chunksToMake.empty() )
	{
		s_chunksPerTick = 0;
	}

	const auto translation = m_translation;
	const auto scaleFactor = m_scaleFactor;

	enki::TaskSet task( 30,
		[&chunks, translation, scaleFactor]( enki::TaskSetPartition range, uint32_t threadnum ) {

			for( auto iRange = range.start; iRange < range.end; ++iRange )
			{
				std::vector<PosChunk> &myList = chunks[iRange];

				for( auto &posChunk : myList )
				{
					auto cbChunkPos = posChunk.Pos;

					//cbChunkPos += cb::Vec3i( x, y, z );

					const vox::Cubit::CPos chunkPos( cbChunkPos.x, cbChunkPos.y, cbChunkPos.z );

					const auto cubit = TChunk::Ptr( new CubitArr( chunkPos ) );

					//cubit->m_translation = translation;
					//cubit->m_scaleFactor = scaleFactor;

					const bool hasValue = cubit->genWorld( nullptr, chunkPos );

					//const auto hashFn = std::hash<CPos>();
					//const auto hash = hashFn( chunkPos );

					if( hasValue )
					{
						posChunk.Chunk = cubit;
					}

					//chunks[threadnum].push_back( { chunkPos, cubit } );
				}
			}
		} );

	PhamApp::Info().Task.AddTaskSetToPipe( &task );
	PhamApp::Info().Task.WaitforTask( &task );

	for( auto &vecChunks : chunks )
	{
		for( auto pair : vecChunks )
		{
			if( pair.Chunk )
			{
				m_sparse[pair.Pos] = pair.Chunk;
				m_generateGeo[pair.Pos] = pair.Chunk;
			}
		}
	}

	return true;

#if 0 
	for( i32 z = 1; z < 14; ++z )
	{
		for( i32 y = s_curY; y < s_curY + s_curYExtent; ++y )
		{
			const i32 x = s_curX;
			{
				auto cbChunkPos = startingChunkPos;

				cbChunkPos += cb::Vec3i( x, y, z );

				const CPos chunkPos( cbChunkPos.x, cbChunkPos.y, cbChunkPos.z );

				const auto cubit = TChunk::Ptr( new CubitArr( chunkPos ) );

				cubit->genWorld( this, chunkPos );

				//const auto hashFn = std::hash<CPos>();
				//const auto hash = hashFn( chunkPos );

				chunks[1].push_back( { chunkPos, cubit } );

				//m_sparse[chunkPos] = cubit;
			}
		}
	}
#endif 


}

struct VertsIndices
{
	std::vector<VertPosNormalUV> Verts;
	std::vector<u32> Indices;
};

static i32 s_chunkCount = 0;
static i32 s_triCount = 0;

#ifdef DEBUG
static const i32 k_maxChunks = 1;
#else
static const i32 k_maxChunks = 128;
#endif

//*
bool vox::CubitPlane::genGeo( const cb::Vec3 inPos )
{
	i32 generated = 0;
	std::array<PosChunk, k_maxChunks> chunks;
	std::array<VertsIndices, k_maxChunks> vertsIndices;


	for( auto it = m_generateGeo.begin(); it != m_generateGeo.end(); ++it )
	{
		const auto chunk = it->second;

		if( !chunk->m_generated )
		{
			chunks[generated] = { it->first, it->second };
			chunk->m_generated = true;
			++generated;

			if( generated >= k_maxChunks ) break;
		}


	}

	if( !generated ) return false;

	const auto timeWorldgen = Timer<>::execution( [&]() {


		for( int i = 0; i < generated; ++i )
		{
			m_generateGeo.erase( chunks[i].Pos );

			// TODO STATIC COLLISION
			//m_genColl[chunks[i].Pos] = chunks[i].Chunk;
		}



		enki::TaskSet task( generated,
			[&chunks, this, &vertsIndices]( enki::TaskSetPartition range, uint32_t threadnum ) {
				for( u32 i = range.start; i < range.end; ++i )
				{
					chunks[i].Chunk->genGeo( this, chunks[i].Pos, &vertsIndices[i].Verts, &vertsIndices[i].Indices );
				}
			} );

		PhamApp::Info().Task.AddTaskSetToPipe( &task );
		PhamApp::Info().Task.WaitforTask( &task );




		for( auto vertIndex : vertsIndices )
		{
			if( vertIndex.Verts.size() == 0 ) continue;

			++s_chunkCount;

			const auto bufVerts = ResDGBufVertex::createRaw( (u32)( vertIndex.Verts.size() * sizeof( VertPosNormalUV ) ), vertIndex.Verts.data() );

			const auto bufIndices = ResDGBufIndex::createRaw( (u32)( vertIndex.Indices.size() * sizeof( u32 ) ), vertIndex.Indices.data() );

			GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/vox.xml" );

			cfg->m_vertexBuf = bufVerts;
			cfg->m_indexBuf = bufIndices;

			s_triCount += (i32)( vertIndex.Verts.size() >> 1 );

			cb::Mat3 mat( cb::Mat3::eIdentity );

			cb::Vec3 pos( 0, 0, 0 );
			cb::Frame3 frame( mat, pos );


			GeoDiligentPtr geo = GeoDiligentPtr( new GeoDiligent( ent::EntityId::makeNext(), cfg ) );

			ResourceMgr::RemResource( "config/geo/vox.xml" );

			DGRenderer::Inst().m_rsVoxels->add( frame, geo );
		}

		} );


	{
		const auto timeBraceF = (f32)timeWorldgen;

		lprintf( "timeWorldgen %.3f ms\n", timeBraceF / 1000.0f );
	}


	lprintf( "Generated %i chunks %i Triangles\n", s_chunkCount, s_triCount );


	return true;
}
//*/


