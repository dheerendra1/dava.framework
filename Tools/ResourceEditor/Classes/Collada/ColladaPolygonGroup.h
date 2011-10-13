#ifndef __COLLADALOADER_COLLADAPOLYGONGROUP_H__
#define __COLLADALOADER_COLLADAPOLYGONGROUP_H__

#include "ColladaIncludes.h"
#include "Base/BaseMath.h"
#include "ColladaMaterial.h"

namespace DAVA
{

class ColladaMesh;

struct ColladaVertex
{	
	ColladaVertex()
	{
		jointCount = 0;
		for (int i = 0; i < 20; ++i)
		{
			joint[i] = 0;
			weight[i] = 0.0f;
		}
	}
        
    static bool IsEqual(const ColladaVertex & v1, const ColladaVertex & c2, int32 vertexFormat);

	Vector3 position;	
	Vector3 normal;
	Vector3 tangent;
	Vector3 binormal;
	Vector2 texCoords[4];
	
	int32	jointCount;
	int32	joint[20];
	float32	weight[20];
};

struct ColladaSortVertex
{
	float	sortValue;
	int		vertexIndex;
	int		faceIndex;
	int		pointInFaceIndex;
};

struct ColladaVertexWeight
{
	ColladaVertexWeight()
	{
		jointCount = 0;
	}
	
	void				AddWeight(int jointI, float32 weight);
	void				Normalize();

	int32					jointCount;
	int32					jointArray[10]; 
	float32					weightArray[10];
};


class ColladaPolygonGroup
{
public:
	ColladaPolygonGroup(ColladaMesh * _mesh, FCDGeometryPolygons * _polygons, ColladaVertexWeight * wertexWeightArray);
	~ColladaPolygonGroup();

	void				Render(ColladaMaterial * material);
	
	inline fstring		GetMaterialSemantic() { return materialSemantic; }; 
	inline int			GetTriangleCount() { return triangleCount; };
	inline std::vector<ColladaVertex> & GetVertices() { return unoptimizedVerteces; }
	inline std::vector<int>	& GetIndices() { return indexArray; };
	
	std::vector<ColladaVertex>	unoptimizedVerteces;
	std::vector<ColladaVertex>	skinVerteces;

	ColladaMesh	* parentMesh;
private:
	bool						skinAnimation;

	FCDGeometryPolygons *		polygons;
	
	int							triangleCount;	
	
	void				RenderMesh();


	std::vector<int>			indexArray;

	int							renderListId;
	AABBox3						bbox;

	fstring						materialSemantic;
};

};

#endif // __COLLADALOADER_COLLADAPOLYGONGROUP_H__


