/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Scene3D/SceneFile.h"
#include "Scene3D/SceneNode3d.h"
#include "Scene3D/MeshInstanceNode.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "Render/3D/AnimatedMesh.h"
#include "Scene3D/PathManip.h"
#include "Scene3D/SkeletonNode.h"
#include "Scene3D/BoneNode.h"
#include "Scene3D/Camera.h"
#include "Scene3D/SceneNodeAnimationList.h"
#include "Utils/StringFormat.h"

namespace DAVA
{

void ReadString(FILE * fp, char * res, int maxSize);  // XCode 4 fix
void ReadString(FILE * fp, char * res, int maxSize)
{
	int pos = 0;
	while(pos < maxSize)
	{
		char c;
		fread(&c, 1, 1, fp);
		res[pos++] = c;
		if (c == 0)break;
	}
}
	
SceneFile::Header::Header()
{
	descriptor[0] = 'D'; descriptor[1] = 'V';
	descriptor[2] = 'S'; descriptor[3] = 'C';
	
	version = 100;
	textureCount = 0;			
	materialCount = 0;
	staticMeshCount = 0;

	lightCount = 0;
	cameraCount = 0;
}
	
SceneFile::SceneFile()
{
	sceneFP = 0;
	scene = 0;
	currentSkeletonNode = 0;
	debugLogEnabled = false;
}
	
void SceneFile::SetDebugLog(bool _debugLogEnabled)
{
	debugLogEnabled = _debugLogEnabled;
}

bool SceneFile::LoadScene( const char * filename, Scene * _scene, bool relToBundle)
{
	scene = _scene;
	String fname(filename);
    rootNodePath = fname;
    
    textureIndexOffset = scene->GetTextureCount();
	staticMeshIndexOffset = scene->GetStaticMeshCount();
	animatedMeshIndexOffset = scene->GetAnimatedMeshCount();
	materialIndexOffset = scene->GetMaterialCount();
	cameraIndexOffset = scene->GetCameraCount();
	animationIndexOffset = scene->GetAnimationCount();
    

	sceneFP = File::Create(fname, File::OPEN | File::READ);
	if (!sceneFP)return false;

	// get scene path, store it to add to texture paths
	scenePath = PathManip(filename).GetPath();

	Logger::Debug("scene start load: path = %s\n", scenePath.c_str());
	
	SceneFile::Header header;
	sceneFP->Read(&header, sizeof(SceneFile::Header));
	
	if (debugLogEnabled)Logger::Debug("File version: %d\n", header.version);
	if (debugLogEnabled)Logger::Debug("Texture count: %d\n", header.textureCount);
	if (debugLogEnabled)Logger::Debug("Material count: %d\n", header.materialCount);
	if (debugLogEnabled)Logger::Debug("Static Mesh count: %d\n", header.staticMeshCount);
	if (debugLogEnabled)Logger::Debug("Animated Mesh count: %d\n", header.animatedMeshCount);
	if (debugLogEnabled)Logger::Debug("Cameras count: %d\n", header.cameraCount);
	if (debugLogEnabled)Logger::Debug("Node animations count: %d\n", header.nodeAnimationsCount);
	if (debugLogEnabled)Logger::Debug("Lights count: %d\n", header.lightCount);
	
	for (uint32 textureIndex = 0; textureIndex < header.textureCount; ++textureIndex)
	{
		ReadTexture();
		if (sceneFP->IsEof())return false;
	}

	for (uint32 materialIndex = 0; materialIndex < header.materialCount; ++materialIndex)
	{
		ReadMaterial();
		if (sceneFP->IsEof())return false;
	}
	
	for (uint32 staticMeshIndex = 0; staticMeshIndex < header.staticMeshCount; ++staticMeshIndex)
	{
		ReadStaticMesh();
		if (sceneFP->IsEof())return false;
	}
	
	for (uint32 animatedMeshIndex = 0; animatedMeshIndex < header.animatedMeshCount; ++animatedMeshIndex)
	{
		ReadAnimatedMesh();
		if (sceneFP->IsEof())return false;
	}
	
	for (uint32 lightIndex = 0; lightIndex < header.lightCount; ++lightIndex)
	{
		ReadLight();
		if (sceneFP->IsEof())return false;
	}
	
	for (uint32 camIndex = 0; camIndex < header.cameraCount; ++camIndex)
	{
		ReadCamera();
		if (sceneFP->IsEof())return false;
	}
	
	for (int32 animationIndex = 0; animationIndex < (int32)header.nodeAnimationsCount; ++animationIndex)
	{
		ReadAnimation();
		if (sceneFP->IsEof())return false;
	}
	
	ReadSceneGraph();
	
	for (int32 animatedMeshIndex = 0; animatedMeshIndex < (int32)header.animatedMeshCount; ++animatedMeshIndex)
	{
		AnimatedMesh * aMesh = scene->GetAnimatedMesh(animatedMeshIndex + animatedMeshIndexOffset);
		aMesh->RestoreBonesFromNames();
	}
	// Binding of animations to scene nodes 
	for (int32 animationIndex = 0; animationIndex < (int32)header.nodeAnimationsCount; ++animationIndex)
	{
		SceneNodeAnimationList * aList = scene->GetAnimation(animationIndex + animationIndexOffset);
		for (int32 k = 0; k < (int32)aList->animations.size(); ++k)
		{
			SceneNodeAnimation * anim = aList->animations[k];
			if (!anim)
			{
				if (debugLogEnabled)Logger::Debug("*** ERROR: animation: %d can't find anim: %s\n", animationIndex, aList->GetName().c_str());
				continue;
			}
			String & name = anim->bindName;
			SceneNode * bindNode = rootNode->FindByName(name);
			anim->SetBindNode(bindNode);
			if (!bindNode)
			{
				if (debugLogEnabled)Logger::Debug("*** ERROR: animation: %d can't find bind node: %s\n", animationIndex, name.c_str());
			}
		}
	}
	SafeRelease(sceneFP);
	
	Logger::Debug("scene loaded");
	return true;
}

bool SceneFile::SaveScene(const char * filename)
{
	
	return true;
};
	
bool SceneFile::ReadTexture()
{
    bool mipMapsEnabled = Texture::IsMipmapGenerationEnabled();
    Texture::EnableMipmapGeneration();
    
	SceneFile::TextureDef textureDef;
	sceneFP->Read(&textureDef.id, sizeof(textureDef.id));
	sceneFP->ReadString(textureDef.name, 512);
	
	// Texture * texture = Texture::CreateFromPNG(Format("XResources//%s//", textureDef.name));
	String tname = textureDef.name;
	if (tname.c_str()[0] == '/')
	{
		tname = tname.substr(1, tname.length()-1);
	}

	tname = scenePath + tname;
	
	DAVA::Texture * texture = DAVA::Texture::CreateFromFile(tname);//textureDef.name);//0;
	if (texture)
	{
		if ((texture->format != DAVA::Texture::FORMAT_PVR4) &&
			(texture->format != DAVA::Texture::FORMAT_PVR2))
		{
			texture->GenerateMipmaps();
		}
		else 
		{
			texture->UsePvrMipmaps();
		}

		texture->SetWrapMode(Texture::WRAP_REPEAT, Texture::WRAP_REPEAT);
//		texture->SetWrapMode(Texture::WRAP_CLAMP, Texture::WRAP_CLAMP);
		scene->AddTexture(texture);
        SafeRelease(texture);
	}else
	{
		Logger::Debug("*** error reading texture: %s\n", textureDef.name);
        uint8 * textureData = new uint8[64 * 64 * 4]; 
        for (int32 k = 0; k < 64 * 64 * 4; k += 4)
        {
            textureData[k + 0] = 0xFF; 
            textureData[k + 1] = 0x00; 
            textureData[k + 2] = 0xFF; 
            textureData[k + 3] = 0xFF; 
        }
        Texture * tex = Texture::CreateFromData(Texture::FORMAT_RGBA8888, textureData, 64, 64);
        scene->AddTexture(tex);
        tex->GenerateMipmaps();
        SafeRelease(tex);
	}
	
	if (debugLogEnabled)Logger::Debug("- Texture: %s %d\n", textureDef.name, textureDef.id);
    
    if (!mipMapsEnabled)
        Texture::DisableMipmapGeneration();
    
	return true;
}
	
bool SceneFile::ReadMaterial()
{
	SceneFile::MaterialDef materialDef; 
	sceneFP->Read(&materialDef.id, sizeof(materialDef.id));
	sceneFP->ReadString(materialDef.name, 512);

	sceneFP->Read(&materialDef.ambient, sizeof(materialDef.ambient));
	sceneFP->Read(&materialDef.diffuse, sizeof(materialDef.diffuse));
	sceneFP->Read(&materialDef.diffuseTextureId, sizeof(materialDef.diffuseTextureId));
	sceneFP->Read(&materialDef.emission, sizeof(materialDef.emission));
	sceneFP->Read(&materialDef.indexOfRefraction, sizeof(materialDef.indexOfRefraction));
	sceneFP->Read(&materialDef.reflective, sizeof(materialDef.reflective));
	sceneFP->Read(&materialDef.reflectivity, sizeof(materialDef.reflectivity));
	sceneFP->Read(&materialDef.shininess, sizeof(materialDef.shininess));
	sceneFP->Read(&materialDef.specular, sizeof(materialDef.specular));
	sceneFP->Read(&materialDef.transparency, sizeof(materialDef.transparency));
	sceneFP->Read(&materialDef.transparent, sizeof(materialDef.transparent));

	Material * mat = new Material(scene);

	mat->ambient = materialDef.ambient;
	mat->diffuse = materialDef.diffuse;
	
	if (materialDef.diffuseTextureId < scene->GetTextureCount())
	{	
		mat->diffuseTexture = scene->GetTexture(materialDef.diffuseTextureId + textureIndexOffset);
	}else 
	{
		mat->diffuseTexture = 0;
	}
	if (debugLogEnabled)Logger::Debug("- Material: %s %d diffuseTexture: %d tp: 0x%08x\n", materialDef.name, materialDef.id, materialDef.diffuseTextureId, mat->diffuseTexture);
	

	mat->emission = materialDef.emission;
	mat->indexOfRefraction = materialDef.indexOfRefraction;
	mat->SetName(materialDef.name);
	mat->reflective = materialDef.reflective;
	// TODO for reflective texture
	//mat->reflectiveTexture = materialDef.r
	mat->reflectivity = materialDef.reflectivity;
	// TODO for shader
	mat->shininess = materialDef.shininess;
	mat->specular = materialDef.specular;
	mat->transparency = materialDef.transparency;
	mat->transparent = materialDef.transparent;
	
	scene->AddMaterial(mat);
	SafeRelease(mat);

	return true;
}
	
bool SceneFile::ReadStaticMesh()
{
	uint32 polyGroupCount;
	sceneFP->Read(&polyGroupCount, sizeof(uint32));
	if (debugLogEnabled)Logger::Debug("- Static Mesh: %d\n", polyGroupCount);
	
	StaticMesh * mesh = new StaticMesh(scene);
	mesh->Create(polyGroupCount);
	for (uint32 polyGroupIndex = 0; polyGroupIndex < polyGroupCount; polyGroupIndex++)
	{
		uint32 vertexCount, indexCount;
		sceneFP->Read(&vertexCount, sizeof(uint32));
		sceneFP->Read(&indexCount, sizeof(uint32));
		if (debugLogEnabled)Logger::Debug("--- vertex count: %d\n", vertexCount);
		if (debugLogEnabled)Logger::Debug("--- index count: %d\n", indexCount);
		
		PolygonGroup * polygonGroup = mesh->GetPolygonGroup(polyGroupIndex);
		polygonGroup->AllocateData(EVF_VERTEX | /*EVF_NORMAL | EVF_COLOR | */EVF_TEXCOORD0, vertexCount, indexCount, 1);
		
		for (uint32 v = 0; v < vertexCount; ++v)
		{
			Vector3 position, normal; 
			Vector2 texCoords0, texCoords1;
			sceneFP->Read(&position, sizeof(Vector3));
			sceneFP->Read(&normal, sizeof(Vector3));
			sceneFP->Read(&texCoords0, sizeof(Vector2));
			sceneFP->Read(&texCoords1, sizeof(Vector2));
		
            if (polygonGroup->GetFormat() & EVF_VERTEX)
                polygonGroup->SetCoord(v, position);
            if (polygonGroup->GetFormat() & EVF_NORMAL)
                polygonGroup->SetNormal(v, normal);
            if (polygonGroup->GetFormat() & EVF_TEXCOORD0)
                polygonGroup->SetTexcoord(0, v, texCoords0);
		}
		
        polygonGroup->BuildVertexBuffer();

		int * indices = new int[indexCount];
		sceneFP->Read(indices, sizeof(int) * indexCount);
		for (uint32 i = 0; i < indexCount; ++i)
        {
			polygonGroup->SetIndex(i, indices[i]);
        }
        delete [] indices;
	}
	
	scene->AddStaticMesh(mesh);
	SafeRelease(mesh);
	
	return true;
}
	
bool SceneFile::ReadAnimatedMesh()
{
	int polyGroupCount;
	sceneFP->Read(&polyGroupCount, sizeof(int));
	if (debugLogEnabled)Logger::Debug("- Animated Mesh: %d\n", polyGroupCount);
	
	AnimatedMesh * mesh = new AnimatedMesh(scene);
	mesh->Create(polyGroupCount);
	for (int polyGroupIndex = 0; polyGroupIndex < polyGroupCount; polyGroupIndex++)
	{
		int vertexCount, indexCount;
		sceneFP->Read(&vertexCount, sizeof(int));
		sceneFP->Read(&indexCount, sizeof(int));
		if (debugLogEnabled)Logger::Debug("--- vertex count: %d\n", vertexCount);
		if (debugLogEnabled)Logger::Debug("--- index count: %d\n", indexCount);
		
		PolygonGroup * polygonGroup = mesh->GetPolygonGroup(polyGroupIndex);
		polygonGroup->AllocateData(EVF_VERTEX | EVF_NORMAL | EVF_COLOR | EVF_TEXCOORD0 | EVF_JOINTWEIGHT, vertexCount, indexCount, 1);
		
		for (int v = 0; v < vertexCount; ++v)
		{
			Vector3 position, normal; 
			Vector2 texCoords0, texCoords1;
			int32 jointIdx;
			float32 weight;
			int32 jointCount;
			
			sceneFP->Read(&position, sizeof(Vector3));
			sceneFP->Read(&normal, sizeof(Vector3));
			sceneFP->Read(&texCoords0, sizeof(Vector2));
			sceneFP->Read(&texCoords1, sizeof(Vector2));
			
			sceneFP->Read(&jointCount, sizeof(int32));
			for (int k = 0; k < jointCount; ++k)
			{
				sceneFP->Read(&jointIdx, sizeof(int32));
				sceneFP->Read(&weight, sizeof(float32));
				polygonGroup->SetJointIndex(v, k, jointIdx);
				polygonGroup->SetWeight(v, k, weight);
			}
			
			polygonGroup->SetCoord(v, position);
			polygonGroup->SetNormal(v, normal);
			polygonGroup->SetTexcoord(0, v, texCoords0);
			polygonGroup->SetJointCount(v, jointCount);
		}
        
        polygonGroup->BuildVertexBuffer();
		
		int32 * indices = new int32[indexCount];
		sceneFP->Read(indices, indexCount *  sizeof(int32));
		for (int i = 0; i < indexCount; ++i)
        {
			polygonGroup->SetIndex(i, indices[i]);
        }
        delete [] indices;
		
		polygonGroup->CreateBaseVertexArray();
	}
	Matrix4 bindShapeMatrix;
	sceneFP->Read(&bindShapeMatrix, sizeof(Matrix4));
	mesh->bindShapeMatrix = bindShapeMatrix;
	scene->AddAnimatedMesh(mesh);
	
	int32  boneCount = 0;
	sceneFP->Read(&boneCount, sizeof(int32));
	for (int32 bone = 0; bone < boneCount; ++bone)
	{
		char8 boneNodeName[512];
		sceneFP->ReadString(boneNodeName, 512);
		mesh->boneNames.push_back(boneNodeName);
	}
	
	SafeRelease(mesh);
	return true;
}


bool SceneFile::ReadSceneNode(SceneNode * parentNode, int level)
{
	if (sceneFP->IsEof())return false;

	int32 id;
	char8 name[512];
	sceneFP->Read(&id, sizeof(id));
	sceneFP->ReadString(name, 512);
	
	SceneFile::SceneNodeDef def;
	sceneFP->Read(&def, sizeof(def));	
	
	char8 nodeType[64];
	nodeType[0] = 0;
	
	
	SceneNode * node = 0;
	if (def.nodeType == SceneNodeDef::SCENE_NODE_BASE)
	{
		node = new SceneNode3d(scene);
        node->SetDefaultLocalTransform(def.localTransform);
		node->SetLocalTransform(def.localTransform);
		node->SetName(name);
        if (parentNode != scene) 
        {
            parentNode->AddNode(node);
        }
		strcpy(nodeType, "scene node");
	}else if (def.nodeType == SceneNodeDef::SCENE_NODE_SKELETON)
	{
		Matrix4 inverse0;
		sceneFP->Read(&inverse0, sizeof(Matrix4));
		
		
		currentSkeletonNode = new SkeletonNode(scene);
		node = currentSkeletonNode;
        node->SetDefaultLocalTransform(def.localTransform);
		node->SetLocalTransform(def.localTransform);
		currentSkeletonNode->inverse0Matrix = inverse0;
		node->SetName(name);
        if (parentNode != scene) 
        {
            parentNode->AddNode(node);
        }
		strcpy(nodeType, "skeleton node");
	}else if (def.nodeType == SceneNodeDef::SCENE_NODE_BONE)
	{
		Matrix4 inverse0;
		sceneFP->Read(&inverse0, sizeof(Matrix4));

		BoneNode * boneNode = new BoneNode(scene, currentSkeletonNode);
		node = boneNode;
        node->SetDefaultLocalTransform(def.localTransform);
		node->SetLocalTransform(def.localTransform);
		node->SetName(name);

		boneNode->inverse0Matrix = inverse0;    // TODO: make inverse0Matrix protected
        if (parentNode != scene) 
        {
            parentNode->AddNode(node);
        }
		strcpy(nodeType, "bone node");
	}else if (def.nodeType == SceneNodeDef::SCENE_NODE_CAMERA)
	{
		int32 camIndex = -1;
		sceneFP->Read(&camIndex, sizeof(int32));
		
		Camera * cam = scene->GetCamera(camIndex + cameraIndexOffset);
		node = cam;//new Camera(scene);
        
        node->SetDefaultLocalTransform(def.localTransform);
		node->SetLocalTransform(def.localTransform);
		node->SetName(name);
        
        if (parentNode != scene) 
        {
            parentNode->AddNode(node);
        }
		cam->RestoreOriginalSceneTransform();
		strcpy(nodeType, "camera node");
	}
	else if (def.nodeType == SceneNodeDef::SCENE_NODE_MESH || def.nodeType == SceneNodeDef::SCENE_NODE_ANIMATED_MESH)
	{
		if (def.nodeType == SceneNodeDef::SCENE_NODE_MESH)strcpy(nodeType, "static mesh node");
		if (def.nodeType == SceneNodeDef::SCENE_NODE_ANIMATED_MESH)strcpy(nodeType, "animated mesh node");
		
		MeshInstanceNode* meshNode = new MeshInstanceNode(scene);
		node = meshNode;
        
        node->SetDefaultLocalTransform(def.localTransform);
		node->SetLocalTransform(def.localTransform);
		node->SetName(name);
        
		int pgInstancesCount = 0;
		sceneFP->Read(&pgInstancesCount, sizeof(int));

		for (int k = 0; k < pgInstancesCount; ++k)
		{
			int meshIndex, polyGroupIndex, materialIndex;
			sceneFP->Read(&meshIndex, sizeof(int32));
			sceneFP->Read(&polyGroupIndex, sizeof(int32));
			sceneFP->Read(&materialIndex, sizeof(int32));

			if (debugLogEnabled)Logger::Debug("%s polygon group: meshIndex:%d polyGroupIndex:%d materialIndex:%d\n", GetIndentString('-', level + 1), meshIndex, polyGroupIndex, materialIndex); 
		
			if (def.nodeType == SceneNodeDef::SCENE_NODE_MESH)
			{
				StaticMesh * staticMesh = scene->GetStaticMesh(meshIndex + staticMeshIndexOffset);
				meshNode->AddPolygonGroup(staticMesh, polyGroupIndex, scene->GetMaterial(materialIndex + materialIndexOffset));
			}else
			{
				// add animated mesh
				AnimatedMesh * animatedMesh = scene->GetAnimatedMesh(meshIndex + animatedMeshIndexOffset);
				meshNode->AddPolygonGroup(animatedMesh, polyGroupIndex, scene->GetMaterial(materialIndex + materialIndexOffset));
			}
		}
        if (parentNode != scene) 
        {
            parentNode->AddNode(node);
        }
	}
	if (debugLogEnabled)Logger::Debug("%s node: %s typeId: %d childCount: %d type: %s\n", GetIndentString('-', level), name, def.nodeType, def.childCount, nodeType); 
	
    if (parentNode == scene) 
    {
        scene->AddRootNode(node, rootNodePath);
        rootNode = node;
    }
		
	for (int k = 0; k < def.childCount; ++k)
	{
		if (!ReadSceneNode(node, level + 1))return false;
	}
	
	SafeRelease(node);
	
	// clear skeleton node 
	if (def.nodeType == SceneNodeDef::SCENE_NODE_SKELETON)
		currentSkeletonNode = 0;
	return true;
}
	
bool SceneFile::ReadSceneGraph()
{
	currentSkeletonNode = 0;
	ReadSceneNode(scene, 0);
	return true;
}

bool SceneFile::ReadCamera()
{
	Camera * cam = new Camera(scene);
	scene->AddCamera(cam);
	
	// read camera options
	
	CameraDef cd;
	sceneFP->Read(&cd, sizeof(CameraDef));
	
	cam->Setup(cd.fovy, 480.0f / 320.0f, cd.znear, cd.zfar, cd.ortho);
	SafeRelease(cam);
	return true;
}

bool SceneFile::ReadAnimation()
{
	SceneNodeAnimationList * animationList = new SceneNodeAnimationList(scene);
	
	char name[512];
	sceneFP->ReadString(name, 512);
	animationList->SetName(name);

	
	int nodeCount;
	sceneFP->Read(&nodeCount, sizeof(int32));
	
	if (debugLogEnabled)Logger::Debug("- scene node anim list: %s nodeCount: %d\n", name, nodeCount); 

	for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		sceneFP->ReadString(name, 512);
		
		float32 duration = 0;
		sceneFP->Read(&duration, sizeof(float32));
		
		int32 keyCount = 0;
		sceneFP->Read(&keyCount, sizeof(int32));

		SceneNodeAnimation * anim = new SceneNodeAnimation(keyCount);
		anim->SetBindName(name);	
		anim->SetDuration(duration); 
		if (debugLogEnabled)Logger::Debug("-- scene node %d anim: %s keyCount: %d duration: %f seconds\n", nodeIndex, name, keyCount, duration); 

		for (int k = 0; k < keyCount; ++k)
		{
			SceneNodeAnimationKey key;
			sceneFP->Read(&key.time, sizeof(float32));
			sceneFP->Read(&key.translation, sizeof(Vector3));
			sceneFP->Read(&key.rotation, sizeof(Quaternion));
			anim->SetKey(k, key);
			//printf("---- key: %f tr: %f %f %f q: %f %f %f %f\n", key.time, key.translation.x, key.translation.y, key.translation.z
			//	   , key.rotation.x, key.rotation.y, key.rotation.z, key.rotation.w); 
		}
		animationList->AddAnimation(anim);
		SafeRelease(anim);
	}
	scene->AddAnimation(animationList);
	SafeRelease(animationList);
	return true;
}
	
bool SceneFile::ReadLight()
{
	// read light options
	
	LightDef ld;
	sceneFP->Read(&ld, sizeof(LightDef));
	
	// TODO add light to light array
	
	return true;
}

	
};