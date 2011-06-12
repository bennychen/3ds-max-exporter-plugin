#ifndef __CSM_SKIN_MESH_H__
#define __CSM_SKIN_MESH_H__

#include "CSMMeshData.h"
#include "AnimatableBase.h"

#include <fstream>
using std::ofstream;
using std::endl;

/*! \brief CSMģ�Ͷ����ʽ */
struct DXCSMVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;

	DXCSMVertex(){}
	DXCSMVertex( D3DXVECTOR3 p, D3DXVECTOR2 t )
	{
		pos = p;
		tex = t;
	}
};

/*! \brief CSMģ�� */
class CSMSkinMesh : public AnimatableBase
{
public:
	/*! \brief Ĭ�Ϲ��캯�� */
	CSMSkinMesh();

	/*! \brief �������� */
	~CSMSkinMesh();

	/*! \brief ��ʼ��CSM Skinģ�� */
	BOOL Initialize( ID3D10Device *pd3dDevice, const string &CSMFile, ID3D10Effect *pEffect );

	/*! \brief ����ģ�� */
	void Render( ID3D10Device *pd3dDevice, const float time, 
		         ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar );

	/*! \brief ����һ֡ */
	void RenderSingleFrame( ID3D10Device *pd3dDevice, ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar );

	/*! \brief ���Ʒ��� */
	void RenderNormal( ID3D10Device *pd3dDevice );

	/*! \brief ���¶����Ķ��� */
	void UpdateFrame();

	void OutputInfoToTXT();

	/////////////////////////////////////Get&Set Functions/////////////////////////////////////
	/*! \brief ���TAG���������� */
	int GetNumTagType() const { return m_pMeshData->GetNumTags() / m_numSceneFrames; }

	/*! \brief ��õ�i��TAG���������� */
	string GetTagTypeName( int i ) const { return m_pMeshData->GetTag( i * m_numSceneFrames ).name; }

	/*! \brief ��õ�i��TAG�ĵ�iFrame֡�İ󶨾��� */
	D3DXMATRIX GetTagFrame( int iTag, int iFrame ){ return m_pMeshData->GetTag( iTag ).vFrameData[iFrame]; }

	/*! \brief ��ö������� */
	virtual int GetTotalNumVtx() const;

	/*! \brief ���UV�������� */
	virtual int GetTotalNumUV() const;

	/*! \brief ��������� */
	virtual int GetTotalNumTri() const;
	///////////////////////////////////////////////////////////////////////////////////////////

private:
	/*! \brief ��ʼ������ */
	BOOL LoadTextures( ID3D10Device *pd3dDevice );

	/*! \brief ��ʼ�����㻺���� */
	BOOL InitVertexBuffer( ID3D10Device *pd3dDevice );

	/*! \brief ��ʼ������������ */
	BOOL InitIndexBuffer( ID3D10Device *pd3dDevice );

	/*! \brief ��ʼ��InputLayout */
	BOOL InitInputLayout( ID3D10Device *pd3dDevice, ID3D10EffectTechnique *pTechnique );

	BOOL CreateAnimationTexture( ID3D10Device *pd3dDevice, ID3D10Texture2D **ppAnimationTexture, ID3D10ShaderResourceView **ppAnimationResourceView );

	/*! \brief ��������Χ�� */
	virtual void CalcEntireBox();

private:
	static const string						TECHNIQUE_NAME;					//!< ����technique������
	ID3D10EffectTechnique					*m_pBasicTechnique;				//!< ����technique

	vector< ID3D10Buffer* >					m_vpVertexBuffers;				//!< ÿ��SubMesh��Ӧһ��VertexBuffer
	vector< ID3D10Buffer* >					m_vpIndexBuffers;				//!< ÿ��SubMesh��Ӧһ��IndexBuffer
	ID3D10InputLayout						*m_pVertexLayout;				//!< ���㲼��

	vector< ID3D10ShaderResourceView* >		m_vpTextureSRV;					//!< ÿ��SubMesh��Ӧһ��Texture
	ID3D10Texture2D							*m_pAnimationTexture;			//!< Animation Texture			
	ID3D10ShaderResourceView				*m_pAnimationResourceView;		//!< Animation Texture RV

	CSMMeshData								*m_pMeshData;					//!< CSMģ������
	string									m_filePath;						//!< CSM�ļ���
};

#endif