#include "CrowdSimulationModel.h"

//////////////////////////////ExportManager//////////////////////////////////

ExportManager::ExportManager()
{
	m_numAnimFrames = 0;
	m_bSplitMode = FALSE;
}

ExportManager::~ExportManager()
{
	for ( CSMList::iterator i = m_vpCSM.begin(); i != m_vpCSM.end(); i ++ )
	{
		SAFE_DELETE( *i );
	}
}

/*! \param outFile[] ����ļ���
*	\param fileName �ļ���
*	\param binary �Ƿ��Ƕ����Ƹ�ʽд��
*	\return ���ļ���ʧ�ܷ���FALSE
*/
BOOL ExportManager::BeginWriting( ofstream &outFile, const string &fileName, BOOL binary )
{
	if ( binary == TRUE )
	{
		outFile.open( fileName.c_str(), ofstream::out | ofstream::trunc | ofstream::binary );
	}
	else
	{
		outFile.open( fileName.c_str(), ofstream::out | ofstream::trunc );
	}

	return outFile.fail() ? FALSE : TRUE;
}

/*! \param outFile[] ����ļ��� */
void ExportManager::EndWriting( ofstream &outFile )
{
	if ( outFile )
	{
		outFile.close();
	}
}
/*! \param pNode �����п�ʼѰ�ҵĽڵ�
 *	\param nodeName Ѱ�ҽڵ������
 */
INode * ExportManager::FindNodeByName( INode *pNode, string nodeName )
{
	if ( nodeName == pNode->GetName() )
	{
		return pNode;
	}
	else
	{
		int numChildren = pNode->NumberOfChildren();
		for ( int i = 0; i < numChildren; i++ )
		{
			INode *childNode = pNode->GetChildNode( i );
			INode *resNode = NULL;
			resNode = FindNodeByName( childNode, nodeName );
			if ( resNode != NULL )
			{
				return resNode;
			}
		}
		return NULL;
	}
}

/*! \param pRootNode 3dmax�����ĸ��ڵ�
 *	\param bExportAnim �Ƿ񵼳�����
 *	\param bTag �Ƿ񵼳�TAG
 *	\param headNode ͷ���ڵ�����
 *	\param upperNode �ϰ���ڵ�����
 *	\param lowerNode �°���ڵ�����
 *	\param bProp �Ƿ񵼳�����
 *	\param propNode ���߽ڵ�����
 *	\param logFile �������־�ļ���
 */
void ExportManager::Gather3DSMAXSceneData( INode *pRootNode, 
										   BOOL bExportAnim, 
										   BOOL bTag, string headNodeName, string upperNodeName, string lowerNodeName, 
										   BOOL bProp, string propNodeName, string mountTo, 
										   const string &logFile /* =  */ )
{
#ifdef LOG
	//! �������log
	m_ofLog.open( logFile.c_str(), ofstream::out | ofstream::trunc );
#endif

	//! �Ѽ�������֡��Ϣ
	if ( bExportAnim == TRUE )
	{
		Interval ivAnimRange = GetCOREInterface()->GetAnimRange();
		m_numAnimFrames = ( ivAnimRange.End() - ivAnimRange.Start() ) / GetTicksPerFrame() + 1;
	}
	else
	{
		// û�ж����򳡾�ֻ��1֡
		m_numAnimFrames = 1;
	}

	vector< string > suffix( NUM_PART_IDENTITY );
	//! ������ģ�Ͳ�֣��Ѽ�CSM��������
	if ( bTag == TRUE )
	{
		m_bSplitMode = TRUE;

		// �ᵼ�����CSM�ļ���*_lower.CSM, *_upper.CSM, *_head.CSM, *_property.CSM
		// suffix�ǵ����ļ�����Ҫ��ӵ�����
		suffix[LOWER] = "_lower";
		suffix[UPPER] = "_upper";
		suffix[HEAD] = "_head";
		suffix[PROPERTY] = "_property";
		for ( int id = LOWER; id <= HEAD; id ++ )
		{
			CSMInfo *csm = new CSMInfo( static_cast< PartIdentity >( id ) );
			csm->strAppend = suffix[id];

			m_vpCSM.push_back( csm );
		}

		// �ҵ���������Ӧ�Ľڵ�
		INode *pLowerNode = FindNodeByName( pRootNode, lowerNodeName );
		INode *pUpperNode = FindNodeByName( pRootNode, upperNodeName );
		INode *pHeadNode = FindNodeByName( pRootNode, headNodeName );
		assert( pHeadNode != NULL && pUpperNode != NULL && pLowerNode != NULL );

		vector< INode* > nodeList( 4 );
		nodeList[LOWER] = pLowerNode;
		nodeList[UPPER] = pUpperNode;
		nodeList[HEAD]  = pHeadNode;

		// �����ڵ��Ӧ��CSM�ļ�
		for ( int id = LOWER; id <= HEAD; id ++ )
		{
			m_vpCSM[id]->pNode = nodeList[id];
		}

		// �������
		INode *pPropNode = NULL;
		if ( bProp == TRUE )
		{
			CSMInfo *csm = new CSMInfo( PROPERTY );
			csm->strAppend = suffix[PROPERTY];
			m_vpCSM.push_back( csm );

			pPropNode = FindNodeByName( pRootNode, propNodeName );
			assert( pPropNode != NULL );
			nodeList.push_back( pPropNode );

			m_vpCSM[PROPERTY]->pNode = nodeList[PROPERTY];
		}

		// �Ѽ���Щ�ض��ڵ����Ϣ
		GatherSpecifiedNodeData( m_vpCSM.begin(), m_vpCSM.end(), bExportAnim );

		// ��Lowerת��������ռ�
		MaxTriObjData *pLowerObj = m_vpCSM[LOWER]->maxObjList[0];
		UTILITY->TransformToWorld( pLowerObj, pLowerObj->worldMat );

		// �Ѽ�TAG��Ϣ
		GenTag( pLowerNode, pUpperNode, m_vpCSM[LOWER], "Tag_Torso" );
		GenTag( pUpperNode, pHeadNode, m_vpCSM[UPPER], "Tag_Head" );
		if ( bProp == TRUE )
		{
			if ( mountTo == "Head" )
			{
				GenTag( pHeadNode, pPropNode, m_vpCSM[PROPERTY], "Tag_Property" );
			}
			else if ( mountTo == "Upper" )
			{
				GenTag( pUpperNode, pPropNode, m_vpCSM[PROPERTY], "Tag_Property" );
			}
			else if ( mountTo == "Lower" )
			{
				GenTag( pLowerNode, pPropNode, m_vpCSM[PROPERTY], "Tag_Property" );
			}
			else
			{
				// should never be here
				assert( FALSE && "unknown mount type" );
			}
		}
	}
	else
	{
		m_bSplitMode = FALSE;

		// ֻ����һ��CSM�ļ�
		suffix[ENTIRETY] = "";
		CSMInfo *csm = new CSMInfo( ENTIRETY );
		csm->strAppend = suffix[ENTIRETY];
		m_vpCSM.push_back( csm );

		// �Ѽ����нڵ���Ϣ
		GatherRecursiveNodeData( pRootNode, bExportAnim );

		csm->maxObjList = m_vpMaxObjs;

		// ����Meshת��������ռ�
		for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
		{
			MaxTriObjData *pMaxObj = *i;
			UTILITY->TransformToWorld( pMaxObj, pMaxObj->worldMat );
		}
	}

	//! �Ѽ���������
	if ( bExportAnim == TRUE )
	{
		//! ���ɹ������ṹ
		GenBoneTree();

		//! �Ѽ�������Ƥ��Ϣ
		for ( SubMeshBoneList::iterator i = m_submeshBones.begin(); i != m_submeshBones.end(); i ++ )
		{
			MaxTriObjData *pMaxObjData = i->pMaxObjData;
			ISkinContextData *pSkinContext = i->pSkinContext;
			ISkin *pSkin = i->pSkin;

			GatherSkinData( pSkinContext, pSkin, pMaxObjData );
		}

		//! �Ѽ�����������Ϣ
		for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
		{
			GatherBoneFrames( &( i->second ) );
		}
	}

	//! ��Max����תΪCSM����
	for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
	{
		MaxTriObjData *pTriData = *i;
		UTILITY->MaxDataToCSMData( pTriData );
	}

#ifdef LOG
	m_ofLog.close();
#endif
}

/*! \param pNode �����еĽڵ�
 *	\param bExportAnim �Ƿ񵼳�����
 */
void ExportManager::GatherRecursiveNodeData( INode *pNode, BOOL bExportAnim )
{
	GatherNodeData( pNode, bExportAnim );

	// �ݹ��Ѽ��������еĽڵ�
	int numChildren = pNode->NumberOfChildren();
	for ( int i = 0; i < numChildren; i++ )
	{
		INode *childNode = pNode->GetChildNode( i );
		GatherRecursiveNodeData( childNode, bExportAnim );
	}
}

/*! \param pNode �����еĽڵ�
 *	\param beg Node�б��begin
 *	\param end Node�б��end
 *	\param bExportAnim �Ƿ񵼳�����
 */
void ExportManager::GatherSpecifiedNodeData( CSMList::iterator beg, CSMList::iterator end, BOOL bExportAnim )
{
	for ( CSMList::iterator i = beg; i != end; i ++ )
	{
		CSMInfo *csm = *i;
		assert( GatherNodeData( csm->pNode, bExportAnim ) );
		csm->maxObjList.push_back( m_vpMaxObjs.back() );
	}
}

/*! \param pNode �����еĽڵ�
 *	\param bExportAnim �Ƿ񵼳�����
 *	\return ����ýڵ��Ǳ��������Ľڵ㣬�򷵻�TRUE�����򷵻�FALSE
 */
BOOL ExportManager::GatherNodeData( INode *pNode, BOOL bExportAnim )
{
	// ���BaseObject
	ObjectState os = pNode->EvalWorldState( 0 );
	Object *pObj = os.obj;

	// �ڱ�export����У�ֻ���δ���ص�triObject
	if ( pObj != NULL &&
		 FALSE == pNode->IsNodeHidden() &&
		 FALSE == IsBone( pNode ) &&		// ���ǹ���
		 pObj->CanConvertToType( triObjectClassID ) )
	{
		TriObject* pTriObj = static_cast< TriObject* >( pObj->ConvertToType( 0, triObjectClassID ) );
		if ( pTriObj != NULL )
		{

#ifdef LOG
			m_ofLog << "=>�Ѽ�Node" << pNode->GetName() << endl;
#endif

			MaxTriObjData *pMaxTriData = new MaxTriObjData();
			memset( pMaxTriData, 0, sizeof( MaxTriObjData ) );

			// ����
			pMaxTriData->objName = pNode->GetName();

			// Mesh���������
			Matrix3 worldMat = pNode->GetNodeTM( 0 );
			pMaxTriData->worldMat = UTILITY->TransformToDXMatrix( worldMat );

			// �Ѽ�Mesh����
			Mesh &lMesh = pTriObj->GetMesh();
			GatherMeshData( lMesh, pMaxTriData );

			// �Ѽ���������
			Mtl* pMtl = pNode->GetMtl();
			if ( pMtl != NULL )
			{
				GatherMaterialData( pMtl, pMaxTriData );
				
				if ( pMaxTriData->numTextures > 0 )
				{
					for ( vector< MaxTriangleData >::iterator i = pMaxTriData->vTriangleData.begin(); i != pMaxTriData->vTriangleData.end(); i ++ )
					{
						int faceID = i - pMaxTriData->vTriangleData.begin();
						// ��ø����������Material ID
						i->materialID = lMesh.getFaceMtlIndex( faceID ) % pMaxTriData->numTextures;
					}
				}
			}
			else
			{
#ifdef LOG
				m_ofLog << "\t=>û�в���" << endl;
#endif
				// ���һ���ղ���
				string texName = "";
				pMaxTriData->vTexNames.push_back( texName );
				pMaxTriData->numTextures ++;
			}

			// ���㶥�㷨�ߣ��������Ѽ�����֮����Ϊ���߷����������أ�
			UTILITY->ComputeVertexNormalsOpt( pMaxTriData );

			if ( bExportAnim == TRUE )
			{
				// �Ѽ���������
				Modifier *pMf = GetSkinMode( pNode );
				if ( pMf != NULL )
				{
					ISkin* pSkin = static_cast< ISkin* > ( pMf->GetInterface( I_SKIN ) );
					ISkinContextData* pContext = pSkin->GetContextInterface( pNode );

					// �ȴ洢�������������Ѽ���Node�Ĺ�����Ƥ��Ϣ����ô����Ҫ�ȴ����еĹ�������������ȷ����
					SubMeshBones pSubMeshBones;
					pSubMeshBones.pMaxObjData = pMaxTriData;
					pSubMeshBones.pSkinContext = pContext;
					pSubMeshBones.pSkin = pSkin;
					m_submeshBones.push_back( pSubMeshBones );

					// �Ѽ�����
					GatherBones( pSkin );

					// �ж���
					pMaxTriData->numAnimFrames = m_numAnimFrames;
				}
				else
				{
#ifdef LOG
					m_ofLog << "\t=>û�й���" << endl;
#endif
					// û�ж���
					pMaxTriData->numAnimFrames = 1;
				}
			}
			
			m_vpMaxObjs.push_back( pMaxTriData );

			if ( pTriObj != pObj )
			{
				pTriObj->DeleteMe();
			}
		}
		return TRUE;
	}
	
	return FALSE;
}

/*! \param lMesh 3dmax����
	\param worldMat �������������
	\param pMaxTriData �洢�Ѽ�������������
 */
void ExportManager::GatherMeshData( Mesh &lMesh, MaxTriObjData *pMaxTriData )
{
	//! �Ѽ�������Ϣ
	int nVerts = lMesh.getNumVerts();
	pMaxTriData->vVertexData.clear();
	for ( int i = 0; i < nVerts; i++ )
	{
		MaxVertexData vd;
		Point3 &pts = lMesh.getVert( i );

		/*! 3Dmax9��Z-up����������ϵ����DX��Y-up����������ϵ��������Ҫ�������꣨����y�����z���꣩ */
		vd.position[0] = pts.x;
		vd.position[1] = pts.z;
		vd.position[2] = pts.y;

		pMaxTriData->vVertexData.push_back( vd );
	}
#ifdef LOG
	m_ofLog << "\t=>�Ѽ�����" << nVerts << "��" << endl;
#endif

	//! �Ѽ�����������Ϣ
	int nTVerts = lMesh.getNumTVerts();
	pMaxTriData->vTexCoordData.clear();
	for ( int i = 0; i < nTVerts; i++ )
	{
		MaxTexCoordData tcd;
		UVVert &lTVert = lMesh.getTVert( i );
		/*! 3dmax��u,v����ϵ������	\n
		  ��0,1) ----- (1,1)		\n
		        |     |				\n
				|     |				\n
		   (0,0) ----- (1,0)		\n
		   ��dx��u,v����ϵ������	\n
		  ��0,0) ----- (1,0)		\n
		        |     |				\n
		        |     |				\n
		   (0,1) ----- (1,1)		\n
		   ���ԣ���Ҫ����v���� */
		tcd.u = lTVert.x;
		tcd.v = 1 - lTVert.y;

		pMaxTriData->vTexCoordData.push_back( tcd );
	}
#ifdef LOG
	m_ofLog << "\t=>�Ѽ���������" << nTVerts << "��" << endl;
#endif

	//! ����������Ϣ
	lMesh.buildNormals();
	//! �Ѽ�����������Ϣ
	int nTris = lMesh.getNumFaces();
#ifdef LOG
	m_ofLog << "\t=>�Ѽ���������" << nTris << "��" << endl;
#endif
	for ( int i = 0; i < nTris; i++ )
	{
		MaxTriangleData td;
		//! ������������
		if ( nVerts > 0 )
		{
			Face& lFace = lMesh.faces[i];
			DWORD* pVIndices = lFace.getAllVerts();

			/*! 3Dmax9������������ʱ�룬��DX��˳ʱ�룬������Ҫ����˳�� */
			td.vertexIndices[0] = pVIndices[0];
			td.vertexIndices[1] = pVIndices[2];
			td.vertexIndices[2] = pVIndices[1];
		}
		else
		{
#ifdef LOG
			m_ofLog << "\t\t=>�Ѽ���������ʧ��" << endl;
#endif
		}

		//! ������������
		if ( nTVerts > 0 )
		{
			TVFace& lTVFace = lMesh.tvFace[i];
			DWORD* pUVIndices = lTVFace.getAllTVerts();		

			/*! 3Dmax9������������ʱ�룬��DX��˳ʱ�룬������Ҫ����˳�� */
			td.texCoordIndices[0] = pUVIndices[0];
			td.texCoordIndices[1] = pUVIndices[2];
			td.texCoordIndices[2] = pUVIndices[1];
		}
		else
		{
#ifdef LOG
			m_ofLog << "\t\t=>�Ѽ���������ʧ��" << endl;
#endif
		}

		//! ����
		Point3& nvtx = Normalize( lMesh.getFaceNormal( i ) );
		/*! 3Dmax9��Z-up����������ϵ����DX��Y-up����������ϵ��������Ҫ�������꣨����y�����z���꣩ */
		td.normal[0] = nvtx.x;
		td.normal[1] = nvtx.z;
		td.normal[2] = nvtx.y;

		//! Smoothing Group
		td.smoothGroup = lMesh.faces[i].smGroup;

		pMaxTriData->vTriangleData.push_back( td );
	}
}

/*! \param pMtl 3dmax����
	\param pSubMesh �洢�Ѽ�������������
*/
void ExportManager::GatherMaterialData( Mtl *pMtl, MaxTriObjData *pMaxTriData )
{
	if ( pMtl == NULL )
	{
		return;
	}

	//! ����Ƿ���һ����׼��Material
	if ( pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0 ) )
	{
		StdMat *pStdMtl = ( StdMat * )pMtl;
		BOOL bTwoSided = pStdMtl->GetTwoSided();

		Texmap *pTex = pMtl->GetSubTexmap( ID_DI );
		//! ���������һ��ͼ���ļ�����
		if ( pTex != NULL && pTex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0 ) )
		{
			BitmapTex* pBmpTex = static_cast< BitmapTex* >( pTex );
			string texName = TruncatePath( pBmpTex->GetMapName() );
			//! �洢��������
			pMaxTriData->vTexNames.push_back( texName );
			pMaxTriData->numTextures ++;
#ifdef LOG
			m_ofLog << "\t=>��ȡ�������" << texName << endl;
			StdUVGen *uv = pBmpTex->GetUVGen();
			float utile = uv->GetUScl( 0 );
			float vtile = uv->GetVScl( 0 );
			m_ofLog << "\t\tuTile=" << utile << ", vTile=" << vtile << endl;
#endif
		}
		else //! ���ʲ���һ��ͼ���ļ�������
		{
			string texName = "";
			pMaxTriData->vTexNames.push_back( texName );
			pMaxTriData->numTextures ++;
		}
	}

	for ( int i = 0; i < pMtl->NumSubMtls(); i ++ )
	{
		Mtl *pSubMtl = pMtl->GetSubMtl( i );
		GatherMaterialData( pSubMtl, pMaxTriData );
	}
}

/*! \param pTagedNode �ṩTAG��Ϣ�Ľڵ�
 *	\param pNode �ڵ�󶨵�pTagedNode�Ľڵ㡣\n
		         ���磬pTagedNode��һ��Lower���ṩһ����Torso��TAG��pNode��һ��Upper�����󶨵�Torso
 *	\param pCSM �洢Tag��Ϣ��CSM������identity���벻��ENTIRETY����pCSM�д洢��NodeӦ����pTagedNode��һ��
 *	\param tagName ��TAG������
 */
void ExportManager::GenTag( INode *pTagedNode, INode *pNode, CSMInfo *pCSM, string tagName )
{
	if ( pCSM->identity == ENTIRETY || pCSM->pNode != pTagedNode )
	{
		return;
	}

	CSMTagData tag;
	strcpy( tag.name, tagName.c_str() );
	tag.numAnimFrames = m_numAnimFrames;
	for ( int i = 0; i < tag.numAnimFrames; i ++ )
	{
		TimeValue t = GetTicksPerFrame() * i;

		Matrix3 matA = pTagedNode->GetNodeTM( t );
		Matrix3 matB = pNode->GetNodeTM( t );
		Matrix3 mountMat = matB * Inverse( matA );

		tag.vFrameData.push_back( UTILITY->TransformToDXMatrix( mountMat ) );
	}
	pCSM->vpTags.push_back( tag );

#ifdef LOG
	m_ofLog << "\t=>��ȡTAG--" << tagName << ", ֡��Ϊ" << m_numAnimFrames << endl;
#endif
}

/*! \param pNode 3dmax�����ڵ�
	\return ������Ƥ��Ϣ��Modifier
 */
Modifier* ExportManager::GetSkinMode( INode *pNode )
{
	Object* pObj = pNode->GetObjectRef();
	if( !pObj )
	{
		return NULL;
	}
	while( pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
	{
		IDerivedObject *pDerivedObj = dynamic_cast< IDerivedObject* >( pObj );
		int modStackIndex = 0;
		while( modStackIndex < pDerivedObj->NumModifiers() )
		{
			Modifier* mod = pDerivedObj->GetModifier( modStackIndex );
			if( mod->ClassID() == SKIN_CLASSID )
			{
				return mod;
			}
			modStackIndex++;
		}
		pObj = pDerivedObj->GetObjRef();
	}
	return NULL;
}

void ExportManager::GenBoneTree()
{
	vector< INode* > freshBoneNodes; // �洢��Ҫ�¼��뵽BoneList�еĹ����ڵ�
	//! Ѱ�ҵ����еĹ���
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		INode *boneNode = i->second.pBoneNode;

		INode *parentNode = boneNode->GetParentNode();
		if ( FALSE == parentNode->IsRootNode() && parentNode != NULL )
		{
			string parentBoneName = parentNode->GetName();
			BoneList::iterator k = m_boneList.find( parentBoneName );
			if ( k == m_boneList.end() ) // �ø������ڵ㲻��ģ�͵���Ƥ�����б���
			{
				// ���뵽�������б���
				freshBoneNodes.push_back( parentNode );
			}
		}
	}
	//! �����������ҵ��Ĺ����������б�
	for ( vector< INode* >::iterator i = freshBoneNodes.begin(); i != freshBoneNodes.end(); i ++ )
	{
		INode *pBoneNode = *i;
		while ( TRUE )
		{
			string boneName = pBoneNode->GetName();
			BoneList::iterator k = m_boneList.find( boneName );
			if ( k == m_boneList.end() )
			{
				Bone newBone;
				newBone.pBoneNode = pBoneNode;
				m_boneList.insert( BoneList::value_type( boneName, newBone ) );

				pBoneNode = pBoneNode->GetParentNode();
				if ( TRUE == pBoneNode->IsRootNode() || pBoneNode == NULL )
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	//! ���ɹ�������
	int realIndex = 0;
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++, realIndex ++ )
	{
		i->second.index = realIndex;
	}

	//! ���������ĸ��ӹ�ϵ
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		INode *boneNode = i->second.pBoneNode;
		INode *parentBoneNode = boneNode->GetParentNode();

		if ( FALSE == parentBoneNode->IsRootNode() && parentBoneNode != NULL )
		{
			string parentBoneName = parentBoneNode->GetName();
			BoneList::iterator k = m_boneList.find( parentBoneName );
			assert( k != m_boneList.end() );
			i->second.pParentBone = &( k->second );
		}
	}

	//! �����븸��������Ծ���
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		i->second.relativeMat = GetRelativeBoneMatrix( &( i->second ) );

#ifdef DEBUG
		Matrix3 relMat = i->second.relativeMat;
		FilterData( relMat );
		m_ofLog << "\t" << i->first << "����Ծ���" << endl;
		m_ofLog << "\t\t" << relMat.GetRow( 0 ).x << "  " << relMat.GetRow( 0 ).y << "  " << relMat.GetRow( 0 ).z << endl;
		m_ofLog << "\t\t" << relMat.GetRow( 1 ).x << "  " << relMat.GetRow( 1 ).y << "  " << relMat.GetRow( 1 ).z << endl;
		m_ofLog << "\t\t" << relMat.GetRow( 2 ).x << "  " << relMat.GetRow( 2 ).y << "  " << relMat.GetRow( 2 ).z << endl;
		m_ofLog << "\t\t" << relMat.GetRow( 3 ).x << "  " << relMat.GetRow( 3 ).y << "  " << relMat.GetRow( 3 ).z << endl;
#endif

	}

	//! �������й�������ڸ������Ķ���֡
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
#ifdef LOG
		m_ofLog << "\t����" << i->first << ": " << endl; 
#endif

		for ( int j = 0; j < m_numAnimFrames; j ++ )
		{
			//! ��j֡��ʱ��time
			int time = j * GetTicksPerFrame();

			Matrix3 relTraMatT = GetLocalBoneTranMatrix( &( i->second ), time );

			i->second.localFrames.push_back( relTraMatT );

#ifdef DEBUG
			FilterData( relTraMatT );
			m_ofLog << "\tFrame" << j << ": " << endl;
			m_ofLog << "\t\t" << relTraMatT.GetRow( 0 ).x << "  " << relTraMatT.GetRow( 0 ).y << "  " << relTraMatT.GetRow( 0 ).z << endl;
			m_ofLog << "\t\t" << relTraMatT.GetRow( 1 ).x << "  " << relTraMatT.GetRow( 1 ).y << "  " << relTraMatT.GetRow( 1 ).z << endl;
			m_ofLog << "\t\t" << relTraMatT.GetRow( 2 ).x << "  " << relTraMatT.GetRow( 2 ).y << "  " << relTraMatT.GetRow( 2 ).z << endl;
			m_ofLog << "\t\t" << relTraMatT.GetRow( 3 ).x << "  " << relTraMatT.GetRow( 3 ).y << "  " << relTraMatT.GetRow( 3 ).z << endl;
#endif
		}
	}
}

/*! ��������ܹ��죬����Bone������FALSE������Biped��footstep����FALSE�������Biped�򷵻�TRUE
	\param pNode 3dmax�����ڵ�
	\return �жϽ��
 */
BOOL ExportManager::IsBone( INode *pNode )
{
	if( pNode == NULL )
	{
		return FALSE; 
	}

	ObjectState os = pNode->EvalWorldState( 0 ); 
	if ( !os.obj ) 
	{
		return FALSE;
	}

	//! ����Ƿ���Bone
	if( os.obj->ClassID() == Class_ID( BONE_CLASS_ID, 0 ) )
	{
		return TRUE;
	}

	//! dummy�ڵ�Ҳ��Ϊ����
	if( os.obj->ClassID() == Class_ID( DUMMY_CLASS_ID, 0 ) )
	{
		return TRUE;
	}

	//! ����Ƿ���Biped
	Control *cont = pNode->GetTMController();   
	if( cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||       //others biped parts    
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID )         //biped root "Bip01"     
	{
		return TRUE;
	}

	return FALSE;   
}

/*! \param pContext 3dmax��Ƥ��Ϣ
	\param pSkin ��mesh��������Ĺ���Ϣ
	\param pSubMesh �洢�Ѽ�������������
 */
void ExportManager::GatherSkinData( ISkinContextData* pContext, ISkin *pSkin, MaxTriObjData *pMaxTriData )
{
	//! ���洢�ڵ���Sub Mesh�е�Bone������Ӧ�����Ѽ��곡�������е�Bone��֮�������
	map< int, int > oriToRealMap;
	int iBoneCnt = pSkin->GetNumBones();
	for ( int oriIndex = 0; oriIndex < iBoneCnt; oriIndex ++ )
	{
		string boneName = pSkin->GetBone( oriIndex )->GetName();

		//! Ѱ�ҵ��������������洢��ӳ���
		BoneList::iterator iter = m_boneList.find( boneName );
		int realIndex = iter->second.index;
		if ( iter != m_boneList.end() )
		{
			oriToRealMap.insert( map<int, int>::value_type( oriIndex, realIndex ) );
		}
		else
		{
			throw std::runtime_error( "Couldn't find bone!" );
		}
	}

	//! �������������洢��Ƥ��Ϣ
	int numVtx = pContext->GetNumPoints();
	for ( int iVtx = 0; iVtx < numVtx; iVtx++ )
	{
		int num = pContext->GetNumAssignedBones( iVtx );
		assert( num <= 4 &&
			"��������һ������ֻ�ܰ�4���������㳬���ˣ����ڽ�ʧ���˳�" );
		for ( int iVBone = 0; iVBone < num; iVBone++ )
		{
			int oriBoneIdx = pContext->GetAssignedBone( iVtx, iVBone );
			float weight = pContext->GetBoneWeight( iVtx, iVBone );

			int realBoneIdx = oriToRealMap.find( oriBoneIdx )->second;
			pMaxTriData->vVertexData[iVtx].bones[iVBone] = realBoneIdx;
			pMaxTriData->vVertexData[iVtx].boneWeights[iVBone] = UTILITY->FilterData( weight );
		}
	}
}

/*! \param pSkin ������mesh��������Ĺ����б� */
void ExportManager::GatherBones( ISkin *pSkin )
{
	int iBoneCnt = pSkin->GetNumBones();
#ifdef LOG
	m_ofLog << "\t=>��ȡ����" << iBoneCnt << "��" << endl;
#endif
	for ( int i = 0; i < iBoneCnt; i ++ )
	{
		Bone bone;
		bone.pBoneNode = pSkin->GetBone( i );
		string boneName = bone.pBoneNode->GetName();
		
		m_boneList.insert( BoneList::value_type( boneName, bone ) );
	}
}

/*! \param pBone ���� */
void ExportManager::GatherBoneFrames( Bone *pBone )
{
	INode *pBoneNode = pBone->pBoneNode;

#ifdef LOG
	m_ofLog << "=>��ȡ����" << pBoneNode->GetName() << "����" << endl;
#endif

	// ��ýڵ��transform control
	Control *c = pBoneNode->GetTMController();

	//! �����Ƿ���һ��biped controller(���ڴ���Ľ�����μ�3dmax SDK Document)
	if ( ( c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ) ||
		 ( c->ClassID() == BIPBODY_CONTROL_CLASS_ID ) ||
		 ( c->ClassID() == FOOTPRINT_CLASS_ID ) )
	{
		//!	Get the Biped Export Interface from the controller 
		IBipedExport *BipIface = ( IBipedExport * ) c->GetInterface( I_BIPINTERFACE );

		//!	Remove the non uniform scale
		BipIface->RemoveNonUniformScale( TRUE );

		//!	Release the interface when you are done with it
		c->ReleaseInterface( I_BIPINTERFACE, BipIface );
	}

	vector< Bone* > tempList;
	//! ���������ĸ��ڵ㣬ֱ�����ڵ㣬���뵽�б�
	Bone *pRootBone = pBone;
	tempList.push_back( pRootBone );
	while ( pRootBone->pParentBone != NULL )
	{
		pRootBone = pRootBone->pParentBone;
		tempList.push_back( pRootBone );
	}

	//! baseMat�ǽ�VworldתΪVlocal
	Matrix3 baseMat;
	baseMat.IdentityMatrix();
	//! �Ӹ��ڵ������ֱ����ǰ�ڵ�
	for ( vector< Bone* >::reverse_iterator riter = tempList.rbegin(); riter != tempList.rend(); riter ++ )
	{
		Bone *pB = *riter;
		baseMat =  baseMat * Inverse( pB->relativeMat );
	}

#ifdef DEBUG
	FilterData( baseMat );
	m_ofLog << "\t" << "Base����" << endl;
	m_ofLog << "\t\t" << baseMat.GetRow( 0 ).x << "  " << baseMat.GetRow( 0 ).y << "  " << baseMat.GetRow( 0 ).z << endl;
	m_ofLog << "\t\t" << baseMat.GetRow( 1 ).x << "  " << baseMat.GetRow( 1 ).y << "  " << baseMat.GetRow( 1 ).z << endl;
	m_ofLog << "\t\t" << baseMat.GetRow( 2 ).x << "  " << baseMat.GetRow( 2 ).y << "  " << baseMat.GetRow( 2 ).z << endl;
	m_ofLog << "\t\t" << baseMat.GetRow( 3 ).x << "  " << baseMat.GetRow( 3 ).y << "  " << baseMat.GetRow( 3 ).z << endl;
#endif

	//! ��֡�Ѽ�
	for ( int i = 0; i < m_numAnimFrames; i ++ )
	{
		Matrix3 frame = baseMat;

		//! ��i֡��ʱ��time
		int time = i * GetTicksPerFrame();

		//! �ӵ�ǰ�ڵ㿪ʼ���򸸽ڵ������ֱ�����ڵ�ֹͣ
		for ( vector< Bone* >::iterator iter = tempList.begin(); iter != tempList.end(); iter ++ )
		{
			Bone *pB = *iter;
			//! ���Ե�i֡���ӹ�������ڱ�������ϵ�ı任
			frame = frame * pB->localFrames[i];
			//! �任��������ϵ
			frame = frame * pB->relativeMat;
		}

		//! ���˻����ʱ��time�Ĺؼ�֡����
		pBone->boneFrames.push_back( frame );

#ifdef DEBUG
		FilterData( frame );
		m_ofLog << "\tFrame" << i << ": " << endl;
		m_ofLog << "\t\t" << frame.GetRow( 0 ).x << "  " << frame.GetRow( 0 ).y << "  " << frame.GetRow( 0 ).z << endl;
		m_ofLog << "\t\t" << frame.GetRow( 1 ).x << "  " << frame.GetRow( 1 ).y << "  " << frame.GetRow( 1 ).z << endl;
		m_ofLog << "\t\t" << frame.GetRow( 2 ).x << "  " << frame.GetRow( 2 ).y << "  " << frame.GetRow( 2 ).z << endl;
		m_ofLog << "\t\t" << frame.GetRow( 3 ).x << "  " << frame.GetRow( 3 ).y << "  " << frame.GetRow( 3 ).z << endl;
#endif
	}

#ifdef LOG
	m_ofLog << "\t=>��ȡ����" << m_numAnimFrames << "֡" << endl;
#endif
}

/*! \param numAnimFrames ����֡�� */
void ExportManager::GenCSMHeaderInfo( int numAnimFrames )
{
	for ( CSMList::iterator i = m_vpCSM.begin(); i != m_vpCSM.end(); i ++ )
	{
		CSMInfo *csm = *i;

		CSMHeader *pHeader = &( csm->header );
		memset( pHeader, 0, sizeof( CSMHeader ) );

		pHeader->ident = CSM_MAGIC_NUMBER;

		pHeader->numTags = static_cast< int >( csm->vpTags.size() );
		pHeader->numBones = static_cast< int >( m_boneList.size() );
		pHeader->numAnimFrames = numAnimFrames;
		pHeader->numSubMesh = 0;
		for ( MaxTriObjList::iterator j = csm->maxObjList.begin(); j != csm->maxObjList.end(); j ++ )
		{
			MaxTriObjData *pMaxObj = *j;
			int numSubMesh = static_cast< int >( pMaxObj->vSubMeshes.size() );
			pHeader->numSubMesh += numSubMesh;
		}

		pHeader->nHeaderSize = sizeof( CSMHeader );

		pHeader->nOffBones = pHeader->nHeaderSize;
		for ( TagList::iterator j = csm->vpTags.begin(); j != csm->vpTags.end(); j ++ )
		{
			CSMTagData pTag = *j;
			pHeader->nOffBones += sizeof( MAX_STRING_LENGTH );
			pHeader->nOffBones += sizeof( int );
			pHeader->nOffBones += sizeof( D3DXMATRIX ) * pTag.numAnimFrames;
		}

		pHeader->nOffSubMesh = pHeader->nOffBones + sizeof( CSMBoneData ) * pHeader->numBones;

		pHeader->nFileSize = pHeader->nOffSubMesh;
		for ( MaxTriObjList::iterator j = csm->maxObjList.begin(); j != csm->maxObjList.end(); j ++ )
		{
			MaxTriObjData *pMaxObj = *j;
			for ( SubMeshList::iterator k = pMaxObj->vSubMeshes.begin(); k != pMaxObj->vSubMeshes.end(); k ++ )
			{
				CSMSubMesh *pSubMesh = *k;
				pHeader->nFileSize += pSubMesh->subMeshHeader.nOffEnd;
			}
		}
	}
}

/*! \param fileName ����ļ���
	\return ���ļ�ʧ�ܣ�����FALSE
 */
BOOL ExportManager::WriteAllCSMFile( const string &fileName )
{
	size_t token = fileName.find_last_of( "CSM" );
	string strFore = fileName.substr( 0, token - 3 );

	for ( CSMList::iterator i = m_vpCSM.begin(); i != m_vpCSM.end(); i ++ )
	{
		CSMInfo *csm = *i;
		string strAppend = csm->strAppend + ".CSM";
		csm->strCSMFile = strFore + strAppend;

		if ( WriteCSMFile( csm ) == FALSE )
		{
			return FALSE;
		}
	}

	if ( TRUE == m_bSplitMode && m_vpCSM.size() > 1 )
	{
		WriteCSM_PACFile( fileName );
	}
	
	return TRUE;
}

/*! \param pCSM ��Ҫ�����CSM��Ϣ
 *	\return ���ļ�ʧ�ܣ�����FALSE
 */
BOOL ExportManager::WriteCSMFile( CSMInfo *pCSM )
{
	string fileName = pCSM->strCSMFile;
	size_t token = fileName.find_last_of( '\\' );
	string path = fileName.substr( 0, token );

	ofstream &ofFileCSM = pCSM->ofCSMFile;
	CSMHeader *pHeader = &( pCSM->header );

	if ( BeginWriting( ofFileCSM, fileName, TRUE ) )
	{
		//! ͳ��CSM��Ϣ�����CSM��Header
		GenCSMHeaderInfo( m_numAnimFrames );

		//! ��Headerд���ļ�
		ofFileCSM.write( reinterpret_cast< char* >( pHeader ), sizeof( CSMHeader ) );

		int offset = pHeader->nHeaderSize;
		//! ��Tag Dataд���ļ�
		for ( TagList::iterator i =pCSM->vpTags.begin(); i != pCSM->vpTags.end(); i ++ )
		{
			CSMTagData pTag = *i;
			
			ofFileCSM.seekp( offset, ofstream::beg );
			ofFileCSM.write( reinterpret_cast< char* >( pTag.name ), MAX_STRING_LENGTH );
			offset += MAX_STRING_LENGTH;

			ofFileCSM.seekp( offset, ofstream::beg );
			ofFileCSM.write( reinterpret_cast< char* >( &pTag.numAnimFrames ), sizeof( int ) );
			offset += sizeof( int );

			for ( int j = 0; j < pTag.numAnimFrames; j ++ )
			{
				D3DXMATRIX mtx = pTag.vFrameData[j];
				ofFileCSM.seekp( offset, ofstream::beg );
				ofFileCSM.write( reinterpret_cast< char* >( &mtx ), sizeof( MATRIX ) );
				offset += sizeof( MATRIX );
			}
		}

		//! ��Bonesд���ļ�
		int n = 0;
		for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i++, n++ )
		{
			CSMBoneData CSMBone;
			string boneName = i->first;
			strcpy( CSMBone.name, boneName.c_str() );
			CSMBone.ID = i->second.index;
			if ( i->second.pParentBone != NULL )
			{
				CSMBone.parentID = i->second.pParentBone->index;
			}
			else
			{
				CSMBone.parentID = -1;
			}
			CSMBone.relativeMat = UTILITY->TransformToDXMatrix( i->second.relativeMat );
			ofFileCSM.seekp( pHeader->nOffBones + n * sizeof( CSMBone ), ofstream::beg );
			ofFileCSM.write( reinterpret_cast< const char* >( &CSMBone ), sizeof( CSMBone ) );
		}

		//! ��Sub Meshд���ļ�
		int lastOffSet = pHeader->nOffSubMesh;
		for ( MaxTriObjList::iterator i = pCSM->maxObjList.begin(); i != pCSM->maxObjList.end(); i ++ )
		{
			MaxTriObjData *pMaxObj = *i;

			for ( SubMeshList::iterator j = pMaxObj->vSubMeshes.begin(); j != pMaxObj->vSubMeshes.end(); j ++ )
			{
				CSMSubMesh *pSubMesh = *j;

				//! д��Sub Mesh Header
				ofFileCSM.seekp( lastOffSet, ofstream::beg );
				ofFileCSM.write( reinterpret_cast< char* >( &pSubMesh->subMeshHeader ), pSubMesh->subMeshHeader.nHeaderSize );

				//! д��Texture File
				string oldTexFile = pSubMesh->textureFile;
				string texName;
				if ( oldTexFile != "" )
				{
					//! ��texture�ļ�������Ŀ¼��
					token = oldTexFile.find_last_of( '\\' );
					texName = oldTexFile.substr( token + 1 );
					string newTexFile = path + "\\" + texName;
					CopyFile( pSubMesh->textureFile, newTexFile.c_str(), FALSE );
				}
				char texFile[ MAX_STRING_LENGTH ];
				strcpy( texFile, texName.c_str() );
				ofFileCSM.seekp( lastOffSet + pSubMesh->subMeshHeader.nHeaderSize, ofstream::beg );
				ofFileCSM.write( reinterpret_cast< char* >( texFile ), MAX_STRING_LENGTH );

				//! д��Vertex Data
				for ( int k = 0; k < pSubMesh->subMeshHeader.numVertices; k ++ )
				{
					CSMVertexData *pVertex = &pSubMesh->vVertexData[k];
#ifdef FILTER
					UTILITY->FilterData( *pVertex );
#endif
					ofFileCSM.seekp( lastOffSet + pSubMesh->subMeshHeader.nOffVertices + k * sizeof( CSMVertexData ), ofstream::beg );
					ofFileCSM.write( reinterpret_cast< char* >( pVertex ), sizeof( CSMVertexData ) );
				}

				//! д��Skin Data
				int numSkinData;
				if ( pSubMesh->subMeshHeader.numAnimFrames > 1 )
				{
					numSkinData = pSubMesh->subMeshHeader.numVertices;
				}
				else
				{
					numSkinData = 0;
				}
				for ( int k = 0; k < numSkinData; k ++ )
				{
					CSMSkinData *pSkinData = &pSubMesh->vSkinData[k];
#ifdef FILTER
					UTILITY->FilterData( *pSkinData );
#endif
					ofFileCSM.seekp( lastOffSet + pSubMesh->subMeshHeader.nOffSkin + k * sizeof( CSMSkinData ), ofstream::beg );
					ofFileCSM.write( reinterpret_cast< char* >( pSkinData ), sizeof( CSMSkinData ) );
				}

				//! д��Triangle Data
				for ( int k = 0; k < pSubMesh->subMeshHeader.numFaces; k ++ )
				{
					CSMTriangleData *pTriData = &pSubMesh->vTriangleData[k];

					ofFileCSM.seekp( lastOffSet + pSubMesh->subMeshHeader.nOffFaces + k * sizeof( CSMTriangleData ), ofstream::beg );
					ofFileCSM.write( reinterpret_cast< char* >( pTriData ), sizeof( CSMTriangleData ) );
				}

				lastOffSet = lastOffSet + pSubMesh->subMeshHeader.nOffEnd;
			}
		}

		EndWriting( ofFileCSM );

		return TRUE;
	}

	return FALSE;
}

/*! \param fileName ����ļ��� 
	\return ���ļ�ʧ�ܣ�����FALSE
 */
BOOL ExportManager::WriteAMFile( const string &fileName )
{
	//! ��д�붯�������ļ�
	if ( FALSE == WriteCFGFile( fileName ) )
	{
		return FALSE;
	}

	size_t token = fileName.find_last_of( ".CSM" );
	string animFile = fileName.substr( 0, token - 3 ) + ".AM";

	if ( BeginWriting( m_ofFileAM, animFile, TRUE ) )
	{
		//! �����д��
		for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
		{
			Bone bone = i->second;
			int boneSeek = bone.index * m_numAnimFrames * sizeof( MATRIX );
			//! ��֡д��
			for ( BoneFrames::iterator j = bone.boneFrames.begin(); j != bone.boneFrames.end(); j++ )
			{
				MATRIX m = UTILITY->TransformToDXMatrix( *j );
#ifdef FILTER
				UTILITY->FilterData( m );
#endif
				int frameIndex = j - bone.boneFrames.begin();
				int frameSeek = boneSeek + frameIndex * sizeof( MATRIX );

				m_ofFileAM.seekp( frameSeek, ofstream::beg );
				m_ofFileAM.write( reinterpret_cast< char* >( &m ), sizeof( MATRIX ) );
			}
		}

		EndWriting( m_ofFileAM );

		return TRUE;
	}
	return FALSE;
}

/*! \param fileName ����ļ���
	\return ���ļ�ʧ�ܣ�����FALSE
 */
BOOL ExportManager::WriteCFGFile( const string& fileName )
{
	size_t token = fileName.find_last_of( ".CSM" );
	string cfgFile = fileName.substr( 0, token - 3 ) + ".CFG";

	if ( BeginWriting( m_ofFileCFG, cfgFile, FALSE ) )
	{
		//! ��������Ķ���֡��
		m_ofFileCFG << "SceneFrame: " << m_numAnimFrames << endl;

		//! ������
		m_ofFileCFG << "NumberOfBone: " << m_boneList.size() << endl;

		//! �����������
		m_ofFileCFG << "NumberOfAnimation: " << m_animList.size() << endl;

		//! �𶯻����������Ϣ
		for ( AnimList::iterator i = m_animList.begin(); i != m_animList.end(); i ++ )
		{
			m_ofFileCFG << i->animName << "\t" 
						<< i->firstFrame << "\t" 
						<< i->numFrames << "\t" 
						<< i->numLoops << "\t" 
						<< i->fps << endl;
		}
		
		EndWriting( m_ofFileCFG );

		return TRUE;
	}
	return FALSE;
}

BOOL ExportManager::WriteCSM_PACFile( const string &fileName )
{
	size_t token = fileName.find_last_of( ".CSM" );
	string pacFile = fileName.substr( 0, token - 3 ) + ".CSM_PAC";

	if ( BeginWriting( m_ofFileCSM_PAC, pacFile, FALSE ) )
	{
		//! ����ܹ���CSM����
		int num = static_cast< int >( m_vpCSM.size() );
		m_ofFileCSM_PAC << num << endl;

		vector< string > vID( 4 );
		vID[ LOWER ] = "lower";
		vID[ UPPER ] = "upper";
		vID[ HEAD  ] = "head";
		vID[ PROPERTY ] = "property";
		//! ���������е�CSM��Ϣ
		for ( CSMList::iterator i = m_vpCSM.begin(); i != m_vpCSM.end(); i ++ )
		{
			CSMInfo *csm = *i;
			assert( csm->identity < ENTIRETY && 
				"��������CSM��Ӧ�õ���csm_pac�ļ�" );

			string id = vID[ csm->identity ];
			string csmFile = csm->strCSMFile;
			token = csmFile.find_last_of( "\\" );
			if ( token != string::npos )
			{
				csmFile = csmFile.substr( token + 1 );
			}

			m_ofFileCSM_PAC << id << "\t" << csmFile << endl;
		}

		return TRUE;
	}
	return FALSE;
}


/*! \param pNode 3dmax�����ڵ�
	\param t ʱ���
	\return ��������
 */
Matrix3 ExportManager::GetBoneTM( INode *pNode, TimeValue t )   
{   
	Matrix3 tm = pNode->GetNodeTM( t );
	tm.NoScale();   
	return tm;   
} 

/*! \param pNode 3dmax�����еĽڵ�
	\param t ʱ��
	\return ��ʱ��t�ڵ�������丸�ڵ�ľ���
 */
Matrix3 ExportManager::GetRelativeMatrix( INode *pNode, TimeValue t /* = 0  */ )
{
	Matrix3 worldMat = pNode->GetNodeTM( t );
	Matrix3 parentMat = pNode->GetParentTM( t );

	//! ��ΪNodeWorldTM = NodeLocalTM * ParentWorldTM��ע�⣺3dmax9�еľ������ҳˣ�\n
	//! ����NodeLocalTM = NodeWorldTM * Inverse( ParentWorldTM )
	Matrix3 relativeMat = worldMat * Inverse( parentMat );

	return relativeMat;
}

/*! �������Ľڵ�����ڳ����л��и��ڵ㣨����Scene Root��
	\param pBone ����
	\param t ʱ��
	\return ��ʱ��t����������丸�����ľ���
 */
Matrix3 ExportManager::GetRelativeBoneMatrix( Bone *pBone, TimeValue t /* = 0  */ )
{
	if ( pBone->pParentBone == NULL )
	{
		Matrix3 worldMat = pBone->pBoneNode->GetNodeTM( t );
		return worldMat;
	}
	else
	{
		return GetRelativeMatrix( pBone->pBoneNode, t );
	}
}

/*! \param pBone 3dmax�����еĽڵ�
	\param t ʱ��
	\return ������ʱ��t�ڱ��ؿռ��ڵı任
 */
Matrix3 ExportManager::GetLocalBoneTranMatrix( Bone *pBone, TimeValue t )
{
	Matrix3 relMatT0 = GetRelativeBoneMatrix( pBone, 0 );
	Matrix3 relMatTN = GetRelativeBoneMatrix( pBone, t );
	//! ��ΪrelMatT = transformMatT * relMat0
	//! ����transformMatT = relMatT * Inverse( relMat0 ) 
	Matrix3 transformMatT = relMatTN * Inverse( relMatT0 );

	return transformMatT;
}