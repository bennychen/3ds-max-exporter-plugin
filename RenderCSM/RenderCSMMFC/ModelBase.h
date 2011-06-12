#ifndef __MODEL_BASE_H__
#define __MODEL_BASE_H__

#include <vector>
using std::vector;

struct BoundingBox
{
	D3DXVECTOR3 boxMin;		//!< ��Χ�е���С��
	D3DXVECTOR3 boxMax;		//!< ��Χ�е�����

	BoundingBox()
	{
		memset( &boxMin, 0, sizeof( D3DXVECTOR3 ) );
		memset( &boxMax, 0, sizeof( D3DXVECTOR3 ) );
	}
};

class ModelBase
{
public:
	ModelBase()
	{
		memset( &m_entireCenter, 0, sizeof( D3DXVECTOR3 ) );
		D3DXMatrixIdentity( &m_worldMatrix  );
	}
	virtual ~ModelBase(){};

	typedef vector< BoundingBox > BoxList;		//! ��Χ���б�

	/*! \brief ��������Χ�� */
	virtual void CalcEntireBox() = 0;

	/*! \brief �������еľֲ���Χ�� */
	virtual void CalcPartialBoxes(){}

	/*! \brief ��ö������� */
	virtual int GetTotalNumVtx() const = 0;

	/*! \brief ���UV�������� */
	virtual int GetTotalNumUV() const = 0;

	/*! \brief ��������� */
	virtual int GetTotalNumTri() const = 0;

	/*! \brief ����ģ�� */
	virtual void Render( ID3D10Device *pd3dDevice, const float time, 
		ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar ) = 0;

	/*! \brief ����һ֡ */
	virtual void RenderSingleFrame( ID3D10Device *pd3dDevice, 
		ID3D10EffectMatrixVariable *worldMatVar, ID3D10EffectShaderResourceVariable *texVar ) = 0;

	virtual void RenderNormal( ID3D10Device *pd3dDevice ) {}
	
	D3DXVECTOR3 GetEntityCenter() { return m_entireCenter; }
	BoundingBox GetEntireBox() { return m_entireBox; }
	int	GetNumBoxes() { return static_cast< int >( m_partialBoxes.size() ); }
	BoundingBox GetPartialBox( int i ) { return m_partialBoxes[i]; }
	/*! \brief ��õ�ǰ������� */
	D3DXMATRIX GetWorldMatrix() const { return m_worldMatrix; }
	/*! \brief ���õ�ǰ������� */
	void SetWorldMatrix( D3DXMATRIX worMat ) { m_worldMatrix = worMat; }
	/*! \brief ��ø��ٵ� */
	D3DXVECTOR3 GetTracePosition() const { return m_entireCenter; }
	/*! \brief ��������پ��� */
	float GetTraceDistanceMax() const { return m_entireLengthMax * 3; }
	/*! \brief �����С���پ��� */
	float GetTraceDistanceMin() const { return m_entireLengthMax / 2; }

protected:
	/*! \brief �����Χ�е����˵����� */
	void CalcEntireBoxInfo();

protected:
	BoundingBox m_entireBox;		//!< ���������Χ��
	D3DXVECTOR3 m_entireCenter;		//!< ���������
	float		m_entireLengthMax;	//!< ��Χ�е����˵�����
	
	BoxList m_partialBoxes;			//!< �����Ϊ��飬ÿ����һ����Χ��

	D3DXMATRIX	m_worldMatrix;		//!< �������
};

#endif