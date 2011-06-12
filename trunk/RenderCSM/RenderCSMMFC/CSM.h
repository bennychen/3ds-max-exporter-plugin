#ifndef __CSM_H__
#define __CSM_H__

#include <fstream>
#include <string>
#include <vector>

using std::ifstream;
using std::string;
using std::vector;

typedef D3DXMATRIX MATRIX;

/*! \brief ��ʼ�����ļ� */
BOOL BeginReading( ifstream &inFile, const string &fileName, BOOL binary = TRUE );
/*! \brief ���������ļ� */
void EndReading( ifstream &inFile );

static const unsigned MAX_STRING_LENGTH = 64;

/*! \brief ��ȡ�ַ��� */
inline char * TruncateString( char str[] )
{
	if ( sizeof( str ) / sizeof( char ) >= MAX_STRING_LENGTH )
	{
		str[MAX_STRING_LENGTH] = '\0';
	}
	return str;
}

/*! \brief CSM�ļ���Header */
struct CSMHeader
{
	int ident;				//!< ���ڱ�����ʶ��.CSM�ļ���ħ�����������CSM1

	int numTags;			//!< ��ǵ�����
	int numAnimFrames;		//!< ������֡����
	int numBones;			//!< ����������
	int numSubMesh;			//!< �����������

	int nHeaderSize;		//!< Header���ֽڴ�С��ͬʱҲ���ڵ�Tag Data���ֽ�ƫ����
	int nOffBones;			//!< ��������Ϣ���ֽ�ƫ����
	int nOffSubMesh;		//!< ��Sub Mesh���ֽ�ƫ����
	int nFileSize;			//!< ����CSM�ļ��Ĵ�С

	CSMHeader()
		:ident( 0 ),
		numTags( 0 ),
		numAnimFrames( 0 ),
		numBones( 0 ),
		numSubMesh( 0 ),
		nHeaderSize( sizeof( CSMHeader ) ),
		nOffBones( 0 ),
		nOffSubMesh( 0 ),
		nFileSize( 0 )
	{
	}
};

/*! \brief ��ʾģ�͵İ󶨵� */
struct CSMTagData
{
	char	name[MAX_STRING_LENGTH];			//!< Tag������
	int		numAnimFrames;						//!< Tag�Ķ���֡����
	vector< MATRIX >	vFrameData;			//!< Tag�İ󶨱任����

	CSMTagData()
	{
		memset( name, 0, sizeof( char ) * MAX_STRING_LENGTH );
		numAnimFrames = 0;
	}
};

/*! \brief �����ڵ���Ϣ */
struct CSMBoneData
{
	char name[MAX_STRING_LENGTH];	//!< ��������
	int ID;							//!< ID
	int parentID;					//!< ��������ID
	D3DXMATRIX relativeMat;			//!< ����ڸ������ľ���

	CSMBoneData()
	{
		memset( name, 0, MAX_STRING_LENGTH );
		ID = -1;
		parentID = -1;
		memset( relativeMat, 0, sizeof( D3DXMATRIX ) );
	}
};

/*! \brief �������� */
struct CSMVertexData
{
	float position[3];		//!< ����λ��
	float normal[3];		//!< ���㷨��
	float u, v;				//!< ��������

	CSMVertexData()
	{
		memset( position, 0, sizeof( float ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
		u = v = 0.0f;
	}
};

/*! \brief ��Ƥ���� */
struct CSMSkinData
{
	int bones[4];			//!< ��������
	float boneWeights[4];	//!< ����������Ȩ��
	CSMSkinData()
	{
		memset( bones, -1, sizeof( int ) * 4 );
		memset( boneWeights, 0, sizeof( float ) * 4 );
	}
};

/*! \brief ������������ */
struct CSMTriangleData
{
	int vertexIndices[3];			//!< �����������������
	float normal[3];				//!< �淨��

	CSMTriangleData()
	{
		memset( vertexIndices, -1, sizeof( int ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
	}
};

/*! \brief ģ�͵���������Ϣ */
struct CSMSubMeshHeader
{
	char	name[MAX_STRING_LENGTH];	//!< �����������

	int		numAnimFrames;		//!< ������֡����
	int		numVertices;		//!< ���������
	int		numFaces;			//!< �������������

	int		nHeaderSize;		//!< Header���ֽڴ�С��ͬʱҲ���ڵ�Texture File���ֽ�ƫ����
	int		nOffVertices;		//!< ���������ݵ��ֽ�ƫ����
	int		nOffSkin;			//!< ����Ƥ���ݵ��ֽ�ƫ����
	int		nOffFaces;			//!< �������������ݵ��ֽ�ƫ����
	int		nOffEnd;			//!< �������������ݲ��ֽ�β���ֽ�ƫ������Ҳ����һ���������ݵĿ�ʼ�㣬�����ļ���β

	/*! \brief Ĭ�Ϲ��캯�� */
	CSMSubMeshHeader()
	{
		memset( name, 0, sizeof( char ) * MAX_STRING_LENGTH );
		numAnimFrames = numVertices = numFaces = 0;
		nHeaderSize = nOffVertices = nOffSkin = nOffFaces = nOffEnd = 0;
	}

	/*! \brief ���캯�� */
	CSMSubMeshHeader( char n[], int nAF, int nV, int nF );
};

/*! \brief ����������Ϣ */
struct CSMAnimation
{
	string animName;
	int firstFrame;
	int numFrames;
	int numLoops;
	int fps;

	CSMAnimation()
	{
		firstFrame = numLoops = numFrames = fps = 0;
	}

	BOOL operator==( const CSMAnimation &rh )
	{
		if ( this->animName == rh.animName )
		{
			return TRUE;
		}
		return FALSE;
	}
};

/*! \brief ���������� */
struct CSMSubMesh
{
	CSMSubMeshHeader	subMeshHeader;						//!< ������Header
	char				textureFile[MAX_STRING_LENGTH];		//!< ��������
	D3DXMATRIX			worldMat;							//!< �������
	vector< CSMVertexData >		vVertexData;				//!< ��������
	vector< CSMSkinData >		vSkinData;					//!< ������������
	vector< CSMTriangleData >	vTriangleData;				//!< ����������

	/*! \brief Ĭ�Ϲ��캯�� */
	CSMSubMesh() 
	{
		memset( &subMeshHeader, 0, sizeof( subMeshHeader ) );
		memset( textureFile, 0, sizeof( char ) * MAX_STRING_LENGTH );
		D3DXMatrixIdentity( &worldMat );
	};

	/*! \brief ���캯�� */
	CSMSubMesh( char meshName[], char texFile[], int numAnimFrame, int numVertices, int numTriangles );

	/*! \brief ����Sub Mesh��Header��Ϣ */
	void GenHeaderInfo( char name[], int numAnimFrames = 1 );
};

#endif