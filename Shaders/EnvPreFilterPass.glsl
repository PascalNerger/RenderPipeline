#type vertex

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// In variables /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) in highp vec3 position;

/////////////////////////////////////////////////////////////
// Vertex shader out variables //////////////////////////////
/////////////////////////////////////////////////////////////

out vertex_shader_out
{

	highp vec3 local_pos;

} vs_out;

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 144
layout(std140, binding = 0) uniform PreFilterInfo
{

	// 0
	highp mat4 view_matrix;

	// 64
	highp mat4 projection_matrix;

	// 128
	highp float roughness;

	// 132
	highp float map_resolution;

	// 136
	highp samplerCube map;

} u_info;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/**
 * The fragment shader entry point.
 */
void main()
{
	gl_Position = u_info.projection_matrix * u_info.view_matrix * vec4(position, 1.0f);
	vs_out.local_pos = position;
}


#type fragment

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// Defines //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define PI 3.14159265359f

/////////////////////////////////////////////////////////////
// Out variables ////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) out vec4 color;

/////////////////////////////////////////////////////////////
// Vertex shader variables //////////////////////////////////
/////////////////////////////////////////////////////////////

in vertex_shader_out
{

	highp vec3 local_pos;

} vs_out;

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 144
layout(std140, binding = 0) uniform PreFilterInfo
{

	// 0
	highp mat4 view_matrix;
	
	// 64
	highp mat4 projection_matrix;
	
	// 128
	highp float roughness;
	
	// 132
	highp float map_resolution;
	
	// 136
	highp samplerCube map;

} u_info;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/*
 *
 */
float RadicalInverseVdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

	return float(bits) * 2.3283064365386963e-10;
}

/*
 *
 */
vec2 Hammersley(uint i, uint n)
{
	return vec2(float(i) / float(n), RadicalInverseVdC(i));
}

/*
 *
 */
float DistributionGGX(vec3 n, vec3 h, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;

	float n_dot_h = max(dot(n, h), 0.0f);
	float n_dot_h_2 = n_dot_h * n_dot_h;

	float nominator = a2;
	float denominator = (n_dot_h_2 * (a2 - 1.0f) + 1.0f);
	denominator = PI * denominator * denominator;

	return nominator / denominator;
}

/*
 *
 */
vec3 ImportanceSampleGGX(vec2 xi, vec3 n, float roughness)
{
	float a = roughness * roughness;

	float phi = 2.0f * PI * xi.x;
	float cos_theta = sqrt((1.0f - xi.y) / (1.0f + (a * a - 1.0f) * xi.y));
	float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

	vec3 h;
	h.x = cos(phi) * sin_theta;
	h.y = sin(phi) * sin_theta;
	h.z = cos_theta;

	vec3 up = abs(n.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 tangent = normalize(cross(up, n));
	vec3 bitangent = cross(n, tangent);

	vec3 sample_vec = tangent * h.x + bitangent * h.y + n * h.z;
	return normalize(sample_vec);
}

vec3 PrefilterEnvMap(float roughness, vec3 n)
{
	// assumption that view direction equals r equals the normal (Epic Games)
	vec3 r = n;
	vec3 v = r;

	const uint sample_count = 1024u;
	vec3 prefiltered_color = vec3(0.0f);

	float total_weight = 0.0f;

	for (uint i = 0u; i < sample_count; ++i)
	{

		vec2 xi = Hammersley(i, sample_count);
		vec3 h = ImportanceSampleGGX(xi, n, roughness);
		vec3 l = normalize(2.0f * dot(v, h) * h - v);

		float n_dot_l = max(dot(n, l), 0.0f);

		if (n_dot_l > 0.0f)
		{
			float d = DistributionGGX(n, h, roughness);
			float n_dot_h = max(dot(n, h), 0.0f);
			float h_dot_v = max(dot(h, v), 0.0f);

			float pdf = d * n_dot_h / (4.0f * h_dot_v) + 0.0001f;

			float resolution = u_info.map_resolution;
			float sa_texel = 4.0f * PI / (6.0f * resolution * resolution);
			float sa_sample = 1.0f / (float(sample_count) * pdf + 0.0001f);

			float mip_level = roughness == 0.0f ? 0.0f : 0.5f * log2(sa_sample / sa_texel);

			prefiltered_color += textureLod(u_info.map, l, mip_level).rgb * n_dot_l;
			total_weight += n_dot_l;
		}
	}

	prefiltered_color = prefiltered_color / total_weight;
	return prefiltered_color;
}

/**
 * The fragment shader entry point.
 */
void main()
{
	vec3 n = normalize(vs_out.local_pos);

	vec3 result = PrefilterEnvMap(u_info.roughness, n);
	color = vec4(result, 1.0f);
}