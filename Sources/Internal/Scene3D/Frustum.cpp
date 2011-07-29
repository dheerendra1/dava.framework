/*======== (C) Copyright 1999 LogGames, L.L.C. All rights reserved. ==========

 The copyright to the contents herein is the property of LogGames, L.L.C.
 The contents may be used and/or copied only with the written permission of
 LogGames, L.L.C., or in accordance with the terms and conditions stipulated in
 the agreement/contract under which the contents have been supplied.

 Revision History
	* Created by Borodovsky Vitaliy 
=============================================================================*/
#include "Render/RenderHelper.h"
#include "Scene3D/Frustum.h"

namespace DAVA
{

Frustum::Frustum()
{
	planeCount = 0;
}
Frustum::~Frustum()
{
}

//! \brief Set view frustum from matrix information
//! \param viewProjection view * projection matrix
void Frustum::Set(const Matrix4 & viewProjection)
{
	planeArray.resize(6);
	
#define SETUP_PLANE(plane, x1, x2, x3, x4) \
	planeArray[plane].n.x = -(x1); planeArray[plane].n.y = -(x2); \
	planeArray[plane].n.z = -(x3); planeArray[plane].d = -(x4); \
	planeArray[plane].Normalize();
	
	// left
	SETUP_PLANE(EFP_LEFT,		
					viewProjection._03 + viewProjection._00,
					viewProjection._13 + viewProjection._10,
					viewProjection._23 + viewProjection._20,
					viewProjection._33 + viewProjection._30);
	// right
	SETUP_PLANE(EFP_RIGHT,	
					viewProjection._03 - viewProjection._00,
					viewProjection._13 - viewProjection._10,
					viewProjection._23 - viewProjection._20,
					viewProjection._33 - viewProjection._30);

	// bottom
	SETUP_PLANE(EFP_BOTTOM,	
					viewProjection._03 + viewProjection._01,
					viewProjection._13 + viewProjection._11,
					viewProjection._23 + viewProjection._21,
					viewProjection._33 + viewProjection._31);

	// top
	SETUP_PLANE(EFP_TOP,
					viewProjection._03 - viewProjection._01,
					viewProjection._13 - viewProjection._11,
					viewProjection._23 - viewProjection._21,
					viewProjection._33 - viewProjection._31);

	// near
	SETUP_PLANE(EFP_NEAR,
					viewProjection._02,
					viewProjection._12,
					viewProjection._22,
					viewProjection._32);

	// far
	SETUP_PLANE(EFP_FAR,	
					viewProjection._03 - viewProjection._02,
					viewProjection._13 - viewProjection._12,
					viewProjection._23 - viewProjection._22,
					viewProjection._33 - viewProjection._32);

	planeCount = 6;

#undef SETUP_PLANE 
}

void Frustum::Set()
{
    const Matrix4 & viewProjection = RenderManager::Instance()->GetUniformMatrix(RenderManager::UNIFORM_MATRIX_MODELVIEWPROJECTION);
    Set(viewProjection);
}


//! \brief Check axial aligned bounding box visibility
//! \param min bounding box minimum point
//! \param max bounding box maximum point
bool Frustum::IsInside(const Vector3 & min, const Vector3 &max) const
{
	for (int plane = 0; plane < planeCount; ++plane)
	{
		Vector3 testPoint;
		if (planeArray[plane].n.x >= 0.0f) testPoint.x = min.x;
		else testPoint.x = max.x;

		if (planeArray[plane].n.y >= 0.0f) testPoint.y = min.y;
		else testPoint.y = max.y;

		if (planeArray[plane].n.z >= 0.0f) testPoint.z = min.z;
		else testPoint.z = max.z;
		
		if (planeArray[plane].DistanceToPoint(testPoint) > 0.0f)
			return false;
	}
	return true;	
}

//! \brief Check axial aligned bounding box visibility
//! \param box bounding box
bool Frustum::IsInside(const AABBox3 & box)const
{
	for (int plane = 0; plane < planeCount; ++plane)
	{
		Vector3 testPoint;
		if (planeArray[plane].n.x >= 0.0f) testPoint.x = box.min.x;
		else testPoint.x = box.max.x;

		if (planeArray[plane].n.y >= 0.0f) testPoint.y = box.min.y;
		else testPoint.y = box.max.y;

		if (planeArray[plane].n.z >= 0.0f) testPoint.z = box.min.z;
		else testPoint.z = box.max.z;

		if (planeArray[plane].DistanceToPoint(testPoint) > 0.0f)
			return false;
	}
	return true;	
}


//! \brief Check axial aligned bounding box visibility
//! \param min bounding box minimum point
//! \param max bounding box maximum point
Frustum::eFrustumResult Frustum::Classify(const Vector3 & min, const Vector3 &max) const
{
	bool intersecting = false;
	for (int plane = 0; plane < planeCount; ++plane)
	{
		Vector3 minTest, maxTest;
		if (planeArray[plane].n.x >= 0.0f)
		{
			minTest.x = min.x;
			maxTest.x = max.x;
		}
		else 
		{
			minTest.x = max.x;
			maxTest.x = min.x;
		}

		if (planeArray[plane].n.y >= 0.0f)
		{
			minTest.y = min.y;
			maxTest.y = max.y;
		}
		else 
		{
			minTest.y = max.y;
			maxTest.y = min.y;
		}

		if (planeArray[plane].n.z >= 0.0f)
		{
			minTest.z = min.z;
			maxTest.z = max.z;
		}
		else 
		{
			minTest.z = max.z;
			maxTest.z = min.z;
		}


		if (planeArray[plane].DistanceToPoint(minTest) > 0.0f)
			return EFR_OUTSIDE;

		if (planeArray[plane].DistanceToPoint(maxTest) >= 0.0f)
			intersecting = true;
	}
	if (intersecting) return EFR_INTERSECT;
	return EFR_INSIDE;
}

Frustum::eFrustumResult Frustum::Classify(const AABBox3 & box) const
{
	return Classify(box.min, box.max);
}

//! \brief check bounding sphere visibility against frustum
//! \param point sphere center point
//! \param radius sphere radius
bool Frustum::IsInside(const Vector3 & point, const float32 radius) const
{
	for (int plane = 0; plane < planeCount; ++plane)
	{
		if (planeArray[plane].DistanceToPoint(point) > radius)
			return true;
	}
	return true;    
}

// 
void Frustum::DebugDraw()
{
	if (planeArray.size() < 6)return;
    Vector3 p[50];
    
	
	//for (int i = 0; i < )
	p[0] = Plane3Intersection(	planeArray[EFP_LEFT],
								planeArray[EFP_NEAR],
								planeArray[EFP_BOTTOM]);
	
	p[1] = Plane3Intersection(	planeArray[EFP_RIGHT],
								planeArray[EFP_NEAR],
								planeArray[EFP_BOTTOM]);

	p[3] = Plane3Intersection(	planeArray[EFP_LEFT],
								planeArray[EFP_NEAR],
								planeArray[EFP_TOP]);
	
	p[2] = Plane3Intersection(	planeArray[EFP_RIGHT],
								planeArray[EFP_NEAR],
								planeArray[EFP_TOP]);

	//for (int i = 0; i < )
	p[4] = Plane3Intersection(	planeArray[EFP_LEFT],
								planeArray[EFP_FAR],
								planeArray[EFP_BOTTOM]);
	
	p[5] = Plane3Intersection(	planeArray[EFP_RIGHT],
								planeArray[EFP_FAR],
								planeArray[EFP_BOTTOM]);

	p[7] = Plane3Intersection(	planeArray[EFP_LEFT],
								planeArray[EFP_FAR],
								planeArray[EFP_TOP]);
	
	p[6] = Plane3Intersection(	planeArray[EFP_RIGHT],
								planeArray[EFP_FAR],
								planeArray[EFP_TOP]);


    RenderHelper::Instance()->DrawLine(	p[0], p[1]);
	RenderHelper::Instance()->DrawLine(	p[1], p[2]);
	RenderHelper::Instance()->DrawLine(	p[2], p[3]);
	RenderHelper::Instance()->DrawLine(	p[3], p[0]);
	
	RenderHelper::Instance()->DrawLine(	p[4], p[5]);
	RenderHelper::Instance()->DrawLine(	p[5], p[6]);
	RenderHelper::Instance()->DrawLine( p[6], p[7]);
	RenderHelper::Instance()->DrawLine(	p[7], p[4]);

	RenderHelper::Instance()->DrawLine(	p[0], p[4]);
	RenderHelper::Instance()->DrawLine(	p[1], p[5]);
	RenderHelper::Instance()->DrawLine(	p[2], p[6]);
	RenderHelper::Instance()->DrawLine(	p[3], p[7]);
}

}; 