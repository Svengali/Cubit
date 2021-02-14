//////////////////////////////////////////////////////////////////////////////
//
//   P h a m 
//
// copyright 2005-2017 Marc Hernandez

#pragma once

#include "verse/verse.h"

namespace verse
{

class FacDiligent : public Factory
{
public:
	virtual GeometryPtr convert( const grx::SimpleMesh *const pMesh ) override;

};





}