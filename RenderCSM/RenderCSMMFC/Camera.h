#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera
{
public:
	/*! \brief Ĭ�Ϲ��캯�� */
	Camera( const D3DXVECTOR3 tracePos, const float distance, const float minDis, const float maxDis );

	/*! \brief �������� */
	~Camera();

	/*! \brief ��������� */
	void Reset();

	/*! \brief �����������ͼ���� */
	D3DXMATRIX UpdateViewMatrix();

	/*! \brief �����������Ԥ��λ�� */
	D3DXVECTOR3 CalculatePossibleCamPosition();

	//set
	void SetTracePosition( const D3DXVECTOR3 pos ) { m_tracePos = pos; }
	void SetPossibleCamPosition( const D3DXVECTOR3 possibleCamPosition ) { m_possibleCameraPos = possibleCamPosition; }
	void IncreaseTracePosition( const D3DXVECTOR3 posInc ) { m_tracePos += posInc; } 
	void SetDistance( float distance ) { m_distance = distance; }
	void SetMinMaxDistance( float minDis, float maxDis ) { m_fMinDis = minDis; m_fMaxDis = maxDis; }
	void SetHArc( const float arc ) { m_HArc = arc; }
	void SetVArc( const float arc ) { m_VArc = arc; }

	/* \brief ����ˮƽ�Ƕ� */
	void IncreaseHArc( const float arcInc );

	/* \brief ���Ӵ�ֱ�Ƕ� */
	void IncreaseVArc( const float arcInc );

	/*! \brief ���Ӹ��پ��� */
	void IncreaseDistance( float distanceInc );

	//get
	D3DXVECTOR3 GetTracePosition() const { return m_tracePos; }
	D3DXVECTOR3 GetCamPosition() const { return m_cameraPos; }

	/*! \brief ������������ */
	D3DXVECTOR3 GetDirection();

	D3DXVECTOR3 GetPossibleCamPosition() { return m_possibleCameraPos; }
	float GetHArc() { return m_HArc; }
	float GetVArc() { return m_VArc; }
	D3DXMATRIX GetViewMatrix() { return m_matView; }

private:
	D3DXVECTOR3 m_cameraPos;			//!< �����λ��
	D3DXVECTOR3 m_possibleCameraPos;	//!< Ԥ��������λ��
	D3DXVECTOR3 m_tracePos;				//!< ���ٵ�
	float m_distance;					//!< ������ٵ�ľ���
	float m_HArc;						//!< ˮƽ��ת�ĽǶ�
	float m_VArc;						//!< ��ֱ��ת�ĽǶ�
	D3DXMATRIX m_matView;				//!< �������ͼ����

	float m_fMaxDis;					//!< ������
	float m_fMinDis;					//!< ��̾���
};

#endif