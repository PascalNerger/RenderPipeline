#type vertex

#version 330 core

/////////////////////////////////////////////////////////////
// In variables /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) in highp vec3 position;
layout(location = 1) in highp vec2 uv_coord;
layout(location = 2) in highp vec3 normal;
layout(location = 3) in highp vec3 binormal;
layout(location = 4) in highp vec3 tangent;

/////////////////////////////////////////////////////////////
// Vertex shader out variables //////////////////////////////
/////////////////////////////////////////////////////////////

out vertex_shader_out{

	highp vec2 uv_coord;
	highp vec3 normal;
	highp vec3 binormal;
	highp vec3 tangent;

	highp vec3 camera_pos;
	highp vec3 frag_pos; // <-- Tangent space
	highp vec4 frag_pos_light_space;
	highp mat3 tbn;

} vs_out;

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

uniform highp mat4 u_model_matrix;
uniform highp mat4 u_view_matrix;
uniform highp mat4 u_projection_matrix;

uniform highp mat4 u_light_space_matrix;

uniform highp vec3 u_camera_pos;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/**
 * Calculates the normal matrix using the model matrix.
 *
 * @return The 3x3 normal matrix.
 */
mat3 NormalMatrix()
{
	mat4 inversed_model_matrix = inverse(u_model_matrix);
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
 * Calculates the re-orthogonalized TBN (tangent, bitangent/binormal, normal) matrix using the Gram-Schmidt process.
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
	// Calculate vertex shader output variables
	mat3 normal_matrix = NormalMatrix();
	mat3 tbn_matrix = ReOrthogonalizedTbnMatrix(normal_matrix);
	mat3 tbn = inverse(tbn_matrix);

	vs_out.uv_coord = uv_coord;
	vs_out.normal = normal_matrix * normal;
	vs_out.binormal = binormal;
	vs_out.tangent = tangent;

	// Transform camera position and fragment position to tangent space
	vs_out.camera_pos = tbn * u_camera_pos;
	vs_out.frag_pos = tbn * (u_model_matrix * vec4(position, 1.0f)).xyz;
	vs_out.tbn = tbn;

	vs_out.frag_pos_light_space = u_light_space_matrix * u_model_matrix * vec4(position, 1.0f);


	// Calculate position
	mat4 mvp = u_projection_matrix * u_view_matrix * u_model_matrix;
	gl_Position = mvp * vec4(position, 1.0f);
}

#type fragment

#version 330 core

/////////////////////////////////////////////////////////////
// Defines //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

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

in vertex_shader_out{

	highp vec2 uv_coord;
	highp vec3 normal;
	highp vec3 binormal;
	highp vec3 tangent;

	highp vec3 camera_pos;
	highp vec3 frag_pos;
	highp vec4 frag_pos_light_space;
	highp mat3 tbn;

} vs_out;

/////////////////////////////////////////////////////////////
// Structs //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

struct Light
{
	highp int type;

	highp vec3 position;
	highp vec3 direction;

	highp vec3 color;

	highp float constant;
	highp float linear;
	highp float quadratic;

	highp float cutoff;
	highp float outer_cutoff;
};

struct Material
{

	highp vec3 diffuse;
	highp float use_diffuse_map;
	highp sampler2D diffuse_map;

	highp vec3 specular;
	highp float use_specular_map;
	highp sampler2D specular_map;

	highp float use_normal_map;
	highp sampler2D normal_map;

	highp float use_displacement_map;
	highp sampler2D displacement_map;
	highp float displacement_height_scale;

	highp float shininess;
};

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

uniform highp int u_light_count;
uniform Light u_light[MAX_LIGHT_COUNT];

uniform Material u_material;

uniform highp float u_ambient_factor = 0.1f;
uniform highp float u_parallax_height_scale = 0.01f;

uniform highp sampler2D u_shadow_map;
uniform highp float u_use_shadow_map = 0.0f;
uniform highp float u_shadow_bias = 0.005f;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/*
 * Calcuates the normal in tangent space.
 *
 * @return The tangent space normal.
 */
vec3 GetNormal(vec2 uv_coord)
{
	vec3 normal = normalize(vs_out.tbn * vs_out.normal);
	if (u_material.use_normal_map > 0.5f)
	{
		normal = texture(u_material.normal_map, uv_coord).xyz;
		normal = normalize(normal * 2.0f - 1.0f);
	}

	return normal;
}

/*
 * Calculates the diffuse color of a material based on its settings.
 *
 * @param uv_coord The uv coordinate used for the diffuse map.
 *
 * @return The material diffuse color.
 */
vec3 GetMaterialDiffuse(vec2 uv_coord)
{
	vec3 diffuse_color = u_material.diffuse;
	if (u_material.use_diffuse_map > 0.5)
		diffuse_color = texture(u_material.diffuse_map, uv_coord).rgb;

	return diffuse_color;
}

/*
 * Calculates the specular color of a material based on its settings.
 *
 * @param uv_coord The uv coordinate used for the specular map.
 *
 * @return The material specular color.
 */
vec3 GetMaterialSpecular(vec2 uv_coord)
{
	vec3 specular_color = u_material.specular;
	if (u_material.use_specular_map > 0.5f)
		specular_color = texture(u_material.specular_map, uv_coord).rgb;

	return specular_color;
}

/**
 * Calculates the light attenuation based on a realistic approximation.
 * This can be used in linear color space as well as in gamma color space.
 *
 * @param light The light to calculate the attenuation for.
 *
 * @return The lights attenuation factor.
 */
float ComplexAttenuation(Light light)
{
	vec3 light_position = vs_out.tbn * light.position;
	float distance = length(light_position - vs_out.frag_pos);

	// Calculate linear and quadratic terms for attenuation formular
	float linear = light.linear * distance;
	float quadratic = light.quadratic * (distance * distance);

	float attenuation = 1.0f / (light.constant + linear + quadratic);
	return attenuation;
}

/**
 * Calculates the linear light attenuation.
 * This is specifically suitable for gamma corrected attenuation.
 *
 * @param light The light to calculate the linear attenuation for.
 *
 * @return The lights attenuation factor.
 */
float LinearAttenuation(Light light)
{
	vec3 light_position = vs_out.tbn * light.position;
	float distance = length(light_position - vs_out.frag_pos);

	return 1.0f / distance;
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
 * Applies parallax mapping to the given uv coordianates.
 *
 * @param uv_coord The original uv coord.
 * @param view_direction The view direction (tangent space).
 *
 * @return The displaced uv coordinates.
 */
vec2 ParallaxMapping(vec2 uv_coord, vec3 view_direction)
{
	if (u_material.use_displacement_map > 0.5f)
	{
		float height = texture(u_material.displacement_map, uv_coord).r;
		vec2 p = view_direction.xy / view_direction.z * (height * u_material.displacement_height_scale);

		uv_coord = uv_coord - p;
	}

	return uv_coord;
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

	const float min_layers = 8.0;
	const float max_layers = 32.0;

	float layer_count = mix(min_layers, max_layers, abs(dot(vec3(0.0, 0.0, 1.0), view_direction)));
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
 * Calculates ambient lighting.
 *
 * @param light The light to calculate the ambient component for.
 * @param view_direction The view direction (tangent space).
 *
 * @return The ambient color.
 */
vec3 Ambient(Light light, vec3 view_direction)
{
	vec2 uv_coord = ParallaxOcclusionMapping(vs_out.uv_coord, view_direction);

	vec3 light_color = u_ambient_factor * light.color;
	vec3 ambient = light_color * GetMaterialDiffuse(uv_coord);

	return ambient;
}

/**
 * Calculates diffuse lighting based on the Phong shading model in linear color space.
 *
 * @param light The light to calculate diffuse lighting for.
 * @param light_direction The normalized vector from the frament position to the light source (tangent space).
 * @param view_direction The view direction (tangent space)
 * @param normal The normalized vertex normal (tangent space).
 *
 * @return The lights diffuse component.
 */
vec3 PhongDiffuse(Light light, vec3 light_direction, vec3 view_direction, vec3 normal)
{
	float diff = dot(normal, light_direction);
	float max_diff = max(diff, 0.0f);

	vec2 uv_coord = ParallaxOcclusionMapping(vs_out.uv_coord, view_direction);
	vec3 diffuse = light.color * max_diff * GetMaterialDiffuse(uv_coord);

	return diffuse;
}

/**
 * Calculates specular lighting based on the Phong shading model in linear color space.
 *
 * @param light The light to calculate specular lighting for.
 * @param light_direction The normalized vector from the frament position to the light source (tangent space).
 * @param view_direction The view direction (tangent space).
 * @param normal The normalized vertex normal (tangent space).
 *
 * @return The lights specular component.
 */
vec3 PhongSpecular(Light light, vec3 light_direction, vec3 view_direction, vec3 normal)
{
	vec3 reflect_direction = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), u_material.shininess);

	vec2 uv_coord = ParallaxOcclusionMapping(vs_out.uv_coord, view_direction);
	vec3 specular = light.color * spec * GetMaterialSpecular(uv_coord);

	return specular;
}

/**
 * Calculates specular lighting based on the Blinn shading model in linear color space.
 *
 * @param light The light to calculate specular lighting for.
 * @param light_direction The normalized vector from the frament position to the light source (tangent space).
 * @param view_direction The view direction (tangent space).
 * @param normal The normalized vertex normal (tangent space).
 *
 * @return The lights specular component.
 */
vec3 BlinnSpecular(Light light, vec3 light_direction, vec3 view_direction, vec3 normal)
{
	vec3 halfway_direction = normalize(light_direction + view_direction);

	float cosine_angle = dot(normal, halfway_direction);
	float max_cosine_angle = max(cosine_angle, 0.0f);
	float spec = pow(max_cosine_angle, u_material.shininess);

	vec2 uv_coord = ParallaxOcclusionMapping(vs_out.uv_coord, view_direction);
	vec3 specular = light.color * spec * GetMaterialSpecular(uv_coord);

	return specular;
}

/*
 * Determines whether a fragment is in shadow.
 *
 * @param frag_pos_light_space The fragment position (light space).
 * @param light_direction The light direction. Must be in the same coordinate space as the normal.
 * @param normal The normalized vertex normal. Must be in the same coordinate space as the light direction.
 *
 * @return 1.0, if the fragment is in shadow. 0.0, if the fragment is not in shadow.
 */
float Shadow(vec4 frag_pos_light_space, vec3 light_direction, vec3 normal)
{
	if (u_use_shadow_map > 0.5f)
	{
		// perform perspective divide
		// returns the fragments light space position in the range [-1; 1]
		vec3 projection_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;

		// transforms coords to range [0,1]
		projection_coords = projection_coords * 0.5f + 0.5f;

		if (projection_coords.z > 1.0f)
			return 0.0f;

		float closest_depth = texture(u_shadow_map, projection_coords.xy).r;
		float current_depth = projection_coords.z;

		// Calculates shadow bias based on the angle to the light source
		// 0.05f is the maximum shadow bias
		float bias = max(0.05f * (1.0f - dot(normal, light_direction)), u_shadow_bias);

		float is_in_shadow = current_depth - bias > closest_depth ? 1.0f : 0.0f;
		return is_in_shadow;
	}

	return 0.0f;
}

/**
 * Calculates directional lighting based on the Blinn-Phong shading model in linear color space.
 *
 * @param light The light to calculate directional lighting for.
 * @param view_direction The view direction (tangent space).
 * @param normal The normalized vertex normal (tangent space).
 *
 * @return The color for the current fragment.
 */
vec3 DirectionalLight(Light light, vec3 view_direction, vec3 normal)
{
	// Calculate the light direction
	vec3 light_direction = normalize(vs_out.tbn * -light.direction);

	// Calculate lighting
	vec3 ambient = Ambient(light, view_direction);
	vec3 diffuse = PhongDiffuse(light, light_direction, view_direction, normal);
	vec3 specular = BlinnSpecular(light, light_direction, view_direction, normal);

	float shadow = Shadow(vs_out.frag_pos_light_space, light_direction, normal);
	float shadow_factor = 1 - shadow;

	vec3 result = ambient + shadow_factor * (diffuse + specular);
	return result;
}

/**
 * Calculates point lighting based on the Blinn-Phong shading model in linear color space.
 *
 * @param light The light to calculate point lighting for.
 * @param view_direction The view direction (tangent space).
 * @param normal The normalized vertex normal (tangent space).
 *
 * @return The color for the current fragment.
 */
vec3 PointLight(Light light, vec3 view_direction, vec3 normal)
{
	// Calculate the light direction
	vec3 light_position = vs_out.tbn * light.position;
	vec3 light_direction = normalize(light_position - vs_out.frag_pos);

	// Calculate lighting
	vec3 ambient = Ambient(light, view_direction);
	vec3 diffuse = PhongDiffuse(light, light_direction, view_direction, normal);
	vec3 specular = BlinnSpecular(light, light_direction, view_direction, normal);

	// Calculate attenuation
	float attenuation = LinearAttenuation(light);

	// Calculate result
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

/**
 * Calculates spot lighting based on the Blinn-Phong shading model in linear color space.
 *
 * @param light The light to calculate directional lighting for.
 * @param view_direction The view direction (tangent space).
 * @param normal The normalized vertex normal (tangent space).
 *
 * @return The color for the current fragment.
 */
vec3 SpotLight(Light light, vec3 view_direction, vec3 normal)
{
	vec3 result = vec3(0.0f);

	// Calculate light direction and theta
	vec3 light_position = vs_out.tbn * light.position;
	vec3 light_direction = normalize(light_position - vs_out.frag_pos);

	float theta = dot(light_direction, normalize(vs_out.tbn * -light.direction));

	// Calculate attenuation
	float attenuation = LinearAttenuation(light);

	// Calculate ambient lighting always
	vec3 ambient = Ambient(light, view_direction);
	ambient *= attenuation;

	// Use cosine values of cutoff, because the dot product calculated for theta returns a cosine angle
	float cos_cutoff = cos(light.cutoff);
	float cos_outer_cutoff = cos(light.outer_cutoff);

	if (theta > cos_cutoff)
	{
		// Calculate diffuse and specular lighting
		vec3 diffuse = PhongDiffuse(light, light_direction, view_direction, normal);
		vec3 specular = BlinnSpecular(light, light_direction, view_direction, normal);

		// Calculate intensity
		float epsilon = cos_cutoff - cos_outer_cutoff;
		float intensity = clamp((theta - cos_outer_cutoff) / epsilon, 0.0f, 1.0f);

		// Calculate result
		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;

		result = diffuse + specular;
	}


	return ambient + result;
}

/**
 * Calculates the total lighting based on the Blinn-Phong shading model in linear color space.
 *
 * @param view_direction The view direction (tangent space).
 * @param normal The normalized vertex normal (tangent space).
 *
 * @return The color for the current fragment.
 */
vec3 CalculateLighting(vec3 view_direction, vec3 normal)
{
	vec3 result = vec3(0.0f);

	int max = MAX_LIGHT_COUNT;
	if (u_light_count < max)
		max = u_light_count;

	for (int i = 0; i < max; i++)
	{
		switch (u_light[i].type)
		{
		case LIGHT_TYPE_DIRECTIONAL_LIGHT:	result += DirectionalLight(u_light[i], view_direction, normal);	break;
		case LIGHT_TYPE_POINT_LIGHT:		result += PointLight(u_light[i], view_direction, normal);		break;
		case LIGHT_TYPE_SPOT_LIGHT:			result += SpotLight(u_light[i], view_direction, normal);		break;

		default: break;
		}
	}

	return result;
}

/**
 * Transforms a color from linear space to gamma space.
 *
 * @param color The linear space color to transform.
 * @param gamma The gamma factor. Usually a value of 2.2.
 *
 * @return The gamma corrected color.
 */
vec3 GammaCorrect(vec3 color, float gamma)
{
	return pow(color, vec3(1.0f / gamma));
}

/**
 * The fragment shader entry point.
 */
void main()
{
	// Calculate shared variables
	vec3 view_direction = normalize(vs_out.camera_pos - vs_out.frag_pos);
	vec3 normal = GetNormal(ParallaxOcclusionMapping(vs_out.uv_coord, view_direction));

	vec3 result = CalculateLighting(view_direction, normal);

	// Do not gamma correct here. This is done by the HDR shader.
	// result = GammaCorrect(result, 2.2f);

	color = vec4(result, 1.0);
}