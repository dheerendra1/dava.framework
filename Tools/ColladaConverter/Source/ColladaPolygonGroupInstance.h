#ifndef __COLLADALOADER_COLLADAPOLYGONGROUPINSTANCE_H__
#define __COLLADALOADER_COLLADAPOLYGONGROUPINSTANCE_H__

#include "ColladaIncludes.h"
#include "ColladaMesh.h"
#include "ColladaMaterial.h"

namespace DAVA
{

class ColladaPolygonGroupInstance
{
public:
	ColladaPolygonGroupInstance(ColladaPolygonGroup * mesh, ColladaMaterial * material);
	~ColladaPolygonGroupInstance();

	void Render();

	ColladaPolygonGroup	* polyGroup;
	ColladaMaterial		* material;
};

};

#endif // __COLLADALOADER_COLLADAPOLYGONGROUPINSTANCE_H__
