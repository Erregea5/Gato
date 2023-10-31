#shader vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 n;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;
out vec2 v_TexCoord;

void main()
{
	gl_Position = u_MVP*vec4(position,1.0);
	v_TexCoord = texCoord;
}


#shader fragment
precision mediump float;
layout(location = 0) out vec4 color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
in vec2 v_TexCoord;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);
	color = u_Color/10.0+texColor-.2;
}