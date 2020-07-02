#type vertex

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// In variables /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) in highp vec2 position;
layout(location = 1) in highp vec2 uv_coord;

/////////////////////////////////////////////////////////////
// Vertex shader out variables //////////////////////////////
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
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	vs_out.uv_coord = uv_coord;
}

#type fragment

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// Defines //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define KERNEL_SIZE 5

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

} vs_out;

/////////////////////////////////////////////////////////////
// Const variables //////////////////////////////////////////
/////////////////////////////////////////////////////////////

const float u_weights[5] = float[](0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 16
layout(std140, binding = 0) uniform BlurInfo
{

	// 0
	highp sampler2D scene_texture;

	// 8
	highp float horizontal;

} u_info;


/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/*
 * Horizontally blurs the texture.
 *
 * @param texture_offset The texture offset, i.e. the x and y values of the texel size.
 *
 * @return The blurred color.
 */
vec3 HorizontalBlur(vec2 texture_offset)
{
	vec3 result = vec3(0.0f);
	for (int i = 1; i < KERNEL_SIZE; ++i)
	{
		result += texture(u_info.scene_texture, vs_out.uv_coord + vec2(texture_offset.x * i, 0.0f)).rgb * u_weights[i];
		result += texture(u_info.scene_texture, vs_out.uv_coord - vec2(texture_offset.x * i, 0.0f)).rgb * u_weights[i];
	}

	return result;
}

/*
 * Vertically blurs the texture.
 *
 * @param texture_offset The texture offset, i.e. the x and y values of the texel size.
 *
 * @return The blurred color.
 */
vec3 VerticalBlur(vec2 texture_offset)
{
	vec3 result = vec3(0.0f);
	for (int i = 1; i < KERNEL_SIZE; ++i)
	{
		result += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0f, texture_offset.y * i)).rgb * u_weights[i];
		result += texture(u_info.scene_texture, vs_out.uv_coord - vec2(0.0f, texture_offset.y * i)).rgb * u_weights[i];
	}

	return result;
}

/*
 * Adapted and modified horizontal blur from Godot Engine.
 */
vec3 HorizontalBlurGodot(float pixel_size)
{
	vec3 color = vec3(0.0f);

	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 0.0, 0.0) * pixel_size).rgb * 0.214607f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 1.0, 0.0) * pixel_size).rgb * 0.189879f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 2.0, 0.0) * pixel_size).rgb * 0.131514f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 3.0, 0.0) * pixel_size).rgb * 0.071303f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(-1.0, 0.0) * pixel_size).rgb * 0.189879f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(-2.0, 0.0) * pixel_size).rgb * 0.131514f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(-3.0, 0.0) * pixel_size).rgb * 0.071303f;

	return color;
}

/*
 * Adapted and modified vertical blur from Godot Engine.
 */
vec3 VerticalBlurGodot(float pixel_size)
{
	vec3 color = vec3(0.0f);

	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0,  0.0) * pixel_size).rgb * 0.38774f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0,  1.0) * pixel_size).rgb * 0.24477f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0,  2.0) * pixel_size).rgb * 0.06136f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0, -1.0) * pixel_size).rgb * 0.24477f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0, -2.0) * pixel_size).rgb * 0.06136f;

	return color;
}

/*
 * Adapted and modified horizontal glow from Godot Engine.
 */
vec3 HorizontalGlowGodot(float pixel_size, float glow_strength)
{
	vec3 color = vec3(0.0f);

	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 0.0, 0.0) * pixel_size).rgb * 0.174938f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 1.0, 0.0) * pixel_size).rgb * 0.165569f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 2.0, 0.0) * pixel_size).rgb * 0.140367f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2( 3.0, 0.0) * pixel_size).rgb * 0.106595f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(-1.0, 0.0) * pixel_size).rgb * 0.165569f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(-2.0, 0.0) * pixel_size).rgb * 0.140367f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(-3.0, 0.0) * pixel_size).rgb * 0.106595f;

	color *= glow_strength;
	return color;
}

/*
 * Adapted and modified vertical glow from Godot Engine.
 */
vec3 VerticalGlowGodot(float pixel_size, float glow_strength)
{
	vec3 color = vec3(0.0f);

	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0,  0.0) * pixel_size).rgb * 0.288713f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0,  1.0) * pixel_size).rgb * 0.233062f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0,  2.0) * pixel_size).rgb * 0.122581f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0, -1.0) * pixel_size).rgb * 0.233062f;
	color += texture(u_info.scene_texture, vs_out.uv_coord + vec2(0.0, -2.0) * pixel_size).rgb * 0.122581f;

	color *= glow_strength;
	return color;
}

/*
 * Performs a Gaussian Blur.
 */
vec3 GaussianBlur(vec2 pixel_size)
{
	// Calculate the current fragments contribution
	vec3 result = texture(u_info.scene_texture, vs_out.uv_coord).rgb * u_weights[0];

	if (u_info.horizontal > 0.5f)
		result += HorizontalBlur(pixel_size);
	else
		result += VerticalBlur(pixel_size);

	return result;
}

/*
 * Performs the Godot Gaussian Blur.
 */
vec3 GaussianBlurGodot(vec2 pixel_size)
{
	vec3 result = vec3(0.0f);

	if (u_info.horizontal > 0.5f)
		result += HorizontalBlurGodot(pixel_size.x);
	else
		result += VerticalBlurGodot(pixel_size.y);

	return result;
}

/*
 * Performs the Godot Glow.
 */
vec3 GlowGodot(vec2 pixel_size, float glow_strength)
{
	vec3 result = vec3(0.0f);

	if (u_info.horizontal > 0.5f)
		result += HorizontalGlowGodot(pixel_size.x, glow_strength);
	else
		result += VerticalGlowGodot(pixel_size.y, glow_strength);

	return result;
}

/**
 * The fragment shader entry point.
 */
void main()
{
	vec2 pixel_size = 1.0f / textureSize(u_info.scene_texture, 0);

	vec3 result = GaussianBlur(pixel_size);
	//vec3 result = GaussianBlurGodot(pixel_size);
	//vec3 result = GlowGodot(pixel_size, 1.0f);

	color = vec4(result, 1.0f);
}