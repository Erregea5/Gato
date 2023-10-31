#shader vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(texCoord.xy*2.0 - 1.0, 0.0, 1.0);
	
	v_TexCoord = texCoord;
}



#shader fragment
precision mediump float;
layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;
in vec2 v_TexCoord;


const float offset = 1.0 / 300.0;
const vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // top-left
    vec2(0.0f, offset), // top-center
    vec2(offset, offset), // top-right
    vec2(-offset, 0.0f),   // center-left
    vec2(0.0f, 0.0f),   // center-center
    vec2(offset, 0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2(0.0f, -offset), // bottom-center
    vec2(offset, -offset)  // bottom-right    
);
void main()
{
    float kernel[9] = float[](
        0, 0, 0,
        0, 1, 0,
        0, 0, 0
    );
    float div = 1.0;

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(u_Texture, v_TexCoord + offsets[i]));

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i] * div;

    //if (distance(col, vec3(1.0)) > 0)
      //  col = vec3(0.0);
    color = vec4(col,1.0);
}