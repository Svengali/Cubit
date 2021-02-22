//////////////////////////////////////////////////////////////////////////////
//
//   P h a m 
//
// copyright 2005-2021 Marc Hernandez
#pragma once

#include "Resource.h"

#if 0 

#include <d3d9.h>
#include <d3dx9mesh.h>
#include "../DXUT/Core/DXUT.h"


SPtrFwd( ResXMesh   );
SPtrFwd( ResXEffect );
SPtrFwd( ResXTex    );


struct SStripData
{
    LPDIRECT3DINDEXBUFFER9  m_pStrips;          // strip indices (single strip)
    LPDIRECT3DINDEXBUFFER9  m_pStripsMany;      // strip indices (many strips)

    DWORD                   m_cStripIndices;
    DWORD                  *m_rgcStripLengths;
    DWORD                   m_cStrips;

    SStripData() :
        m_pStrips(NULL),
        m_pStripsMany(NULL),
        m_cStripIndices(0),
        m_rgcStripLengths(NULL)
    { }
};


struct SMeshData
{
    LPD3DXMESH              m_pMeshSysMem;      // System memory copy of mesh

    LPD3DXMESH              m_pMesh;            // Local version of mesh, copied on resize
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;    // vertex buffer of mesh

    SStripData             *m_rgStripData;      // strip indices split by attribute
    DWORD                   m_cStripDatas;

    SMeshData() :
        m_pMeshSysMem(NULL),
        m_pMesh(NULL),
        m_pVertexBuffer(NULL),
        m_rgStripData(NULL),
        m_cStripDatas(0)
    { }

    void ReleaseLocalMeshes()
    {
        SAFE_RELEASE( m_pMesh );
        SAFE_RELEASE( m_pVertexBuffer );
    }

    void ReleaseAll()
    {
        SAFE_RELEASE( m_pMeshSysMem );
        SAFE_RELEASE( m_pMesh );
        SAFE_RELEASE( m_pVertexBuffer );

        for( DWORD iStripData = 0; iStripData < m_cStripDatas; iStripData++ )
        {
            SAFE_RELEASE( m_rgStripData[iStripData].m_pStrips );
            SAFE_RELEASE( m_rgStripData[iStripData].m_pStripsMany );
            delete[] m_rgStripData[iStripData].m_rgcStripLengths;
        }

        delete[] m_rgStripData;
        m_rgStripData = NULL;
        m_cStripDatas = 0;
    }
};

class ResXMesh : public Resource
{
public:
	CLASS( ResXMesh, Resource );

	static ResourcePtr Create( const char * const pFilename, const Symbol &type );

	virtual ~ResXMesh();
	
	virtual void Load( const char * const pFilename );
	
	friend class GeoD3D;

	REFLECT_BEGIN( ResXMesh, Resource )
		//REFLECT( m_meshData );
		REFLECT( m_tex );
		REFLECT( m_effect );
		//REFLECT( m_effectInst );
		//REFLECT( m_dwNumMaterials );
		//REFLECT( m_pMeshMaterials );
		//REFLECT( m_vObjectCenter );
		REFLECT( m_fObjectRadius );
	REFLECT_END();
	

private:
	HRESULT LoadMeshData( IDirect3DDevice9 *pd3dDevice, const char * const pFilename, LPD3DXMESH *pMeshSysMemLoaded, LPD3DXBUFFER *ppAdjacencyBuffer );

	//ID3DXMesh *m_pD3DMesh;

	SMeshData m_meshData;

	cb::vector< ResXTexPtr > m_tex;

	ResXEffectPtr           m_effect;
	D3DXEFFECTINSTANCE      m_effectInst;	
	
	//DWORD                   m_dwNumMaterials;   // Number of materials
	//D3DMATERIAL9           *m_pMeshMaterials;
	
	D3DXVECTOR3             m_vObjectCenter;        // Center of bounding sphere of object
	FLOAT                   m_fObjectRadius;        // Radius of bounding sphere of object

	
};

#endif

