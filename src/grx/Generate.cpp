//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez



#include "../stdafx.h"

#include "./Generate.h"

#include "res/ResDGLayout.h"
#include "res/ResDGBuffer.h"



ResDGBufVertexPtr grx::gen::createCenteredCubeVertices( const float s )
{

  // Cube vertices

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

  // clang-format off
  // This time we have to duplicate verices because texture coordinates cannot
  // be shared


  const cb::Vec3 v1 ( -1, -1, -1 );
  const cb::Vec3 v2 { -1, -1, -1 };


  dg::float3 ft { -1, -1, -1 };


  VertPosUV verPos1 ={ cb::Vec3 { -1,-1,-1 }, cb::Vec2{ 0,1 } };
  //VertPosUV verPos2 ={ { -1,-1,-1 }, { 0,1 } };


  VertPosUV CubeVerts[] =
  {
      {cb::Vec3( -s,-s,-s ), cb::Vec2( 0,1 )},
      {cb::Vec3( -s,+s,-s ), cb::Vec2( 0,0 )},
      {cb::Vec3( +s,+s,-s ), cb::Vec2( 1,0 )},
      {cb::Vec3( +s,-s,-s ), cb::Vec2( 1,1 )},

      {cb::Vec3( -s,-s,-s ), cb::Vec2( 0,1 )},
      {cb::Vec3( -s,-s,+s ), cb::Vec2( 0,0 )},
      {cb::Vec3( +s,-s,+s ), cb::Vec2( 1,0 )},
      {cb::Vec3( +s,-s,-s ), cb::Vec2( 1,1 )},

      {cb::Vec3( +s,-s,-s ), cb::Vec2( 0,1 )},
      {cb::Vec3( +s,-s,+s ), cb::Vec2( 1,1 )},
      {cb::Vec3( +s,+s,+s ), cb::Vec2( 1,0 )},
      {cb::Vec3( +s,+s,-s ), cb::Vec2( 0,0 )},

      {cb::Vec3( +s,+s,-s ), cb::Vec2( 0,1 )},
      {cb::Vec3( +s,+s,+s ), cb::Vec2( 0,0 )},
      {cb::Vec3( -s,+s,+s ), cb::Vec2( 1,0 )},
      {cb::Vec3( -s,+s,-s ), cb::Vec2( 1,1 )},

      {cb::Vec3( -s,+s,-s ), cb::Vec2( 1,0 )},
      {cb::Vec3( -s,+s,+s ), cb::Vec2( 0,0 )},
      {cb::Vec3( -s,-s,+s ), cb::Vec2( 0,1 )},
      {cb::Vec3( -s,-s,-s ), cb::Vec2( 1,1 )},

      {cb::Vec3( -s,-s,+s ), cb::Vec2( 1,1 )},
      {cb::Vec3( +s,-s,+s ), cb::Vec2( 0,1 )},
      {cb::Vec3( +s,+s,+s ), cb::Vec2( 0,0 )},
      {cb::Vec3( -s,+s,+s ), cb::Vec2( 1,0 )}
  };
  // clang-format on

  dg::BufferDesc VertBuffDesc;
  VertBuffDesc.Name          = "Cube vertex buffer";
  VertBuffDesc.Usage         = dg::USAGE_STATIC;
  VertBuffDesc.BindFlags     = dg::BIND_VERTEX_BUFFER;
  VertBuffDesc.uiSizeInBytes = sizeof( CubeVerts );

  dg::BufferData VBData;
  VBData.pData    = CubeVerts;
  VBData.DataSize = sizeof( CubeVerts );

  const auto buf = ResDGBufVertex::createRaw( sizeof( CubeVerts ), CubeVerts );

  return buf;

  /*
  dg::RefCntAutoPtr<dg::IBuffer> cubeVerticies;

  dg::App::Info().Device()->CreateBuffer( VertBuffDesc, &VBData, &cubeVerticies );
  */
}


ResDGBufIndexPtr  grx::gen::createCenteredCubeIndicies()
{
    // clang-format off
  u32 indicies[] =
  {
      2,0,1,    2,3,0,
      4,6,5,    4,7,6,
      8,10,9,   8,11,10,
      12,14,13, 12,15,14,
      16,18,17, 16,19,18,
      20,21,22, 20,22,23
  };
  // clang-format on

  dg::BufferDesc IndBuffDesc;
  IndBuffDesc.Name          = "Cube index buffer";
  IndBuffDesc.Usage         = dg::USAGE_STATIC;
  IndBuffDesc.BindFlags     = dg::BIND_INDEX_BUFFER;
  IndBuffDesc.uiSizeInBytes = sizeof( indicies );
  dg::BufferData IBData;
  IBData.pData    = indicies;
  IBData.DataSize = sizeof( indicies );

  const auto buf = ResDGBufIndex::createRaw( sizeof( indicies ), indicies );

  return buf;

  //m_pDevice->CreateBuffer( IndBuffDesc, &IBData, &m_CubeIndexBuffer );


}


