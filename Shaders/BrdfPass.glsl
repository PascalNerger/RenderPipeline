#type vertex

#version 450 core

/////////////////////////////////////////////////////////////
// In variables ////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) in highp vec3 position;
layout(location = 1) in highp vec2 uv_coord;

/////////////////////////////////////////////////////////////
// Vertex shader variables //////////////////////////////////
/////////////////////////////////////////////////////////////

out vertex_shader_out
{

	highp vec2 uv_coord;

} vs_out;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/**
 * The vertex shader entry point.
 */
void main()
{
	gl_Position = vec4(position, 1.0f);
	vs_out.uv_coord = uv_coord;
}

#type fragment

#version 450 core

/////////////////////////////////////////////////////////////
// Defines //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define PI 3.14159265359f

/////////////////////////////////////////////////////////////
// Out variables ////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) out vec2 color;

/////////////////////////////////////////////////////////////
// Vertex shader variables //////////////////////////////////
/////////////////////////////////////////////////////////////

in vertex_shader_out
{

	highp vec2 uv_coord;

} vs_out;

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
float GeometrySchlickGGX(float n_dot_v, float roughness)
{
	// Different from PBR calculation!
	float r = roughness;
	float k = (r * r) / 2.0f;

	float nominator = n_dot_v;
	float denominator = n_dot_v * (1.0f - k) + k;

	return nominator / denominator;
}

/*
 *
 */
float GeometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
	float n_dot_v = max(dot(n, v), 0.0f);
	float n_dot_l = max(dot(n, l), 0.0f);

	float ggx2 = GeometrySchlickGGX(n_dot_v, roughness);
	float ggx1 = GeometrySchlickGGX(n_dot_l, roughness);

	return ggx1 * ggx2;
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

vec2 IntegrateBrdf(float n_dot_v, float roughness)
{
	vec3 v;
	v.x = sqrt(1.0f - n_dot_v * n_dot_v);
	v.y = 0.0f;
	v.z = n_dot_v;

	float a = 0.0f;
	float b = 0.0f;

	vec3 n = vec3(0.0f, 0.0f, 1.0f);

	const uint sample_count = 1024u;
	for (uint i = 0u; i < sample_count; ++i)
	{
		vec2 xi = Hammersley(i, sample_count);
		vec3 h = ImportanceSampleGGX(xi, n, roughness);
		vec3 l = normalize(2.0f * dot(v, h) * h - v);

		float n_dot_l = max(l.z, 0.0f);
		float n_dot_h = max(h.z, 0.0f);
		float v_dot_h = max(dot(v, h), 0.0f);

		if (n_dot_l > 0.0f)
		{
			float g = GeometrySmith(n, v, l, roughness);
			float g_vis = (g * v_dot_h) / (n_dot_h * n_dot_v);
			float fc = pow(1.0f - v_dot_h, 5.0f);

			a += (1.0f - fc) * g_vis;
			b += fc * g_vis;
		}
	}

	a /= float(sample_count);
	b /= float(sample_count);

	return vec2(a, b);
}

/**
 * The fragment shader entry point.
 */
void main()
{
	vec2 integrated_brdf = IntegrateBrdf(vs_out.uv_coord.x, vs_out.uv_coord.y);
	color = integrated_brdf;
}