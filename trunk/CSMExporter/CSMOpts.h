#ifndef __CSMOPTS_H__
#define __CSMOPTS_H__

#include "resource.h"
#include "CrowdSimulationModelExporter.h"

#include <string>
#include <vector>


using std::string;
using std::vector;

struct AnimRecord
{
	int firstFrame;
	int lastFrame;
	string animName;

	AnimRecord()
	{
		firstFrame = lastFrame = 0;
	}
};

/*! \brief ��������Ľ��棨�����Ի��� */
class CSMOpts
{
public:
	CSMOpts( int firstFrame, int lastFrame, vector< string > objList );
	~CSMOpts();

	// Calling this functions will pop
	// the options dialog for this exporter.
	// If this function returns false, cancel the export
	BOOL ShowOptionDialog();
	
	BOOL IfExportAnim() { return m_bExportAnim; }
	BOOL IfExportTags() { return m_bExportTags; }
	BOOL IfExprotProperty() { return m_bExportProperty; }
	int GetFristFrame() { return m_firstSceneFrame; }
	int GetLastFrame() { return m_lastSceneFrame; }
	int GetSettedFrame() { return m_settedFrame; }
	AnimRecord GetNewAnimRecord() { return m_newAnimRecord; }
	AnimRecord GetCurAnimRecord() { return m_curAnimRecord; }
	BOOL IsEditingAnim() { return m_bIsEditingAnim; }
	int GetAnimRecordCount() { return static_cast< int >( m_vAnimRecords.size() ); }
	AnimRecord GetAnimRecord( int i ) { return m_vAnimRecords[i]; }
	int GetNumObj() { return static_cast< int >( m_objList.size() ); }
	string GetObj( int i ) { return m_objList[i]; }

	// ��������������Option�Ի�����OK��ťʱִ��
	void SetExportAnim( HWND hWnd, BOOL b );
	void SetExportTags( HWND hWnd, BOOL b );
	void SetExportProp( HWND hWnd, BOOL b );

	void SetSettedFrame( int frame ) { m_settedFrame = frame; }
	void SetNewAnimRecord( string animName, int firstFrame, int lastFrame )
	{
		m_newAnimRecord.animName = animName;
		m_newAnimRecord.firstFrame = firstFrame;
		m_newAnimRecord.lastFrame = lastFrame;
	}
	void SetCurAnimRecord( string animName, int firstFrame, int lastFrame )
	{
		m_curAnimRecord.animName = animName;
		m_curAnimRecord.firstFrame = firstFrame;
		m_curAnimRecord.lastFrame = lastFrame;
	}
	void SetEditingAnim( BOOL b ) { m_bIsEditingAnim = b; }
	void AddAnimRecord( AnimRecord anim ) { m_vAnimRecords.push_back( anim ); }

private:
	/*! \brief ѡ��Ի������Ϣ������ */
	static INT_PTR CALLBACK OptionDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	/*! \brief ��ʾAnimation�Ի��� */
	BOOL ShowAnimDialog();

	/*! \brief �����Ի������Ϣ������ */
	static INT_PTR CALLBACK AnimDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	/*! \brief ��ʾ�����б�Ի��� */
	BOOL ShowObjListDialog();

	/*! \brief �����б�Ի��� */
	static INT_PTR CALLBACK ObjListDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	void EnableAnimControls( BOOL bEnable );

	void EnableTagControls( BOOL bEnable );

	void EnablePropControls( BOOL bEnable );

public:
	string m_sHeadNode;
	string m_sUpperNode;
	string m_sLowerNode;
	string m_sPropNode;
	string m_propMountTo;

	string m_sGettingNode;

private:
	BOOL	m_bExportAnim;					//!< �Ƿ񵼳�����
	BOOL	m_bExportTags;					//!< �Ƿ񵼳�Tags
	BOOL	m_bExportProperty;				//!< �Ƿ񵼳�����

	vector< AnimRecord > m_vAnimRecords;	//!< ������¼
	vector< string > m_objList;				//!< �����ڵ��б�

	static HWND	m_hCheckAnim;
	static HWND m_hListAnim;
	static HWND m_hBtnAddAnim;
	static HWND m_hBtnEditAnim;
	static HWND m_hBtnDelAnim;

	static HWND m_hCheckTag;
	static HWND m_hBtnHead;
	static HWND m_hBtnUpper;
	static HWND m_hBtnLower;
	static HWND m_hBtnProp;
	static HWND m_hBtnHeadX;
	static HWND m_hBtnUpperX;
	static HWND m_hBtnLowerX;
	static HWND m_hBtnPropX;
	static HWND m_hCheckProp;
	static HWND m_hComBoxMount;

	static HWND m_hEditAnimName;
	static HWND m_hEditFirstFrame;
	static HWND m_hEditLastFrame;

	static HWND m_hListObj;

	int		m_firstSceneFrame;			//!< �����ĵ�һ֡
	int		m_lastSceneFrame;			//!< ���������һ֡
	int		m_settedFrame;				//!< ��ǰ���ö������õ������֡

	AnimRecord	m_newAnimRecord;		//!< ��Ҫ������¶�����¼
	AnimRecord	m_curAnimRecord;		//!< ��ǰѡ�еĶ�����¼
	BOOL		m_bIsEditingAnim;		//!< �Ƿ��Ǵ��ڱ༭״̬����������Ӷ���
};

#endif