#ifndef __COLLADALOADER_COLLADASCENE_H__
#define __COLLADALOADER_COLLADASCENE_H__

#include "ColladaIncludes.h"
#include "ColladaSceneNode.h"
#include "ColladaMesh.h"
#include "ColladaLight.h"
#include "ColladaMaterial.h"
#include "ColladaMeshInstance.h"
#include "ColladaAnimatedMesh.h"
#include "ColladaCamera.h"
#include "ColladaAnimation.h"

namespace DAVA
{

Matrix4 ConvertMatrix(FMMatrix44 & matrix);



class ColladaScene
{
public:	
	ColladaScene(FCDSceneNode * rootNode);
	~ColladaScene();

	void	ExportAnimations(ColladaAnimation * anim, FCDSceneNode * currentNode, float32 anStart, float32 anEnd);
	void	ExportScene(FCDSceneNode * fcdNode = 0,  ColladaSceneNode * node = 0);
	void	Render();
	
	void	RenderAxes();
	void	RenderGrid();

	void	SetupDefaultLights();

	ColladaMeshInstance * CreateMeshInstance(ColladaMesh * mesh, FCDGeometryInstance  * geometryInstance, bool animated);

	ColladaMesh * FindMeshWithName(const fm::string & name);
	ColladaTexture * FindTextureWithName(const fm::string & name); 
	ColladaMaterial * FindMaterialWithName(const fm::string & name); 
	ColladaLight * FindLightWithName(const fm::string & name); 
	ColladaAnimatedMesh * FindAnimatedMeshWithName(const fm::string & name); 
	
	int	FindMaterialIndex(ColladaMaterial * material);
	int	FindMeshIndex(ColladaMesh * mesh);
	bool FindPolyGroupIndex(ColladaPolygonGroup * group, int & meshIndex, int & polygroupIndex);
	
	ColladaCamera * FindCameraWithName( const fm::string & name );
	int	FindCameraIndex(ColladaCamera * cam);

	std::vector<ColladaMesh*> colladaMeshes;
	std::vector<ColladaLight*> colladaLights;
	std::vector<ColladaMaterial*> colladaMaterials;
	std::vector<ColladaTexture*> colladaTextures;
	std::vector<ColladaAnimatedMesh*> colladaAnimatedMeshes;

	std::vector<ColladaLight*> colladaActiveSceneLights;
	std::vector<ColladaCamera*> colladaCameras;
	
	std::vector<ColladaAnimation*> colladaAnimations;
	
	void SetExclusiveAnimation(int32 index);
	
	
	float32 animationStartTime;
	float32 animationEndTime;
	
	int					exportSceneLevel;
	ColladaSceneNode *	rootNode;
	FCDSceneNode *		rootFCDNode;
	float				currentTime;
};

};

#endif // __COLLADALOADER_COLLADASCENE_H__


