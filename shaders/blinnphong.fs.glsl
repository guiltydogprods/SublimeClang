#version 450 core

// Output
layout (location = 0) out vec4 color;

// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
	flat int material_id;
} fs_in;

struct MaterialProperties
{
	vec4 diffuse;
	vec3 specular;
	float specularPower;
};

// Material properties
layout (std430, binding = 1) readonly buffer MATERIALS
{
	MaterialProperties materials[];
};

void main(void)
{
	vec3 Kd = materials[fs_in.material_id].diffuse.xyz;
	vec3 Ks = materials[fs_in.material_id].specular;
	float m = materials[fs_in.material_id].specularPower;

    vec3 n = normalize(fs_in.N);
    vec3 l = normalize(fs_in.L);
    vec3 v = normalize(fs_in.V);
    vec3 h = normalize(l + v);
	vec3 Lo = vec3(0.0);
 
	float nDotL = clamp(dot(n, l), 0.0, 1.0);
	float nDotH = clamp(dot(n, h), 0.0, 1.0);
	Lo += (Kd + Ks * pow(nDotH, m)) * nDotL;

	color = vec4(Lo, 1.0);
}

