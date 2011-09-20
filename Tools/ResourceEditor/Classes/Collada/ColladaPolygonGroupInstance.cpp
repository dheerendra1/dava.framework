#include "stdafx.h"
#include "ColladaPolygonGroupInstance.h"

namespace DAVA
{

ColladaPolygonGroupInstance::ColladaPolygonGroupInstance( ColladaPolygonGroup * _polyGroup, ColladaMaterial * _material )
{
	polyGroup = _polyGroup;
	material = _material;
}

void ColladaPolygonGroupInstance::Render()
{
	polyGroup->Render(material);
}

};