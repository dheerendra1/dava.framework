#include "stdafx.h"
#include "ColladaPolygonGroup.h"
#include <time.h>
#include "DAVAEngine.h"

namespace DAVA
{

    
bool ColladaVertex::IsEqual(const ColladaVertex & v1, const ColladaVertex & v2, int32 vertexFormat)
{
    if (vertexFormat & DAVA::EVF_VERTEX)
    {
        if (!FLOAT_EQUAL(v1.position.x, v2.position.x))return false;
        if (!FLOAT_EQUAL(v1.position.y, v2.position.y))return false;
        if (!FLOAT_EQUAL(v1.position.z, v2.position.z))return false;
    }
    
    if (vertexFormat & EVF_NORMAL)
    {
        if (!FLOAT_EQUAL(v1.normal.x, v2.normal.x))return false;
        if (!FLOAT_EQUAL(v1.normal.y, v2.normal.y))return false;
        if (!FLOAT_EQUAL(v1.normal.z, v2.normal.z))return false;
    }
    
    if (vertexFormat & EVF_TANGENT)
    {
        if (!FLOAT_EQUAL(v1.tangent.x, v2.tangent.x))return false;
        if (!FLOAT_EQUAL(v1.tangent.y, v2.tangent.y))return false;
        if (!FLOAT_EQUAL(v1.tangent.z, v2.tangent.z))return false;
    }

    if (vertexFormat & EVF_BINORMAL)
    {
        if (!FLOAT_EQUAL(v1.binormal.x, v2.binormal.x))return false;
        if (!FLOAT_EQUAL(v1.binormal.y, v2.binormal.y))return false;
        if (!FLOAT_EQUAL(v1.binormal.z, v2.binormal.z))return false;
    }
    
    if (vertexFormat & EVF_TEXCOORD0)
    {
        if (!FLOAT_EQUAL(v1.texCoords[0].x, v2.texCoords[0].x))return false;
        if (!FLOAT_EQUAL(v1.texCoords[0].y, v2.texCoords[0].y))return false;
    }
    
    if (vertexFormat & EVF_TEXCOORD1)
    {
        if (!FLOAT_EQUAL(v1.texCoords[1].x, v2.texCoords[1].x))return false;
        if (!FLOAT_EQUAL(v1.texCoords[1].y, v2.texCoords[1].y))return false;
    }

    if (vertexFormat & EVF_TEXCOORD2)
    {
        if (!FLOAT_EQUAL(v1.texCoords[2].x, v2.texCoords[2].x))return false;
        if (!FLOAT_EQUAL(v1.texCoords[2].y, v2.texCoords[2].y))return false;
    }

    if (vertexFormat & EVF_TEXCOORD3)
    {
        if (!FLOAT_EQUAL(v1.texCoords[3].x, v2.texCoords[3].x))return false;
        if (!FLOAT_EQUAL(v1.texCoords[3].y, v2.texCoords[3].y))return false;
    }
    return true;
}

    
#define MAX_BONES_PER_VERTEX 4
	
void ColladaVertexWeight::AddWeight(int32 jindex, float32 weight)
{
	if (weight <= 0.01)return;
	
	int i;
	for (i = 0; i < jointCount; ++i)
	{
		if (weight > weightArray[i])
		{
			for (int j = jointCount; j > i; --j)
			{
				weightArray[j] = weightArray[j - 1];
				jointArray[j] = jointArray[j - 1];
			}
			break;
		}
	}
	
	weightArray[i] = weight;
	jointArray[i] = jindex;
	
	jointCount ++;
	if (jointCount > MAX_BONES_PER_VERTEX)
		jointCount = MAX_BONES_PER_VERTEX;
		return;
}
	
void ColladaVertexWeight::Normalize()
{
	float size = 0;
	for (int i = 0; i < jointCount; ++i)
	{
		size += weightArray[i];
	}
	for (int i = 0; i < jointCount; ++i)
	{
		weightArray[i] /= size;
	}
}

bool sortFunc(const ColladaVertex & a, const ColladaVertex & b)
{
	if (a.position.x < b.position.x)return true;
	else if ((a.position.x == b.position.x) && (a.position.y < b.position.y))return true;
	else if ((a.position.y == b.position.y) && (a.position.z < b.position.z))return true;

	return false;
}

ColladaPolygonGroup::ColladaPolygonGroup(ColladaMesh * _parentMesh, FCDGeometryPolygons * _polygons, ColladaVertexWeight * vertexWeightArray)
{
    vertexFormat = EVF_VERTEX | EVF_NORMAL;
	parentMesh = _parentMesh;
	polygons = _polygons;
	materialSemantic = polygons->GetMaterialSemantic();
	skinAnimation = (vertexWeightArray != 0);

    

	FCDGeometryPolygonsInput* pVertexInput = polygons->FindInput(FUDaeGeometryInput::POSITION);
	FCDGeometryPolygonsInput* pTexCoordInput0 = polygons->FindInput(FUDaeGeometryInput::TEXCOORD);
	FCDGeometryPolygonsInput* pNormalInput = polygons->FindInput(FUDaeGeometryInput::NORMAL);

	FCDGeometrySource* pVertexSource = polygons->GetParent()->FindSourceByType(FUDaeGeometryInput::POSITION);
	FCDGeometrySource* pTexCoordSource0 = polygons->GetParent()->FindSourceByType(FUDaeGeometryInput::TEXCOORD);
	FCDGeometrySource* pNormalSource = polygons->GetParent()->FindSourceByType(FUDaeGeometryInput::NORMAL);

    if (pTexCoordInput0 && pTexCoordSource0)
        vertexFormat |= EVF_TEXCOORD0;
    
    FCDGeometryPolygonsInputList texCoordInputList;
    polygons->FindInputs(FUDaeGeometryInput::TEXCOORD, texCoordInputList);
    FCDGeometrySourceList texCoordSourcesList;
    polygons->GetParent()->FindSourcesByType(FUDaeGeometryInput::TEXCOORD, texCoordSourcesList);

    FCDGeometryPolygonsInput* pTexCoordInput1 = 0;
    FCDGeometrySource* pTexCoordSource1 = 0;   
    if (texCoordInputList.size() == 2 && texCoordSourcesList.size() == 2)
    {
        pTexCoordInput1 = texCoordInputList[1];
        pTexCoordSource1 = texCoordSourcesList[1];
        vertexFormat |= EVF_TEXCOORD1;
    }
    
    
//	int faceCount = (int)polygons->GetFaceCount();
//	int polyType = (int)polygons->TestPolyType();
//	int inputCount = (int)polygons->GetInputCount();

//	printf("faceCount: %d polyType: %d inputCount: %d\n", faceCount, polyType, inputCount);
//	printf("primitive: %d\n", polygons->GetPrimitiveType());

// 	int vertCount = 0; 
// 	if (pVertexSource)
// 		vertCount = (int)pVertexSource->GetDataCount();
// 	
// 	int texCount = 0; 
// 	if (pTexCoordSource)
// 		texCount = (int)pTexCoordSource->GetDataCount();
// 
// 	int normalCount = 0; 
// 	if (pNormalSource)
// 		normalCount = (int)pNormalSource->GetDataCount();
//	printf("vertexCount: %d texCount: %d normalCount: %d\n", vertCount, texCount, normalCount);

	int vertexIndexCount = -1; 
	int texIndexCount0 = -1; 
    int texIndexCount1 = -1;
	int normalIndexCount = -1; 
	uint32 * vertexIndeces = 0, * normalIndeces = 0, * texIndices0 = 0, * texIndices1 = 0;

	if (pVertexInput)
	{
		vertexIndexCount = (int)pVertexInput->GetIndexCount();
		vertexIndeces = (uint32*)pVertexInput->GetIndices();
	}

	if (pTexCoordInput0)	
	{
		texIndexCount0 = (int)pTexCoordInput0->GetIndexCount();
		texIndices0 = (uint32*)pTexCoordInput0->GetIndices();
	}
	
    if (pTexCoordInput1)	
	{
		texIndexCount1 = (int)pTexCoordInput1->GetIndexCount();
		texIndices1 = (uint32*)pTexCoordInput1->GetIndices();
	}

	if (pNormalInput)	
	{
		normalIndexCount = (int)pNormalInput->GetIndexCount();
		normalIndeces = (uint32*)pNormalInput->GetIndices();
	}	

	triangleCount = vertexIndexCount / 3;
	unoptimizedVerteces.resize(vertexIndexCount);	

	printf("- submesh verteces:%d indeces: %d %d %d\n", pVertexSource->GetDataCount(), vertexIndexCount, texIndexCount0, normalIndexCount);
	
	if ((vertexIndexCount != texIndexCount0) || (texIndexCount0 != normalIndexCount) || (texIndexCount1 != normalIndexCount))
	{	
		//printf("*** ERROR: Index count is different for different sources\n");
	}

	

	for (int v = 0; v < vertexIndexCount; ++v)
	{
		ColladaVertex tv;
		int vertexIndex = vertexIndeces[v];
		float * p = 0;
		
		if (pVertexSource)
		{
			p = &pVertexSource->GetData()[vertexIndex * 3];
			tv.position.x = p[0]; 
			tv.position.y = p[1]; 
			tv.position.z = p[2];
		}
		
		if (pNormalSource)
		{
			int normalIndex = normalIndeces[v];
			p = &pNormalSource->GetData()[normalIndex * 3];
			tv.normal.x=p[0]; tv.normal.y=p[1]; tv.normal.z=p[2];
		}
	
		if (pTexCoordSource0)
		{
			int texIndex = texIndices0[v];
			int stride = pTexCoordSource0->GetStride();

			p = &pTexCoordSource0->GetData()[texIndex * stride];
			tv.texCoords[0].x = p[0]; 
			tv.texCoords[0].y = p[1]; 
			//if (stride == 3)tv.texCoords[0].z=p[2];
		}
        
        if (pTexCoordSource1)
		{
			int texIndex = texIndices1[v];
			int stride = pTexCoordSource1->GetStride();
            
			p = &pTexCoordSource1->GetData()[texIndex * stride];
			tv.texCoords[1].x = p[0]; 
			tv.texCoords[1].y = p[1]; 
			//if (stride == 3)tv.texCoords[0].z=p[2];
		}
		
		if (vertexWeightArray)
		{
			tv.jointCount = vertexWeightArray[vertexIndex].jointCount;
			for (int jointi = 0; jointi < tv.jointCount;  ++jointi)
			{
				tv.joint[jointi] = vertexWeightArray[vertexIndex].jointArray[jointi];
				tv.weight[jointi] = vertexWeightArray[vertexIndex].weightArray[jointi];
			}
			//printf("ind: %d, ", vertexIndex);
		}

		unoptimizedVerteces[v] = tv;
		bbox.AddPoint(tv.position);
	}

	indexArray.resize(triangleCount * 3);
	for (int i = 0; i < triangleCount; ++i)
	{
		indexArray[i * 3 + 0] = i * 3 + 0;
		indexArray[i * 3 + 1] = i * 3 + 1;
		indexArray[i * 3 + 2] = i * 3 + 2;
	}
	
 	int vertexCountBeforeOptimization = vertexIndexCount;
	
//	std::vector<ColladaSortVertex> vertexSortArray;
//	time_t tm1 = time(0);
//	std::stable_sort(unoptimizedVerteces.begin(), unoptimizedVerteces.end(), sortFunc);
//	time_t tm2 = time(0);
//	printf("Optimization Time: %0.3f\n", difftime(tm2, tm1));
	
	int **equalMap;
	int *equalCount;
	
	equalMap = new int*[vertexIndexCount];
	equalCount = new int[vertexIndexCount];
	
	for (int k = 0; k < vertexIndexCount; ++k)
		equalMap[k] = new int[vertexIndexCount];
	
	//for (int i = 0; i < vertexIndexCount - 1; ++i)
	//	for (int 
	

 	//ColladaVertex *optimizedVerteces = new ColladaVertex[200000];
 	//for (int32 i = 0; i < vertexIndexCount; ++i)
 	//	optimizedVerteces[i] = unoptimizedVerteces[i];
 
 	/*for (int32 i = 0; i < vertexIndexCount - 1; ++i)
 	{
		ColladaVertex * tvi = &unoptimizedVerteces[i];
		equalCount[i] = 0;
		//minJ = vertexIndexCount;
 		if (i % 100 == 0) printf("%d,", i);
 		for (int32 j = i + 1; j < vertexIndexCount; ++j)
 		{
 			ColladaVertex * tvj = &unoptimizedVerteces[j];
 
 			bool equalIJ = true;
			//equalMap[i][j] = 0;
 			if (tvi->position != tvj->position)
			{
				equalIJ = false;
				continue;
			}
 			if (tvi->texCoords[0] != tvj->texCoords[0])
			{
				equalIJ = false;
				continue;
			}
 			if (tvi->normal != tvj->normal)
			{
				equalIJ = false;
				continue;
			}

 
 			if (equalIJ)
 			{	
				equalMap[i][j] = 1;
				equalCount[i] ++;
				//minJ = Min(minJ, j);
				
 				for (int k = 0; k < triangleCount; ++k)
 				{
					int k3 = k * 3;
 					for (int pp = 0; pp < 3; pp++)
 					{
						// we remove index j, so we deduce all indexes that higher than j
 						if (indexArray[k3 + pp] > j)indexArray[k3 + pp]--;
 						else if (indexArray[k3 + pp] == j) indexArray[k3 + pp] = i; // here we replace all j indexes by i index
 					}
 				}
				// remove index j, and shift all vertices
 				for (int k = j + 1; k < vertexIndexCount; k++)
 				{
 					unoptimizedVerteces[k - 1] = unoptimizedVerteces[k];
 				}
 
 				vertexIndexCount--;
 			}
 		}
	}
	*/
	
	std::vector<ColladaVertex> optVertexArray;
	optVertexArray.clear();
	for (int index = 0; index < (int)triangleCount * 3; ++index)
	{
		ColladaVertex * tvj = &unoptimizedVerteces[indexArray[index]];

		int optSize = optVertexArray.size();
		int oIndex = 0;
		for (oIndex = 0; oIndex < optSize; ++oIndex)
 		{
			ColladaVertex * tvi = &optVertexArray[oIndex];
			
 			bool equalIJ = true;
 			/*if (tvi->position != tvj->position)
			{
				equalIJ = false;
			}
 			if (tvi->normal != tvj->normal)
			{
				equalIJ = false;
			} 

			if (tvi->texCoords[0] != tvj->texCoords[0])
			{
				equalIJ = false;
			}*/
            const float32 EPS = 0.00001f;
            
            if (!FLOAT_EQUAL_EPS( tvi->position.x, tvj->position.x, EPS))
            {
                equalIJ = false;
            }
            if (!FLOAT_EQUAL_EPS( tvi->position.y, tvj->position.y, EPS))
            {
                equalIJ = false;
            }
            if (!FLOAT_EQUAL_EPS( tvi->position.z, tvj->position.z, EPS))
            {
                equalIJ = false;
            }
            
            /*if (!FLOAT_EQUAL_EPS( tvi->normal.x, tvj->normal.x, EPS))
            {
                equalIJ = false;
            }
            if (!FLOAT_EQUAL_EPS( tvi->normal.y, tvj->normal.y, EPS))
            {
                equalIJ = false;
            }
            if (!FLOAT_EQUAL_EPS( tvi->normal.z, tvj->normal.z, EPS))
            {
                equalIJ = false;
            }*/
            
            if (!FLOAT_EQUAL_EPS( tvi->texCoords[0].x, tvj->texCoords[0].x, EPS))
            {
                equalIJ = false;
            }
            if (!FLOAT_EQUAL_EPS( tvi->texCoords[0].y, tvj->texCoords[0].y, EPS))
            {
                equalIJ = false;
            }
            
            if (pTexCoordSource1)
            {
                if (!FLOAT_EQUAL_EPS( tvi->texCoords[1].x, tvj->texCoords[1].x, EPS))
                {
                    equalIJ = false;
                }
                if (!FLOAT_EQUAL_EPS( tvi->texCoords[1].y, tvj->texCoords[1].y, EPS))
                {
                    equalIJ = false;
                }
            }

			if (equalIJ)break;
		}		
		if (oIndex == optSize)
		{
//            Logger::Debug("vertex added: (%f, %f, %f) - (%f, %f)", 
//                          tvj->position.x, tvj->position.y, tvj->position.z, 
//                          tvj->texCoords[0].x, tvj->texCoords[0].y);
			indexArray[index] = optSize;
			optVertexArray.push_back(*tvj);
		}else
			indexArray[index] = oIndex;
		
	}
	
	vertexIndexCount = optVertexArray.size();
	unoptimizedVerteces.clear();
	for (int k = 0; k < vertexIndexCount; ++k)
		unoptimizedVerteces.push_back(optVertexArray[k]); 
	
	skinVerteces.resize(vertexIndexCount);
	for (int k = 0; k < vertexIndexCount; ++k)
		skinVerteces[k] = unoptimizedVerteces[k];

	
	printf("\t\tOptimization: %0.2f initial vertexes: %d opt vertexes: %d\n", ((float)vertexIndexCount / (float)vertexCountBeforeOptimization), vertexCountBeforeOptimization, vertexIndexCount);

#ifdef COLLADA_GLUT_RENDER

	renderListId = glGenLists(1);
	glNewList(renderListId, GL_COMPILE); 
	glBegin (GL_TRIANGLES);
	// each 3 vertex, we build a triangle
	for(int i = 0; i < (int)triangleCount; i++) 
	{
		for (int k = 0; k < 3; ++k)
		{
			ColladaVertex v = unoptimizedVerteces[ indexArray[i * 3 + k] ];

			glMultiTexCoord2fARB(
				GL_TEXTURE0,
				v.texCoords[0].x,
				v.texCoords[0].y); 
 

			glNormal3f ( 
				v.normal.x, 
				v.normal.y, 
				v.normal.z);

			// vertex values
			glVertex3f(
				v.position.x, 
				v.position.y, 
				v.position.z
				);
		}
	}

	glEnd();
	glEndList();

#endif
	printf("\t\tbbox: min: %0.3f %0.3f %0.3f max: %0.3f %0.3f %0.3f\n", bbox.min.x, bbox.min.y, bbox.min.z, bbox.max.x, bbox.max.y, bbox.max.z);

}

void ColladaPolygonGroup::Render(ColladaMaterial * material)
{
/*#ifdef COLLADA_GLUT_RENDER
	glCallList(renderListId);
#endif*/
	if (material == 0)
	{
		material = ColladaMaterial::GetDefaultMaterial();
		//printf("default material\n");
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// material properties
	GLfloat matAmbient[4];
	GLfloat matDiffuse[4];
	GLfloat matSpecular[4];
	GLfloat matEmission[4];
	GLfloat matShininess;

	matAmbient[0] = material->ambient.x;
	matAmbient[1] = material->ambient.y;
	matAmbient[2] = material->ambient.z;
	matAmbient[3] = 1.0f;

	matDiffuse[0] = material->diffuse.x; 
	matDiffuse[1] = material->diffuse.y;
	matDiffuse[2] = material->diffuse.z;
	matDiffuse[3] = material->transparency;
//	printf("%s %0.3f %0.3f %0.3f\n", material->material->GetDaeId().c_str(), matDiffuse[0], matDiffuse[1], matDiffuse[2]);

	matSpecular[0] = material->specular.x;
	matSpecular[1] = material->specular.y;
	matSpecular[2] = material->specular.z;
	matSpecular[3] = material->specular.w;

	matEmission[0] = material->emission.x;
	matEmission[1] = material->emission.y;
	matEmission[2] = material->emission.z;
	matEmission[3] = material->emission.w;

	matShininess = material->shininess;

	// allocate material values to opengl
	int face = GL_FRONT_AND_BACK;
	glMaterialfv(face, GL_AMBIENT, matAmbient);
	glMaterialfv(face, GL_DIFFUSE, matDiffuse);
	glMaterialfv(face, GL_SPECULAR, matSpecular);
	glMaterialfv(face, GL_EMISSION, matEmission);
	glMaterialf(face, GL_SHININESS, matShininess);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// save actual cull mode, to restore it after render this polygon
	GLboolean is_cull_mode;
	int current_cull_mode;
	if (material->IsTransparent()) {
		glGetBooleanv(GL_CULL_FACE, &is_cull_mode);
		glGetIntegerv(GL_CULL_FACE_MODE,&current_cull_mode);
	}

	// diffuse texture
	if (material->hasDiffuseTexture) 
	{
		if (material->diffuseTexture->textureId == -1)
		{
			printf("Trying to render missing texture\n");
		}
		glClientActiveTextureARB( GL_TEXTURE0_ARB );
		glActiveTextureARB( GL_TEXTURE0_ARB );
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, material->diffuseTexture->textureId);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	}

	// reflective texture
	if (material->hasReflectiveTexture) 
	{
		// "Reflex" texture
		glClientActiveTextureARB( GL_TEXTURE1_ARB );
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, material->reflectiveTexture->textureId);

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable( GL_TEXTURE_GEN_S );
		glEnable( GL_TEXTURE_GEN_T );

		GLfloat factor=0.15f;
		GLfloat constColor[4] = {factor, factor, factor, factor};

		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
		glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_CONSTANT_ARB);
		glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	// if transparent, render firstly its back
	if (material->IsTransparent()) 
	{	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		// render the back of the transparent polygon
		RenderMesh();
	}

	// if polygon is not transparent, just render it!
	if  (material->IsTransparent()==false) 
	{
		RenderMesh();
	}

	// render the front of the transparent polygon
	if (material->IsTransparent()) 
	{
		glCullFace(GL_BACK);
		RenderMesh();
	}

	// deactivate blending if it was used
	if (material->IsTransparent()) 
	{
		glDisable(GL_BLEND);
	}

	// deactivate diffuse texturing
	if (material->hasDiffuseTexture) 
	{
		glActiveTextureARB (GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);
	}

	// deactivate environment texturing
	if (material->hasReflectiveTexture) 
	{
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glDisable( GL_TEXTURE_GEN_S );
		glDisable( GL_TEXTURE_GEN_S );
	}

	// restore cull mode which was before render this polygon
	if (material->IsTransparent()) 
	{
		glCullFace(current_cull_mode);
		if (is_cull_mode==GL_TRUE) 
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}
	
}

void ColladaPolygonGroup::RenderMesh()
{
	if (!skinAnimation)
	{
		glCallList(renderListId);
	}else
	{
		glBegin (GL_TRIANGLES);
		// each 3 vertex, we build a triangle
		for(int i = 0; i < (int)triangleCount; i++) 
		{
			for (int k = 0; k < 3; ++k)
			{
				ColladaVertex v = skinVerteces[ indexArray[i * 3 + k] ];

				glMultiTexCoord4fARB(
					GL_TEXTURE0,
					v.texCoords[0].x,
					v.texCoords[0].y,
					0.0f,
					1.0f
					);

				glNormal3f ( 
					v.normal.x, 
					v.normal.y, 
					v.normal.z);

				// vertex values
				glVertex3f(
					v.position.x, 
					v.position.y, 
					v.position.z
					);
			}
		}

		glEnd();

	}
}

};