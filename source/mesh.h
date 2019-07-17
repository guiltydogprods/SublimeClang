#pragma once
//#ifndef MESH_H
//#define MESH_H

#ifndef ION_MAKEFOURCC
#define ION_MAKEFOURCC(ch0, ch1, ch2, ch3)											\
					((uint32_t)(uint8_t)(ch0) |	((uint32_t)(uint8_t)(ch1) << 8) |	\
					((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif //defined(ION_MAKEFOURCC)

static int kVersionMajor = 0;
static int kVersionMinor = 2;
static const uint32_t kFourCC_SCNE = ION_MAKEFOURCC('S', 'C', 'N', 'E');	// Scene Chunk FourCC
static const uint32_t kFourCC_TEXT = ION_MAKEFOURCC('T', 'E', 'X', 'T');	// Texture Chunk FourCC
static const uint32_t kFourCC_MATL = ION_MAKEFOURCC('M', 'A', 'T', 'L');	// Material Chunk FourCC
static const uint32_t kFourCC_SKEL = ION_MAKEFOURCC('S', 'K', 'E', 'L');	// Skeleton Chunk FourCC
static const uint32_t kFourCC_MESH = ION_MAKEFOURCC('M', 'E', 'S', 'H');	// Mesh Chunk FourCC
	
struct mat4x4;

struct ChunkId
{
	uint32_t fourCC;
	uint32_t size;
};

struct ModelHeader
{
	struct ChunkId	chunkId;
	uint32_t		versionMajor;
	uint32_t		versionMinor;
};

struct RenderableInfo
{
	uint32_t	numVertices;
	uint32_t	numIndices;
	uint32_t	materialHash;
	float		aabbMinX;
	float		aabbMinY;
	float		aabbMinZ;
	float		aabbMaxX;
	float		aabbMaxY;
	float		aabbMaxZ;
	uint32_t	verticesOffset;
	uint32_t	indicesOffset;
};

struct MeshInfo
{
	struct mat4x4	worldMatrix;
	float			aabbMinX;
	float			aabbMinY;
	float			aabbMinZ;
	float			aabbMaxX;
	float			aabbMaxY;
	float			aabbMaxZ;
	uint32_t		numRenderables;
	uint32_t		numChildren;
};

struct MeshChunk
{
	struct ChunkId		chunkId;
	uint32_t	numMeshes;
};

struct TextureInfo
{
	uint32_t width;
	uint32_t height;
	uint32_t textureOffset;
};

struct TextureChunk
{
	struct ChunkId chunkId;
	uint32_t numTextures;
};

struct MaterialInfo
{
	float		diffuse[3];
	float		ambient[3];
	float		specular[4];
	float		alpha;
	uint32_t	flags;
	uint32_t	emissiveMapHash;
	uint32_t	diffuseMapHash;
	uint32_t	normalMapHash;
	uint32_t	heightMapHash;
};

struct MaterialInfoV1
{
	float		diffuse[3];
	float		ambient[3];
	float		specular[4];
	float		alpha;
	uint32_t	flags;
	float		reflectVal;
	uint32_t	diffuseMapHash;
	uint32_t	normalMapHash;
};

struct MaterialChunk
{
	struct ChunkId chunkId;
	uint32_t numMaterials;
};

struct MeshNode
{
	int32_t	 m_parentId;
	uint32_t m_numRenderables;
	uint32_t m_numChildren;
};

struct SkeletonChunk
{
	struct ChunkId chunkId;
	uint32_t numBones;
};

void loadTextureChunk(struct ChunkId* chunk);
void loadMaterialChunk(struct ChunkId *chunk); //, const ResourceManager& resourceManager);
void loadSkeletonChunk(struct ChunkId *chunk); //, const MeshResourceInfo& resource);
int32_t loadMeshChunk(struct ChunkId *chunk); //, const MeshResourceInfo& resource, VertexBuffer& vertexBuffer, int64_t& vertexBufferOffset, IndexBuffer& indexBuffer, int64_t& indexBufferOffset);
void loadMeshChunkRecursive(uint8_t* ptr); //, uint32_t& renderableIndex, uint32_t& nodeIndex, int32_t parentIndex, VertexBuffer& vertexBuffer, int64_t& vertexBufferOffset, IndexBuffer& indexBuffer, int64_t& indexBufferOffset);

//#endif //MESH_H