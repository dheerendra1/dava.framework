#ifndef __COLLADALOADER_COLLADADOCUMENT_H__
#define __COLLADALOADER_COLLADADOCUMENT_H__

#include "ColladaIncludes.h"
#include "ColladaMesh.h"
#include "ColladaScene.h"

#include "DAVAEngine.h"

namespace DAVA
{

class ColladaDocument
{
public:
	bool	Open(const char * filename);
	bool	ExportAnimations(const char * filename);
	bool	ExportNodeAnimations(FCDocument * exportDoc, FCDSceneNode * exportNode);
	
	void	Close();
	
	void	Render();
	void	LoadTextures();
	
	bool	IsEmptyNode(ColladaSceneNode * node);
	
	void	SaveScene(const String & scenePath, const String & sceneName);

	void	WriteTexture(SceneFile::TextureDef * texture);
	void	WriteMaterial(SceneFile::MaterialDef * material);
	void	WriteLight(SceneFile::LightDef * light);
	
	//StaticMesh * ConvertMesh(ColladaMesh * mesh);
	void	WriteStaticMesh(ColladaMesh * mesh, int32 meshIndex);
	void	WriteAnimatedMesh(ColladaAnimatedMesh * mesh, int32 animatedMeshIndex);
	
	void	WriteSceneNode(ColladaSceneNode * node, int32 & nodeId, int32 parentId, int32 level);
	void	WriteMeshNode(ColladaMeshInstance * node, int32 & nodeId, int32 parentId, int32 level, int32 instanceIdx);
	void	WriteCameraNode(ColladaCamera * node, int32 & globalNodeId, int32 parentId, int32 level);
	
	void	WriteCamera(ColladaCamera * cam, int32 i);
	void	WriteLight(ColladaLight * light, int32 i);
	
	void	WriteNodeAnimationList(ColladaAnimation * animation);
	
	void	GetAnimationTimeInfo(FCDocument * document, float32 & timeStart, float32 & timeEnd);
	
	FILE	* sceneFP;
	ColladaScene *				colladaScene;
private:
	FCDocument *				document; 
};
};

#endif 