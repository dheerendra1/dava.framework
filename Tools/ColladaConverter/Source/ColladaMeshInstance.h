#ifndef __COLLADALOADER_COLLADAMESHINSTANCE_H__
#define __COLLADALOADER_COLLADAMESHINSTANCE_H__

#include "ColladaIncludes.h"
#include "ColladaMesh.h"
#include "ColladaMaterial.h"
#include "ColladaPolygonGroupInstance.h"

namespace DAVA
{

class ColladaMeshInstance
{
public:
	ColladaMeshInstance(bool _animated);
	~ColladaMeshInstance();
	
	void Render();
	void AddPolygonGroupInstance(ColladaPolygonGroupInstance * instance);
	bool IsAnimated() { return animated; };

	std::vector<ColladaPolygonGroupInstance*> polyGroupInstances;
	
	
	FCDGeometryInstance * geometryInstance;
private:
	bool animated;
};

	
	
};

#endif // __COLLADALOADER_COLLADALIGHT_H__
