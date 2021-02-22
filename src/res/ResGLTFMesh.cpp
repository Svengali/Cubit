//////////////////////////////////////////////////////////////////////////////
//
//   P h a m 
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

#if 0 

#include "./ResXMesh.h"
#include "./ResXEffect.h"
#include "./ResXTex.h"

#include "../DXUT/Optional/SDKMisc.h"

//Forward
HRESULT OptimizeMeshData( LPD3DXMESH pMeshSysMem, const int numMaterials, LPD3DXBUFFER pAdjacencyBuffer, DWORD dwOptFlags, SMeshData *pMeshData );
HRESULT UpdateLocalMeshes( IDirect3DDevice9 *pd3dDevice, SMeshData *pMeshData );


ResourcePtr ResXMesh::Create( const char * const pFilename, const Symbol &type )
{
	ResXMeshPtr resXMesh( new ResXMesh() );
	
	resXMesh->Load( pFilename );

	return resXMesh;
}

ResXMesh::~ResXMesh(void)
{
	/*
	if( m_pD3DMesh != NULL )
	{
		m_pD3DMesh->Release();
	}
	*/
	
	m_meshData.ReleaseAll();
}

void ResXMesh::Load( const char * const pFilename )
{
	//ASSERT( m_pD3DMesh == NULL );

	LPD3DXBUFFER adjBuff;		
	LPD3DXMESH   pMeshSysMem = NULL;      // System memory copy of mesh

	if( strcmp( "<Sphere>", pFilename ) != 0 )
	{
		LoadMeshData( DXUTGetD3D9Device(), pFilename, &pMeshSysMem, &adjBuff );
	}
	else
	{
		D3DXCreateSphere( DXUTGetD3D9Device(), 1.0f, 10, 10, &pMeshSysMem, NULL );
	}
	
	OptimizeMeshData( pMeshSysMem, 1, adjBuff, D3DXMESHOPT_VERTEXCACHE, &m_meshData );
	
	UpdateLocalMeshes( DXUTGetD3D9Device(), &m_meshData );
}



void GetPathPart( const char * const pPath, char * const pBuf, const size_t bufLen )
{
	size_t index = strlen( pPath ) - 1;
	
	while( index > 0 && pPath[ index ] != '/' && pPath[ index ] != '\\' )
	{
		--index;
	}
	
	if( index > 0 )
	{
		++index;
		
		for( size_t i=0; i<index && i<bufLen; ++i )
		{
			pBuf[ index ] = pPath[ index ];
		}
		
		pBuf[ index ] = 0;
	}
}

HRESULT ResXMesh::LoadMeshData( IDirect3DDevice9 *pd3dDevice, const char * const pFilename, LPD3DXMESH *pMeshSysMemLoaded, LPD3DXBUFFER *ppAdjacencyBuffer )
{
	LPDIRECT3DVERTEXBUFFER9 pMeshVB = NULL;
	//LPD3DXBUFFER pD3DXMtrlBuffer = NULL;
	LPD3DXBUFFER pD3DXEffectBuffer = NULL;
	void*        pVertices;
	WCHAR        strMesh[512];
	HRESULT      hr = S_OK;
	LPD3DXMESH   pMeshSysMem = NULL;
	LPD3DXMESH   pMeshTemp;
	//D3DXMATERIAL* d3dxMaterials;
	
	D3DXEFFECTINSTANCE *effects;
	
	WCHAR wszMeshFile[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, pFilename, -1, wszMeshFile, MAX_PATH );
	wszMeshFile[MAX_PATH - 1] = L'\0';

	char path[ MAX_PATH ];

	GetPathPart( pFilename, path, MAX_PATH );

	// Get a path to the media file
	if( FAILED( hr = DXUTFindDXSDKMediaFileCch( strMesh, 512, wszMeshFile ) ) )
			goto End;

	// Load the mesh from the specified file
	hr = D3DXLoadMeshFromX( strMesh, D3DXMESH_SYSTEMMEM, pd3dDevice, 
													ppAdjacencyBuffer, NULL, &pD3DXEffectBuffer,
													NULL, &pMeshSysMem );
	if( FAILED(hr) )
			goto End;


	// Get the array of materials out of the returned buffer, and allocate a texture array
	effects = (D3DXEFFECTINSTANCE*)pD3DXEffectBuffer->GetBufferPointer();

	m_effectInst = *effects;

	m_effect = ResourceMgr::GetResource<ResXEffect>( m_effectInst.pEffectFilename );

	if( m_effect == NULL )
	{
		//gLog::Error( "Error loading effect %s.\n", m_effectInst.pEffectFilename );
	}
	
	for( size_t i=0; i<m_effectInst.NumDefaults; ++i )
	{
		const D3DXEFFECTDEFAULT * const pEffectDef = &m_effectInst.pDefaults[ i ];
		
		D3DXPARAMETER_DESC paramDesc;
		
		m_effect->GetD3DEffect()->GetParameterDesc( pEffectDef->pParamName, &paramDesc );
		
		if( paramDesc.Class == D3DXPC_OBJECT && paramDesc.Type == D3DXPT_TEXTURE )
		{
			ResXTexPtr res = ResourceMgr::GetResource<ResXTex>( (const char *)pEffectDef->pValue );
			
			if( res != NULL )
			{
				m_tex.push_back( res );
			}
		}
	}	



    // Lock the vertex buffer, to generate a simple bounding sphere
    hr = pMeshSysMem->GetVertexBuffer( &pMeshVB );
    if( SUCCEEDED(hr) )
    {
        hr = pMeshVB->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
        if( SUCCEEDED(hr) )
        {
            D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, pMeshSysMem->GetNumVertices(),
                                       D3DXGetFVFVertexSize( pMeshSysMem->GetFVF() ),
                                       &m_vObjectCenter, &m_fObjectRadius );
            pMeshVB->Unlock();
        }
        pMeshVB->Release();
    } else
        goto End;

    // remember if there were normals in the file, before possible clone operation
    bool bNormalsInFile = ( pMeshSysMem->GetFVF() & D3DFVF_NORMAL ) != 0;

	unsigned int fvf = pMeshSysMem->GetFVF();

    // if using 32byte vertices, check fvf
   
    if( fvf == 0 )
    {
        // force 32 byte vertices
        if( pMeshSysMem->GetFVF() != (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) )
        {
            hr = pMeshSysMem->CloneMeshFVF( pMeshSysMem->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1, 
                                            pd3dDevice, &pMeshTemp );
            if( FAILED(hr) )
                goto End;

            pMeshSysMem->Release();
            pMeshSysMem = pMeshTemp;
        }
    }
    // otherwise, just make sure that there are normals in mesh
    else if ( !(pMeshSysMem->GetFVF() & D3DFVF_NORMAL) )
    {
        hr = pMeshSysMem->CloneMeshFVF( pMeshSysMem->GetOptions(), pMeshSysMem->GetFVF() | D3DFVF_NORMAL, 
                                        pd3dDevice, &pMeshTemp );
        if (FAILED(hr))
            goto End;

        pMeshSysMem->Release();
        pMeshSysMem = pMeshTemp;
    }

    // Compute normals for the mesh, if not present
    if( !bNormalsInFile )
        D3DXComputeNormals( pMeshSysMem, NULL );

    *pMeshSysMemLoaded = pMeshSysMem;
    pMeshSysMem = NULL;

End:
    SAFE_RELEASE( pMeshSysMem );

    return hr;
}


HRESULT OptimizeMeshData( LPD3DXMESH pMeshSysMem, const int numMaterials, LPD3DXBUFFER pAdjacencyBuffer, DWORD dwOptFlags, SMeshData *pMeshData )
{
    HRESULT hr = S_OK;
    LPD3DXBUFFER pbufTemp = NULL;

    // Attribute sort - the un-optimized mesh option
    // remember the adjacency for the vertex cache optimization
    hr = pMeshSysMem->Optimize( dwOptFlags | D3DXMESH_SYSTEMMEM,
                                 (DWORD*)pAdjacencyBuffer->GetBufferPointer(),
                                 NULL, NULL, NULL, &pMeshData->m_pMeshSysMem );
    if( FAILED(hr) )
        goto End;

    pMeshData->m_cStripDatas = numMaterials;
    pMeshData->m_rgStripData = new SStripData[ pMeshData->m_cStripDatas ];
    if( pMeshData->m_rgStripData == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto End;
    }

    bool bCantDoSingleStrip = false;
    for( DWORD iMaterial = 0; iMaterial < numMaterials; iMaterial++ )
    {
        hr = D3DXConvertMeshSubsetToSingleStrip( pMeshData->m_pMeshSysMem, iMaterial,
                                D3DXMESH_IB_MANAGED, &pMeshData->m_rgStripData[iMaterial].m_pStrips,
                                &pMeshData->m_rgStripData[iMaterial].m_cStripIndices);
        if( FAILED(hr) )
            goto End;

        UINT primCount = pMeshData->m_rgStripData[iMaterial].m_cStripIndices - 2;

        IDirect3DDevice9 *pd3dDevice;
        D3DCAPS9 d3dCaps;
        pMeshSysMem->GetDevice( &pd3dDevice );
        pd3dDevice->GetDeviceCaps( &d3dCaps );
        SAFE_RELEASE( pd3dDevice );
        if( primCount > d3dCaps.MaxPrimitiveCount )
        {
            bCantDoSingleStrip = true;
        }

        hr = D3DXConvertMeshSubsetToStrips( pMeshData->m_pMeshSysMem, iMaterial, 
                                D3DXMESH_IB_MANAGED, &pMeshData->m_rgStripData[iMaterial].m_pStripsMany, 
                                NULL, &pbufTemp, &pMeshData->m_rgStripData[iMaterial].m_cStrips);
        if( FAILED(hr) )
            goto End;

        pMeshData->m_rgStripData[iMaterial].m_rgcStripLengths = new DWORD[pMeshData->m_rgStripData[iMaterial].m_cStrips];
        if( pMeshData->m_rgStripData[iMaterial].m_rgcStripLengths == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto End;
        }
        memcpy( pMeshData->m_rgStripData[iMaterial].m_rgcStripLengths,
                pbufTemp->GetBufferPointer(),
                sizeof(DWORD) * pMeshData->m_rgStripData[iMaterial].m_cStrips );
    }

End:
    SAFE_RELEASE( pbufTemp );

    return hr;
}


HRESULT UpdateLocalMeshes( IDirect3DDevice9 *pd3dDevice, SMeshData *pMeshData )
{
    HRESULT hr = S_OK;

    // if a mesh was loaded, update the local meshes
    if( pMeshData->m_pMeshSysMem != NULL )
    {
        hr = pMeshData->m_pMeshSysMem->CloneMeshFVF( /*g_dwMemoryOptions |*/ D3DXMESH_VB_WRITEONLY, pMeshData->m_pMeshSysMem->GetFVF(),
                                          pd3dDevice, &pMeshData->m_pMesh );
        if( FAILED(hr) )
            return hr;

        hr = pMeshData->m_pMesh->GetVertexBuffer( &pMeshData->m_pVertexBuffer );
        if( FAILED(hr) )
            return hr;
    }

    return hr;
}

/*

HRESULT ResXMesh::LoadMeshData( IDirect3DDevice9 *pd3dDevice, const char * const pFilename, LPD3DXMESH *pMeshSysMemLoaded, LPD3DXBUFFER *ppAdjacencyBuffer )
{
    LPDIRECT3DVERTEXBUFFER9 pMeshVB = NULL;
    //LPD3DXBUFFER pD3DXMtrlBuffer = NULL;
    LPD3DXBUFFER pD3DXEffectBuffer = NULL;
    void*        pVertices;
    //WCHAR        strMesh[512];
    HRESULT      hr = S_OK;
    LPD3DXMESH   pMeshSysMem = NULL;
    LPD3DXMESH   pMeshTemp;
    //D3DXMATERIAL* d3dxMaterials;
	D3DXEFFECTINSTANCE *effects;

	WCHAR wszMeshFile[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, pFilename, -1, wszMeshFile, MAX_PATH );
	wszMeshFile[MAX_PATH - 1] = L'\0';

	char path[ MAX_PATH ];

	GetPathPart( pFilename, path, MAX_PATH );

    // Get a path to the media file
    //if( FAILED( hr = DXUTFindDXSDKMediaFileCch( strMesh, 512, wszMeshFile ) ) )
     //   goto End;

    // Load the mesh from the specified file
    hr = D3DXLoadMeshFromX( wszMeshFile, D3DXMESH_SYSTEMMEM, pd3dDevice, 
                            ppAdjacencyBuffer, NULL, &pD3DXEffectBuffer,
                            NULL, &pMeshSysMem );
    if( FAILED(hr) )
        goto End;
            

    // Get the array of materials out of the returned buffer, and allocate a texture array
    effects = (D3DXEFFECTINSTANCE*)pD3DXEffectBuffer->GetBufferPointer();
    
    m_effectInst = *effects;
    
    m_effect = ResourceMgr::GetResource<ResXEffect>( m_effectInst.pEffectFilename );

	
	for( size_t i=0; i<m_effectInst.NumDefaults; ++i )
	{
		const D3DXEFFECTDEFAULT * const pEffectDef = &m_effectInst.pDefaults[ i ];
		
		D3DXPARAMETER_DESC paramDesc;
		
		m_effect->GetD3DEffect()->GetParameterDesc( pEffectDef->pParamName, &paramDesc );
		
		if( paramDesc.Class == D3DXPC_OBJECT && paramDesc.Type == D3DXPT_TEXTURE )
		{
			ResXTexPtr res = ResourceMgr::GetResource<ResXTex>( (const char *)pEffectDef->pValue );
			
			if( res != NULL )
			{
				m_tex.push_back( res );
			}
		}
	}	
	
    // Lock the vertex buffer, to generate a simple bounding sphere
    hr = pMeshSysMem->GetVertexBuffer( &pMeshVB );
    if( SUCCEEDED(hr) )
    {
        hr = pMeshVB->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
        if( SUCCEEDED(hr) )
        {
            D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, pMeshSysMem->GetNumVertices(),
                                       D3DXGetFVFVertexSize( pMeshSysMem->GetFVF() ),
                                       &m_vObjectCenter, &m_fObjectRadius );
            pMeshVB->Unlock();
        }
        pMeshVB->Release();
    } else
        goto End;

    // remember if there were normals in the file, before possible clone operation
    bool bNormalsInFile = ( pMeshSysMem->GetFVF() & D3DFVF_NORMAL ) != 0;

	/*
    // if using 32byte vertices, check fvf
    if( g_bForce32ByteFVF )
    {
        // force 32 byte vertices
        if( pMeshSysMem->GetFVF() != (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) )
        {
            hr = pMeshSysMem->CloneMeshFVF( pMeshSysMem->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1, 
                                            pd3dDevice, &pMeshTemp );
            if( FAILED(hr) )
                goto End;

            pMeshSysMem->Release();
            pMeshSysMem = pMeshTemp;
        }
    }
    // otherwise, just make sure that there are normals in mesh
    else * /if ( !(pMeshSysMem->GetFVF() & D3DFVF_NORMAL) )
    {
        hr = pMeshSysMem->CloneMeshFVF( pMeshSysMem->GetOptions(), pMeshSysMem->GetFVF() | D3DFVF_NORMAL, 
                                        pd3dDevice, &pMeshTemp );
        if (FAILED(hr))
            goto End;

        pMeshSysMem->Release();
        pMeshSysMem = pMeshTemp;
    }

    // Compute normals for the mesh, if not present
    if( !bNormalsInFile )
        D3DXComputeNormals( pMeshSysMem, NULL );

    *pMeshSysMemLoaded = pMeshSysMem;
    pMeshSysMem = NULL;

End:
    SAFE_RELEASE( pMeshSysMem );

    return hr;
}
*/

#endif
