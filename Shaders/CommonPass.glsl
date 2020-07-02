#type vertex

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// In variables /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) in highp vec3 position;
layout(location = 1) in highp vec2 uv_coord;
layout(location = 2) in highp vec3 normal;
layout(location = 3) in highp vec3 binormal;
layout(location = 4) in highp vec3 tangent;
layout(location = 5) in highp mat4 model;
//layout(location=6) in highp mat4 model; }
//layout(location=7) in highp mat4 model; } reserved for model matrix, because maximum input size is vec4, so each column has its own location
//layout(location=8) in highp mat4 model; }

/////////////////////////////////////////////////////////////
// Vertex shader out variables //////////////////////////////
/////////////////////////////////////////////////////////////

out vertex_shader_out
{

	highp vec2 uv_coord;
	highp vec3 normal;
	highp vec3 tangent;

	highp vec3 frag_pos;

	highp mat3 tbn;
	highp mat3 tbn_inverse;

	highp vec3 camera_pos;

	highp vec4 frag_pos_light_space;

} vs_out;

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 224
layout(std140, binding = 0) uniform SceneData
{

	// 0
	highp vec3 camera_pos;

	// 16
	highp mat4 view_matrix;

	// 80
	highp mat4 projection_matrix;

	// 144
	highp mat4 model_matrix;

	// 208
	highp float use_instancing;

} u_scene;

// 144
layout(std140, binding = 4) uniform ShadowData
{

	// 0
	highp float use_shadow_map;

	// 4
	highp float shadow_bias;
	
	// 8
	highp sampler2DShadow shadow_map;
	
	// 16
	highp mat4 light_view_matrix;
	
	// 80
	highp mat4 light_projection_matrix;

} u_shadow;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/**
 * Calculates the normal matrix using the model matrix.
 *
 * @return The 3x3 normal matrix.
 */
mat3 NormalMatrix(mat4 model_matrix)
{
	mat4 inversed_model_matrix = inverse(model_matrix);
	mat4 transposed_inverse = transpose(inversed_model_matrix);

	mat3 normal_matrix = mat3(transposed_inverse);
	return normal_matrix;
}

/**
 * Calculates the TBN (tangent, bitangent/binormal, normal) matrix using the normal matrix.
 *
 * @return The 3x3 TBN matrix.
 */
mat3 TbnMatrix(mat3 normal_matrix)
{
	vec3 t = normalize(normal_matrix * tangent);
	vec3 b = normalize(normal_matrix * binormal);
	vec3 n = normalize(normal_matrix * normal);

	mat3 result = mat3(t, b, n);
	return result;
}

/**
 * Calculates the re-orthogonalized TBN (tangent, bitangent/binormal, normal) matrix
 * using the Gram-Schmidt process.
 *
 * @return The 3x3 TBN matrix.
 */
mat3 ReOrthogonalizedTbnMatrix(mat3 normal_matrix)
{
	vec3 n = normalize(normal_matrix * normal);
	vec3 t = normalize(normal_matrix * tangent);

	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(n, t);

	mat3 result = mat3(t, b, n);
	return result;
}

/**
 * The vertex shader entry point.
 */
void main()
{

	// Determine model matrix
	mat4 actual_model_matrix = u_scene.model_matrix;
	if (u_scene.use_instancing > 0.5f)
		actual_model_matrix = model;

	// Calculate vertex position
	gl_Position = u_scene.projection_matrix * u_scene.view_matrix * actual_model_matrix * vec4(position, 1.0f);

	// Calculate normal matrix and TBN matrix
	mat3 normal_matrix = NormalMatrix(actual_model_matrix);
	mat3 tbn_matrix = ReOrthogonalizedTbnMatrix(normal_matrix);

	// Set the vertex data out variables
	vs_out.uv_coord = uv_coord;
	vs_out.normal = normal_matrix * normal;
	vs_out.tangent = tangent;

	vs_out.frag_pos = (actual_model_matrix * vec4(position, 1.0f)).xyz;

	vs_out.tbn = tbn_matrix;
	vs_out.tbn_inverse = transpose(tbn_matrix);

	vs_out.camera_pos = u_scene.camera_pos;

	vs_out.frag_pos_light_space = u_shadow.light_projection_matrix * u_shadow.light_view_matrix * actual_model_matrix * vec4(position, 1.0f);
}


#type fragment

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// Defines //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define PI 3.14159265359f
#define MAX_LIGHT_COUNT 4

#define LIGHT_TYPE_NONE 0
#define LIGHT_TYPE_DIRECTIONAL_LIGHT 1
#define LIGHT_TYPE_POINT_LIGHT 2
#define LIGHT_TYPE_SPOT_LIGHT 3

/////////////////////////////////////////////////////////////
// Out variables ////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) out vec4 color;

/////////////////////////////////////////////////////////////
// Vertex shader variables //////////////////////////////////
/////////////////////////////////////////////////////////////

in vertex_shader_out
{

	highp vec2 uv_coord;
	highp vec3 normal;
	highp vec3 tangent;

	highp vec3 frag_pos;

	highp mat3 tbn;
	highp mat3 tbn_inverse;

	highp vec3 camera_pos;

	highp vec4 frag_pos_light_space;

} vs_out;

/////////////////////////////////////////////////////////////
// Structs //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 64
struct Light
{

	// 0
	highp int type;

	// 16
	highp vec3 color;

	// 32
	highp vec3 position;

	// 48
	highp vec3 direction;

};

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(std140, binding = 1) uniform Lights
{

	Light u_light[MAX_LIGHT_COUNT];

};

// 112
layout(std140, binding = 2) uniform Material
{

	// 0
	highp vec3 albedo;

	// 12
	highp float use_albedo_map;

	// 16
	highp float metalness;

	// 20
	highp float use_metalness_map;

	// 24
	highp float roughness;

	// 28
	highp float use_roughness_map;

	// 32
	highp float ao;

	// 36
	highp float use_ao_map;

	// 40
	highp float use_normal_map;

	// 44
	highp float use_displacement_map;

	// 48
	highp float displacement_height_scale;

	// 56
	highp sampler2D albedo_map;

	// 64
	highp sampler2D metalness_map;

	// 72
	highp sampler2D roughness_map;

	// 80
	highp sampler2D ao_map;

	// 88
	highp sampler2D normal_map;

	// 96
	highp sampler2D displacement_map;

} u_material;

// 32
layout(std140, binding = 3) uniform ImageData
{

	// 0
	highp float use_image_based_lighting;
	
	// 8
	highp samplerCube irradiance_map;

	// 16
	highp samplerCube prefiltered_map;

	// 24
	highp sampler2D brdf_texture;

} u_image;

// 144
layout(std140, binding = 4) uniform ShadowData
{

	// 0
	highp float use_shadow_map;
	
	// 4
	highp float shadow_bias;

	// 8
	highp sampler2DShadow shadow_map;
	
	// 16
	highp mat4 light_view_matrix;

	// 80
	highp mat4 light_projection_matrix;

} u_shadow;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/*
 * Gets the objects normal.
 *
 * @param uv_coord The uv coord to use for sampling the normal map.
 *
 * @return The objects surface normal.
 */
vec3 GetNormal(vec2 uv_coord)
{
	vec3 normal = normalize(vs_out.normal);
	if (u_material.use_normal_map > 0.5f)
	{
		normal = texture(u_material.normal_map, uv_coord).xyz;
		normal = normalize(normal * 2.0f - 1.0f);
		normal = normalize(vs_out.tbn * normal);
	}

	return normal;
}

/*
 * Gets the materials albedo color.
 *
 * @param uv_coord The uv coord to use for sampling the albedo map.
 *
 * @return The materials albedo color.
 */
vec3 GetMaterialAlbedo(vec2 uv_coord)
{
	vec3 albedo = u_material.albedo;
	if (u_material.use_albedo_map > 0.5f)
		albedo = texture(u_material.albedo_map, vs_out.uv_coord).rgb;

	return albedo;
}

/*
 * Gets the materials metalness.
 *
 * @param uv_coord The uv coord to use for sampling the metalness map.
 *
 * @return The materials metalness.
 */
float GetMaterialMetalness(vec2 uv_coord)
{
	float metalness = u_material.metalness;
	if (u_material.use_metalness_map > 0.5f)
		metalness = texture(u_material.metalness_map, uv_coord).r;

	return metalness;
}

/*
 * Gets the materials roughness.
 *
 * @param uv_coord The uv coord to use for sampling the roughness map.
 *
 * @return The materials roughness.
 */
float GetMaterialRoughness(vec2 uv_coord)
{
	float roughness = u_material.roughness;
	if (u_material.use_roughness_map > 0.5f)
		roughness = texture(u_material.roughness_map, uv_coord).r;

	return roughness;
}

/*
 * Gets the materials ambient occlusion factor.
 *
 * @param uv_coord The uv coord to use for sampling the ambient occlusion map.
 *
 * @return The materials ambient occlusion factor.
 */
float GetMaterialAO(vec2 uv_coord)
{
	float ao = u_material.ao;
	if (u_material.use_ao_map > 0.5f)
		ao = texture(u_material.ao_map, uv_coord).r;

	return ao;
}

/*
 * The Fresnel-Schlick equation.
 * Calculates the ratio between specular and diffuse reflection,
 * i.e. how much light is reflected vs how mush light is refracted by the surface.
 *
 * @param cos_theta The angle between the halfway vector (between view direction and light direction)
 *					and the view direction.
 * @param f0 The surface reflection at zero incidence,
 *			 i.e. how much the surface reflects if looking directly at the surface.
 *
 * @return The Fresnel-Schlick result.
 */
vec3 FresnelSchlick(float cos_theta, vec3 f0)
{
	return f0 + (1.0f - f0) * pow(1.0f - cos_theta, 5.0f);
}

/*
 *
 */
vec3 FresnelSchlickRoughness(float cos_theta, vec3 f0, float roughness)
{
	return f0 + (max(vec3(1.0f - roughness), f0) - f0) * pow(1.0f - cos_theta, 5.0f);
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
float GeometrySchlickGGX(float n_dot_v, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;

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
 * Calculates light attenuation according to the inverse square law.
 *
 * @param light_position The lights position (world space).
 * @param world_position The position of the current fragment (world space).
 *
 * @return The attenuation factor.
 */
float QuadraticAttenuation(vec3 light_position, vec3 world_position)
{
	float distance = length(light_position - world_position);
	float attenuation = 1.0f / (distance * distance);

	return attenuation;
}

/*
 * Applies parallax occlusion mapping to the given uv coordianates.
 *
 * @param uv_coord The original uv coord.
 * @param view_direction The view direction (tangent space).
 *
 * @return The displaced uv coordinates.
 */
vec2 ParallaxOcclusionMapping(vec2 uv_coord, vec3 view_direction)
{
	if (u_material.use_displacement_map < 0.5f)
		return uv_coord;

	const float min_layers = 8.0f;
	const float max_layers = 32.0f;

	float layer_count = mix(min_layers, max_layers, abs(dot(vec3(0.0f, 0.0f, 1.0f), view_direction)));
	float layer_depth = 1.0f / layer_count;

	vec2 p = view_direction.xy / view_direction.z * u_material.displacement_height_scale;
	vec2 delta_tex_coord = p / layer_count;

	float current_layer_depth = 0.0f;
	vec2 current_uv_coord = uv_coord;
	float current_displacement_map_value = texture(u_material.displacement_map, current_uv_coord).r;

	while (current_layer_depth < current_displacement_map_value)
	{
		current_uv_coord -= delta_tex_coord;
		current_displacement_map_value = texture(u_material.displacement_map, current_uv_coord).r;

		current_layer_depth += layer_depth;
	}

	vec2 previous_uv_coord = current_uv_coord + delta_tex_coord;

	float after_depth = current_displacement_map_value - current_layer_depth;
	float before_depth = texture(u_material.displacement_map, previous_uv_coord).r - current_layer_depth + layer_depth;

	float weight = after_depth / (after_depth - before_depth);
	vec2 final_uv_coord = previous_uv_coord * weight + current_uv_coord * (1.0f - weight);

	return final_uv_coord;
}

/*
 * Determines whether a fragment is in shadow.
 *
 * @param frag_pos_view_space The fragment position (light view space).
 * @param light_direction The light direction.
 * @param normal The normalized vertex normal.
 *
 * @return 1.0, if the fragment is in shadow. 0.0, if the fragment is not in shadow.
 */
float Shadow(vec4 frag_pos_view_space, vec3 light_direction, vec3 normal)
{
	const float sample_count = 4.0f;
	const float sample_start = (sample_count - 1.0f) / 2.0f;
	const float samples2 = sample_count * sample_count;

	if (u_shadow.use_shadow_map > 0.5f)
	{
		// perform perspective divide
		// returns the fragments light space position in the range [-1; 1]
		vec3 projection_coords = frag_pos_view_space.xyz / frag_pos_view_space.w;

		// transforms coords to range [0,1]
		projection_coords = projection_coords * 0.5f + 0.5f;

		if (projection_coords.z > 1.0f)
			return 0.0f;

		// PCF
		float shadow = 0.0f;
		vec2 texel_size = 1.0f / textureSize(u_shadow.shadow_map, 0);

		for (float x = -sample_start; x <= sample_start; x += 1.0f)
		{
			for (float y = -sample_start; y <= sample_start; y += 1.0f)
			{
				vec2 offset = vec2(x, y) * texel_size;
				vec3 uvc = vec3(projection_coords.xy + offset, projection_coords.z - u_shadow.shadow_bias);

				shadow += texture(u_shadow.shadow_map, uvc);
			}
		}

		shadow /= samples2;


		return shadow;
	}

	return 0.0f;
}

/*
 *
 */
vec3 DirectionalLight(Light light, vec3 n, vec3 v, vec3 f0, vec3 albedo, float metalness, float roughness, float ao)
{
	// Calculate light direction and halfway vector
	vec3 l = normalize(-light.direction);
	vec3 h = normalize(v + l);

	// Radiance
	vec3 radiance = light.color;

	// Cook-Torrance BRDF
	float ndf = DistributionGGX(n, h, roughness);
	float g = GeometrySmith(n, v, l, roughness);
	vec3 f = FresnelSchlick(max(dot(h, v), 0.0f), f0);

	vec3 nominator = ndf * g * f;
	float denominator = 4.0f * max(dot(n, v), 0.0f) * max(dot(n, l), 0.0f);
	vec3 specular = nominator / max(denominator, 0.001f); // <-- prevent dividing by zero

	vec3 ks = f;
	vec3 kd = vec3(1.0f) - ks;
	kd *= 1.0f - metalness;

	float n_dot_l = max(dot(n, l), 0.0f);
	vec3 result = (kd * albedo / PI + specular) * radiance * n_dot_l;

	float shadow = Shadow(vs_out.frag_pos_light_space, l, n);
	return result * shadow;
}

/*
 *
 */
vec3 PointLight(Light light, vec3 n, vec3 v, vec3 f0, vec3 albedo, float metalness, float roughness, float ao)
{
	// Calculate light direction and halfway vector
	vec3 l = normalize(light.position - vs_out.frag_pos);
	vec3 h = normalize(v + l);

	// Radiance
	vec3 radiance = light.color * QuadraticAttenuation(light.position, vs_out.frag_pos);

	// Cook-Torrance BRDF
	float ndf = DistributionGGX(n, h, roughness);
	float g = GeometrySmith(n, v, l, roughness);
	vec3 f = FresnelSchlick(max(dot(h, v), 0.0f), f0);

	vec3 nominator = ndf * g * f;
	float denominator = 4.0f * max(dot(n, v), 0.0f) * max(dot(n, l), 0.0f);
	vec3 specular = nominator / max(denominator, 0.001f); // <-- prevent dividing by zero

	vec3 ks = f;
	vec3 kd = vec3(1.0f) - ks;
	kd *= 1.0f - metalness;

	float n_dot_l = max(dot(n, l), 0.0f);
	vec3 result = (kd * albedo / PI + specular) * radiance * n_dot_l;

	return result;
}

/*
 *
 */
vec3 AmbientLight(vec3 n, vec3 v, vec3 r, vec3 f0, vec3 albedo, float roughness, float ao)
{
	vec3 ambient = vec3(0.03f) * albedo * ao;
	if (u_image.use_image_based_lighting > 0.5f)
	{
		vec3 f = FresnelSchlickRoughness(max(dot(n, v), 0.0f), f0, roughness);

		vec3 ks = f;
		vec3 kd = 1.0f - ks;

		vec3 irradiance = texture(u_image.irradiance_map, n).rgb;
		vec3 diffuse = irradiance * albedo;


		const float max_reflection_lod = 4.0f;
		vec3 prefiltered_color = textureLod(u_image.prefiltered_map, r, roughness * max_reflection_lod).rgb;
		vec2 brdf = texture(u_image.brdf_texture, vec2(max(dot(n, v), 0.0f), roughness)).rg;
		vec3 specular = prefiltered_color * (f * brdf.x + brdf.y);


		ambient = (kd * diffuse + specular) * ao;
	}

	return ambient;
}

/*
 * Calculates the total lighting.
 *
 * @param n The normalized normal vector (world space).
 * @param v The normalized view direction (world space).
 * @param uv_coord The uv coordinate to use for sampling the material textures.
 *
 * @return The total color.
 */
vec3 CalculateLighting(vec3 n, vec3 v, vec3 r, vec2 uv_coord)
{
	vec3 albedo = GetMaterialAlbedo(uv_coord);
	float metalness = GetMaterialMetalness(uv_coord);
	float roughness = GetMaterialRoughness(uv_coord);
	float ao = GetMaterialAO(uv_coord);

	vec3 f0 = vec3(0.04f);
	f0 = mix(f0, albedo, metalness);

	vec3 lo = vec3(0.0f);
	for (int i = 0; i < MAX_LIGHT_COUNT; ++i)
	{
		Light light = u_light[i];

		switch (light.type)
		{
			case LIGHT_TYPE_DIRECTIONAL_LIGHT:
				lo += DirectionalLight(light, n, v, f0, albedo, metalness, roughness, ao);
				break;

			case LIGHT_TYPE_POINT_LIGHT:
				lo += PointLight(light, n, v, f0, albedo, metalness, roughness, ao);
				break;

			default: break;
		}

	}

	vec3 ambient = AmbientLight(n, v, r, f0, albedo, roughness, ao);
	return ambient + lo;
}

/**
 * The fragment shader entry point.
 */
void main()
{
	// Calculate view direction
	vec3 v = normalize(vs_out.camera_pos - vs_out.frag_pos);

	// Calculate parallax mapping and normal
	vec2 parallax_uv = ParallaxOcclusionMapping(vs_out.uv_coord, vs_out.tbn_inverse * v);

	// TODO: Does parallax mapping apply to everything? It probably does. It does not make sense to mix offseted and non offseted uv coordinates, does it?
	vec3 n = GetNormal(parallax_uv);

	// Calculate reflection vector
	vec3 r = reflect(-v, n);

	// Calculate result
	vec3 result = CalculateLighting(n, v, r, parallax_uv);
	color = vec4(result, 1.0f);
}