#ifndef __CSMMESH_DATA_H__
#define __CSMMESH_DATA_H__

#include <vector>
#include <fstream>
#include <string>

#include "CSM.h"
#include "DrawNormal.h"

using std::vector;
using std::ifstream;
using std::string;
using std::runtime_error;


/*! \brief Crowd Simulation Model���� */
class CSMMeshData
{
public:
	typedef vector< CSMTagData > TagList;			//!< TAG�б�
	typedef vector< CSMSubMesh* > SubMeshList;		//!< SubMesh�б�
	typedef vector< CSMBoneData > BoneList;				//!< ���������б�

	/*! \brief ���캯�� */
	CSMMeshData();
	/*! \brief �������� */
	~CSMMeshData();

	/*! \brief ����CSM�ļ�,CSM=CrowdSimulationModel */
	BOOL LoadCSM( const string &fileName );

	DrawNormal *GetDrawNormal() const { return m_pDrawNormal; }

	///////////////////////////////////Get&Set Functions///////////////////////////////////////
	/*! \brief ���TAG������ */
	int GetNumTags() const { return static_cast< int >( m_vpTags.size() ); }
	/*! \brief ���CSM������֡�� */
	int GetNumFrames() const { return m_pHeader->numAnimFrames; }
	/*! \brief CSM�Ƿ�������� */
	BOOL HasAnimation() const { return m_pHeader->numAnimFrames > 1; }
	/*! \brief ��ù����� */
	int GetNumBones() const { return m_pHeader->numBones; }
	CSMBoneData GetBone( int iBone ) const { return m_boneList[iBone]; }
	/*! \brief ���SubMesh�� */
	int GetNumSubMeshes() const { return m_pHeader->numSubMesh; }
	/*! \brief ��õ�iMesh��SubMesh������ */
	string GetMeshName( int iMesh ) const { return m_vpSubMeshes[iMesh]->subMeshHeader.name; }
	/*! \brief ��õ�iMesh��SubMesh�Ķ����� */
	int GetMeshNumVtx( int iMesh ) const { return m_vpSubMeshes[iMesh]->subMeshHeader.numVertices; }
	int GetMeshNumSkin( int iMesh ) const
	{ 
		if ( m_vpSubMeshes[iMesh]->subMeshHeader.numAnimFrames > 1 ) 
		{
			return GetMeshNumVtx( iMesh );
		}
		return 0;
	}
	/*! \brief ��õ�iMesh��SubMesh������ */
	int GetMeshNumTri( int iMesh ) const { return m_vpSubMeshes[iMesh]->subMeshHeader.numFaces; }
	/*! \brief ��õ�iMesh��SubMesh�Ķ���֡�� */
	int GetMeshNumAnim( int iMesh ) const { return m_vpSubMeshes[iMesh]->subMeshHeader.numAnimFrames; }
	/*! \brief ��õ�iMesh��SubMesh������ */
	string GetMeshTexture( int iMesh ) const { return m_vpSubMeshes[iMesh]->textureFile; }
	/*! \brief ��õ�iMesh��SubMesh�ĵ�iVtx������ */
	CSMVertexData GetMeshVtx( int iMesh, int iVtx ) const { return m_vpSubMeshes[iMesh]->vVertexData[iVtx]; }
	CSMSkinData GetMeshSkin( int iMesh, int iVtx ) const { return m_vpSubMeshes[iMesh]->vSkinData[iVtx]; }
	/*! \brief ��õ�iMesh��SubMesh�ĵ�iTri���� */
	CSMTriangleData GetMeshTri( int iMesh, int iTri ) const { return m_vpSubMeshes[iMesh]->vTriangleData[iTri]; }
	/*! \brief ��õ�i��TAG */
	CSMTagData GetTag( int i ) const { return m_vpTags[i]; }
	//////////////////////////////////////////////////////////////////////////////////////////

private:
	ifstream	m_inFileCSM;					//!< .CSM�ļ�

	CSMHeader	*m_pHeader;						//!< ָ��Header��ָ��
	TagList		m_vpTags;						//!< �洢���е�Tags
	SubMeshList m_vpSubMeshes;					//!< �洢���е�SubMesh

	BoneList	m_boneList;						//!< �����������б�

	DrawNormal	*m_pDrawNormal;					//!< ���ڻ���Normal
};

#endif