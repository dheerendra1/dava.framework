#ifndef __COLLADALOADER_COLLADAMESH_H__
#define __COLLADALOADER_COLLADAMESH_H__

#include "ColladaIncludes.h"
#include "ColladaPolygonGroup.h"

namespace DAVA
{

class ColladaMesh
{
public:
	ColladaMesh(FCDGeometryMesh * _mesh, ColladaVertexWeight * wertexWeightArray);
	~ColladaMesh();

	inline int		GetPolygonGroupCount() { return (int)polygons.size(); }
	inline ColladaPolygonGroup * GetPolygonGroup(int index) { return polygons[index]; };


	FCDGeometryMesh *						mesh;


	fstring							name;
	std::vector< ColladaPolygonGroup * > polygons; 
};
};

#endif // __COLLADALOADER_COLLADAMESH_H__


