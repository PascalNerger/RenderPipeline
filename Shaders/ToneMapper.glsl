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

#define TONE_MAPPER_REINHARD		1
#define TONE_MAPPER_EXPOSURE		2
#define TONE_MAPPER_ACES			3
#define TONE_MAPPER_GODOT_FILMIC	4
#define TONE_MAPPER_GODOT_ACES		5

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
// Uniforms /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(std140, binding = 0) uniform ToneMapperData
{

	highp int type;
	highp float exposure;

	highp sampler2D hdr_source_texture;

} u_data;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/**
 * The reinhard tone mapping calculation.
 * Maps a color from HDR color range to LDR color range.
 * Attention: The color on which the tone mapping is applied must be in linear color space.
 *
 * @param color The color to apply tone mapping on.
 *
 * @return The tone mapped color.
 */
vec3 ReinhardToneMapping(vec3 color)
{
	return color / (color + vec3(1.0f));
}

/**
 * Applies exposure tone mapping on a color.
 * Attention: The color on which the tone mapping is applied must be in linear color space.
 *
 * @param color The color to apply tone mapping on.
 * @param exposure The exposure to use for tone mapping.
 *
 * @return The tone mapped color.
 */
vec3 ExposureToneMapping(vec3 color, float exposure)
{
	return vec3(1.0f) - exp(-color * exposure);
}

/**
 * ACES filmic tonemapping.
 * Maps a color from HDR color range to LDR color range.
 * Attention: The color on which the tone mapping is applied must be in linear color space.
 *
 * @param color The color to apply tonemapping on.
 *
 * @return The tone mapped color.
 */
vec3 AcesFilmic(vec3 color)
{
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;

	return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);
}

vec3 GodotFilmic(vec3 color, float white) 
{
	const float exposure_bias = 2.0f;

	const float A = 0.22f * exposure_bias * exposure_bias;
	const float B = 0.30f * exposure_bias;
	const float C = 0.10f;
	const float D = 0.20f;
	const float E = 0.01f;
	const float F = 0.30f;

	vec3 color_tonemapped = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white_tonemapped = ((white * (A * white + C * B) + D * E) / (white * (A * white + B) + D * F)) - E / F;

	return clamp(color_tonemapped / white_tonemapped, vec3(0.0f), vec3(1.0f));
}

vec3 GodotAces(vec3 color, float white) 
{
	const float exposure_bias = 0.85f;

	const float A = 2.51f * exposure_bias * exposure_bias;
	const float B = 0.03f * exposure_bias;
	const float C = 2.43f * exposure_bias * exposure_bias;
	const float D = 0.59f * exposure_bias;
	const float E = 0.14f;

	vec3 color_tonemapped = (color * (A * color + B)) / (color * (C * color + D) + E);
	float white_tonemapped = (white * (A * white + B)) / (white * (C * white + D) + E);

	return clamp(color_tonemapped / white_tonemapped, vec3(0.0f), vec3(1.0f));
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
 * Tonemaps a given color with the specified Tone Mapper type.
 *
 * @param type The type of Tone Mapper to use.
 * @param hdr_color The HDR color to tonemap.
 *
 * @return The tonemapped color.
 */
vec3 GetToneMappedColor(int type, vec3 hdr_color, float exposure)
{
	switch (type)
	{
		case TONE_MAPPER_REINHARD:		return ReinhardToneMapping(hdr_color);
		case TONE_MAPPER_EXPOSURE:		return ExposureToneMapping(hdr_color, exposure);
		case TONE_MAPPER_ACES:			return AcesFilmic(hdr_color);
		case TONE_MAPPER_GODOT_FILMIC:	return GodotFilmic(hdr_color, 1.0f);
		case TONE_MAPPER_GODOT_ACES:	return GodotAces(hdr_color, 1.0f);

		default: break;
	}

	return AcesFilmic(hdr_color);
}

/**
 * The fragment shader entry point.
 */
void main()
{
	vec3 hdr_color = texture(u_data.hdr_source_texture, vs_out.uv_coord).rgb;
	vec3 mapped_color = GetToneMappedColor(u_data.type, hdr_color, u_data.exposure);

	vec3 result = GammaCorrect(mapped_color, 2.2f);

	color = vec4(result, 1.0f);
}