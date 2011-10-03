#include "stdafx.h"
#include "ColladaMesh.h"

namespace DAVA
{

ColladaMesh::ColladaMesh( FCDGeometryMesh * _mesh , ColladaVertexWeight * vertexWeightArray)
:	mesh(_mesh)
{
	name = _mesh->GetDaeId();
	printf("- mesh: %s\n", mesh->GetDaeId().c_str());

	if (!mesh->IsTriangles()) FCDGeometryPolygonsTools::Triangulate(mesh);
	

	int polygonsCount = (int)mesh->GetPolygonsCount();
	for (int p = 0; p < polygonsCount; ++p)
	{
		FCDGeometryPolygons * poly = mesh->GetPolygons(p);
		ColladaPolygonGroup * polyGroup = new ColladaPolygonGroup(this, poly, vertexWeightArray);
		polygons.push_back(polyGroup);
		
		printf("- polygroup: 0x%08x\n", polyGroup);
	}
}

ColladaMesh::~ColladaMesh()
{

}
};
