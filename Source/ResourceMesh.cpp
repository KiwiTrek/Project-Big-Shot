#include "RenderGlobals.h"
#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(UID uid) : Resource(uid,Resource::Type::MESH), mType(Shape::NONE), vertexBuf(-1), indexBuf(-1), textureBuf(-1),
normalsBuf(-1)
{
	vertexNum = 0;
	normalNum = 0;
	indexNum = 0;
}

ResourceMesh::ResourceMesh(UID uid, Shape shape) : Resource(uid, Resource::Type::MESH), mType(shape), vertexBuf(-1), indexBuf(-1),
normalsBuf(-1)
{
	vertexNum = 0;
	normalNum = 0;
	indexNum = 0;

	switch (shape)
	{
	case Shape::CUBE:
		CopyParMesh(par_shapes_create_cube());
		break;
	case Shape::CYLINDER:
		CopyParMesh(par_shapes_create_cylinder(20, 20));
		break;
	case Shape::SPHERE:
		CopyParMesh(par_shapes_create_parametric_sphere(20, 20));
		break;
	case Shape::TORUS:
		CopyParMesh(par_shapes_create_torus(20, 20, 0.2));
		break;
	case Shape::PLANE:
		CopyParMesh(par_shapes_create_plane(20, 20));
		break;
	case Shape::CONE:
		CopyParMesh(par_shapes_create_cone(20, 20));
		break;
	}
}

ResourceMesh::~ResourceMesh()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexBuf);
	vertices.clear();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBuf);
	indices.clear();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &normalsBuf);
	normals.clear();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &textureBuf);
	texCoords.clear();
}

void ResourceMesh::GenerateBuffers()
{
	//Vertex
	glGenBuffers(1, (GLuint*)&vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertexNum, &vertices[0], GL_STATIC_DRAW);

	//Normals
	glGenBuffers(1, &normalsBuf);
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuf);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * vertexNum * 3, &normals[0], GL_STATIC_DRAW);

	//Indices
	glGenBuffers(1, &indexBuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, &indices[0], GL_STATIC_DRAW);

	if (!texCoords.empty())
	{
		//Textures
		glGenBuffers(1, &textureBuf);
		glBindBuffer(GL_ARRAY_BUFFER, textureBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
	}
}

void ResourceMesh::CopyParMesh(par_shapes_mesh* parMesh)
{
	vertexNum = parMesh->npoints;
	indexNum = parMesh->ntriangles * 3;
	normalNum = parMesh->ntriangles;

	vertices.resize(vertexNum);
	normals.resize(normalNum);
	indices.resize(indexNum);
	texCoords.resize(vertexNum);

	par_shapes_compute_normals(parMesh);
	for (size_t i = 0; i < vertexNum; ++i)
	{
		memcpy(&vertices[i], &parMesh->points[i * 3], sizeof(float) * 3);
		memcpy(&normals[i], &parMesh->normals[i * 3], sizeof(float) * 3);
		if (parMesh->tcoords != nullptr)
		{
			memcpy(&texCoords[i], &parMesh->tcoords[i * 2], sizeof(float) * 2);
		}
		else if (mType == Shape::CUBE)
		{
			switch (i % 4)
			{
			case 0:
				texCoords.at(i).x = 0.0f;
				texCoords.at(i).y = 0.0f;
				break;
			case 1:
				texCoords.at(i).x = 1.0f;
				texCoords.at(i).y = 0.0f;
				break;
			case 2:
				texCoords.at(i).x = 1.0f;
				texCoords.at(i).y = 1.0f;
				break;
			case 3:
				texCoords.at(i).x = 0.0f;
				texCoords.at(i).y = 1.0f;
				break;
			}
		}
		else
		{
			texCoords.at(i).x = 0.0f;
			texCoords.at(i).y = 0.0f;
		}
	}
	for (size_t i = 0; i < indices.size(); ++i)
	{
		indices[i] = parMesh->triangles[i];
	}
	memcpy(&normals[0], parMesh->normals, vertexNum);

	par_shapes_free_mesh(parMesh);
}