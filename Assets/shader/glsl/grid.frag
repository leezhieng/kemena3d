#version 330 core

in vec4 vertexPositionFrag;
in vec3 viewPosFrag;
in vec2 texCoordFrag;
in mat4 modelMatrixFrag;
in float grid_sizeFrag;

uniform float alphaCutoff = 0.01;

out vec4 frag_color;

float LinearizeDepth(float depth , float zNear , float zFar)
{
	return (2 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

const float cell_size = 1.0f;
const float half_cell_size = cell_size * 0.5f;
const float subcell_size = 1.0f;
const float half_subcell_size = subcell_size * 0.5f;

const float cell_line_thickness    = 0.01f;
const float subcell_line_thickness = 0.02f;
const vec4 cell_color    = vec4( 0.75, 0.75, 0.75, 0.8 );
const vec4 subcell_color = vec4(  0.5,  0.5,  0.5, 0.8 );

void main ()
{
	float zbuffer = LinearizeDepth(gl_FragCoord.z, 2.0, 4500.0);
	
	vec2 cell_coords = mod(texCoordFrag / 100 + half_cell_size + 0.5, cell_size);
	vec2 subcell_coords = mod(texCoordFrag / 20 + half_subcell_size + 0.5, subcell_size);
	
	vec4 color = vec4(0);
	
	// Preview depth
	//frag_color = vec4(zbuffer, zbuffer, zbuffer, 1.0);
	
	// Preview UV
	//frag_color = vec4(cell_coords, 0.0, 1.0);
	
	if(fract(cell_coords.x / 0.1) < cell_line_thickness || fract(cell_coords.y / 0.1) < cell_line_thickness)
        color = cell_color;
	else if(fract(subcell_coords.x / 0.1) < subcell_line_thickness || fract(subcell_coords.y / 0.1) < subcell_line_thickness)
		color = subcell_color;
	
	color.a = max(color.a - zbuffer, 0.0);
	
	// Cutout based on alpha
    //if (color.a < alphaCutoff)
        //discard;
	
	frag_color = color;
}