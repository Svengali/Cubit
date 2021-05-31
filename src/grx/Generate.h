//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

PtrFwd( ResDGBufVertex );
PtrFwd( ResDGBufIndex );

namespace grx
{
namespace gen
{


	// s is the size of the cube.
ResDGBufVertexPtr createCenteredCubeVertices( const float s );
ResDGBufVertexPtr createArbitraryCube( const cb::Vec3 size, const cb::Vec3 offset );
ResDGBufIndexPtr  createCenteredCubeIndicies(  );






}
}