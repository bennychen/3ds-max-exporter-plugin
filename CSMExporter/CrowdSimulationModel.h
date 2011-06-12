#ifndef __CROWDSIMULATIONMODEL_H__
#define __CROWDSIMULATIONMODEL_H__

#include "Utility.h"

/*! \brief �����������������Ѽ�3dmax��Ϣ�����CSM,AM,CFG���ļ� */
class ExportManager
{
public:	
	/*! \brief Ĭ�Ϲ��캯�� */
	ExportManager();

	/*! \brief �������� */
	~ExportManager();

	/*! \brief �洢���˹�����submesh�Լ�����ص���Ƥ��Ϣ */
	struct SubMeshBones
	{
		MaxTriObjData *pMaxObjData;			//!< ���˹�����MaxTriObjData
		ISkinContextData *pSkinContext;		//!< �洢����Ƥ��Ϣ
		ISkin *pSkin;						//!< ������mesh��������Ĺ����б�

		SubMeshBones()
		{
			pMaxObjData = NULL;
			pSkinContext = NULL;
			pSkin = NULL;
		}
	};

	typedef vector< CSMTagData > TagList;					//!< �󶨵��б�
	typedef vector< SubMeshBones > SubMeshBoneList;			//!< ������Ƥ���������б�
	typedef vector< Matrix3 > BoneFrames;					//!< �����Ķ���֡��Ϣ
	typedef vector< CSMAnimation > AnimList;				//!< �����б�

	/*! \brief ������Ϣ */
	struct Bone
	{
		INode *pBoneNode;				//!< �洢������3dmax�ڵ�
		int index;						//!< ����
		Bone *pParentBone;				//!< ������
		Matrix3 relativeMat;			//!< ����ڸ������ľ���
		BoneFrames localFrames;			//!< �ڱ��ؿռ��ڵı任
		BoneFrames boneFrames;			//!< �ؼ�֡

		Bone()
		{
			index = -1;
			pBoneNode = NULL;
			pParentBone = NULL;
			memset( &relativeMat, 0, sizeof( Matrix3 ) );
		}
	};

	typedef map< string, Bone > BoneList;			//!< ���������ƣ�������

	/*! \brief ģ��������һ���� */
	enum PartIdentity
	{
		LOWER,				//!< �°���
		UPPER,				//!< �ϰ���
		HEAD,				//!< ͷ��	
		PROPERTY,			//!< ���ߣ�������
		ENTIRETY,			//!< ����ģ��
		NUM_PART_IDENTITY
	};

	/*! \brief һ��������CSM����������Ϣ������ */
	struct CSMInfo
	{
		PartIdentity		identity;						//!< ��ʾģ��
		string				strCSMFile;						//!< ������ļ����ƣ������������ļ�·����
		string				strAppend;						//!< �ļ�������Ҫ�ӵ����ݣ�������_head����_upper�ȵ�
		ofstream			ofCSMFile;						//!< �ļ���
		CSMHeader			header;							//!< Header
		TagList				vpTags;							//!< �洢���е�Tags
		MaxTriObjList		maxObjList;						//!< ģ����Ϣ�����identity����ENTIRETY������б����ֻ����1��Ԫ��
		INode				*pNode;							//!< max�ڵ㣬���identityΪENTIRETY����pNodeΪNULL

		CSMInfo()
		{
			identity = ENTIRETY;
			pNode = NULL;
		}

		CSMInfo( PartIdentity id )
		{
			identity = id;
			pNode = NULL;
		}

		~CSMInfo()
		{
			for ( MaxTriObjList::iterator i = maxObjList.begin(); i != maxObjList.end(); i ++ )
			{
				SAFE_DELETE( *i );
			}
		}
	};

	typedef vector< CSMInfo* > CSMList;		//!< CSM�ļ���Ϣ�б�

	/*! \brief ��3DS MAX�ĳ������Ѽ�������������� */
	void Gather3DSMAXSceneData( INode *pRootNode, 
								BOOL bExportAnim, 
								BOOL bTag, 
								string headNode, 
								string upperNode, 
								string lowerNode, 
								BOOL bProp, 
								string propNode,
								string mountTo,
								const string &logFile = "log.txt" );

	/*! \brief ������е�CSM�ļ�,CSM=CrowdSimulationModel */
	BOOL WriteAllCSMFile( const string &fileName );

	/*! \brief ���CFG�ļ������������ļ��� */
	BOOL WriteCFGFile( const string &fileName );

	/*! \brief ���.AM�ļ�,AM=AnimationTexture */
	BOOL WriteAMFile( const string &fileName );

	/*! \brief ���.CSM_PAC�ļ� */
	BOOL WriteCSM_PACFile( const string &fileName );

	/*! \brief ��Ӷ�����Ϣ */
	void AddAnim( const CSMAnimation anim )
	{
		m_animList.push_back( anim );
	}

private:
	/*! \brief ���ο������캯�� */
	ExportManager( const ExportManager &rhEM );

	/*! \brief ���θ�ֵ������ */
	ExportManager& operator=( const ExportManager &rhEM );

	/*! \brief ��ʼд���ļ� */
	BOOL BeginWriting( ofstream &outFile, const string &fileName, BOOL binary );

	/*! \brief ����д���ļ� */
	void EndWriting( ofstream &outFile );

	/*! \brief ���ݽڵ������Ѱ�ҽڵ� */
	INode *FindNodeByName( INode *pNode, string nodeName );

	/*! \brief �Ѽ��ڵ���Ϣ������һ���ݹ������������ĺ��� */
	void GatherRecursiveNodeData( INode *pNode, BOOL bExportAnim );

	/*! \brief �����ض��Ľڵ���Ϣ */
	void GatherSpecifiedNodeData( CSMList::iterator beg, CSMList::iterator end, BOOL bExportAnim );

	/*! \brief �Ѽ��ڵ���Ϣ */
	BOOL GatherNodeData( INode *pNode, BOOL bExportAnim );

	/*! \brief �Ѽ�Mesh�����ݴ洢��CSMSubMesh�� */
	void GatherMeshData( Mesh &lMesh, MaxTriObjData *pMaxTriData );

	/*! \brief �Ѽ�Material��ֻ�����洢��CSMSubMesh�� */
	void GatherMaterialData( Mtl *pMtl, MaxTriObjData *pMaxTriData );

	/*! \brief ����TAG���� */
	void GenTag( INode *pTagedNode, INode *pNode, CSMInfo *pCSM, string tagName );

	/*! \brief ����INode��modifier stack������skin modifier(SKIN_CLASSID) */
	Modifier* GetSkinMode( INode *pNode );

	/*! \brief �Ѽ������Skin��Ϣ��Ӱ�������Ȩ�أ��洢��CSMSubMesh�� */
	void GatherSkinData( ISkinContextData* pContext, ISkin *pSkin, MaxTriObjData *pMaxTriData );

	/*! \brief �Ѽ�ISkin���������Ĺ��� */
	void GatherBones( ISkin *pSkin );

	/*! \brief ���ɹ����� */
	void GenBoneTree();

	/*! \brief ���һ���ڵ��Ƿ��ǹ��� */
	BOOL IsBone( INode *pNode );

	/*! \brief ��ȡ��������ȥ��scale�� */
	Matrix3 GetBoneTM( INode *pNode, TimeValue t = 0 );

	/*! \brief �Ѽ��������� */
	void GatherBoneFrames( Bone *pBone );

	/*! \brief ����CSM�ļ���Header��Ϣ */
	void GenCSMHeaderInfo( int numAnimFrames );

	/*! \brief ��ýڵ�������丸�ڵ����Ծ��� */
	Matrix3 GetRelativeMatrix( INode *pNode, TimeValue t = 0 );

	/*! \brief ��ù���������丸��������Ծ��� */
	Matrix3 GetRelativeBoneMatrix( Bone *pBone, TimeValue t = 0 );

	/*! \brief ��ù����ڱ��ؿռ��ڵı任���� */
	Matrix3 GetLocalBoneTranMatrix( Bone *pBone, TimeValue t );

	/*! \brief ���CSM�ļ� */
	BOOL WriteCSMFile( CSMInfo *pCSM );

private:
	//! CSM��ħ�� 
	static const int CSM_MAGIC_NUMBER = 'C' + ( 'S' << 8 ) + ( 'M' << 16 ) + ( '1' << 24 );

	static const BOOL BINARY_MODE = TRUE;		//!< �Ƿ���ö�����дģʽ

	int			m_numAnimFrames;				//!< ������֡��Ϣ

	MaxTriObjList	m_vpMaxObjs;				//!< �洢���������еĽڵ�

	BOOL		m_bSplitMode;					//!< TRUE��ʾ�ǽ�ģ�Ͳ�֣����CSM�ļ�������������������1��CSM�ļ���
	CSMList		m_vpCSM;						//!< ������Ҫ������CSM��Ϣ

	SubMeshBoneList m_submeshBones;				//!< Sub Mesh�Ĺ�����Ϣ
	BoneList	m_boneList;						//!< �����б�
	AnimList	m_animList;						//!< �����б�

	ofstream	m_ofFileAM;						//!< �����.AM�ļ�
	ofstream	m_ofFileCFG;					//!< �����.CFG�ļ�
	ofstream	m_ofFileCSM_PAC;				//!< �����.CSM_PAC�ļ�
	ofstream	m_ofLog;						//!< ���LOG
};

#endif