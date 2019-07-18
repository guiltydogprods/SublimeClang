#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "mat4x4.h"
#include "mesh.h"
#include "file.h"

// Private function prototypes.
void loadTextureChunk(struct ChunkId *chunk);
void loadMaterialChunk(struct ChunkId *chunk);
void loadSkeletonChunk(struct ChunkId *chunk);
int32_t loadMeshChunk(struct ChunkId *chunk, struct Mesh *mesh);
uint8_t* loadMeshChunkRecursive(uint8_t* ptr, struct Mesh *mesh); //, uint32_t& renderableIndex, uint32_t& nodeIndex, int32_t parentIndex, VertexBuffer& vertexBuffer, int64_t& vertexBufferOffset, IndexBuffer& indexBuffer, int64_t& indexBufferOffset)

void loadMesh(const char *meshName, struct Mesh *mesh)
{
	FILE *fptr = file_open(meshName, "rt");
	if (!fptr)
	{
		printf("Error: Unable to open file %s\n", meshName);
		exit(1);
	}

	size_t fileLen = file_length(fptr);
	uint8_t *buffer = malloc(fileLen);
	file_read(buffer, fileLen, fptr);
	fclose(fptr);

	struct ModelHeader *header = (struct ModelHeader *)buffer;
	uint32_t versionMajor = header->versionMajor;
	uint32_t versionMinor = header->versionMinor;
	bool bLoaded = false;
	uint8_t *ptr = (uint8_t *)(buffer + sizeof(struct ModelHeader));
	int32_t res = 0;
	char fourCC[5] = { "NULL" };
	while (!bLoaded)
	{
		struct ChunkId *chunk = (struct ChunkId *)ptr;
		strncpy(fourCC, (char *)(&chunk->fourCC), 4);
		switch (chunk->fourCC)
		{
		case kFourCC_TEXT:
			loadTextureChunk(chunk);
			break;
		case kFourCC_MATL:
			loadMaterialChunk(chunk);
			break;
		case kFourCC_SKEL:
			loadSkeletonChunk(chunk);
			break;
		case kFourCC_MESH:
			res = loadMeshChunk(chunk, mesh);
			if (res >= 0)
				bLoaded = true;
			else
			{
				ptr = (uint8_t *)(buffer + sizeof(struct ModelHeader));
				ptr -= chunk->size;
			}
			break;
		default:
			printf("Warning: Skipping unknown Chunk '%s' in file %s\n", fourCC, meshName);
		}
		ptr = ptr + chunk->size;
	}
}

void loadTextureChunk(struct ChunkId *chunk)
{
	
}

void loadMaterialChunk(struct ChunkId *chunk)
{
	
}

void loadSkeletonChunk(struct ChunkId *chunk)
{
	
}

int32_t loadMeshChunk(struct ChunkId *chunk, struct Mesh *mesh)
{
	struct MeshChunk *meshChunk = (struct MeshChunk *)chunk;
	uint8_t *ptr = (uint8_t *)chunk + sizeof(struct MeshChunk);
	uint32_t renderableIndex = 0;
	uint32_t nodeIndex = 0;
	int32_t parentIndex = -1;
	for (uint32_t i = 0; i < meshChunk->numMeshes; ++i)
	{
		ptr = loadMeshChunkRecursive(ptr, mesh); //, renderableIndex, nodeIndex, parentIndex, vertexBuffer, vertexBufferOffset, indexBuffer, indexBufferOffset);
		if (ptr == NULL)
			return -1;
	}
	return 0;
}

#define MAX_VERTEX_ELEMENTS (16)
#define MAX_STREAM_COUNT (2)

struct Import_VertexElement
{
	uint8_t			m_index;
	int8_t			m_size;
	uint16_t		m_type;
	uint8_t			m_normalized;
	uint8_t			m_offset;			//CLR - is this big enough for MultiDrawIndirect?
};

struct Import_VertexStream
{
	uint32_t				m_glBufferId;
	uint8_t					m_bufferType;
	uint8_t					m_numElements;
	uint16_t				m_stride;
	uint32_t				m_dataOffset;
	struct Import_VertexElement	m_elements[MAX_VERTEX_ELEMENTS];
};

struct Import_VertexBuffer
{
	uint32_t			m_numStreams;
	struct Import_VertexStream	m_streams[MAX_STREAM_COUNT];
};

uint8_t* loadMeshChunkRecursive(uint8_t* ptr, struct Mesh *mesh) //, uint32_t& renderableIndex, uint32_t& nodeIndex, int32_t parentIndex, VertexBuffer& vertexBuffer, int64_t& vertexBufferOffset, IndexBuffer& indexBuffer, int64_t& indexBufferOffset)
{
	//CLR - Temp
	uint32_t renderableIndex = 0;
	uint32_t nodeIndex = 0;
	int32_t parentIndex = -1;
	int64_t vertexBufferOffset = 0;
	int64_t indexBufferOffset = 0;
	//CLR

	struct MeshInfo *info = (struct MeshInfo *)ptr;
	struct RenderableInfo* rendInfo = (struct RenderableInfo*)(ptr + sizeof(struct MeshInfo));

	mesh->hierarchy[nodeIndex].parentId = parentIndex;
	mesh->hierarchy[nodeIndex].numRenderables = info->numRenderables;
	mesh->hierarchy[nodeIndex].numChildren = info->numChildren;
	uint32_t numRenderables = info->numRenderables;

	uint8_t* meshData = (uint8_t*)((uint8_t*)rendInfo + sizeof(struct RenderableInfo) * numRenderables);
	for (uint32_t rend = 0; rend < numRenderables; ++rend)
	{
		struct Import_VertexBuffer* srcVertexBuffer = (struct Import_VertexBuffer*)meshData;
		uint32_t numVertices = rendInfo[rend].numVertices;
		uint32_t stride = srcVertexBuffer->m_streams[0].m_stride;
		uint32_t verticesSize = sizeof(struct Import_VertexBuffer) + stride * numVertices;
		uint32_t numIndices = rendInfo[rend].numIndices;
		uint32_t indicesSize = sizeof(uint32_t) * numIndices;

		mesh->aabbMin[0] = rendInfo->aabbMinX;
		mesh->aabbMin[1] = rendInfo->aabbMinY;
		mesh->aabbMin[2] = rendInfo->aabbMinZ;
		mesh->aabbMax[0] = rendInfo->aabbMaxX;
		mesh->aabbMax[1] = rendInfo->aabbMaxY;
		mesh->aabbMax[2] = rendInfo->aabbMaxZ;
/*			
		int32_t materialIndex = FindMaterial(rendInfo->materialHash);
		if (materialIndex == -1)
		{
			return nullptr;
			materialIndex = FindMaterial(rendInfo->materialHash);
		}
*/		
//CLR - Temp
		int32_t materialIndex = 0;
//CLR
		uint32_t firstVertex = (uint32_t)(vertexBufferOffset / stride);
		uint32_t firstIndex = (uint32_t)(indexBufferOffset / sizeof(uint32_t));
		uint32_t indexCount = numIndices;
		Renderable *renderable = mesh->renderables + renderableIndex;
		renderable->firstVertex = firstVertex;
		renderable->firstIndex = firstIndex;
		renderable->indexCount = indexCount;
		renderable->materialIndex = materialIndex;

//		MaterialPtr pMaterial = MaterialManager::Get()->Find(rendInfo[rend].materialHash);
		uint32_t numStreams = srcVertexBuffer->m_numStreams;

//		OddJob::dispatchAsync(OddJob::getMainQueue(), [=, &vertexBuffer, &indexBuffer]() {
			for (uint32_t i = 0; i < numStreams; ++i)
			{
				uint8_t* vertexData = (uint8_t*)srcVertexBuffer + sizeof(struct Import_VertexBuffer);
//CLR - PortMe.				
//				vertexBuffer.writeData(i, vertexBufferOffset, numVertices * stride, vertexData);
			}
			uint32_t* indices = (uint32_t*)((uint8_t*)srcVertexBuffer + verticesSize);
//CLR - PortMe.				
//			indexBuffer.writeData(indexBufferOffset, numIndices * sizeof(uint32_t), indices);
//		});
		vertexBufferOffset += (numVertices * stride);
		indexBufferOffset += (numIndices * sizeof(uint32_t));
		meshData = meshData + verticesSize + indicesSize;
		renderableIndex++;
	}
	ptr = (uint8_t*)meshData;
//	(uint32_t*)&info->worldMatrix, 16;
	memcpy(&mesh->transforms[nodeIndex], &info->worldMatrix, sizeof(mat4x4));
	parentIndex = nodeIndex++;
	uint32_t numChildren = info->numChildren;
	for (uint32_t i = 0; i < numChildren; ++i) {
		ptr = loadMeshChunkRecursive(ptr, mesh); //renderableIndex, nodeIndex, parentIndex, vertexBuffer, vertexBufferOffset, indexBuffer, indexBufferOffset);
//		renderableIndex++;
	}
	return ptr;
}
