/*
* Copyright (c) 2009 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_DYNAMIC_TREE_H
#define B2_DYNAMIC_TREE_H

#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Common/b2GrowableStack.h>
#include <vector>

/// A dynamic AABB tree broad-phase, inspired by Nathanael Presson's btDbvt.

#define b2_nullNode (-1)

/// A node in the dynamic tree. The client does not interact with this directly.
struct b2DynamicTreeNode
{
	bool IsLeaf() const
	{
		return child1 == b2_nullNode;
	}

	/// This is the fattened AABB.
	b2AABB aabb;

	void* userData;

	union
	{
		b2_int32 parent;
		b2_int32 next;
	};

	b2_int32 child1;
	b2_int32 child2;
	b2_int32 leafCount;
};

/// A dynamic tree arranges data in a binary tree to accelerate
/// queries such as volume queries and ray casts. Leafs are proxies
/// with an AABB. In the tree we expand the proxy AABB by b2_fatAABBFactor
/// so that the proxy AABB is bigger than the client object. This allows the client
/// object to move by small amounts without triggering a tree update.
///
/// Nodes are pooled and relocatable, so we use node indices rather than pointers.
class b2DynamicTree
{
public:

	/// Constructing the tree initializes the node pool.
	b2DynamicTree();

	/// Destroy the tree, freeing the node pool.
	~b2DynamicTree();

	/// Create a proxy. Provide a tight fitting AABB and a userData pointer.
	b2_int32 CreateProxy(const b2AABB& aabb, void* userData);

	/// Destroy a proxy. This asserts if the id is invalid.
	void DestroyProxy(b2_int32 proxyId);

	/// Move a proxy with a swepted AABB. If the proxy has moved outside of its fattened AABB,
	/// then the proxy is removed from the tree and re-inserted. Otherwise
	/// the function returns immediately.
	/// @return true if the proxy was re-inserted.
	bool MoveProxy(b2_int32 proxyId, const b2AABB& aabb1, const b2Vec2& displacement);

	/// Perform some iterations to re-balance the tree.
	void Rebalance(b2_int32 iterations);

	/// Get proxy user data.
	/// @return the proxy user data or 0 if the id is invalid.
	void* GetUserData(b2_int32 proxyId) const;

	/// Get the fat AABB for a proxy.
	const b2AABB& GetFatAABB(b2_int32 proxyId) const;

	/// Compute the height of the binary tree in O(N) time. Should not be
	/// called often.
	b2_int32 ComputeHeight() const;

	/// Query an AABB for overlapping proxies. The callback class
	/// is called for each proxy that overlaps the supplied AABB.
	template <typename T>
	void Query(T* callback, const b2AABB& aabb) const;

	/// Ray-cast against the proxies in the tree. This relies on the callback
	/// to perform a exact ray-cast in the case were the proxy contains a shape.
	/// The callback also performs the any collision filtering. This has performance
	/// roughly equal to k * log(n), where k is the number of collisions and n is the
	/// number of proxies in the tree.
	/// @param input the ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
	/// @param callback a callback class that is called for each proxy that is hit by the ray.
	template <typename T>
	void RayCast(T* callback, const b2RayCastInput& input) const;

	void Validate() const;

		//added for tiles
	void CreateTiledMap(const b2Vec2 &location, const b2Vec2 &tileSize, b2_int32 mapWidth, b2_int32 mapHeight);
		//	void DestroyTiledMap();
		//added for tiles end
	
private:

	b2_int32 AllocateNode();
	void FreeNode(b2_int32 node);

	void InsertLeaf(b2_int32 node);
	void RemoveLeaf(b2_int32 node);

	b2_int32 ComputeHeight(b2_int32 nodeId) const;
	
	b2_int32 CountLeaves(b2_int32 nodeId) const;

	b2_int32 m_root;

	b2DynamicTreeNode* m_nodes;
	b2_int32 m_nodeCount;
	b2_int32 m_nodeCapacity;

	b2_int32 m_freeList;

	/// This is used incrementally traverse the tree for re-balancing.
	b2_uint32 m_path;

	b2_int32 m_insertionCount;
	
		//added for tiles
	std::vector<b2_int32> *tileMap;
	b2Vec2 mapLocation;
	b2Vec2 tileSize;
	b2_int32 mapWidth;
	b2_int32 mapHeight;
		//added for tiles end
};



inline void* b2DynamicTree::GetUserData(b2_int32 proxyId) const
{
	b2Assert(0 <= proxyId && proxyId < m_nodeCapacity);
	return m_nodes[proxyId].userData;
}

inline const b2AABB& b2DynamicTree::GetFatAABB(b2_int32 proxyId) const
{
	b2Assert(0 <= proxyId && proxyId < m_nodeCapacity);
	return m_nodes[proxyId].aabb;
}

template <typename T>
inline void b2DynamicTree::Query(T* callback, const b2AABB& aabb) const
{
	b2GrowableStack<b2_int32, 256> stack;
	stack.Push(m_root);

	while (stack.GetCount() > 0)
	{
		b2_int32 nodeId = stack.Pop();
		if (nodeId == b2_nullNode)
		{
			continue;
		}

		const b2DynamicTreeNode* node = m_nodes + nodeId;

		if (b2TestOverlap(node->aabb, aabb))
		{
			if (node->IsLeaf())
			{
				bool proceed = callback->QueryCallback(nodeId);
				if (proceed == false)
				{
					return;
				}
			}
			else
			{
				stack.Push(node->child1);
				stack.Push(node->child2);
			}
		}
	}

		//added for tiles
	if (tileMap) 
	{//TODO: optimize it's later
		b2_int32 minX = floor((aabb.lowerBound.x - b2_aabbExtension - mapLocation.x) / tileSize.x);
		b2_int32 maxX = ceil((aabb.upperBound.x + b2_aabbExtension - mapLocation.x) / tileSize.x);
		b2_int32 minY = floor((aabb.lowerBound.y - b2_aabbExtension - mapLocation.y) / tileSize.y);
		b2_int32 maxY = ceil((aabb.upperBound.y + b2_aabbExtension - mapLocation.y) / tileSize.y);

		
		minX = b2Max(0, minX);
		minY = b2Max(0, minY);
		maxX = b2Max(0, maxX);
		maxY = b2Max(0, maxY);
		
		minX = b2Min(mapWidth - 1, minX);
		minY = b2Min(mapHeight - 1, minY);
		maxX = b2Min(mapWidth - 1, maxX);
		maxY = b2Min(mapHeight - 1, maxY);
		
		for (b2_int32 y = minY; y <= maxY; y++) 
		{
			for (b2_int32 x = minX; x <= maxX; x++) 
			{
				
				b2_int32 mi = x + y * mapWidth;
				if (tileMap->at(mi) != -1)
				{
					bool proceed = callback->QueryCallback(tileMap->at(mi));
					if (proceed == false)
					{
						return;
					}
				}
			}
		}
	}
		//added for tiles end

}

template <typename T>
inline void b2DynamicTree::RayCast(T* callback, const b2RayCastInput& input) const
{
	b2Vec2 p1 = input.p1;
	b2Vec2 p2 = input.p2;
	b2Vec2 r = p2 - p1;
	b2Assert(r.LengthSquared() > 0.0f);
	r.Normalize();

	// v is perpendicular to the segment.
	b2Vec2 v = b2Cross(1.0f, r);
	b2Vec2 abs_v = b2Abs(v);

	// Separating axis for segment (Gino, p80).
	// |dot(v, p1 - c)| > dot(|v|, h)

	b2_float32 maxFraction = input.maxFraction;

	// Build a bounding box for the segment.
	b2AABB segmentAABB;
	{
		b2Vec2 t = p1 + maxFraction * (p2 - p1);
		segmentAABB.lowerBound = b2Min(p1, t);
		segmentAABB.upperBound = b2Max(p1, t);
	}

	b2GrowableStack<b2_int32, 256> stack;
	stack.Push(m_root);

	while (stack.GetCount() > 0)
	{
		b2_int32 nodeId = stack.Pop();
		if (nodeId == b2_nullNode)
		{
			continue;
		}

		const b2DynamicTreeNode* node = m_nodes + nodeId;

		if (b2TestOverlap(node->aabb, segmentAABB) == false)
		{
			continue;
		}

		// Separating axis for segment (Gino, p80).
		// |dot(v, p1 - c)| > dot(|v|, h)
		b2Vec2 c = node->aabb.GetCenter();
		b2Vec2 h = node->aabb.GetExtents();
		b2_float32 separation = b2Abs(b2Dot(v, p1 - c)) - b2Dot(abs_v, h);
		if (separation > 0.0f)
		{
			continue;
		}

		if (node->IsLeaf())
		{
			b2RayCastInput subInput;
			subInput.p1 = input.p1;
			subInput.p2 = input.p2;
			subInput.maxFraction = maxFraction;

			b2_float32 value = callback->RayCastCallback(subInput, nodeId);

			if (value == 0.0f)
			{
				// The client has terminated the ray cast.
				return;
			}

			if (value > 0.0f)
			{
				// Update segment bounding box.
				maxFraction = value;
				b2Vec2 t = p1 + maxFraction * (p2 - p1);
				segmentAABB.lowerBound = b2Min(p1, t);
				segmentAABB.upperBound = b2Max(p1, t);
			}
		}
		else
		{
			stack.Push(node->child1);
			stack.Push(node->child2);
		}
	}
}

#endif
