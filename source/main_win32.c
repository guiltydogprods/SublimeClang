#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <Memoryapi.h>
#include "file.h"
#include "mat4x4.h"
#include "vec4.h"

#include "mesh.h"

#define NUM_X (1)
#define NUM_Y (1)
#define NUM_Z (1)
#define NUM_DRAWS (NUM_X * NUM_Y * NUM_Z)

struct Transforms
{
	mat4x4 viewMatrix;
	mat4x4 projectionMatrix;
	mat4x4 viewProjMatrix;
};

struct ModelMatrices
{
	mat4x4	modelMatrices[NUM_DRAWS];
};

struct CandidateDraw
{
	float		aabbMin[3];
	uint32_t	firstIndex;
	float		aabbMax[3];
	uint32_t	indexCount;
	uint32_t	materialID;
	uint32_t: 32;
	uint32_t: 32;
	uint32_t: 32;
};

struct DrawElementsIndirectCommand
{
	uint32_t	count;
	uint32_t	instanceCount;
	uint32_t	firstIndex;
	uint32_t	baseVertex;
	uint32_t	baseInstance;
};

struct MaterialProperties
{
	float diffuse[4];
	float specular[3];
	float specularPower;
};

struct Vertex
{
	float position[3];
	float normal[3];
	float texCoord[2];
};

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct Vertex	*m_vertexBufferPtr;
uint32_t *m_indexBufferPtr;
_Bool m_bSpacePressed;
uint32_t m_screenWidth;
uint32_t m_screenHeight;
GLuint	m_vertexArrayObject;
GLuint	m_vertexBufferName;
GLuint  m_indexBufferName;
GLuint	m_transformsBuffer;
GLuint	m_modelMatricesBuffer;
GLuint	m_materialBuffer;
GLuint	m_parameterBuffer;
GLuint	m_drawCandidatesBuffer;
GLuint	m_drawCommandBuffer;
GLuint	m_visibleMatricesBuffer;

GLuint m_glslProgram;

GLsizei m_numIndices;

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void initialize();
void update();
void render();
void loadShader(const char *vertexShaderName, const char *fragmentShaderName);

/*
FILE *file_open(const char *filename, const char *mode);
void file_close(FILE *fptr);
size_t file_length(FILE *fptr);
void file_read(void *buffer, size_t bytes, FILE *fptr);
*/

typedef struct test_struct
{
	int a;
	float b;
	char *string;
} test_struct;

void test_func(const test_struct *ts)
{
	printf(".a = %d\n", ts->a);
	printf(".b = %f\n", ts->b);
	printf(".string = %s\n", ts->string);
}

int main(int argc, char *argv[])
{
	test_struct ts = {
		.a = 1,
		.b = 2.0f,
		.string = "Hello World!"
	};

	test_func(&(test_struct){
		.b = 3.1f,
		.a = 2,
		.string = "A new string"
	});

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const int32_t kDefaultOpenGLMajor = 4;
	const int32_t kDefaultOpenGLMinor = 5;

	GLFWwindow *window = NULL;
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kDefaultOpenGLMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kDefaultOpenGLMinor);
	window = glfwCreateWindow(1280, 720, "SubimeClang", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwShowWindow(window);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	fprintf(stdout, "Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(stdout, "Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, "Version: %s\n", glGetString(GL_VERSION));
/*
	fprintf(stdout, "Extensions:\n");
	GLint n, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (i = 0; i < n; i++)
	{
		fprintf(stdout, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
*/
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	glfwSetKeyCallback(window, key_callback);

	const uint64_t kKilobytes = 1024;
	const uint64_t kMegaBytes = 1024 * kKilobytes;
	const uint64_t kGigaBytes = 1024 * kMegaBytes;
	const uint64_t kTerraBytes = 1024 * kGigaBytes;
	uint64_t kSystemBaseAddress = 1 * 1024 * kGigaBytes;
	uint64_t kGameBaseAddress = 2 * 1024 * kGigaBytes;
	uint64_t kSystemMemorySize = 1 * kGigaBytes;
	uint64_t kGameMemorySize = 1 * kGigaBytes;

	LPVOID systemMemory = VirtualAlloc((void *)kSystemBaseAddress, kSystemMemorySize, MEM_RESERVE, PAGE_READWRITE);
	LPVOID gameMemory = VirtualAlloc((void *)kGameBaseAddress, kGameMemorySize, MEM_RESERVE, PAGE_READWRITE);

	size_t memorySize = 32 * 1024 * 1024;
	uint8_t *memoryBlock = (uint8_t *)_aligned_malloc(memorySize, 32);

	__attribute__ ((aligned(16))) float data[8] = 
	{
		1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
	};


	__attribute__ ((aligned(16))) float resultData[4] = {};
	__attribute__ ((aligned(16))) float resultData2[4] = {};
	
	vec4 value1 = vec4_load(&data[0]);
	vec4 value2 = vec4_load(&data[4]);
	vec4 result = vec4_add(value1, value2);
	vec4_store(resultData, result);
	vec4 result2 = vec4_dot(value1, value2);
	vec4_store(resultData2, result2);

	printf("SIMD Result:  %.3f, %.3f, %.3f, %.3f\n", resultData[0], resultData[1], resultData[2], resultData[3]);
	printf("SIMD Result2: %.3f, %.3f, %.3f, %.3f\n", resultData2[0], resultData2[1], resultData2[2], resultData2[3]);

	initialize();
	while (!glfwWindowShouldClose(window))
	{
//		static int frame = 0;
//		if ((frame++ & 1) == 0)
		update();
		render();		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

	static void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		(void)source; (void)type; (void)id;
		(void)severity; (void)length; (void)userParam;

		if (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH)
			printf("%s\n", message);
		if (severity == GL_DEBUG_SEVERITY_HIGH)
		{

			printf("High Severity...\n");
//			abort();
		}
	}

//void initialize(ScopeStack& allocator)
void initialize()
{
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openglCallbackFunction, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
#endif

	static _Bool waitingForDebugger = true;
//	while (waitingForDebugger)
	{
		printf("Waiting!\n");
	}
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	glGenBuffers(1, &m_transformsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_transformsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(struct Transforms), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_modelMatricesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_modelMatricesBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(struct ModelMatrices), NULL, GL_MAP_WRITE_BIT);

	glGenBuffers(1, &m_parameterBuffer);
	glBindBuffer(GL_PARAMETER_BUFFER_ARB, m_parameterBuffer);
	glBufferStorage(GL_PARAMETER_BUFFER_ARB, 256, NULL, GL_MAP_READ_BIT);
/*
	glGenBuffers(1, &m_drawCandidatesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_drawCandidatesBuffer);

	{
		ScopeStack tempScopeStack(allocator);
		CandidateDraw *pDraws = static_cast<CandidateDraw *>(tempScopeStack.alloc(sizeof(CandidateDraw) * kNumDraws));

		for (uint32_t i = 0; i < kNumDraws; ++i)
		{
			m_pMesh->getAABB(pDraws[i].aabbMin, pDraws[i].aabbMax);
			pDraws[i].firstIndex = 0;
			pDraws[i].indexCount = m_pMesh->getNumIndices();	
			pDraws[i].materialID = i;
		}
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, kNumDraws * sizeof(CandidateDraw), pDraws, 0);
	}
*/
	glGenBuffers(1, &m_drawCommandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_drawCommandBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_DRAWS * sizeof(struct DrawElementsIndirectCommand), NULL, GL_MAP_READ_BIT);

	glGenBuffers(1, &m_visibleMatricesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibleMatricesBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(struct ModelMatrices), NULL, GL_MAP_READ_BIT);

	glGenBuffers(1, &m_materialBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_DRAWS * sizeof(struct MaterialProperties), NULL, GL_STATIC_DRAW);
	struct MaterialProperties *materials = (struct MaterialProperties *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_DRAWS * sizeof(struct MaterialProperties), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	uint32_t materialIndex = 0;
	float green = 0.0f;
	float deltaGreen = 1.0f / (float)NUM_Y;
	for (uint32_t y = 0; y < NUM_Y; ++y)
	{
		float blue = 0.0f;
		float deltaBlue = 1.0f / (float)NUM_Z;
		for (uint32_t z = 0; z < NUM_Z; ++z)
		{
			for (uint32_t x = 0; x < NUM_X; ++x)
			{
				materials[materialIndex].diffuse[0] = 1.0f;
				materials[materialIndex].diffuse[1] = 0.0f; //green;
				materials[materialIndex].diffuse[2] = 1.0f; //blue/
				materials[materialIndex].specular[0] = materials[materialIndex].specular[1] = materials[materialIndex].specular[2] = 0.7f;
				materials[materialIndex].specularPower = 5.0f + 10.0f * x;
				materialIndex++;
			}
			blue += deltaBlue;
		}
		green += deltaGreen;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	uint16_t stride = sizeof(struct Vertex);

	#define NUM_VERTICES (4)
	struct Vertex vertexData[NUM_VERTICES] =
	{
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
	};


	glCreateBuffers(1, &m_vertexBufferName);
	glNamedBufferStorage(m_vertexBufferName, NUM_VERTICES * stride, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_vertexBufferPtr = (struct Vertex *)glMapNamedBufferRange(m_vertexBufferName, 0, NUM_VERTICES * stride, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	memcpy(m_vertexBufferPtr, vertexData, stride * NUM_VERTICES);

	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferName);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(12));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(24));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	#define NUM_INDICES (6)
	uint32_t indices[NUM_INDICES] = { 0, 1, 2, 0, 2, 3 };

	glCreateBuffers(1, &m_indexBufferName);
	glNamedBufferStorage(m_indexBufferName, NUM_INDICES * sizeof(uint32_t), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_indexBufferPtr = (uint32_t *)glMapNamedBufferRange(m_indexBufferName, 0, NUM_INDICES * sizeof(uint32_t), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	memcpy(m_indexBufferPtr, indices, sizeof(uint32_t) * NUM_INDICES);

	loadShader("shaders/blinnphong.vs.glsl", "shaders/blinnphong.fs.glsl");

	struct Mesh mesh = {};
//	loadMesh("assets/torus.s3d", &mesh);
}

#define ION_PI (3.14159265359f)
#define ION_PI_OVER_180 (ION_PI / 180.0f)
	
#define RADIANS(x) (x * ION_PI_OVER_180)
	
void update()
{
	static float angle = 0.0f;
	float sina = 0.0f * sinf(RADIANS(angle));
	float cosa = 1.0f * cosf(RADIANS(angle));
	float radius = 4.5f + (float)(NUM_Z - 1) / 2.0f;

	vec4 eye = vec4_init(sina * radius, -sina * radius, cosa * radius, 1.0f);
	vec4 at = vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 up = vec4_init(0.0f, 1.0f, 0.0f, 0.0f);
	mat4x4 viewMatrix = mat4x4_lookAt(eye, at, up);

	const float fov = RADIANS(90.0f);
	const float aspectRatio = 720.0f / 1280.0f;
	const float nearZ = 0.1f;
	const float farZ = 100.0f;
	const float focalLength = 1.0f / tanf(fov * 0.5f);

	float left = -nearZ / focalLength;
	float right = nearZ / focalLength;
	float bottom = -aspectRatio * nearZ / focalLength;
	float top = aspectRatio * nearZ / focalLength;

	mat4x4 projectionMatrix = mat4x4_frustum(left, right, bottom, top, nearZ, farZ);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_transformsBuffer);
	struct Transforms *block = (struct Transforms *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(struct Transforms),
																	 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	block->viewMatrix = viewMatrix;
	block->projectionMatrix = projectionMatrix;
	mat4x4_mul((float *)&projectionMatrix, (float *)&viewMatrix, (float *)&block->viewProjMatrix);
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	mat4x4 modelMatrix;
	// Bind and write model matrices buffer.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_modelMatricesBuffer);
	struct ModelMatrices *transformsBlock = (struct ModelMatrices *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(struct ModelMatrices), 

																					 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	float baseRotation = (float)glfwGetTime() * 50.0f;
	mat4x4 *instanceMatrix = &transformsBlock->modelMatrices[0];
	vec4 rotAxis = vec4_init(0.0f, 0.0f, -1.0f, 0.0f);
	mat4x4 transMat;
	mat4x4_translate(vec4_init(0.0f, 0.0f, 0.0f, 1.0f), &transMat);

	mat4x4 rotMat;
	mat4x4_rotate(RADIANS(-baseRotation), rotAxis, &rotMat);

	mat4x4_mul((float *)&transMat, (float *)&rotMat, (float *)instanceMatrix);

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);	
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 1280, 720);
/*
	// Bind static transforms block.
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_transformsBuffer);
	Transforms *block = (Transforms *)glMapBufferRange(GL_UNIFORM_BUFFER,
		0,
		sizeof(Transforms),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	block->viewMatrix = m_pCamera->getViewMatrix();
	block->projectionMatrix = m_pCamera->getProjectionMatrix();
	block->viewProjMatrix = m_pCamera->getProjectionMatrix() * m_pCamera->getViewMatrix();
	glUnmapBuffer(GL_UNIFORM_BUFFER);
*/
/*
	Matrix44 modelMatrix;
	// Bind and write model matrices buffer.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_modelMatricesBuffer);
	ModelMatrices *transformsBlock = (ModelMatrices *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ModelMatrices), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	float baseRotation = (float)glfwGetTime() * 50.0f;
	modelMatrix.SetRotation(Deg2Rad(baseRotation), Vector(0.0f, 0.0f, -1.0f));
	transformsBlock->modelMatrices[0] = modelMatrix;
	transformsBlock->modelMatrices[0].SetTranslate(Point(0.0f, 0.0f, 0.0f));

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
*/
	// Bind Visible Matrices Buffer.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_modelMatricesBuffer);
	// Bind Material Buffer.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_materialBuffer);

	// Bind Vertex and Index Buffers.
	glBindVertexArray(m_vertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferName);

	glUseProgram(m_glslProgram);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	
}

void checkForCompileErrors(GLuint shader, GLint shaderType)
{
	int32_t error = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &error);
	if (!error)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			GLint shaderType = 0;
			glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
			if (shaderType == GL_VERTEX_SHADER)
			{
				printf("Error: compiling vertexShader\n%s\n", infoLog);
			}
			else if (shaderType == GL_FRAGMENT_SHADER)
			{
				printf("Error: compiling fragmentShader\n%s\n", infoLog);
			}
			else if (shaderType == GL_COMPUTE_SHADER)
			{
				printf("Error: compiling computeShader\n%s\n", infoLog);
			}
			else
			{
				printf("Error: unknown shader type\n%s\n", infoLog);
			}
			free(infoLog);
		}
	}
}

void loadShader(const char *vertexShaderName, const char *fragmentShaderName)
{

	const char *compileStrings[2] = { NULL, NULL };
		
	m_glslProgram = glCreateProgram();
	if (vertexShaderName)
	{
		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
		FILE *fptr = file_open(vertexShaderName, "rt");
		size_t length = file_length(fptr);
		char* vs = (char *)malloc(length);
		file_read(vs, length, fptr);
		file_close(fptr);

		compileStrings[0] = vs;
		glShaderSource(vertexShader, 1, compileStrings, NULL);
		glCompileShader(vertexShader);
		checkForCompileErrors(vertexShader, GL_VERTEX_SHADER);
		glAttachShader(m_glslProgram, vertexShader);
		glDeleteShader(vertexShader);
		free(vs);
	}
		
	if (fragmentShaderName)
	{
		uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		FILE *fptr = file_open(fragmentShaderName, "rt");
		size_t length = file_length(fptr);
		char* fs = (char *)malloc(length);
		file_read(fs, length, fptr);
		file_close(fptr);

		compileStrings[0] = fs;
		glShaderSource(fragmentShader, 1, compileStrings, NULL);
		glCompileShader(fragmentShader);
		checkForCompileErrors(fragmentShader, GL_FRAGMENT_SHADER);
		glAttachShader(m_glslProgram, fragmentShader);
		glDeleteShader(fragmentShader);
		free(fs);
	}
	glLinkProgram(m_glslProgram);
}
