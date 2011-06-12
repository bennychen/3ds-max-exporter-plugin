#ifndef __ANIMATABLE_BASE_H__
#define __ANIMATABLE_BASE_H__

#include "CSM.h"
#include "ModelBase.h"

#include <algorithm>

/*! \brief ����״̬ */
struct AnimState
{
	int		startframe;				//!< first frame
	int		endframe;				//!< last frame
	int		fps;					//!< frame per second for this animation

	float	curr_time;				//!< current time
	float	old_time;				//!< old time
	float	interpol;				//!< percent of interpolation

	string  name;					//!< animation name

	int		curr_frame;				//!< current frame
	int		next_frame;				//!< next frame

	AnimState()
	{
		startframe = endframe = fps = 0;
		curr_time = old_time = interpol = 0.0f;
		curr_frame = next_frame = 0;
	}
} ;

/*! \brief һ�����󶯻��� */
class AnimatableBase : public ModelBase
{
public:
	/*! \brief ���캯�� */
	AnimatableBase()
	{
		m_numSceneFrames = 1;
		m_numTotalFrames = 0;
		m_bInitAnim = FALSE;
	}

	/*! \brief �������� */
	virtual ~AnimatableBase() {};

	typedef vector< D3DXMATRIX > BoneFrames;		//!< ����֡�б�
	typedef vector< CSMAnimation > AnimList;		//!< �����б�

	/*! \brief ��ʼ��������Ϣ */
	BOOL InitAnim( const string &fileAM, const string &fileCFG );

	/*! \brief ���õ�ǰ֡ */
	void SetFrame( int frame );

	/*! \brief ���ݶ����������ö��� */
	BOOL SetAnimation( const string &animName );

	/*! \brief �����������ö��� */
	BOOL SetAnimation( const int index );

	/*! \brief ����.AM�ļ�,AM=AnimationTexture */
	BOOL LoadAM( const string &fileName, int numFrames );

	/*! \brief ����.CFG�ļ������������ļ��� */
	BOOL LoadCFG( const string &fileName );

	///////////////////////////Get&Set Functions//////////////////////////////////////
	/*! \brief ��ö������� */
	int GetAnimNum() const { return static_cast< int >( m_animList.size() ); }
	/*! \brief ��ö�����Ϣ */
	CSMAnimation GetAnimInfo( int i ) const { return m_animList[i]; } 
	/*! \brief ��ö������е���֡�� */
	int GetCurFrame() { return m_curAnimState.curr_frame; }
	/*! \brief ��ö����ĵ�ǰ״̬ */
	AnimState GetCurAnimState() { return m_curAnimState; }
	//////////////////////////////////////////////////////////////////////////////////

protected:
	/*! \brief ���㵱ǰ֡����һ֡���Լ���ֵ��Ϣ */
	void Animate( const float time );

	/*! \brief ����֡��Ϣ */
	virtual void UpdateFrame() = 0;

protected:
	AnimState					m_curAnimState;					//!< ��ǰ����״̬

	AnimList					m_animList;						//!< �����б�
	BoneFrames					m_frameList;					//!< ֡�б�
	int							m_numSceneFrames;				//!< ��������֡��
	int							m_numTotalFrames;				//!< �����ļ�����������֡��

	BOOL						m_bInitAnim;					//!< �Ƿ��ʼ���˶���

	ifstream	m_inFileAM;						//!< .AM�ļ�
	ifstream	m_inFileCFG;					//!< .CFG�ļ�
};

#endif