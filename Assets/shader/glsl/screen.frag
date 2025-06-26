#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;

// For sharpen, blur
//const float offset = 1.0 / 300.0;

uniform int enable_autoExposure;
uniform float exposure;			// Computed from average luminance
uniform float contrast;			// User-defined or auto-adjusted
uniform float gamma;			// Usually 2.2

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722)); // Rec. 709
}

void main()
{
	vec3 color = texture(screenTexture, TexCoord).rgb;
	
	// Grayscale
    //vec4 result = vec4(vec3(dot(color.rgb, vec3(0.299, 0.587, 0.114))), 1.0);
	
	// Invert color
	//vec4 result = vec4(vec3(1.0) - color.rgb, 1.0);
	
	// Sharpen, blur (choose kernel)
	/*vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );*/
	// Sharpen kernel
    /*float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );*/
	// Blur kernel
	float kernel[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);
	// Edge detection kernel
	/*float kernel[9] = float[](
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	);*/
    
	/*vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
	vec4 result = vec4(col, 1.0);*/
	
	// --- Exposure ---
    vec3 mapped = color * exposure;

    // --- Contrast ---
    mapped = (mapped - 0.5) * contrast + 0.5;

    // --- Gamma correction ---
    mapped = pow(mapped, vec3(1.0 / gamma));
	
	vec4 result = vec4(mapped, 1.0);
	
    //FragColor = color;
    FragColor = result;
}