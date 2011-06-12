#ifndef __CSM_MERGING_MESH_H__
#define __CSM_MERGING_MESH_H__

#include "CSMSkinMesh.h"

#include <map>
using std::map;

/*! \brief ���ģ�ͣ�һ����head(ͷ��)��upper���ϰ�����lower���°�����property�����ߣ���Щ��ģ����϶��� */
class CSMMergingMesh : public AnimatableBase
{
public:
	/*! \brief Ĭ�Ϲ��캯�� */
	CSMMergingMesh();

	/*! \brief �������� */
	~CSMMergingMesh();

	/*! \brief ģ��������һ���� */
	enum PartIdentity
	{
		LOWER,						//!< �°���
		UPPER,						//!< �ϰ���
		HEAD,						//!< ͷ��
		PROPERTY,					//!< ����
		ENTIRETY,					//!< ����
		NUM_PART_IDENTITY
	};

	/*! \brief ����ģ�͵�һ���ڵ� */
	struct MeshNode
	{
		CSMSkinMesh *pMesh;			//!< Skin Mesh
		MeshNode *pPreNode;			//!< ǰһ��Mesh�ڵ�
		int	tagID;					//!< ������Mesh��tag ID

		MeshNode()
		{
			pMesh = NULL;
			pPreNode = NULL;
			tagID = -1;
		}
	};

	typedef map< string, MeshNode > SkinMeshList;	//!< ģ���б�

	/*! \brief ��ʼ�� */
	BOOL Initialize( ID3D10Device *pd3dDevice, const string &pacName, ID3D10Effect *pEffect );

	/*! \brief ��Ⱦ */
	virtual void Render( ID3D10Device *pd3dDevice, const float time, 
		ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar );

	/*! \brief ����һ֡ */
	virtual void RenderSingleFrame( ID3D10Device *pd3dDevice, 
		ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar );

	/*! \brief ���¶��� */
	virtual void UpdateFrame();

	/*! \brief ���¾��� */
	void UpdateMatrix( int frame );

	/*! \brief ��ö������� */
	virtual int GetTotalNumVtx() const;

	/*! \brief ���UV�������� */
	virtual int GetTotalNumUV() const;

	/*! \brief ��������� */
	virtual int GetTotalNumTri() const;

private:
	/*! \brief ��������Χ�� */
	virtual void CalcEntireBox();

private:
	SkinMeshList	m_vpMeshList;			//!< CSM Skin�б�
};

#endif