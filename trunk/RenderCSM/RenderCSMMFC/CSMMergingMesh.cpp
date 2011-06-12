#include "stdafx.h"
#include "CSMMergingMesh.h"

CSMMergingMesh::CSMMergingMesh()
{
}

CSMMergingMesh::~CSMMergingMesh()
{
	for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		SAFE_DELETE( i->second.pMesh );
	}
}

/*! \param pd3dDevice D3D10�豸
	\param pEffect EffectЧ���ļ�
	\return ��ʼ���ɹ���ʧ��
*/
BOOL CSMMergingMesh::Initialize( ID3D10Device *pd3dDevice, const string &pacName, ID3D10Effect *pEffect )
{
	//! ����ģ���б�
	ifstream inFile( pacName.c_str() );
	if ( !inFile.fail() )
	{
		for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
		{
			SAFE_DELETE( i->second.pMesh );
		}

		size_t token = pacName.find_last_of( '\\' );
		string path = pacName.substr( 0, token );

		int n;
		inFile >> n;
		for ( int i = 0; i < n; i ++ )
		{
			string meshType, meshName;
			inFile >> meshType >> meshName;

			meshName = path + "\\" + meshName;

			CSMSkinMesh *pMesh = new CSMSkinMesh();
			if ( FALSE == pMesh->Initialize( pd3dDevice, meshName, pEffect ) )
			{
				return FALSE;
			}

			MeshNode node;
			node.pMesh = pMesh;
			m_vpMeshList.insert( SkinMeshList::value_type( meshType, node ) );
		}
	}
	else
	{
		return FALSE;
	}

	//! ����ģ�ͼ�İ�����
	for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		MeshNode *pMeshNode = &( i->second );

		int tagNum = pMeshNode->pMesh->GetNumTagType();
		for ( int j = 0; j < tagNum; j ++ )
		{
			string tagName = pMeshNode->pMesh->GetTagTypeName( j );

			if ( tagName == "Tag_Torso" )	// ��upper�󶨵�lower
			{
				assert( i->first == "lower" );
				SkinMeshList::iterator k = m_vpMeshList.find( "upper" );
				if ( k != m_vpMeshList.end() )
				{
					k->second.pPreNode = pMeshNode;
					k->second.tagID = j;
				}
			}
			else if ( tagName == "Tag_Head" )	// ��head�󶨵�upper
			{
				assert( i->first == "upper" );
				SkinMeshList::iterator k = m_vpMeshList.find( "head" );
				if ( k != m_vpMeshList.end() )
				{
					k->second.pPreNode = pMeshNode;
					k->second.tagID = j;
				}
			}
			else if ( tagName == "Tag_Property" )	// �����߰󶨵������ĳһ��λ
			{
				SkinMeshList::iterator k = m_vpMeshList.find( "property" );
				if ( k != m_vpMeshList.end() )
				{
					k->second.pPreNode = pMeshNode;
					k->second.tagID = j;
				}
			}
		}
	}

	// ���¾���
	UpdateMatrix( 0 );

	//! ��������İ�Χ��
	CalcEntireBox();

	return TRUE;
}

/*! \param pd3dDevice D3D10�豸
	\param time ��������ʱ��
	\param worldMatVar �����������Ľӿ�
	\param texVar ��������Ľӿ�
*/
void CSMMergingMesh::Render( ID3D10Device *pd3dDevice, const float time, ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar )
{
	assert( pd3dDevice != NULL && worldMatVar != NULL && texVar != NULL );

	for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		MeshNode *pMN = &( i->second );
		pMN->pMesh->Render( pd3dDevice, time, worldMatVar, texVar );
	}
}

/*! \param pd3dDevice D3D10�豸
	\param worldMatVar �����������Ľӿ�
	\param texVar ��������Ľӿ�
*/
void CSMMergingMesh::RenderSingleFrame( ID3D10Device *pd3dDevice, ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar )
{
	assert( pd3dDevice != NULL && worldMatVar != NULL && texVar != NULL );

	for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		MeshNode *pMN = &( i->second );
		pMN->pMesh->RenderSingleFrame( pd3dDevice, worldMatVar, texVar );
	}
}

void CSMMergingMesh::UpdateFrame()
{
	//! ����ģ�ͼ�İ���Ϣ�����������
	UpdateMatrix( m_curAnimState.curr_frame );

	//! ����ģ�Ͷ���
	for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		MeshNode *pMN = &( i->second );
		pMN->pMesh->UpdateFrame();
	}
}

/*! \param frame ��ǰ֡ */
void CSMMergingMesh::UpdateMatrix( int frame )
{
	assert( frame >= 0 && frame < m_numSceneFrames && "frameԽ��" );

	for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		MeshNode *pMeshNode = &( i->second );
		D3DXMATRIX worldMatrix = m_worldMatrix;

		MeshNode *pMN = pMeshNode;
		while ( pMN->pPreNode != NULL )
		{
			CSMSkinMesh *pPreMesh = pMN->pPreNode->pMesh;
			D3DXMATRIX mountMat = pPreMesh->GetTagFrame( pMN->tagID, frame );

			D3DXMatrixMultiply( &worldMatrix, &worldMatrix, &mountMat );

			pMN = pMN->pPreNode;
		}

		pMeshNode->pMesh->SetWorldMatrix( worldMatrix );
	}
}

void CSMMergingMesh::CalcEntireBox()
{
	D3DXVECTOR3 min, max;
	memset( &min, 0, sizeof( D3DXVECTOR3 ) );
	memset( &max, 0, sizeof( D3DXVECTOR3 ) );
	for ( SkinMeshList::iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		MeshNode *pMeshNode = &( i->second );
		CSMSkinMesh *pCSM = pMeshNode->pMesh;

		D3DXVECTOR3 bMin = pCSM->GetEntireBox().boxMin;
		D3DXVECTOR3 bMax = pCSM->GetEntireBox().boxMax;
		D3DXVec3TransformCoord( &bMin, &bMin, &( pCSM->GetWorldMatrix() ) );
		D3DXVec3TransformCoord( &bMax, &bMax, &( pCSM->GetWorldMatrix() ) );
		if ( min.x > bMin.x )
		{
			min.x = bMin.x;
		}
		if ( min.y > bMin.y )
		{
			min.y = bMin.y;
		}
		if ( min.z > bMin.z )
		{
			min.z = bMin.z;
		}
		if ( max.x  < bMax.x )
		{
			max.x = bMax.x;
		}
		if ( max.y < bMax.y )
		{
			max.y = bMax.y;
		}
		if ( max.z < bMax.z )
		{
			max.z = bMax.z;
		}
	}
	m_entireBox.boxMin = min;
	m_entireBox.boxMax = max;

	CalcEntireBoxInfo();
}

int CSMMergingMesh::GetTotalNumVtx() const
{
	int n = 0;
	for ( SkinMeshList::const_iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		const MeshNode *pMeshNode = &( i->second );
		const CSMSkinMesh *pCSM = pMeshNode->pMesh;

		n += pCSM->GetTotalNumVtx();
	}
	return n;
}

int CSMMergingMesh::GetTotalNumUV() const
{
	int n = 0;
	for ( SkinMeshList::const_iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		const MeshNode *pMeshNode = &( i->second );
		const CSMSkinMesh *pCSM = pMeshNode->pMesh;

		n += pCSM->GetTotalNumUV();
	}
	return n;
}

int CSMMergingMesh::GetTotalNumTri() const
{
	int n = 0;
	for ( SkinMeshList::const_iterator i = m_vpMeshList.begin(); i != m_vpMeshList.end(); i ++ )
	{
		const MeshNode *pMeshNode = &( i->second );
		const CSMSkinMesh *pCSM = pMeshNode->pMesh;

		n += pCSM->GetTotalNumTri();
	}
	return n;
}