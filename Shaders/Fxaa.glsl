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

#define EDGE_THRESHOLD_MIN 0.0312f
#define EDGE_THRESHOLD_MAX 0.125f

#define SUB_PIXEL_QUALITY 0.75f

#define ITERATIONS 12

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

// 16
layout(std140, binding = 0) uniform FxaaData
{

	// 0
	highp sampler2D source_texture;
	
	// 8
	highp float display_edges;

} u_data;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

float RgbToLuma(vec3 color)
{
	const vec3 luma = vec3(0.299, 0.587, 0.144);
	return sqrt(dot(color, luma));
}

float FxaaQuality(int i)
{
	switch (i)
	{
		case 0: return 1.5f;
		case 1: return 2.0f;
		case 2: return 2.0f;
		case 3: return 2.0f;
		case 4: return 2.0f;
		case 5: return 4.0f;
		case 6: return 8.0f;

		case 7: return 8.0f;
		case 8: return 8.0f;
		case 9: return 8.0f;
		case 10: return 8.0f;
		case 11: return 8.0f;

		default: break;
	}

	return 1.5f;
}

/**
 * The fragment shader entry point.
 */
void main()
{
	vec2 texture_size = textureSize(u_data.source_texture, 0);
	vec2 texel_size = 1.0f / texture_size;

	vec3 color_center	= texture(u_data.source_texture, vs_out.uv_coord).rgb;
	vec3 color_up		= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2( 0,  1)).rgb;
	vec3 color_down		= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2( 0, -1)).rgb;
	vec3 color_left		= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2(-1,  0)).rgb;
	vec3 color_right	= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2( 1,  0)).rgb;


	float luma_center	= RgbToLuma(color_center);
	float luma_up		= RgbToLuma(color_up);
	float luma_down		= RgbToLuma(color_down);
	float luma_left		= RgbToLuma(color_left);
	float luma_right	= RgbToLuma(color_right);


	float luma_min = min(luma_center, min(min(luma_up, luma_down), min(luma_left, luma_right)));
	float luma_max = max(luma_center, max(max(luma_up, luma_down), max(luma_left, luma_right)));


	float luma_range = luma_max - luma_min;

	if (luma_range < max(EDGE_THRESHOLD_MIN, luma_max * EDGE_THRESHOLD_MAX))
	{
		if(u_data.display_edges > 0.5f)
			discard;

		color = vec4(color_center, 1.0f);
		return;
	}


	vec3 color_up_left		= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2(-1,  1)).rgb;
	vec3 color_up_right		= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2( 1,  1)).rgb;
	vec3 color_down_left	= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2(-1, -1)).rgb;
	vec3 color_down_right	= textureOffset(u_data.source_texture, vs_out.uv_coord, ivec2( 1, -1)).rgb;


	float luma_up_left		= RgbToLuma(color_up_left);
	float luma_up_right		= RgbToLuma(color_up_right);
	float luma_down_left	= RgbToLuma(color_down_left);
	float luma_down_right	= RgbToLuma(color_down_right);


	float luma_up_down = luma_up + luma_down;
	float luma_left_right = luma_left + luma_right;


	float luma_up_corners = luma_up_left + luma_up_right;
	float luma_down_corners = luma_down_left + luma_down_right;
	float luma_left_corners = luma_up_left + luma_down_left;
	float luma_right_corners = luma_up_right + luma_down_right;


	float edge_horizontal	= abs(-2.0f * luma_left + luma_left_corners)	+ abs(-2.0f * luma_center + luma_up_down)	 * 2.0f	+ abs(-2.0f * luma_right + luma_right_corners);
	float edge_vertical		= abs(-2.0f * luma_up + luma_up_corners)		+ abs(-2.0f * luma_center + luma_left_right) * 2.0f + abs(-2.0f * luma_down + luma_down_corners);


	bool is_horizontal = edge_horizontal >= edge_vertical;


	float luma_1 = is_horizontal ? luma_down : luma_left;
	float luma_2 = is_horizontal ? luma_up : luma_right;


	float gradient_1 = luma_1 - luma_center;
	float gradient_2 = luma_2 - luma_center;


	bool is_gradient_1_steepest = abs(gradient_1) >= abs(gradient_2);
	float gradient_scaled = 0.25f * max(abs(gradient_1), abs(gradient_2));


	float step_size = is_horizontal ? texel_size.y : texel_size.x;
	float luma_local_coverage = 0.0f;


	if (is_gradient_1_steepest)
	{
		step_size = -step_size;
		luma_local_coverage = 0.5f * (luma_1 + luma_center);
	}
	else
	{
		luma_local_coverage = 0.5f * (luma_2 + luma_center);
	}

	vec2 uv = vs_out.uv_coord;
	if (is_horizontal)
		uv.y += step_size * 0.5f;
	else
		uv.x += step_size * 0.5f;



	vec2 offset = is_horizontal ? vec2(texel_size.x, 0.0f) : vec2(0.0f, texel_size.y);


	vec2 uv_1 = uv - offset;
	vec2 uv_2 = uv + offset;


	vec3 color_end_1 = texture(u_data.source_texture, uv_1).rgb;
	vec3 color_end_2 = texture(u_data.source_texture, uv_2).rgb;


	float luma_end_1 = RgbToLuma(color_end_1);
	float luma_end_2 = RgbToLuma(color_end_2);


	luma_end_1 -= luma_local_coverage;
	luma_end_2 -= luma_local_coverage;


	bool reached_1 = abs(luma_end_1) >= gradient_scaled;
	bool reached_2 = abs(luma_end_2) >= gradient_scaled;
	bool reached_both = reached_1 && reached_2;


	if (!reached_1)
		uv_1 -= offset;
	
	if (!reached_2)
		uv_2 += offset;

	if (!reached_both)
	{

		for (int i = 2; i < ITERATIONS; ++i)
		{

			if (!reached_1)
			{
				color_end_1 = texture(u_data.source_texture, uv_1).rgb;
				luma_end_1 = RgbToLuma(color_end_1);

				luma_end_1 -= luma_local_coverage;
			}

			if (!reached_2)
			{
				color_end_2 = texture(u_data.source_texture, uv_2).rgb;
				luma_end_2 = RgbToLuma(color_end_2);

				luma_end_2 -= luma_local_coverage;
			}


			reached_1 = abs(luma_end_1) >= gradient_scaled;
			reached_2 = abs(luma_end_2) >= gradient_scaled;
			reached_both = reached_1 && reached_2;


			if (!reached_1)
				uv_1 -= offset * FxaaQuality(i);

			if (!reached_2)
				uv_2 += offset * FxaaQuality(i);

			if (reached_both)
				break;

		}

	}


	float distance_1 = is_horizontal ? (vs_out.uv_coord.x - uv_1.x) : (vs_out.uv_coord.y - uv_1.y);
	float distance_2 = is_horizontal ? (uv_2.x - vs_out.uv_coord.x) : (uv_2.y - vs_out.uv_coord.y);


	bool is_direction_1 = distance_1 < distance_2;
	float distance_final = min(distance_1, distance_2);


	float edge_thickness = distance_1 + distance_2;
	float pixel_offset = -distance_final / edge_thickness + 0.5f;


	bool is_luma_center_smaller = luma_center < luma_local_coverage;
	bool is_correct_variation = ((is_direction_1 ? luma_end_1 : luma_end_2) < 0.0f) != is_luma_center_smaller;


	float final_offset = is_correct_variation ? pixel_offset : 0.0f;



	// subpixel antialiasing
	float luma_average = (1.0f / 12.0f) * (2.0f * (luma_up_down + luma_left_right) + luma_left_corners + luma_right_corners);


	float sub_pixel_offset_1 = clamp(abs(luma_average - luma_center) / luma_range, 0.0f, 1.0f);
	float sub_pixel_offset_2 = (-2.0f * sub_pixel_offset_1 + 3.0f) * sub_pixel_offset_1 * sub_pixel_offset_1;


	float sub_pixel_offset_final = sub_pixel_offset_2 * sub_pixel_offset_2 * SUB_PIXEL_QUALITY;
	final_offset = max(final_offset, sub_pixel_offset_final);



	// comute final uv
	vec2 final_uv = vs_out.uv_coord;

	if (is_horizontal)
		final_uv.y += final_offset * step_size;
	else
		final_uv.x += final_offset * step_size;


	vec3 final_color = texture(u_data.source_texture, final_uv).rgb;
	color = vec4(final_color, 1.0f);

	// http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
}