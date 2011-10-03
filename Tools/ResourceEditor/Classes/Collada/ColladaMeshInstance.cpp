#include "stdafx.h"
#include "ColladaMeshInstance.h"

namespace DAVA
{

ColladaMeshInstance::ColladaMeshInstance(bool _animated)
{
	animated = _animated;
	geometryInstance = 0;
}

void ColladaMeshInstance::AddPolygonGroupInstance( ColladaPolygonGroupInstance * instance )
{
	polyGroupInstances.push_back(instance);
}

void ColladaMeshInstance::Render()
{
	for (int i = 0; i < (int)polyGroupInstances.size(); ++i)
	{
		polyGroupInstances[i]->Render();	
	}
}

};