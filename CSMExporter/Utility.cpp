#include "Utility.h"

BOOL operator==( const MaxVertexNormal &lh, const MaxVertexNormal &rh )
{
	if ( lh.materialID == rh.materialID &&
		 lh.smoothGroup == rh.smoothGroup )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL operator==( const MediateVertex &lh, const MediateVertex &rh )
{
	if ( lh.vtxIndex == rh.vtxIndex &&
		 lh.smoothGroup == rh.smoothGroup &&
		 UTILITY->IsFloatEqual( lh.u, rh.u ) &&
		 UTILITY->IsFloatEqual( lh.v, rh.v ) )
	{
		return TRUE;
	}
	return FALSE;
}

float Utility::FilterData( float &f )
{
	if ( fabs( f ) < EPSILON )
	{
		f = 0.0f;
	}
	return f;
}

void Utility::FilterData( MATRIX &mat )
{
	FilterData( mat._11 );
	FilterData( mat._12 );
	FilterData( mat._13 );
	FilterData( mat._14 );
	FilterData( mat._21 );
	FilterData( mat._22 );
	FilterData( mat._23 );
	FilterData( mat._24 );
	FilterData( mat._31 );
	FilterData( mat._32 );
	FilterData( mat._33 );
	FilterData( mat._34 );
	FilterData( mat._41 );
	FilterData( mat._42 );
	FilterData( mat._43 );
	FilterData( mat._44 );
}

void Utility::FilterData( Matrix3 &mat )
{
	for ( int i = 0; i < 4; i ++ )
	{
		Point3 p( mat.GetRow( i ).x, mat.GetRow( i ).y, mat.GetRow( i ).z );
		FilterData( p.x );
		FilterData( p.y );
		FilterData( p.z );

		mat.SetRow( i, p );
	}
}

void Utility::FilterData( CSMVertexData &vtx )
{
	FilterData( vtx.position[0] );
	FilterData( vtx.position[1] );
	FilterData( vtx.position[2] );
	FilterData( vtx.normal[0] );
	FilterData( vtx.normal[1] );
	FilterData( vtx.normal[2] );
	FilterData( vtx.u );
	FilterData( vtx.v );
}

void Utility::FilterData( CSMSkinData &skinData )
{
	FilterData( skinData.boneWeights[0] );
	FilterData( skinData.boneWeights[1] );
	FilterData( skinData.boneWeights[2] );
	FilterData( skinData.boneWeights[3] );
}

BOOL Utility::IsFloatEqual( const float lf, const float rf )
{
	if ( fabs( lf - rf ) < EPSILON )
	{
		return TRUE;
	}
	return FALSE;
}

/*! \brief D3DXMATRIX�ṩ�ġ�==�����������̫��ϸ����������дһ������ */
BOOL Utility::IsMatrixEqual( const MATRIX lmat, const MATRIX rmat )
{
	if ( IsFloatEqual( lmat._11, rmat._11 ) && 
		IsFloatEqual( lmat._12, rmat._12 ) &&
		IsFloatEqual( lmat._13, rmat._13 ) &&
		IsFloatEqual( lmat._14, rmat._14 ) &&
		IsFloatEqual( lmat._21, rmat._21 ) && 
		IsFloatEqual( lmat._22, rmat._22 ) &&
		IsFloatEqual( lmat._23, rmat._23 ) &&
		IsFloatEqual( lmat._24, rmat._24 ) &&
		IsFloatEqual( lmat._31, rmat._31 ) &&
		IsFloatEqual( lmat._32, rmat._32 ) &&
		IsFloatEqual( lmat._33, rmat._33 ) &&
		IsFloatEqual( lmat._34, rmat._34 ) &&
		IsFloatEqual( lmat._41, rmat._41 ) &&
		IsFloatEqual( lmat._42, rmat._42 ) &&
		IsFloatEqual( lmat._43, rmat._43 ) &&
		IsFloatEqual( lmat._44, rmat._44 ) )
	{
		return TRUE;
	}
	return FALSE;
}

/*! \param pSubMesh ��Ҫת����Mesh��ע�⣺��ʱpSubMesh������������Ѿ�ת����DX����ϵ�µ�����
	\param mat �������
*/
void Utility::TransformToWorld( CSMSubMesh *pSubMesh, MATRIX mat )
{
	for ( int i = 0; i < pSubMesh->subMeshHeader.numVertices; i ++ )
	{
		CSMVertexData &vtx = pSubMesh->vVertexData[i];

		// ����λ��
		D3DXVECTOR3 pts( vtx.position[0], vtx.position[1], vtx.position[2] );
		D3DXVec3TransformCoord( &pts, &pts, &mat );
		vtx.position[0] = pts.x;
		vtx.position[1] = pts.y;
		vtx.position[2] = pts.z;

		// ���㷨��
		D3DXVECTOR3 nml( vtx.normal[0], vtx.normal[1], vtx.normal[2] );
		D3DXVec3TransformNormal( &nml, &nml, &mat );
		vtx.normal[0] = nml.x;
		vtx.normal[1] = nml.y;
		vtx.normal[2] = nml.z;
	}

	for ( int i = 0; i < pSubMesh->subMeshHeader.numFaces; i ++ )
	{
		CSMTriangleData &tri = pSubMesh->vTriangleData[i];

		// �淨��
		D3DXVECTOR3 nml( tri.normal[0], tri.normal[1], tri.normal[2] );
		D3DXVec3TransformNormal( &nml, &nml, &mat );
		tri.normal[0] = nml.x;
		tri.normal[1] = nml.y;
		tri.normal[2] = nml.z;
	}
}

void Utility::TransformToWorld( MaxTriObjData *pMaxTriObj, MATRIX mat )
{
	for ( vector< MaxVertexData >::iterator i = pMaxTriObj->vVertexData.begin(); i != pMaxTriObj->vVertexData.end(); i ++ )
	{
		MaxVertexData &vtx = *i;

		// ����λ��
		D3DXVECTOR3 pts( vtx.position[0], vtx.position[1], vtx.position[2] );
		D3DXVec3TransformCoord( &pts, &pts, &mat );
		vtx.position[0] = pts.x;
		vtx.position[1] = pts.y;
		vtx.position[2] = pts.z;

		// ���㷨��
		for ( vector< MaxVertexNormal >::iterator j = vtx.vtxNormals.begin(); j != vtx.vtxNormals.end(); j ++ )
		{
			MaxVertexNormal &maxNor = *j;

			D3DXVECTOR3 nml( maxNor.normal[0], maxNor.normal[1], maxNor.normal[2] );
			D3DXVec3TransformNormal( &nml, &nml, &mat );
			maxNor.normal[0] = nml.x;
			maxNor.normal[1] = nml.y;
			maxNor.normal[2] = nml.z;
		}
	}

	for ( vector< MaxTriangleData >::iterator i = pMaxTriObj->vTriangleData.begin(); i != pMaxTriObj->vTriangleData.end(); i ++ )
	{
		MaxTriangleData &tri = *i;

		// �淨��
		D3DXVECTOR3 nml( tri.normal[0], tri.normal[1], tri.normal[2] );
		D3DXVec3TransformNormal( &nml, &nml, &mat );
		tri.normal[0] = nml.x;
		tri.normal[1] = nml.y;
		tri.normal[2] = nml.z;
	}
}

/*! \param matrix ��Ҫת����max����
	\return ת���õ���dx����
*/
D3DXMATRIX Utility::TransformToDXMatrix( const Matrix3 &matrix )
{
	//! ��DX��vector�任��max��vector�ľ���
	Matrix3 dx2Max;
	dx2Max.SetRow( 0,Point3( 1,0,0 ) );
	dx2Max.SetRow( 1,Point3( 0,0,1 ) );
	dx2Max.SetRow( 2,Point3( 0,1,0 ) );
	dx2Max.SetRow( 3,Point3( 0,0,0 ) );

	//! ��max��vector�任��DX��vector�ľ���
	Matrix3 max2dx = dx2Max;
	max2dx.Invert();

	/*! ����Ҫ��DX������ת��Ϊmax�����꣬������max����ϵ�еľ�����������ת����\n
	Ȼ��Ծ���ת�����������д�max����ϵ��DX����ϵ�ı任�����õ����յĽ�� */
	Matrix3 rslt = dx2Max * matrix * max2dx;
	FilterData( rslt );

	D3DXMATRIX dxMatrix;
	dxMatrix._11 = rslt.GetRow( 0 ).x;
	dxMatrix._12 = rslt.GetRow( 0 ).y;
	dxMatrix._13 = rslt.GetRow( 0 ).z;
	dxMatrix._14 = 0.0f;
	dxMatrix._21 = rslt.GetRow( 1 ).x;
	dxMatrix._22 = rslt.GetRow( 1 ).y;
	dxMatrix._23 = rslt.GetRow( 1 ).z;
	dxMatrix._24 = 0.0f;
	dxMatrix._31 = rslt.GetRow( 2 ).x;
	dxMatrix._32 = rslt.GetRow( 2 ).y;
	dxMatrix._33 = rslt.GetRow( 2 ).z;
	dxMatrix._34 = 0.0f;
	dxMatrix._41 = rslt.GetRow( 3 ).x;
	dxMatrix._42 = rslt.GetRow( 3 ).y;
	dxMatrix._43 = rslt.GetRow( 3 ).z;
	dxMatrix._44 = 1.0f;

	return dxMatrix;
}

void Utility::ComputeVertexNormals( MaxTriObjData *pMaxTriData )
{
	size_t nVtx = pMaxTriData->vVertexData.size();
	size_t nTri = pMaxTriData->vTriangleData.size();

	// �������еĶ���
	for ( size_t i = 0; i < nVtx; i ++ )
	{
		MaxVertexData &vtx = pMaxTriData->vVertexData[ i ];

		// �������е������Σ��ҵ������ö���������εķ���
		for ( size_t j = 0; j < nTri; j ++ )
		{
			MaxTriangleData &tri = pMaxTriData->vTriangleData[ j ];

			for ( int k = 0; k < 3; k ++ )
			{
				if ( tri.vertexIndices[k] == i )
				{
					Point3 n = Point3( tri.normal );
					n.Normalize();

					vector< MaxVertexNormal >::iterator iter;
					for (  iter = vtx.vtxNormals.begin(); iter != vtx.vtxNormals.end(); iter ++ )
					{
						if ( iter->materialID == tri.materialID )
						{
							break;
						}
					}

					if ( iter != vtx.vtxNormals.end() )
					{
						iter->normal = iter->normal + n;
					}
					else
					{
						// �½�һ��normal
						MaxVertexNormal maxNor;
						maxNor.materialID = tri.materialID;
						maxNor.normal = maxNor.normal + n;
					}

					break;
				}
			}
		}

		for ( vector< MaxVertexNormal >::iterator iter = vtx.vtxNormals.begin(); iter != vtx.vtxNormals.end(); iter ++ )
		{
			iter->normal.Normalize();
		}
	}
}

void Utility::ComputeVertexNormalsOpt( MaxTriObjData *pMaxTriData )
{
	size_t nVtx = pMaxTriData->vVertexData.size();
	size_t nTri = pMaxTriData->vTriangleData.size();

	// �������еĶ���
	for ( size_t i = 0; i < nVtx; i ++ )
	{
		MaxVertexData &vtx = pMaxTriData->vVertexData[ i ];

		// �������е������Σ��ҵ������ö���������εķ���
		for ( size_t j = 0; j < nTri; j ++ )
		{
			MaxTriangleData &tri = pMaxTriData->vTriangleData[ j ];

			for ( int k = 0; k < 3; k ++ )
			{
				if ( tri.vertexIndices[k] == i )
				{
					Point3 n = Point3( tri.normal );
					n = n.Normalize();

					vector< MaxVertexNormal >::iterator iter;
					for (  iter = vtx.vtxNormals.begin(); iter != vtx.vtxNormals.end(); iter ++ )
					{
						if ( iter->materialID == tri.materialID &&
							 iter->smoothGroup == tri.smoothGroup )
						{
							break;
						}
					}

					if ( iter != vtx.vtxNormals.end() )
					{
						iter->normal = iter->normal + n;
					}
					else
					{
						// �½�һ��normal
						MaxVertexNormal maxNor;
						maxNor.materialID = tri.materialID;
						maxNor.smoothGroup = tri.smoothGroup;
						maxNor.normal = maxNor.normal + n;

						vtx.vtxNormals.push_back( maxNor );
					}

					break;
				}
			}
		}

		for ( vector< MaxVertexNormal >::iterator iter = vtx.vtxNormals.begin(); iter != vtx.vtxNormals.end(); iter ++ )
		{
			iter->normal = iter->normal.Normalize();
		}
	}
}

void Utility::MaxDataToCSMData( MaxTriObjData *pMaxObjData )
{
	vector< MediateGroup > vVtxGroup;
	for ( int i = 0; i < pMaxObjData->numTextures; i ++ )
	{
		MediateGroup vg;
		vg.iMeshID = i;;
		vVtxGroup.push_back( vg );
	}

	//! �������е���������
	size_t nTris = pMaxObjData->vTriangleData.size();
	for ( size_t i = 0; i < nTris; i++ )
	{
		MaxTriangleData maxTri = pMaxObjData->vTriangleData[i];
		CSMTriangleData csmTri;

		// ���ݸ����������Material ID�����ID����Ӧ��VertexGroup�Ķ����б���������б�
		int mtlID = maxTri.materialID;
		MedVertexList &medVertices = vVtxGroup[mtlID].vVertices;
		vector< CSMTriangleData > &medTriangles = vVtxGroup[mtlID].vTriangles;

		int smGroup = maxTri.smoothGroup;

		//! ���������ε��������㣨������
		for ( int j = 0; j < 3; j ++ )
		{
			int vtxID = maxTri.vertexIndices[j];
			MaxVertexData maxVtx = pMaxObjData->vVertexData[vtxID];
			int texID = maxTri.texCoordIndices[j];
			MaxTexCoordData maxTex = pMaxObjData->vTexCoordData[texID];

			MediateVertex medVtx;
			medVtx.vtxIndex = vtxID;
			medVtx.smoothGroup = smGroup;
			medVtx.u = maxTex.u;
			medVtx.v = maxTex.v;

			// �ڶ����б�����ѰmedVtx������vtxIndex,smoothGroup,u��v��ֵ��
			MedVertexIter iter = find( medVertices.begin(), medVertices.end(), medVtx );

			if ( iter != medVertices.end() )
			{
				// ���б����Ѿ�����
				csmTri.vertexIndices[j] = static_cast< int >( iter - medVertices.begin() );
			}
			else
			{
				// ���û�У���Ҫ��Ӷ���
				for ( int k = 0; k < 4; k ++ )
				{
					medVtx.bones[k] = maxVtx.bones[k];
					medVtx.boneWeights[k] = maxVtx.boneWeights[k];
				}
				for ( int k = 0; k < 3; k ++ )
				{
					medVtx.pos[k] = maxVtx.position[k];
				}

				// Ѱ�Ҷ�Ӧ�ķ���
				MaxVertexNormal tempNor;
				tempNor.materialID = mtlID;
				tempNor.smoothGroup = smGroup;
				vector< MaxVertexNormal >::iterator iterNor = find( maxVtx.vtxNormals.begin(), maxVtx.vtxNormals.end(), tempNor );
				if ( iterNor != maxVtx.vtxNormals.end() )
				{
					medVtx.normal[0] = iterNor->normal.x;
					medVtx.normal[1] = iterNor->normal.y;
					medVtx.normal[2] = iterNor->normal.z;
				}
				else
				{
					assert( FALSE &&
						"Ѱ�Ҳ�����Ӧ�Ķ��㷨��" );
				}

				medVertices.push_back( medVtx );

				csmTri.vertexIndices[j] = static_cast< int >( medVertices.size() - 1 );
			}
		}

		csmTri.normal[0] = maxTri.normal[0];
		csmTri.normal[1] = maxTri.normal[1];
		csmTri.normal[2] = maxTri.normal[2];
		// ��������ӵ��������б���
		medTriangles.push_back( csmTri );
	}

	//! ���ˣ�ԭMaxObjData������������ָ����һ������Sub Mesh
	//! ���ڸ�������ת��������ݴ���Sub Mesh
	for ( vector< MediateGroup >::iterator i = vVtxGroup.begin(); i != vVtxGroup.end(); i ++ )
	{
		int numVtx = static_cast< int >( i->vVertices.size() );
		int numTri = static_cast< int >( i->vTriangles.size() );

		// �п���һ��ģ�Ͱ���һ��������ȴû��һ������ʹ����������
		if ( numVtx == 0 && numTri == 0 )
		{
			continue;
		}

		char suffix = 'a' + i->iMeshID;
		string strSubMeshName = pMaxObjData->objName + suffix;
		string strTexName = pMaxObjData->vTexNames[i->iMeshID];
		char subMeshName[ MAX_STRING_LENGTH ], texName[ MAX_STRING_LENGTH ];
		strcpy( subMeshName, strSubMeshName.c_str() );
		strcpy( texName, strTexName.c_str() );

		// ������Sub Mesh
		CSMSubMesh *pSubMesh = new CSMSubMesh( subMeshName, texName, pMaxObjData->numAnimFrames, numVtx, numTri );

		// Sub Mesh�Ķ�������
		for ( int j = 0; j < numVtx; j ++ )
		{
			MediateVertex medVtx = i->vVertices[j];
			CSMVertexData csmVtx;
			memcpy( csmVtx.position, medVtx.pos, sizeof( float ) * 3 );
			memcpy( csmVtx.normal, medVtx.normal, sizeof( float ) * 3 );
			csmVtx.u = medVtx.u;
			csmVtx.v = medVtx.v;

			pSubMesh->vVertexData.push_back( csmVtx );
		}

		// Sub Mesh����Ƥ����
		if ( pMaxObjData->numAnimFrames > 1 )
		{
			for ( int j = 0; j < numVtx; j ++ )
			{
				MediateVertex medVtx = i->vVertices[j];
				CSMSkinData csmSkin;
				memcpy( csmSkin.bones, medVtx.bones, sizeof( int ) * 4 );
				memcpy( csmSkin.boneWeights, medVtx.boneWeights, sizeof( float ) * 4 );

				pSubMesh->vSkinData.push_back( csmSkin );
			}
		}

		// Sub Mesh������������
		for ( int j = 0; j < numTri; j ++ )
		{
			CSMTriangleData csmTri = i->vTriangles[j];

			pSubMesh->vTriangleData.push_back( csmTri );
		}

		//���Sub Mesh��MaxTriObjData���б�
		pMaxObjData->vSubMeshes.push_back( pSubMesh );
	}
}