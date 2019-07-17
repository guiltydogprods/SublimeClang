#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "mat4x4.h"
#include "mesh.h"
#include "file.h"


void loadSkeletonChunk(struct ChunkId *chunk)
{
	
}

void loadMaterialChunk(struct ChunkId *chunk)
{
	
}

int32_t loadMeshChunk(struct ChunkId *chunk)
{
	return 0;
}

void loadMesh(const char *meshName)
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
//			LoadTextureChunk(chunk);
			break;
		case kFourCC_MATL:
			loadMaterialChunk(chunk); //, resourceManager);
			break;
		case kFourCC_SKEL:
			loadSkeletonChunk(chunk); //, resource);
			break;
		case kFourCC_MESH:
			res = loadMeshChunk(chunk); //, resource, vertexBuffer, vertexBufferOffset, indexBuffer, indexBufferOffset);
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
