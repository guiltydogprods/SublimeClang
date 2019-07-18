#pragma once

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
	struct ChunkId	chunkId;
	uint32_t		numMeshes;
};

struct TextureInfo
{
	uint32_t width;
	uint32_t height;
	uint32_t textureOffset;
};

struct TextureChunk
{
	struct ChunkId 	chunkId;
	uint32_t 		numTextures;
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
	struct ChunkId 	chunkId;
	uint32_t 		numMaterials;
};

struct MeshNode
{
	int32_t	 parentId;
	uint32_t numRenderables;
	uint32_t numChildren;
};

struct SkeletonChunk
{
	struct ChunkId 	chunkId;
	uint32_t 		numBones;
};
	
typedef struct Renderable
{
	uint32_t	firstVertex;
	uint32_t	firstIndex;
	uint32_t	indexCount;
	int32_t		materialIndex;
} Renderable;

struct Mesh
{
	float		aabbMin[3];
	float		aabbMax[3];
	uint32_t	numNodes;
	uint32_t	numRenderables;
	uint32_t	numTextures;
	uint32_t	numMaterials;
	uint32_t	numBones;
	uint32_t	versionMajor;
	uint32_t	versionMinor;
	struct MeshNode	*hierarchy;
//	HashTable<uint32_t, uint32_t> *m_boneHashes;
	mat4x4	*bones;
	mat4x4	*inverseBindPose;
	mat4x4	*transforms;
	struct Renderable	*renderables;
	struct Material	*materials;
};

// Public function prototypes.
void loadMesh(const char *meshName, struct Mesh *mesh);
