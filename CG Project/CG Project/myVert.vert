#version 130

vec4 positions[6] = vec4[6](
	vec4(-1, -1, 0, 1),
	vec4( 1, -1, 0, 1),
	vec4(-1,  1, 0, 1),

	vec4(-1,  1, 0, 1),
	vec4( 1, -1, 0, 1),
	vec4( 1,  1, 0, 1)
);

vec4 colors[6] = vec4[6](
	vec4(1, 0, 0, 1),
	vec4(0, 1, 0, 1),
	vec4(0, 0, 1, 1),

	vec4(0, 0, 1, 1),
	vec4(0, 1, 0, 1),
	vec4(1, 1, 1, 1)
);



// a pipeline-ban tovább adandó értékek
out vec4 vs_out_col;
out vec4 vs_out_pos;

// shader külső paraméterei - most a három transzformációs mátrixot egyben vesszük át
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * positions[gl_VertexID];
	vs_out_pos =  MVP * positions[gl_VertexID];
	vs_out_col = colors[gl_VertexID];
}