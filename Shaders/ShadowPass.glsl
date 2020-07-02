#type vertex

#version 420 core

/////////////////////////////////////////////////////////////
// In variables /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) in highp vec3 position;
layout(location = 5) in highp mat4 model;
//layout(location=6) in highp mat4 model; }
//layout(location=7) in highp mat4 model; } Reserved for model matrix
//layout(location=8) in highp mat4 model; }

/////////////////////////////////////////////////////////////
// Uniform variables ////////////////////////////////////////
/////////////////////////////////////////////////////////////

// 208
layout(std140, binding = 0) uniform ShadowData
{

	// 0
	highp mat4 model_matrix;

	// 64
	highp mat4 light_view_matrix;

	// 128
	highp mat4 light_projection_matrix;

	// 192
	highp float use_instancing;

} u_data;

/**
 * The fragment shader entry point.
 */
void main()
{
	mat4 actual_model_matrix = u_data.model_matrix;
	if (u_data.use_instancing > 0.5f)
		actual_model_matrix = model;

	gl_Position = u_data.light_projection_matrix * u_data.light_view_matrix * actual_model_matrix * vec4(position, 1.0f);
}

#type fragment

#version 420 core

/////////////////////////////////////////////////////////////
// Out variables ////////////////////////////////////////////
/////////////////////////////////////////////////////////////

layout(location = 0) out float depth;

/////////////////////////////////////////////////////////////
// Functions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/**
 * The fragment shader entry point.
 */
void main()
{
	// gl_FragDepth = gl_FragCoord.z;
	depth = gl_FragCoord.z;
}