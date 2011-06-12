#include "stdafx.h"
#include "AnimatableBase.h"

/*!	\param fileAM ��Ҫ��ȡ��AM�ļ���
	\param fileCFG ��Ҫ��ȡ��CFG�ļ���
	\return ��ʼ��ʧ�ܷ���FALSE
 */
BOOL AnimatableBase::InitAnim( const string &fileAM, const string &fileCFG )
{
	if ( FALSE == LoadCFG( fileCFG ) )
	{
		return FALSE;
	}
	if ( FALSE == LoadAM( fileAM, m_numTotalFrames ) )
	{
		return FALSE;
	}
	m_bInitAnim = TRUE;
	return TRUE;
}

/*!	\param frame ���õ�֡�� */
void AnimatableBase::SetFrame( int frame )
{
	assert( m_bInitAnim &&
		"���ȳ�ʼ��������Ϣ" );

	if ( frame < m_curAnimState.startframe )
	{
		frame = m_curAnimState.endframe;
	}
	if ( frame > m_curAnimState.endframe )
	{
		frame = m_curAnimState.startframe;
	}

	m_curAnimState.curr_frame = frame;
	m_curAnimState.next_frame = frame + 1;

	if( m_curAnimState.next_frame > m_curAnimState.endframe )
	{
		m_curAnimState.next_frame = m_curAnimState.startframe;
	}

	UpdateFrame();
}

/*! \param time ��ǰʱ�� */
void AnimatableBase::Animate( const float time )
{
	assert( m_bInitAnim &&
		"���ȳ�ʼ��������Ϣ" );

	m_curAnimState.curr_time = time;

	// ���㵱ǰ֡����һ֡
	if( m_curAnimState.curr_time - m_curAnimState.old_time > ( 1.0 / m_curAnimState.fps ) )
	{
		m_curAnimState.curr_frame = m_curAnimState.next_frame;
		m_curAnimState.next_frame++;

		if( m_curAnimState.next_frame > m_curAnimState.endframe )
		{
			m_curAnimState.next_frame = m_curAnimState.startframe;
		}

		m_curAnimState.old_time = m_curAnimState.curr_time;
	}

	if( m_curAnimState.curr_frame > ( m_numSceneFrames - 1 ) )
	{
		m_curAnimState.curr_frame = 0;
	}

	if( m_curAnimState.next_frame > ( m_numSceneFrames - 1 ) )
	{
		m_curAnimState.next_frame = 0;
	}

	// ��ֵ
	m_curAnimState.interpol = m_curAnimState.fps * ( m_curAnimState.curr_time - m_curAnimState.old_time );

	// ���¶���
	UpdateFrame();
}

/*! \param animName ��������
	\return û�ж�������FALSE
 */
BOOL AnimatableBase::SetAnimation( const string &animName )
{
	assert( m_bInitAnim &&
		"���ȳ�ʼ��������Ϣ" );

	if ( 0 == m_numSceneFrames )
	{
		return FALSE;
	}

	CSMAnimation anim;
	anim.animName = animName;
	AnimList::iterator i = find( m_animList.begin(), m_animList.end(), anim );
	if ( i == m_animList.end() )
	{
		return FALSE;
	}

	m_curAnimState.startframe	= i->firstFrame;
	m_curAnimState.endframe		= i->firstFrame + i->numFrames - 1;
	m_curAnimState.curr_frame   = i->firstFrame;
	m_curAnimState.next_frame	= i->firstFrame + 1;
	if ( m_curAnimState.next_frame > m_curAnimState.endframe )
	{
		m_curAnimState.next_frame = m_curAnimState.startframe;
	}
	m_curAnimState.fps			= i->fps;
	m_curAnimState.name			= i->animName;

	// ���¶���
	UpdateFrame();

	return TRUE;
}

/*! \param index ����ID
	\return û�ж�������FALSE
 */
BOOL AnimatableBase::SetAnimation( const int index )
{
	assert( m_bInitAnim &&
		"���ȳ�ʼ��������Ϣ" );

	if ( 0 == m_numSceneFrames )
	{
		return FALSE;
	}

	CSMAnimation anim = m_animList[index];

	m_curAnimState.startframe	= anim.firstFrame;
	m_curAnimState.endframe		= anim.firstFrame + anim.numFrames - 1;
	m_curAnimState.curr_frame   = anim.firstFrame;
	m_curAnimState.next_frame	= anim.firstFrame + 1;
	if ( m_curAnimState.next_frame > m_curAnimState.endframe )
	{
		m_curAnimState.next_frame = m_curAnimState.startframe;
	}
	m_curAnimState.fps			= anim.fps;
	m_curAnimState.name			= anim.animName;

	// ���¶���
	UpdateFrame();

	return TRUE;
}


/*! \param fileName AM�ļ�����
	\param numFrames ��ȡ�Ķ�����֡��
	\return ��ȡʧ�ܷ���FALSE
 */
BOOL AnimatableBase::LoadAM( const string &fileName, int numFrames )
{
	if ( numFrames <= 0 )
	{
		return FALSE;
	}

	if ( BeginReading( m_inFileAM, fileName ) )
	{
		// ��֡��ȡ
		for ( int n = 0; n < numFrames; n ++ )
		{
			D3DXMATRIX m;
			int frameSeek = n * sizeof( D3DXMATRIX );
			m_inFileAM.seekg( frameSeek, ifstream::beg );
			m_inFileAM.read( reinterpret_cast< char* >( &m ), sizeof( D3DXMATRIX ) );

			m_frameList.push_back( m );
		}
		EndReading( m_inFileAM );

		return TRUE;
	}

	return FALSE;
}

/*! \param fileName CFG�ļ�����
	\return ��ȡʧ�ܷ���FALSE
*/
BOOL AnimatableBase::LoadCFG( const string &fileName )
{
	if ( BeginReading( m_inFileCFG, fileName, FALSE ) )
	{
		string placeholder;

		//! ���볡��֡��
		m_inFileCFG >> placeholder >> m_numSceneFrames;

		//! ���������
		int boneNum = 0;
		m_inFileCFG >> placeholder >> boneNum;

		m_numTotalFrames = boneNum * m_numSceneFrames;

		//! ���붯������
		int numAnim = 0;
		m_inFileCFG >> placeholder >> numAnim;

		//! ����ÿ����������ϸ��Ϣ
		for ( int i = 0; i < numAnim; i ++ )
		{
			CSMAnimation animation;
			m_inFileCFG >> animation.animName
				>> animation.firstFrame
				>> animation.numFrames
				>> animation.numLoops
				>> animation.fps;

			m_animList.push_back( animation );
		}

		EndReading( m_inFileCFG );

		return TRUE;
	}

	return FALSE;
}