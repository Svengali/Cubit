//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez



#include "../stdafx.h"

#include "./DGRenderer.h"

SERIALIZABLE( DGRenderer );

static DGRenderer s_renderer;

DGRenderer &DGRenderer::Inst()
{
	return s_renderer;
}

void DGRenderer::startup()
{
	Renderer::SetupSingleton( &s_renderer );
}

void DGRenderer::shutdown()
{

}



DGRenderer::DGRenderer()
{
}


DGRenderer::~DGRenderer(void)
{
}

