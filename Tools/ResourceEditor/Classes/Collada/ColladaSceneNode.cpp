#include "stdafx.h"
#include "ColladaSceneNode.h"

namespace DAVA
{

Matrix4 ConvertMatrix( FMMatrix44 & matrix )
{
	Matrix4 result;
	for (int k = 0; k < 4; ++k)
		for (int l = 0; l < 4; ++l)
			result._data[k][l] = matrix.m[k][l];;
	return result;
}
	
Matrix4 ConvertMatrixT( FMMatrix44 & matrix )
{
	Matrix4 result;
	for (int k = 0; k < 4; ++k)
		for (int l = 0; l < 4; ++l)
			result._data[k][l] = matrix.m[l][k];;
	return result;
}


ColladaSceneNode::ColladaSceneNode(ColladaScene * _scene, FCDSceneNode * _node)
{
	isJoint = false;
	originalNode = _node;
	localTransform.Identity();
	parent = 0;
	animation = 0;
	scene = _scene;
	inverse0.Identity();
}

ColladaSceneNode::~ColladaSceneNode()
{

}


void ColladaSceneNode::AddNode( ColladaSceneNode * node )
{
	childs.push_back(node);
	node->parent = this;
}

void ColladaSceneNode::AddMeshInstance( ColladaMeshInstance * meshInstance )
{
	meshInstances.push_back(meshInstance);
}

void ColladaSceneNode::PreProcessLights(ColladaLightState & state)
{
	Matrix4 localTransformTransposed = localTransform;

	glPushMatrix();
	glMultMatrixf(localTransformTransposed.data);
		

	for (int lightIndex = 0; lightIndex < (int)lights.size(); ++lightIndex)
	{
		ColladaLight * light = lights[lightIndex];		
		light->ApplyLight(state);
	}

	for (int c = 0; c < (int)childs.size(); ++c)
		childs[c]->PreProcessLights(state);
	
	glPopMatrix();
}

void ColladaSceneNode::Render(Matrix4 currentMatrix)
{
#ifdef COLLADA_GLUT_RENDER
	worldTransform =  localTransform * currentMatrix;
	//Matrix4 worldTransformCopy = worldTransform;
	//worldTransformCopy.Transpose(); */

	Matrix4 localTransformTransposed = localTransform;
	//localTransformTransposed.Transpose();

	glPushMatrix();
	glMultMatrixf(worldTransform.data);

	if (isJoint)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0.984375, 0.078125, 0.64453125);
		glutWireCube(0.5f);
		glEnable(GL_LIGHTING);
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	for (int m = 0; m < (int)meshInstances.size(); ++m)
	{
		meshInstances[m]->Render();
	}
	glPopMatrix();

	for (int c = 0; c < (int)childs.size(); ++c)
		childs[c]->Render(worldTransform);

#endif
}

void ColladaSceneNode::AddLight( ColladaLight * light )
{
	lights.push_back(light);
}

void ColladaSceneNode::MarkJoint()
{
	isJoint = true;	
}

void ColladaSceneNode::UpdateTransforms( float time )
{
	/*FMMatrix44 colladaLocalMatrix;
	colladaLocalMatrix = FMMatrix44::Identity;// = FMMatrix44::Identity(); 

	for (int t = 0; t < originalNode->GetTransformCount(); ++t)
	{
		FCDTransform * transform = originalNode->GetTransform(t);
		if (transform->IsAnimated()) // process all animations to make CalculateWorldTransform work
		{
			FCDAnimated * animated = transform->GetAnimated();
			animated->Evaluate(time);
		}
		
		if (transform->GetType() == FCDTransform::TRANSLATION)
		{
			FCDTTranslation * translation = dynamic_cast<FCDTTranslation*>(transform);
			FMVector3 point = FMVector3(0.0f, 0.0f, 0.0f);
			point = translation->GetTranslation();
			if (transform->IsAnimated()) 
			{
				FCDAnimationCurve* curve;

				// look for x animation
				curve = transform->GetAnimated()->FindCurve(".X");
				if (curve != 0) 
					point.x = curve->Evaluate(time);

				// look for y animation
				curve = transform->GetAnimated()->FindCurve(".Y");
				if (curve != 0) 
					point.y = curve->Evaluate(time);

				// look for z animation
				curve = transform->GetAnimated()->FindCurve(".Z");
				if (curve != 0) 
					point.z = curve->Evaluate(time);
			}
			colladaLocalMatrix = colladaLocalMatrix * FMMatrix44::TranslationMatrix(point);
		}else if (transform->GetType() == FCDTransform::ROTATION)
		{
			FCDTRotation * rotation = dynamic_cast<FCDTRotation*>(transform);
			FMVector3 axis = FMVector3(0.0f, 0.0f, 0.0f);
			float angle = 0;
			axis = rotation->GetAxis();
			angle = rotation->GetAngle();

			if (rotation->IsAnimated()) 
			{
				FCDAnimationCurve* curve;

				// look for x animation
				curve = rotation->GetAnimated()->FindCurve(".X");
				if (curve != 0) 
					axis.x = curve->Evaluate(time);

				// look for y animation
				curve = rotation->GetAnimated()->FindCurve(".Y");
				if (curve != 0) 
					axis.y = curve->Evaluate(time);

				// look for z animation
				curve = rotation->GetAnimated()->FindCurve(".Z");
				if (curve != 0) 
					axis.z = curve->Evaluate(time);

				// look for z animation
				curve = rotation->GetAnimated()->FindCurve(".ANGLE");
				if (curve != 0) 
					angle = curve->Evaluate(time);
			}
			colladaLocalMatrix = colladaLocalMatrix * FMMatrix44::AxisRotationMatrix(axis, angle * PI / 180.0f);
		}else
		{
			colladaLocalMatrix = colladaLocalMatrix * transform->ToMatrix();
		}

	}*/

	//colladaLocalMatrix = originalNode->ToMatrix();
	//localTransform = ConvertMatrix(colladaLocalMatrix);

	if (animation)
	{
		SceneNodeAnimationKey & key = animation->Intepolate(time);
		key.GetMatrix(localTransform);
	}else 
	{
// merged:
		FMMatrix44 colladaLocalMatrix = ColladaSceneNode::CalculateTransformForTime(originalNode, time);
		localTransform = ConvertMatrix(colladaLocalMatrix);
	}
	
	for (int c = 0; c < (int)childs.size(); ++c)
		childs[c]->UpdateTransforms(time); 
}
	
bool ColladaSceneNode::IsAnimated(FCDSceneNode * originalNode)
{
	for (int t = 0; t < originalNode->GetTransformCount(); ++t)
	{
		FCDTransform * transform = originalNode->GetTransform(t);
		if (transform->IsAnimated()) // process all animations to make CalculateWorldTransform work
		{
			return true;
		}
		if (transform->GetType() == FCDTransform::TRANSLATION)
		{
			FCDTTranslation * translation = dynamic_cast<FCDTTranslation*>(transform);
			if (translation->IsAnimated()) 
			{
				return true;
			}
		}else if (transform->GetType() == FCDTransform::ROTATION)
		{
			FCDTRotation * rotation = dynamic_cast<FCDTRotation*>(transform);
			if (rotation->IsAnimated()) 
			{
				return true;
			}
		}
	}
	return false;
}
	
FMMatrix44 ColladaSceneNode::CalculateTransformForTime(FCDSceneNode * originalNode, float32 time)
{
	FMMatrix44 colladaLocalMatrix;
	colladaLocalMatrix = FMMatrix44::Identity;// = FMMatrix44::Identity(); 
	
	for (int t = 0; t < originalNode->GetTransformCount(); ++t)
	{
		FCDTransform * transform = originalNode->GetTransform(t);
		if (transform->IsAnimated()) // process all animations to make CalculateWorldTransform work
		{
			FCDAnimated * animated = transform->GetAnimated();
			animated->Evaluate(time);
		}
		
		if (transform->GetType() == FCDTransform::TRANSLATION)
		{
			FCDTTranslation * translation = dynamic_cast<FCDTTranslation*>(transform);
			FMVector3 point = FMVector3(0.0f, 0.0f, 0.0f);
			point = translation->GetTranslation();
			if (transform->IsAnimated()) 
			{
				FCDAnimationCurve* curve;
				
				// look for x animation
				curve = transform->GetAnimated()->FindCurve(".X");
				if (curve != 0) 
					point.x = curve->Evaluate(time);
					
				// look for y animation
				curve = transform->GetAnimated()->FindCurve(".Y");
				if (curve != 0) 
					point.y = curve->Evaluate(time);
						
				// look for z animation
				curve = transform->GetAnimated()->FindCurve(".Z");
				if (curve != 0) 
					point.z = curve->Evaluate(time);
			}
			colladaLocalMatrix = colladaLocalMatrix * FMMatrix44::TranslationMatrix(point);
		}else if (transform->GetType() == FCDTransform::ROTATION)
		{
			FCDTRotation * rotation = dynamic_cast<FCDTRotation*>(transform);
			FMVector3 axis = FMVector3(0.0f, 0.0f, 0.0f);
			float angle = 0;
			axis = rotation->GetAxis();
			angle = rotation->GetAngle();
			
			if (rotation->IsAnimated()) 
			{
				FCDAnimationCurve* curve;
				
				// look for x animation
				curve = rotation->GetAnimated()->FindCurve(".X");
				if (curve != 0) 
					axis.x = curve->Evaluate(time);
					
				// look for y animation
				curve = rotation->GetAnimated()->FindCurve(".Y");
				if (curve != 0) 
					axis.y = curve->Evaluate(time);
						
				// look for z animation
				curve = rotation->GetAnimated()->FindCurve(".Z");
				if (curve != 0) 
					axis.z = curve->Evaluate(time);
							
							// look for z animation
				curve = rotation->GetAnimated()->FindCurve(".ANGLE");
				if (curve != 0) 
					angle = curve->Evaluate(time);
			}
			colladaLocalMatrix = colladaLocalMatrix * FMMatrix44::AxisRotationMatrix(axis, angle * PI / 180.0f);
		}else
		{
			colladaLocalMatrix = colladaLocalMatrix * transform->ToMatrix();
		}
		
	}
	return colladaLocalMatrix;
}

SceneNodeAnimationKey ColladaSceneNode::ExportAnimationKey(FCDSceneNode * originalNode, float32 time)
{
	FMMatrix44 colladaLocalMatrix = ColladaSceneNode::CalculateTransformForTime(originalNode, time);
	Matrix4 lt = ConvertMatrix(colladaLocalMatrix);
	
	SceneNodeAnimationKey key;
	key.time = time;
	key.translation.x = lt._30;
	key.translation.y = lt._31;
	key.translation.z = lt._32;
	key.rotation.Construct(lt);
	return key;
}
	
SceneNodeAnimation * ColladaSceneNode::ExportNodeAnimation(FCDSceneNode * originalNode, float32 startTime, float32 endTime, float32 fps)
{
	if (!IsAnimated(originalNode))return 0;
		
	
	int frameCount = (endTime - startTime) * fps;
	float32 t0 = startTime;
	float32 tInc = (endTime - startTime) / (float)frameCount;
	
	
	std::vector<SceneNodeAnimationKey> keys;
	
	for (int k = 0; k < frameCount; ++k)
	{
		SceneNodeAnimationKey key = ExportAnimationKey(originalNode, t0);
//		duplicate keys
//		if (keys.size() != 0)
//		{
//			SceneNodeAnimationKey & keyPrev = keys[keys.size() - 1];
//			if ((keyPrev.translation == key.translation)
//				&& (keyPrev.rotation == key.rotation))
//			{
//				keyPrev.time = t0;
//			}else
//			{
//				keys.push_back(key);
//			}
//		}else
		{
			keys.push_back(key);
		}
		t0 += tInc;
	}
	
	
	SceneNodeAnimation * anim = new SceneNodeAnimation(keys.size());
	anim->SetDuration(endTime);
	for (int k = 0; k < keys.size(); ++k)
	{
		anim->SetKey(k, keys[k]);
	}
	
	printf("= keys export: frameCount: %d keyCount:%d compression: %f\n", frameCount, keys.size(), 100.0f * (float32)keys.size() / (float32)frameCount);
		 
	ExportAnimationKey(originalNode, 0);
	return anim;
}

ColladaSceneNode * ColladaSceneNode::FindNode(const fstring & daeId)
{
	if (originalNode->GetDaeId() == daeId)return this;
	else
	{
		for (int k = 0; k < childs.size(); ++k)
		{
			ColladaSceneNode * node = childs[k]->FindNode(daeId);
			if (node != 0)return node;
		}
	}
	return 0;
}

void ColladaSceneNode::SetAnimation(SceneNodeAnimation * _animation, bool recursive)
{
	animation = _animation;
	if (recursive)
	{
		for (int k = 0; k < childs.size(); ++k)
		{
			ColladaSceneNode * node = childs[k];
			node->SetAnimation(_animation, recursive);
		}
	}
}
	
void ColladaSceneNode::AddCamera(ColladaCamera * cam)
{
	cameras.push_back(cam);
}
};
