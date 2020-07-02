#type vertex

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// In variables ////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) in highp vec2 position;
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
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	vs_out.uv_coord = uv_coord;
}




#type fragment

#version 450 core
#extension GL_ARB_bindless_texture : require

/////////////////////////////////////////////////////////////
// Defines //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define BLEND_MODE_NONE		0
#define BLEND_MODE_ADDITIVE 1
#define BLEND_MODE_SCREEN	2

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
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 48
layout(std140, binding = 0) uniform Textures
{

	// 0
	highp int blend_mode;

	// 8
	highp sampler2D texture1;

	// 16
	highp sampler2D texture2;

	// 24
	highp float use_texture1_bicubic_upscale;

	// 28
	highp float use_texture2_bicubic_upscale;

	// 32
	highp float texture1_mix;

	// 36
	highp float texture2_mix;

} u_textures;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

float W0(float a) 
{
	return (1.0f / 6.0f) * (a * (a * (-a + 3.0f) - 3.0f) + 1.0f);
}

float W1(float a) 
{
	return (1.0f / 6.0f) * (a * a * (3.0f * a - 6.0f) + 4.0f);
}

float W2(float a) 
{
	return (1.0f / 6.0f) * (a * (a * (-3.0f * a + 3.0f) + 3.0f) + 1.0f);
}

float W3(float a) 
{
	return (1.0f / 6.0f) * (a * a * a);
}

float G0(float a) 
{
	return W0(a) + W1(a);
}

float G1(float a) 
{
	return W2(a) + W3(a);
}

float H0(float a) 
{
	return -1.0f + W1(a) / (W0(a) + W1(a));
}

float H1(float a) 
{
	return 1.0f + W3(a) / (W2(a) + W3(a));
}

vec3 TextureBicubic(sampler2D source, vec2 uv_coord)
{
	vec2 pixel_size = 1.0f / textureSize(source, 0);
	uv_coord = uv_coord * pixel_size + vec2(0.5f);

	vec2 iuv = floor(uv_coord);
	vec2 fuv = fract(uv_coord);

	float g0x = G0(fuv.x);
	float g1x = G1(fuv.x);
	float h0x = H0(fuv.x);
	float h1x = H1(fuv.x);
	float h0y = H0(fuv.y);
	float h1y = H1(fuv.y);

	vec2 p0 = (vec2(iuv.x + h0x, iuv.y + h0y) - vec2(0.5f)) * pixel_size;
	vec2 p1 = (vec2(iuv.x + h1x, iuv.y + h0y) - vec2(0.5f)) * pixel_size;
	vec2 p2 = (vec2(iuv.x + h0x, iuv.y + h1y) - vec2(0.5f)) * pixel_size;
	vec2 p3 = (vec2(iuv.x + h1x, iuv.y + h1y) - vec2(0.5f)) * pixel_size;

	vec3 source_p0 = texture(source, p0).rgb;
	vec3 source_p1 = texture(source, p1).rgb;
	vec3 source_p2 = texture(source, p2).rgb;
	vec3 source_p3 = texture(source, p3).rgb;

	vec3 first = G0(fuv.y) * (g0x * source_p0 + g1x * source_p1);
	vec3 second = G1(fuv.y) * (g0x * source_p2 + g1x * source_p3);

	return first + second;
}

vec3 CubicHermite(vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
	float t2 = t * t;
	float t3 = t * t * t;

	vec3 aa = -a / 2.0 + (3.0 * b) / 2.0 - (3.0 * c) / 2.0 + d / 2.0;
	vec3 bb = a - (5.0 * b) / 2.0 + 2.0 * c - d / 2.0;
	vec3 cc = -a / 2.0 + c / 2.0;
	vec3 dd = b;

	return aa * t3 + bb * t2 + cc * t + dd;
}

vec3 BicubicHermiteTextureSample(sampler2D source, vec2 uv_coord)
{
	vec2 texture_size = textureSize(source, 0);
	float pixel_size = 1.0f / texture_size.x;
	float half_pixel_size = 2.0f / texture_size.x;

	vec2 pixel = uv_coord * texture_size + 0.5f;

	vec2 frac = fract(pixel);
	pixel = floor(pixel) / texture_size - vec2(pixel_size / 2.0f);

	vec3 c00 = texture(source, pixel + vec2(    -pixel_size,		   -pixel_size)).rgb;
	vec3 c10 = texture(source, pixel + vec2(           0.0f,		   -pixel_size)).rgb;
	vec3 c20 = texture(source, pixel + vec2(     pixel_size,		    pixel_size)).rgb;
	vec3 c30 = texture(source, pixel + vec2(half_pixel_size,		   -pixel_size)).rgb;

	vec3 c01 = texture(source, pixel + vec2(    -pixel_size,				  0.0f)).rgb;
	vec3 c11 = texture(source, pixel + vec2(           0.0f,				  0.0f)).rgb;
	vec3 c21 = texture(source, pixel + vec2(	 pixel_size,				  0.0f)).rgb;
	vec3 c31 = texture(source, pixel + vec2(half_pixel_size,				  0.0f)).rgb;

	vec3 c02 = texture(source, pixel + vec2(    -pixel_size,			 pixel_size)).rgb;
	vec3 c12 = texture(source, pixel + vec2(           0.0f,			 pixel_size)).rgb;
	vec3 c22 = texture(source, pixel + vec2(     pixel_size,			 pixel_size)).rgb;
	vec3 c32 = texture(source, pixel + vec2(half_pixel_size,			 pixel_size)).rgb;

	vec3 c03 = texture(source, pixel + vec2(    -pixel_size,		half_pixel_size)).rgb;
	vec3 c13 = texture(source, pixel + vec2(           0.0f,		half_pixel_size)).rgb;
	vec3 c23 = texture(source, pixel + vec2(     pixel_size,		half_pixel_size)).rgb;
	vec3 c33 = texture(source, pixel + vec2(half_pixel_size,		half_pixel_size)).rgb;

	vec3 cp0x = CubicHermite(c00, c10, c20, c30, frac.x);
	vec3 cp1x = CubicHermite(c01, c11, c21, c31, frac.x);
	vec3 cp2x = CubicHermite(c02, c12, c22, c32, frac.x);
	vec3 cp3x = CubicHermite(c03, c13, c23, c33, frac.x);

	vec3 result = CubicHermite(cp0x, cp1x, cp2x, cp3x, frac.y);
	return result;
}

vec3 GetTextureColor(sampler2D source, float use_bicubic_upscale)
{
	if (use_bicubic_upscale > 0.5f)
		return BicubicHermiteTextureSample(source, vs_out.uv_coord);
	else
		return texture(source, vs_out.uv_coord).rgb;
}

/**
 * The fragment shader entry point.
 */
void main()
{
	vec3 color_1 = GetTextureColor(u_textures.texture1, u_textures.use_texture1_bicubic_upscale);
	vec3 color_2 = GetTextureColor(u_textures.texture2, u_textures.use_texture2_bicubic_upscale);


	vec3 result = vec3(0.0f);
	switch (u_textures.blend_mode)
	{
		case BLEND_MODE_NONE:		result = color_1 * u_textures.texture1_mix;										break;
		case BLEND_MODE_ADDITIVE:	result = color_1 * u_textures.texture1_mix + color_2 * u_textures.texture2_mix;	break;

		// Do not use!
		// TODO: Not correct for HDR colors!!!
		case BLEND_MODE_SCREEN:		result = 1.0f - (1.0f - color_1) * (1.0f - color_2);							break;

		default: break;
	}

	color = vec4(result, 1.0f);
}