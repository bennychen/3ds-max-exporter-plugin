#ifndef __DXMANAGER_H__
#define __DXMANAGER_H__

#include <vector>
#include <string>

using std::vector;
using std::string;

#include "SceneState.h"
#include "CSMSkinMesh.h"
#include "CSMMergingMesh.h"
#include "Camera.h"
#include "Timer.h"

class DXManager
{	
public:
	/*! \brief D3D��ʼ�� */
	BOOL Initialize( HWND* );

	/*! \brief ��Ⱦ���� */
	void RenderScene();

	/*! \brief �������� */
	void ProcessInput( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	/*! \brief ����������� */
	void ProcessKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );

	/*! \brief ������껬������ */
	void ProcessMouseWheel( UINT nFlags, short zDelta, CPoint pt );

	/*! \brief ����״̬���� */
	void StateUpdate();

	/*! \brief ��������״̬ */
	void ChangeState( State< DXManager > *pNewState );

	/*! \brief �Ƿ���ĳһ״̬ */
	bool  IsInState( const State<DXManager>& st ) const
	{
		return typeid( *m_pCurState ) == typeid( st );
	}

	/*! \brief ��ȡ����ģ�ͣ�ֻ��һ��CSM�� */
	BOOL LoadEntireMesh( const string &meshName );

	/*! \brief ��ȡ�ϲ�ģ�ͣ����CSM�� */
	BOOL LoadMergingMesh( const string &pacname );

	void SetFillMode( D3D10_FILL_MODE fillMode );
	D3D10_FILL_MODE GetFillMode() { return FILL_MODE; }

	ModelBase *GetMesh() { return m_pMesh; }
	void Pause( BOOL bPaused ) { m_bPaused = bPaused; }

	Camera *GetCamera() { return m_pCamera; }

	BOOL IsDrawNormal() const { return m_bDrawNormal; }
	void SetDrawNormal( BOOL b ) { m_bDrawNormal = b; }

	static DXManager * GetManager()
	{
		static DXManager *pSingleton = new DXManager();
		return pSingleton;
	}

private:
	DXManager();		//!< Ĭ�Ϲ��캯��
	~DXManager();		//!< ��������

	/*! \brief �������������豸 */
	BOOL CreateSwapChainAndDevice( UINT width, UINT height );

	/*! \brief ��ʼ��Shader*/
	BOOL LoadShaders();

	/*! \brief �����ӿ� */
	void CreateViewports( UINT width, UINT height );

	/*! \brief ��ʼ����դ��״̬ */
	void InitRasterizerState();

	/*! \brief ������ȾĿ�� */
	BOOL CreateRenderTargets( UINT width, UINT height );

	/*! \brief ��ʼ������ */
	BOOL InitScene();

	/*! \brief ���³��� */
	void UpdateScene();

	/*! \brief Fatal Error Handler */
	BOOL FatalError( const LPCSTR msg );

private:

	D3D10_FILL_MODE FILL_MODE;

	HWND*						m_hWnd;					//!< ���ھ��

	ID3D10Device				*m_pD3DDevice;			//!< D3D�豸
	IDXGISwapChain				*m_pSwapChain;			//!< D3D Swap Chain
	ID3D10RenderTargetView		*m_pRenderTargetView;	//!< D3D��ȾĿ��
	D3D10_VIEWPORT				m_viewPort;				//!< �ӿ�
	ID3D10Texture2D				*m_pDepthStencil;		//!< ���ģ������
	ID3D10DepthStencilView		*m_pDepthStencilView;	//!< ���ģ����ͼ

	ID3D10Effect*				m_pBasicEffect;			//!< FX�ļ�
	ID3D10RasterizerState*		m_pRS;

	ID3D10EffectMatrixVariable* m_pViewMatrixEffectVariable;			//!< FX����ͼ�������ָ��
	ID3D10EffectMatrixVariable* m_pProjectionMatrixEffectVariable;		//!< FX��ͶӰ�������ָ��
	ID3D10EffectMatrixVariable* m_pWorldMatrixEffectVariable;			//!< FX������������ָ��
	ID3D10EffectShaderResourceVariable		*m_pTextureSR;				//!< FX���������ָ��

	D3DXMATRIX                  m_viewMatrix;					//!< ��ͼ����
	D3DXMATRIX                  m_projectionMatrix;				//!< ͶӰ����

	CSMSkinMesh					*m_pSkinMesh;			//!< CSM Skinģ��
	CSMMergingMesh				*m_pMergingMesh;		//!< ��ϵ�CSMģ��
	ModelBase					*m_pMesh;				//!< ��ǰ��Ⱦ��ģ��

	Camera						*m_pCamera;				//!< �����
	Timer						*m_pTimer;				//!< ��ʱ��

	BOOL						m_bPaused;				//!< �Ƿ�����ͣ

	// ״̬����
	State< DXManager >			*m_pCurState;			//!< ��ǰ������״̬

	BOOL						m_bDrawNormal;			//!< �Ƿ���Ʒ���
};

#define DXMANAGER DXManager::GetManager()


#endif