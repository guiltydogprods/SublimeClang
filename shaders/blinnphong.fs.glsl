#version 410 core

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
	vec4 specular;
};

// Material properties
layout (std140, binding = 2) uniform MATERIALS
{
	MaterialProperties materials[16];
};

void main(void)
{
	vec3 Kd = materials[fs_in.material_id].diffuse.xyz;
	vec3 Ks = materials[fs_in.material_id].specular.xyz;
	float m = materials[fs_in.material_id].specular.w;

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

