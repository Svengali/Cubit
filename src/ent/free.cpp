//////////////////////////////////////////////////////////////////////////////
//
//   P h a m 
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

#include "free.h"

#include "grx/DGRenderer.h"
#include "grx/GeoDiligent.h"

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


void FreefallArr::gatherActiveBlocks( std::vector<FreefallData *> *pFreefalVec )
{
	const auto size = m_arr.m_arr.size();

	for( i32 index = 0; index < (i32)size; ++index )
	{
		if( m_arr.m_arr[index].m_count == 0 ) continue;

		pFreefalVec->push_back( &m_arr.m_arr[index].m_data );
	}


}



void FreefallPlane::updateBlocks( double dt )
{
	std::vector<FreefallData *> freeFallVec;

	for( auto [k, v] : m_sparse )
	{
		v->gatherActiveBlocks( &freeFallVec );
	}

	//lprintf( "Active blocks %i\n", (i32)freeFallVec.size() );

	auto task = enki::TaskSet( (u32)freeFallVec.size(),
		[dt, freeFallVec]( enki::TaskSetPartition range, uint32_t threadnum ) {

			for( u32 iData = range.start; iData < range.end; ++iData )
			{
				auto *pData = freeFallVec[iData];

				const auto blockCount = pData->m_com.m_blocks.m_block.size();

				for( i32 bIndex = 0; bIndex < blockCount; ++bIndex )
				{
					auto *pBlock = pData->m_com.m_blocks.m_block[bIndex].get();
					auto max = pData->m_com.m_blocks.m_allocated[bIndex];

					const auto *__restrict const pSrcFrame = pBlock->src<FreefallData::Frame, cb::Frame3>();
					const auto *__restrict const pSrcVel = pBlock->src<FreefallData::Velocity, cb::Vec3>();
					const auto *__restrict const pSrcAccel = pBlock->src<FreefallData::Accel, cb::Vec3>();


					auto *__restrict pDstFrameArr = pBlock->dst<FreefallData::Frame, cb::Frame3>();
					auto *__restrict pDstVelArr = pBlock->dst<FreefallData::Velocity, cb::Vec3>();
					auto *__restrict pDstAccelArr = pBlock->dst<FreefallData::Accel, cb::Vec3>();

					//const auto *__restrict const pSrcGeo = pBlock->src<FreefallData::Geometry, GeoDiligentPtr>();

					for( i32 i = 0; i < (i32)max; ++i )
					{
						const auto &srcFm = pSrcFrame[i];
						const auto &srcVel = pSrcVel[i];
						const auto &srcAccel = pSrcAccel[i];

						auto *pDstFm = &( pDstFrameArr[i] );
						auto *pDstVel = &( pDstVelArr[i] );
						auto *pDstAccel = &( pDstAccelArr[i] );

						const auto srcPos = srcFm.GetTranslation();

						const auto newAccel = srcAccel + cb::Vec3( 0, 0, -2.8 ) * dt;
						const auto newVel = srcVel + newAccel * dt;
						const auto newPos = srcPos + newVel * dt;

						*pDstAccel = newAccel;
						*pDstVel = newVel;
						pDstFm->SetTranslation( newPos );

						cb::Segment seg(srcPos, newPos, 0.1f);

						cb::SegmentResults res;

						PhamApp::Info().m_cubit->collide( seg, &res );

						if( res.Collided() )
						{
							//lprintf( "X" );
						}

						/*
						const auto fmVel = srcVel * (f32)dt;

						const auto fwdVel = srcFm.GetMatrix().GetColumnX() * fmVel.x;
						const auto rgtVel = srcFm.GetMatrix().GetColumnY() * fmVel.y;
						const auto uppVel = srcFm.GetMatrix().GetColumnZ() * fmVel.z;

						auto newPos = srcFm.GetTranslation() + fwdVel + rgtVel + uppVel;

						pDstFm->SetTranslation( newPos );
						*/
					}

					pBlock->swap();

				}

			}

		} );

	PhamApp::Info().Task.AddTaskSetToPipe( &task );
	PhamApp::Info().Task.WaitforTask( &task );



#if 0
	const auto size = m_arr.m_arr.size();

	for( i32 index = 0; index < (i32)size; ++index )
	{
		if( m_arr.m_arr[index].m_count == 0 ) continue;

		auto *pBlock = m_arr.m_arr[index].m_data.m_com.m_blocks.m_block[index].get();
		auto max = m_arr.m_arr[index].m_data.m_com.m_blocks.m_allocated[index];


		auto *pTask = new enki::TaskSet( size,
			[dt, pBlock, max]( enki::TaskSetPartition range, uint32_t threadnum ) {

				const auto *__restrict const pSrcFrame = pBlock->src<FreefallData::Frame, cb::Frame3>();
				const auto *__restrict const pSrcVel = pBlock->src<FreefallData::Velocity, cb::Vec3>();


				auto *__restrict pDstFrame = pBlock->dst<FreefallData::Frame, cb::Frame3>();

				//const auto *__restrict const pSrcGeo = pBlock->src<FreefallData::Geometry, GeoDiligentPtr>();

				for( i32 i = 0; i < (i32)max; ++i )
				{
					const auto &srcFm = pSrcFrame[i];
					const auto srcVel = pSrcVel[i];

					auto pDstFm = &( pDstFrame[i] );

					const auto fmVel = srcVel * (f32)dt;

					const auto fwdVel = srcFm.GetMatrix().GetRowX() * fmVel.x;
					const auto rgtVel = srcFm.GetMatrix().GetRowY() * fmVel.y;
					const auto uppVel = srcFm.GetMatrix().GetRowZ() * fmVel.z;

					auto newPos = srcFm.GetTranslation() + fwdVel + rgtVel + uppVel;

					pDstFm->SetTranslation( newPos );
				}

				pBlock->swap();

			} );

		auto taskPtr = enki::TaskSetPtr( pTask );
		PhamApp::Info().Task.AddTaskSetToPipe( pTask );
}
#endif



}
